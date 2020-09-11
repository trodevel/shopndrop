/*

Core.

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

// $Revision: 13792 $ $Date:: 2020-09-11 #$ $Author: serge $

#include "core.h"                       // self

#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK
#include "utils/utils_assert.h"      // ASSERT
#include "utils/dummy_logger.h"      // dummy_log

namespace shopndrop {

Core::Core()
{
}

Core::~Core()
{
    {
        std::string error_msg;
    }
}

bool Core::init(
        const Config                            & config,
        const session_manager::Manager::Config  & sesman_config,
        const shopndrop::LeadDB::Config         & lead_db_config,
        uint32_t                                log_id_db,
        uint32_t                                log_id_handler,
        uint32_t                                log_id_ride,
        uint32_t                                log_id_order,
        scheduler::IScheduler                   * sched )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    if( !sched )
        return false;

    config_ = config;

    bool b = sh_.init( & perm_checker_, & ht_, config.request_log, config.request_log_rotation_interval_min );
    ASSERT( b );

    b = gh_.init( & sess_man_, & user_man_ );
    ASSERT( b );

    b = user_man_.init();
    ASSERT( b );

    std::string error_msg;

    b = user_man_.load( config.credentials_file, & error_msg );
    ASSERT( b );

    b = authen_.init( & user_man_ );
    ASSERT( b );

    b = generic_perm_checker_.init( & sess_man_ );
    ASSERT( b );

    b = perm_checker_.init( & generic_perm_checker_, & sess_man_, & db_ );
    ASSERT( b );

    try
    {
        b = tzc_.init( config_.timezone_file );
    }
    catch( std::exception & e )
    {
        dummy_log_fatal( "Core", "cannot initialize TZC: %s", e.what() );
        return false;
    }

    b = time_adj_.init( & tzc_ );
    ASSERT( b );

    b = sess_man_.init( & authen_, sesman_config );
    ASSERT( b );

    goodies_db_.init( config_.goodies_db_file );

    b = ht_.init( log_id_handler,
            & gh_, & h_ );
    ASSERT( b );

    b = h_.init( log_id_handler,
            & user_man_, & db_, & lead_db_, & tzc_,
            & time_adj_, & db_obj_gen_, & goodies_db_ );
    ASSERT( b );

    b = db_obj_gen_.init( & time_adj_ );
    ASSERT( b );

    db::OrderDB::Config job_db_config;

    job_db_config.status_file  = config.db_status_file;

    b = db_.init( job_db_config, log_id_db, log_id_ride, log_id_order, & user_man_ /*, & db_obj_gen_ */);
    if( b == false )
        return false;

    b = lead_db_.init( lead_db_config );
    if( b == false )
        return false;

    periodic_call_gen_.init( sched );

    periodic_call_gen_.register_callee( this );

    return true;
}

restful_interface::IHandler* Core::get_http_handler()
{
    MUTEX_SCOPE_LOCK( mutex_ );

    return & sh_;
}

// interface DefaultPeriodic
void Core::once_per_minute()
{
    //once_per_hour();    // for tests
}

void Core::once_per_hour()
{
//    db_.save_status();

    {
    }
}

} // namespace shopndrop
