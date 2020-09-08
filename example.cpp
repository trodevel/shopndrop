/*

Example.

Copyright (C) 2019 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// $Revision: 13757 $ $Date:: 2020-09-08 #$ $Author: serge $

#include <iostream>
#include <thread>                           // std::thread
#include <functional>                       // std::bind
#include <vector>                           // std::vector

#include "utils/dummy_logger.h"             // dummy_log_set_log_level
#include "utils/logfile_time_writer.h"      // utils::LogfileTimeWriter

#include "daemons/daemon.h"                 // Daemon
#include "http_server_wrap/server.h"        // Server
#include "scheduler/scheduler.h"            // Scheduler
#include "scheduler/periodic_job_aux.h"     // scheduler::create_and_insert_periodic_job
#include "threcon/controller.h"             // Controller
#include "daemons/signal_poller.h"          // SignalPoller
#include "config_reader/config_reader.h"    // config_reader::ConfigReader

#include "core.h"
#include "config_extractor.h"               // init_config

void check_signals( threcon::Controller * controller )
{
    if( daemons::SignalPoller::poll_term() || daemons::SignalPoller::poll_int() )
        controller->send_shutdown();

    if( daemons::SignalPoller::poll_hup() )
        return;
}

void init_check_signals(
        scheduler::IScheduler   & sched,
        threcon::Controller     * controller )
{
    scheduler::job_id_t job_id;
    std::string         error_msg;

    auto now = scheduler::get_now();

    scheduler::Time     exec_time   = now + std::chrono::seconds( 1 );
    scheduler::Duration period      = std::chrono::milliseconds( 10 );

    scheduler::create_and_insert_periodic_job(
                & job_id, & error_msg, sched, "check_signals", exec_time, period,
                std::bind( & check_signals, controller ) );
}

int main( int argc, char **argv )
{
    std::cout << "Hello, world" << std::endl;

    try
    {
        std::string config_file( "shopndrop.ini" );

        config_reader::ConfigReader cr;

        if( cr.init( config_file ) == false )
        {
            std::cerr << "ERROR: cannot read config file " + config_file << std::endl;

            return 0;
        }
        else
        {
            std::cout << "loaded config file " + config_file << std::endl;
        }

        std::string                         filename;
        uint32_t                            rotation_interval;
        http_server_wrap::Config            server_config;
        shopndrop::Core::Config                  core_config;
        shopndrop::LeadDB::Config                lead_db_config;
        uint32_t                            granularity_ms;
        session_manager::Manager::Config    sesman_cfg;

        shopndrop::init_logs( & filename, & rotation_interval, cr );
        shopndrop::init_config( & server_config, cr );
        shopndrop::init_config( & core_config, cr );
        shopndrop::init_scheduler( & granularity_ms, cr );
        shopndrop::init_config( & sesman_cfg, cr );
        shopndrop::init_config( & lead_db_config, cr );

        utils::LogfileTimeWriter w( filename, rotation_interval );

        dummy_logger::set_log_level( log_levels_log4j::TRACE );
        dummy_logger::set_writer( & w );

        auto log_id_main            = dummy_logger::register_module( "main" );
        auto log_id_core_handler    = dummy_logger::register_module( "shopndrop::Handler" );

        auto log_id_http_server     = dummy_logger::register_module( "http_server_wrap" );

        auto log_id_db              = dummy_logger::register_module( "OrderDB" );
        auto log_id_ride            = dummy_logger::register_module( "Ride" );
        auto log_id_order           = dummy_logger::register_module( "Order" );

        dummy_logger::set_log_level( log_id_http_server,      log_levels_log4j::INFO );

        threcon::Controller     controller;

        http_server_wrap::Server http_server;

        std::chrono::milliseconds granularity( granularity_ms );
        scheduler::Duration dur( granularity );
        scheduler::Scheduler sched( dur );

        shopndrop::Core core;

        bool should_demonize    = true;

        if( argc > 1 )
        {
            if( std::string( argv[1] ) == "--nodaemon" )
            {
                should_demonize = false;
            }
            else
            {
                std::cout << "unsupported param " << argv[1] << std::endl;
                dummy_log_fatal( log_id_main, "unsupported param %s", argv[1] );
                return EXIT_FAILURE;
            }
        }

        if( should_demonize )
        {
            if( daemons::Deamon::daemonize() )
            {
                std::cout << "demonized" << std::endl;
                dummy_log_info( log_id_main, "daemonized" );
            }
            else
            {
                std::cout << "cannot demonize" << std::endl;
                dummy_log_fatal( log_id_main, "cannot daemonize" );

                return EXIT_FAILURE;
            }
        }
        else
        {
            std::cout << "contining without daemon" << std::endl;
            dummy_log_info( log_id_main, "contining without daemon" );
        }

        http_server.init( server_config, log_id_http_server, core.get_http_handler() );
        auto b = core.init(
                core_config, sesman_cfg,
                lead_db_config,
                log_id_db,
                log_id_core_handler,
                log_id_ride,
                log_id_order,
                &sched );

        if( b == false )
        {
            dummy_log_fatal( log_id_main, "cannot init core" );
            return 1;
        }

        init_check_signals( sched, & controller );

        controller.register_client( & http_server );

        std::vector< std::thread > tg;

        tg.push_back( std::thread( std::bind( &threcon::Controller::thread_func, &controller ) ) );

        sched.run();
        http_server.start();

        for( auto & t : tg )
            t.join();

        sched.shutdown();

        dummy_log_info( log_id_main, "exit" );
    }
    catch( std::exception& e )
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }

    return 0;
}
