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

#ifndef SHOPNDROP_SHOPNDROP_H
#define SHOPNDROP_SHOPNDROP_H

#include "session_manager/session_manager.h"// session_manager::SessionManager
#include <mutex>                            // std::mutex

#include "thunk.h"                          // Thunk
#include "utils/boost_timezone.h"        // utils::TimeZoneConverter
#include "generic_handler/handler.h"     // generic_handler::Handler
#include "generic_handler/perm_checker.h"        // generic_handler::PermChecker
#include "periodic_call_gen/default_periodic.h"  // periodic_call_gen::DefaultPeriodic
#include "periodic_call_gen/periodic_call_gen.h" // periodic_call_gen::PeriodicCallGen
#include "handler_thunk.h"                  // HandlerThunk
#include "handler.h"                        // Handler
#include "authenticator.h"                  // Authenticator
#include "db_order_db.h"                    // db::OrderDB
#include "db_obj_generator.h"               // ObjGenerator
#include "perm_checker.h"                   // PermChecker
#include "time_adjuster.h"                  // TimeAdjuster
#include "user_manager/user_manager.h"           // user_manager::UserManager
#include "user_reg_email/user_reg_email.h"  // UserRegEmail
#include "user_reg_handler/handler.h"       // user_reg_handler::Handler
#include "user_reg_handler/handler_thunk.h" // user_reg_handler::HandlerThunk
#include "goodies_db.h"                     // GoodiesDB

namespace scheduler
{
class IScheduler;
}

namespace shopndrop {

class Core: public periodic_call_gen::DefaultPeriodic
{
public:
    struct Config
    {
        std::string db_status_file;
        std::string request_log;
        uint32_t    request_log_rotation_interval_min;
        std::string users_db_file;
        std::string user_reg_email_credentials_file;
        std::string timezone_file;
        std::string goodies_db_file;
    };

public:

    Core();
    ~Core();

    void init(
            const Config                            & config,
            const session_manager::Config           & sesman_config,
            const user_reg::Config                  & user_reg_config,
            const user_reg_email::Config            & user_reg_email_config,
            uint32_t                                log_id_db,
            uint32_t                                log_id_handler,
            uint32_t                                log_id_ride,
            uint32_t                                log_id_order,
            scheduler::IScheduler                   * sched );

    void shutdown();

    restful_interface::IHandler* get_http_handler();

    // interface DefaultPeriodic
    void once_per_minute() override;
    void once_per_hour() override;

private:


private:
    mutable std::mutex          mutex_;

    Config                          config_;

    shopndrop::Thunk                sh_;
    generic_handler::Handler        gh_;
    generic_handler::PermChecker    generic_perm_checker_;
    shopndrop::HandlerThunk         ht_;
    shopndrop::Handler              h_;
    shopndrop::ObjGenerator         db_obj_gen_;
    shopndrop::db::OrderDB           db_;
    shopndrop::Authenticator        authen_;
    shopndrop::PermChecker          perm_checker_;
    user_manager::UserManager   user_man_;
    session_manager::SessionManager    sess_man_;
    user_reg::UserReg               user_reg_;
    user_reg_email::UserRegEmail    user_reg_email_;
    user_reg_handler::Handler       user_reg_handler_;
    user_reg_handler::HandlerThunk  user_reg_handler_thunk_;
    periodic_call_gen::PeriodicCallGen      periodic_call_gen_;
    GoodiesDB                       goodies_db_;

    utils::TimeZoneConverter    tzc_;
    TimeAdjuster                time_adj_;
};

} // namespace shopndrop

#endif // SHOPNDROP_SHOPNDROP_H
