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

// $Revision: 13938 $ $Date:: 2020-10-03 #$ $Author: serge $

#ifndef SHOPNDROP__HANDLER_H
#define SHOPNDROP__HANDLER_H

#include <mutex>                    // std::mutex
#include <map>                      // std::map
#include "user_manager/user_manager.h"      // user_manager::UserManager
#include "shopndrop_protocol/protocol.h"            // shopndrop_protocol::...
#include "shopndrop_web_protocol/protocol.h"        // shopndrop_web_protocol::...
#include "user_management_protocol/protocol.h"      // user_management_protocol::...
#include "user_reg_protocol/protocol.h"             // user_reg_protocol::...
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

namespace shopndrop {

class Handler
{
public:

    Handler();

    bool init(
            unsigned int                        log_id,
            user_manager::UserManager           * user_man,
            db::OrderDB                         * order_db,
            utils::TimeZoneConverter            * tzc,
            TimeAdjuster                        * time_adj,
            ObjGenerator                        * obj_gen,
            GoodiesDB                           * goodies_db );

    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::AddRideRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::GetRideRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::CancelRideRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const user_management_protocol::GetUserInfoRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::AddOrderRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::CancelOrderRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::AcceptOrderRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::DeclineOrderRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::MarkDeliveredOrderRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_protocol::RateShopperRequest & r );

    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_web_protocol::GetProductItemListRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_web_protocol::GetShoppingRequestInfoRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_web_protocol::GetShoppingListWithTotalsRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_web_protocol::GetDashScreenUserRequest & r );
    generic_protocol::BackwardMessage* handle( user_id_t session_user_id, const shopndrop_web_protocol::GetDashScreenShopperRequest & r );

private:

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

    user_manager::UserManager           * user_man_;
    db::OrderDB                         * order_db_;
    utils::TimeZoneConverter            * tzc_;
    TimeAdjuster                        * time_adj_;
    ObjGenerator                        * obj_gen_;
    GoodiesDB                           * goodies_db_;

    id_t                                last_order_id_;
};

} // namespace shopndrop

#endif // SHOPNDROP__HANDLER_H
