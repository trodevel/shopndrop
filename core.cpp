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

// $Revision: 13938 $ $Date:: 2020-10-03 #$ $Author: serge $

#include "core.h"                       // self

#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK
#include "utils/utils_assert.h"      // ASSERT
#include "utils/dummy_logger.h"      // dummy_log

#define MODULENAME      "Core"

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

void Core::init(
        const Config                            & config,
        const session_manager::Config           & sesman_config,
        const user_reg::Config                  & user_reg_config,
        const user_reg_email::Config            & user_reg_email_config,
        uint32_t                                log_id_db,
        uint32_t                                log_id_handler,
        uint32_t                                log_id_ride,
        uint32_t                                log_id_order,
        scheduler::IScheduler                   * sched )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    assert( sched );

    config_ = config;

    user_reg_handler_thunk_.init( log_id_handler, & gh_, & user_reg_handler_ );

    sh_.init( & perm_checker_, & ht_, & user_reg_handler_thunk_, config.request_log, config.request_log_rotation_interval_min );

    gh_.init( & sess_man_, & user_man_ );

    user_man_.init( config.users_db_file );

    authen_.init( & user_man_ );

    generic_perm_checker_.init( & sess_man_ );

    perm_checker_.init( & generic_perm_checker_, & sess_man_, & db_ );

    tzc_.init( config_.timezone_file );

    time_adj_.init( & tzc_ );

    sess_man_.init( & authen_, sesman_config );

    goodies_db_.init( config_.goodies_db_file );

    ht_.init( log_id_handler,
            & gh_, & h_ );

    user_reg_.init( user_reg_config, & user_man_ );

    user_reg_email_.init( user_reg_email_config, & user_reg_ );

    user_reg_handler_.init( & user_reg_email_ );

    h_.init( log_id_handler,
            & user_man_, & db_, & tzc_,
            & time_adj_, & db_obj_gen_, & goodies_db_ );

    db_obj_gen_.init( & time_adj_ );

    db::OrderDB::Config job_db_config;

    job_db_config.status_file  = config.db_status_file;

    db_.init( job_db_config, log_id_db, log_id_ride, log_id_order, & user_man_ /*, & db_obj_gen_ */);

    periodic_call_gen_.init( sched );

    periodic_call_gen_.register_callee( this );
}

void Core::shutdown()
{
    dummy_log_info( MODULENAME, "shutdown" );

    MUTEX_SCOPE_LOCK( mutex_ );

    std::string error_msg;

    user_man_.save( & error_msg, config_.users_db_file );
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
    dummy_log_trace( MODULENAME, "once_per_hour" );

    MUTEX_SCOPE_LOCK( mutex_ );

    std::string error_msg;

    user_man_.save( & error_msg, config_.users_db_file );
}

} // namespace shopndrop
