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

// $Revision: 13757 $ $Date:: 2020-09-08 #$ $Author: serge $

#ifndef SHOPNDROP_SHOPNDROP_H
#define SHOPNDROP_SHOPNDROP_H

#include <mutex>                            // std::mutex

#include "thunk.h"                          // Thunk
#include "utils/boost_timezone.h"        // utils::TimeZoneConverter
#include "generic_handler/handler.h"     // generic_handler::Handler
#include "generic_handler/perm_checker.h"        // generic_handler::PermChecker
#include "periodic_call_gen/default_periodic.h"  // periodic_call_gen::DefaultPeriodic
#include "periodic_call_gen/periodic_call_gen.h" // periodic_call_gen::PeriodicCallGen
#include "handler.h"                        // Handler
#include "authenticator.h"                  // Authenticator
#include "db_order_db.h"                    // db::OrderDB
#include "db_obj_generator.h"               // ObjGenerator
#include "perm_checker.h"                   // PermChecker
#include "time_adjuster.h"                  // TimeAdjuster
#include "lead_db.h"                        // LeadDB
#include "session_manager/manager.h"     // session_manager::Manager
#include "user_manager/user_manager.h"           // user_manager::UserManager
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
        std::string credentials_file;
        std::string timezone_file;
        std::string goodies_db_file;
    };

public:

    Core();
    ~Core();

    bool init(
            const Config                            & config,
            const session_manager::Manager::Config  & sesman_config,
            const shopndrop::LeadDB::Config         & lead_db_config,
            uint32_t                                log_id_db,
            uint32_t                                log_id_handler,
            uint32_t                                log_id_ride,
            uint32_t                                log_id_order,
            scheduler::IScheduler                   * sched );

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
    shopndrop::Handler              h_;
    shopndrop::ObjGenerator         db_obj_gen_;
    shopndrop::db::OrderDB           db_;
    shopndrop::Authenticator        authen_;
    shopndrop::PermChecker          perm_checker_;
    user_manager::UserManager   user_man_;
    session_manager::Manager    sess_man_;
    periodic_call_gen::PeriodicCallGen      periodic_call_gen_;
    GoodiesDB                       goodies_db_;

    utils::TimeZoneConverter    tzc_;
    TimeAdjuster                time_adj_;

    shopndrop::LeadDB                lead_db_;
};

} // namespace shopndrop

#endif // SHOPNDROP_SHOPNDROP_H
