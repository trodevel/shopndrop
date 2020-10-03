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

#ifndef SHOPNDROP__HANDLER_THUNK_H
#define SHOPNDROP__HANDLER_THUNK_H

#include <mutex>                    // std::mutex

#include "generic_protocol/protocol.h"  // generic_protocol::BackwardMessage

#include "types.h"                  // user_id_t

namespace generic_handler
{
class Handler;
}

namespace shopndrop {

class Handler;

class HandlerThunk
{
public:

    HandlerThunk();

    bool init(
            unsigned int                        log_id,
            generic_handler::Handler            * generic_handler,
            Handler                             * handler );

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

    bool is_inited__() const;

private:
    mutable std::mutex          mutex_;

    unsigned int                        log_id_;

    generic_handler::Handler            * generic_handler_;
    Handler                             * handler_;
};

} // namespace shopndrop

#endif // SHOPNDROP__HANDLER_THUNK_H
