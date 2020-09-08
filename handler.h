/*

Request handler.

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

#ifndef SHOPNDROP_HANDLER_H
#define SHOPNDROP_HANDLER_H

#include <mutex>                    // std::mutex
#include <map>                      // std::map
#include "user_manager/user_manager.h"      // user_manager::UserManager
#include "shopndrop_protocol/protocol.h"            // shopndrop_protocol::SayRequest, ...
#include "shopndrop_web_protocol/protocol.h"        // shopndrop_web_protocol::...
#include "db_order_db.h"                    // db::OrderDB
#include "utils/boost_timezone.h"        // utils::TimeZoneConverter
#include "time_adjuster.h"          // TimeAdjuster
#include "db_obj_generator.h"       // ObjGenerator
#include "goodies_db.h"             // GoodiesDB

#include "types.h"                  // job_id_t

#include "db_ride.h"                // db::Ride

namespace generic_protocol
{
class ForwardMessage;
class BackwardMessage;
}

namespace generic_handler
{
class Handler;
}

namespace shopndrop {

class LeadDB;

class Handler
{
public:

    Handler();

    bool init(
            unsigned int                        log_id,
            generic_handler::Handler            * generic_handler,
            user_manager::UserManager           * user_man,
            db::OrderDB                         * order_db,
            LeadDB                              * lead_db,
            utils::TimeZoneConverter            * tzc,
            TimeAdjuster                        * time_adj,
            ObjGenerator                        * obj_gen,
            GoodiesDB                           * goodies_db );

    // quasi-interface IHandler
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const basic_parser::Object * r );

private:

    generic_protocol::BackwardMessage* handle_AddRideRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_GetRideRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_CancelRideRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_user_management_GetUserInfoRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_AddOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_CancelOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_AcceptOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_DeclineOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_MarkDeliveredOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_RateShopperRequest( user_id_t session_user_id, const basic_parser::Object * r );

    generic_protocol::BackwardMessage* handle_web_GetProductItemListRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_web_GetShoppingRequestInfoRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_web_GetShoppingListWithTotalsRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_web_GetDashScreenUserRequest( user_id_t session_user_id, const basic_parser::Object * r );
    generic_protocol::BackwardMessage* handle_web_GetDashScreenShopperRequest( user_id_t session_user_id, const basic_parser::Object * r );

    generic_protocol::BackwardMessage* handle_user_reg_RegisterUserRequest( user_id_t session_user_id, const basic_parser::Object * r );

    bool validate( std::string * error_msg, const shopndrop_protocol::RideSummary & r, const std::string & timezone ) const;
    bool validate( std::string * error_msg, uint32_t * delivery_time, user_id_t * shopper_id, const shopndrop_protocol::AddOrderRequest & r ) const;

    typedef std::map<std::string, std::string>  MapKeyValue;

    double get_minimal_basket_size() const;
    bool is_minimal_basket_size_reached( double sum ) const;
    static double calculate_earning( double sum );

    bool get_user_timezone( std::string * timezone, user_id_t user_id );

    bool is_inited__() const;

private:
    mutable std::mutex          mutex_;

    unsigned int                        log_id_;

    generic_handler::Handler            * generic_handler_;
    user_manager::UserManager           * user_man_;
    db::OrderDB                         * order_db_;
    LeadDB                              * lead_db_;
    utils::TimeZoneConverter            * tzc_;
    TimeAdjuster                        * time_adj_;
    ObjGenerator                        * obj_gen_;
    GoodiesDB                           * goodies_db_;

    id_t                                last_order_id_;
};

} // namespace shopndrop

#endif // SHOPNDROP_HANDLER_H
