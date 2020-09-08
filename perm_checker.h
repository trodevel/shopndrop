/*

Permission checker.

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

#ifndef SHOPNDROP_PERM_CHECKER_H
#define SHOPNDROP_PERM_CHECKER_H

#include "generic_handler/perm_checker.h"        // generic_handler::PermChecker
#include "session_manager/manager.h"             // session_manager::Manager
#include "db_order_db.h"                         // db::OrderDB

namespace shopndrop {

class PermChecker
{
public:

    PermChecker();

    bool init(
            generic_handler::PermChecker        * generic_perm_checker,
            session_manager::Manager            * sess_man,
            db::OrderDB                         * order_db );

    // quasi-interface IHandler
    bool is_authenticated( user_id_t * session_user_id, const basic_parser::Object * r );
    bool is_allowed( user_id_t session_user_id, const basic_parser::Object * r );

private:
    bool is_allowed_AddRideRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_GetRideRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_CancelRideRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_user_management_GetUserInfoRequest( user_id_t session_user_id, const basic_parser::Object * r );

    bool is_allowed_AddOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_CancelOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_AcceptOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_DeclineOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_MarkDeliveredOrderRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_RateShopperRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_GetRideOrderInfoRequest( user_id_t session_user_id, const basic_parser::Object * r );

    bool is_allowed_web_GetProductItemListRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_web_GetShoppingListWithTotalsRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_web_GetShoppingRequestInfoRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_web_GetDashScreenUserRequest( user_id_t session_user_id, const basic_parser::Object * r );
    bool is_allowed_web_GetDashScreenShopperRequest( user_id_t session_user_id, const basic_parser::Object * r );

    bool is_allowed_lead_reg_RegisterUserRequest( user_id_t session_user_id, const basic_parser::Object * r );

    static bool does_belong_to_user( user_id_t user_id, user_id_t session_user_id, bool should_belong_to_user );

    bool is_ride_id_valid( user_id_t session_user_id, uint32_t ride_id, bool should_belong_to_user );
    bool is_order_id_valid( user_id_t session_user_id, uint32_t order_id, bool should_belong_to_user );
    bool is_shopping_list_id_valid( user_id_t session_user_id, uint32_t shopping_list_id, bool should_belong_to_user );
    bool validate_user_id( user_id_t session_user_id, uint32_t user_id );

private:
    generic_handler::PermChecker        * generic_perm_checker_;
    session_manager::Manager            * sess_man_;
    db::OrderDB                         * order_db_;
};

} // namespace shopndrop

#endif // SHOPNDROP_PERM_CHECKER_H
