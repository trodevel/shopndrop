/*

shopndrop_protocol::Request handler.

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

#include "handler_thunk.h"      // self

#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>

#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"      // dummy_log
#include "utils/utils_assert.h"      // ASSERT

#include "shopndrop_protocol/protocol.h"            // shopndrop_protocol::...
#include "shopndrop_web_protocol/protocol.h"        // shopndrop_web_protocol::...

#include "user_reg_protocol/protocol.h"    // user_reg_protocol::
#include "user_management_protocol/protocol.h"  // user_management_protocol::

#include "generic_handler/handler.h"                // generic_handler::Handler

#include "handler.h"                                // Handler

#define MODULENAME      "shopndrop::HandlerThunk"

namespace shopndrop {

HandlerThunk::HandlerThunk():
    log_id_( 0 ),
    generic_handler_( nullptr ),
    handler_( nullptr )
{
}


bool HandlerThunk::init(
        unsigned int                        log_id,
        generic_handler::Handler            * generic_handler,
        Handler                             * handler )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    ASSERT( generic_handler );
    ASSERT( handler );

    log_id_             = log_id;
    generic_handler_    = generic_handler;
    handler_            = handler;

    return true;
}


bool HandlerThunk::is_inited__() const
{
    return true;
}

generic_protocol::BackwardMessage* HandlerThunk::handle( user_id_t session_user_id, const basic_parser::Object * req )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    ASSERT( is_inited__() );

    typedef HandlerThunk Type;

    typedef generic_protocol::BackwardMessage* (Type::*PPMF)( user_id_t session_user_id, const basic_parser::Object * r );

#define MAP_ENTRY(_v)       { typeid( shopndrop_protocol::_v ),        & Type::handle_##_v }
#define MAP_ENTRY_WEB(_v)   { typeid( shopndrop_web_protocol::_v ),   & Type::handle_web_##_v }
#define MAP_ENTRY_USER_REG(_v)  { typeid( user_reg_protocol::_v ),  & Type::handle_user_reg_##_v }
#define MAP_ENTRY_USER_MANAGEMENT(_v)   { typeid( user_management_protocol::_v ),  & Type::handle_user_management_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        MAP_ENTRY( AddRideRequest ),
        MAP_ENTRY( GetRideRequest ),
        MAP_ENTRY( CancelRideRequest ),
        MAP_ENTRY_USER_MANAGEMENT( GetUserInfoRequest ),
        MAP_ENTRY( AddOrderRequest ),
        MAP_ENTRY( CancelOrderRequest ),
        MAP_ENTRY( AcceptOrderRequest ),
        MAP_ENTRY( DeclineOrderRequest ),
        MAP_ENTRY( MarkDeliveredOrderRequest ),
        MAP_ENTRY( RateShopperRequest ),

        MAP_ENTRY_WEB( GetProductItemListRequest ),
        MAP_ENTRY_WEB( GetShoppingRequestInfoRequest ),
        MAP_ENTRY_WEB( GetShoppingListWithTotalsRequest ),
        MAP_ENTRY_WEB( GetDashScreenUserRequest ),
        MAP_ENTRY_WEB( GetDashScreenShopperRequest ),

        MAP_ENTRY_USER_REG( RegisterUserRequest ),
    };

#undef MAP_ENTRY
#undef MAP_ENTRY_WEB
#undef MAP_ENTRY_USER_REG
#undef MAP_ENTRY_USER_MANAGEMENT

    auto it = funcs.find( typeid( * req ) );

    if( it == funcs.end() )
    {
        return generic_handler_->handle( session_user_id, req );
    }

    return (this->*it->second)( session_user_id, req );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_AddRideRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    // private: no mutex lock

    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::AddRideRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_GetRideRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::GetRideRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_CancelRideRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::CancelRideRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_user_management_GetUserInfoRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const user_management_protocol::GetUserInfoRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_AddOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::AddOrderRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_CancelOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::CancelOrderRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_AcceptOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::AcceptOrderRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_DeclineOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::DeclineOrderRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_MarkDeliveredOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::MarkDeliveredOrderRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_RateShopperRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_protocol::RateShopperRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_web_GetProductItemListRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_web_protocol::GetProductItemListRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_web_GetShoppingRequestInfoRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_web_protocol::GetShoppingRequestInfoRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_web_GetShoppingListWithTotalsRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_web_protocol::GetShoppingListWithTotalsRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_web_GetDashScreenUserRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_web_protocol::GetDashScreenUserRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_web_GetDashScreenShopperRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const shopndrop_web_protocol::GetDashScreenShopperRequest &>( * rr ) );
}

generic_protocol::BackwardMessage* HandlerThunk::handle_user_reg_RegisterUserRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    return handler_->handle( session_user_id, dynamic_cast< const user_reg_protocol::RegisterUserRequest &>( * rr ) );
}


} // namespace shopndrop
