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

#include "perm_checker.h"               // self

#include <typeindex>                    // std::type_index
#include <unordered_map>

#include "user_reg_protocol/protocol.h"        // user_reg_protocol::
#include "user_management_protocol/protocol.h"  // user_management_protocol::
#include "shopndrop_protocol/protocol.h"      // shopndrop_protocol::
#include "shopndrop_protocol/protocol.h"      // shopndrop_web_protocol
#include "utils/dummy_logger.h"      // dummy_log
#include "utils/utils_assert.h"      // ASSERT
#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK

#define MODULENAME      "shopndrop::PermChecker"

namespace shopndrop {

PermChecker::PermChecker():
    generic_perm_checker_( nullptr ),
    sess_man_( nullptr ),
    order_db_( nullptr )
{
}

bool PermChecker::init(
        generic_handler::PermChecker        * generic_perm_checker,
        session_manager::Manager            * sess_man,
        db::OrderDB                         * order_db )
{
    if( !generic_perm_checker || !sess_man )
        return false;

    generic_perm_checker_   = generic_perm_checker;
    sess_man_               = sess_man;
    order_db_                = order_db;

    return true;
}

bool PermChecker::is_authenticated( user_id_t * session_user_id, const basic_parser::Object * rr )
{
    return generic_perm_checker_->is_authenticated( session_user_id, rr );
}

bool PermChecker::is_allowed( user_id_t session_user_id, const basic_parser::Object * req )
{
    typedef PermChecker Type;

    typedef bool (Type::*PPMF)( user_id_t session_user_id, const basic_parser::Object * rr );

#define MAP_ENTRY(_v)       { typeid( shopndrop_protocol::_v ),         & Type::is_allowed_##_v }
#define MAP_ENTRY_WEB(_v)   { typeid( shopndrop_web_protocol::_v ),    & Type::is_allowed_web_##_v }
#define MAP_ENTRY_LEAD_REG(_v)  { typeid( user_reg_protocol::_v ),      & Type::is_allowed_lead_reg_##_v }
#define MAP_ENTRY_USER_MANAGEMENT(_v)   { typeid( user_management_protocol::_v ),  & Type::is_allowed_user_management_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        MAP_ENTRY( AddRideRequest ),
        MAP_ENTRY( CancelRideRequest ),
        MAP_ENTRY( GetRideRequest ),
        MAP_ENTRY( AddOrderRequest ),
        MAP_ENTRY( CancelOrderRequest ),
        MAP_ENTRY( AcceptOrderRequest ),
        MAP_ENTRY( DeclineOrderRequest ),
        MAP_ENTRY( MarkDeliveredOrderRequest ),
        MAP_ENTRY( RateShopperRequest ),
        MAP_ENTRY_USER_MANAGEMENT( GetUserInfoRequest ),

        MAP_ENTRY_WEB( GetProductItemListRequest ),
        MAP_ENTRY_WEB( GetShoppingRequestInfoRequest ),
        MAP_ENTRY_WEB( GetShoppingListWithTotalsRequest ),
        MAP_ENTRY_WEB( GetDashScreenUserRequest ),
        MAP_ENTRY_WEB( GetDashScreenShopperRequest ),

        MAP_ENTRY_LEAD_REG( RegisterUserRequest ),
    };

#undef MAP_ENTRY
#undef MAP_ENTRY_WEB
#undef MAP_ENTRY_LEAD_REG
#undef MAP_ENTRY_USER_MANAGEMENT

    auto it = funcs.find( typeid( * req ) );

    if( it == funcs.end() )
    {
        return generic_perm_checker_->is_allowed( session_user_id, req );
    }

    return (this->*it->second)( session_user_id, req );
}

bool PermChecker::is_allowed_AddRideRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    //auto & r = dynamic_cast< const shopndrop_protocol::AddRideRequest &>( * rr );

    return true;
}

bool PermChecker::is_allowed_GetRideRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_protocol::GetRideRequest &>( * rr );

    return is_ride_id_valid( session_user_id, r.ride_id, true );
}

bool PermChecker::is_allowed_CancelRideRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_protocol::CancelRideRequest &>( * rr );

    return is_ride_id_valid( session_user_id, r.ride_id, true );
}

bool PermChecker::is_allowed_AddOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    //auto & r = dynamic_cast< const shopndrop_protocol::AddOrderRequest &>( * rr );

    return true;
}

bool PermChecker::is_allowed_CancelOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_protocol::CancelOrderRequest &>( * rr );

    return is_order_id_valid( session_user_id, r.order_id, true );
}

bool PermChecker::is_allowed_AcceptOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_protocol::AcceptOrderRequest &>( * rr );

    return is_order_id_valid( session_user_id, r.order_id, false );
}

bool PermChecker::is_allowed_DeclineOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_protocol::DeclineOrderRequest &>( * rr );

    return is_order_id_valid( session_user_id, r.order_id, false );
}

bool PermChecker::is_allowed_MarkDeliveredOrderRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_protocol::MarkDeliveredOrderRequest &>( * rr );

    return is_order_id_valid( session_user_id, r.order_id, false );
}

bool PermChecker::is_allowed_RateShopperRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_protocol::RateShopperRequest &>( * rr );

    return is_order_id_valid( session_user_id, r.order_id, true );
}

bool PermChecker::is_allowed_user_management_GetUserInfoRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const user_management_protocol::GetUserInfoRequest &>( * rr );

    return validate_user_id( session_user_id, r.user_id );
}

bool PermChecker::is_allowed_web_GetProductItemListRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    //auto & r = dynamic_cast< const shopndrop_web_protocol::GetProductItemListRequest &>( * rr );

    return true;
}

bool PermChecker::is_allowed_web_GetShoppingListWithTotalsRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_web_protocol::GetShoppingListWithTotalsRequest &>( * rr );

    return is_shopping_list_id_valid( session_user_id, r.shopping_list_id, true );
}

bool PermChecker::is_allowed_web_GetShoppingRequestInfoRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
    auto & r = dynamic_cast< const shopndrop_web_protocol::GetShoppingRequestInfoRequest &>( * rr );

    return is_ride_id_valid( session_user_id, r.ride_id, true );
}

bool PermChecker::is_allowed_web_GetDashScreenUserRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
//    auto & r = dynamic_cast< const shopndrop_web_protocol::GetDashScreenUserRequest &>( * rr );

    return true;
}

bool PermChecker::is_allowed_web_GetDashScreenShopperRequest( user_id_t session_user_id, const basic_parser::Object * rr )
{
//    auto & r = dynamic_cast< const shopndrop_web_protocol::GetDashScreenShopperRequest &>( * rr );

    return true;
}

bool PermChecker::is_allowed_lead_reg_RegisterUserRequest( user_id_t session_user_id, const basic_parser::Object * r )
{
    return true;
}

bool PermChecker::does_belong_to_user( user_id_t user_id, user_id_t session_user_id, bool should_belong_to_user )
{
    if( user_id == session_user_id )
        return should_belong_to_user ? true : false ;

    return should_belong_to_user ? false : true;
}

bool PermChecker::is_ride_id_valid( user_id_t session_user_id, uint32_t ride_id, bool should_belong_to_user )
{
    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    auto ride = order_db_->find_ride__unlocked( ride_id );

    if( ride == nullptr )
        return false;

    return does_belong_to_user( ride->get_attrib().user_id, session_user_id, should_belong_to_user );
}

bool PermChecker::is_order_id_valid( user_id_t session_user_id, uint32_t order_id, bool should_belong_to_user )
{
    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    auto order = order_db_->find_order__unlocked( order_id );

    if( order == nullptr )
        return false;

    return does_belong_to_user( order->get_attrib().user_id, session_user_id, should_belong_to_user );
}

bool PermChecker::is_shopping_list_id_valid( user_id_t session_user_id, uint32_t shopping_list_id, bool should_belong_to_user )
{
    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    auto shopping_list = order_db_->find_shopping_list__unlocked( shopping_list_id );

    if( shopping_list == nullptr )
        return false;

    //return does_belong_to_user( shopping_list->get_attrib().user_id, session_user_id, should_belong_to_user );
    return true;
}

bool PermChecker::validate_user_id( user_id_t session_user_id, uint32_t user_id )
{
    if( user_id == session_user_id )
        return true;

    return false;
}

} // namespace shopndrop
