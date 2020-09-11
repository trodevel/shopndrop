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

// $Revision: 13792 $ $Date:: 2020-09-11 #$ $Author: serge $

#include "handler.h"            // self

#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"      // dummy_log
#include "utils/utils_assert.h"      // ASSERT

#include "generic_protocol/object_initializer.h"          // generic_protocol::create_ErrorResponse
#include "user_reg_protocol/protocol.h"    // user_reg_protocol::
#include "user_reg_protocol/object_initializer.h"         // user_reg_protocol::
#include "user_reg_protocol/str_helper.h"           // user_reg_protocol::str_helper
#include "user_management_protocol/protocol.h"  // user_management_protocol::
#include "user_management_protocol/object_initializer.h"  // user_management_protocol::
#include "shopndrop_protocol/object_initializer.h"        // shopndrop_protocol
#include "shopndrop_web_protocol/object_initializer.h"    // shopndrop_web_protocol
#include "generic_handler/handler.h"                // generic_handler::Handler

#include "db_obj_generator.h"           // ObjGenerator
#include "epoch_now.h"                  // epoch_now_utc(), epoch_to_simple_string()
#include "lead_db.h"                    // LeadDB

#define MODULENAME      "shopndrop::Handler"

namespace shopndrop {

Handler::Handler():
    log_id_( 0 ),
    user_man_( nullptr ),
    order_db_( nullptr ),
    lead_db_( nullptr ),
    tzc_( nullptr ),
    time_adj_( nullptr ),
    obj_gen_( nullptr ),
    goodies_db_( nullptr ),
    last_order_id_( 0 )
{
}


bool Handler::init(
        unsigned int                        log_id,
        user_manager::UserManager           * user_man,
        db::OrderDB                         * order_db,
        LeadDB                              * lead_db,
        utils::TimeZoneConverter            * tzc,
        TimeAdjuster                        * time_adj,
        ObjGenerator                        * obj_gen,
        GoodiesDB                           * goodies_db )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    ASSERT( tzc );

    if( !user_man )
        return false;

    log_id_             = log_id;
    user_man_           = user_man;
    order_db_           = order_db;
    lead_db_            = lead_db;
    tzc_                = tzc;
    time_adj_           = time_adj;
    obj_gen_            = obj_gen;
    goodies_db_         = goodies_db;

    return true;
}


bool Handler::is_inited__() const
{
    return true;
}

bool Handler::validate( std::string * error_msg, const shopndrop_protocol::RideSummary & r, const std::string & timezone ) const
{
    auto now = epoch_now_utc();

    auto delivery_time = time_adj_->to_utc( r.delivery_time, timezone );

    if( delivery_time <= now )
    {
        * error_msg = "delivery time lies in the past ( " + epoch_to_simple_string( delivery_time ) +
                " UTC < " + epoch_to_simple_string( now ) + " UTC )";

        return false;
    }

    return true;
}

bool Handler::validate( std::string * error_msg, uint32_t * delivery_time, user_id_t * shopper_id, const shopndrop_protocol::AddOrderRequest   & r ) const
{
    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    auto ride = order_db_->find_ride__unlocked( r.ride_id );

    if( ride == nullptr )
    {
        * error_msg = "ride id " + std::to_string( r.ride_id ) + "not found";
        return false;
    }

    * delivery_time     = ride->get_delivery_time();
    * shopper_id        = ride->get_attrib().user_id;

    if( goodies_db_->validate( r.shopping_list, error_msg ) == false )
    {
        return false;
    }

    return true;
}

bool Handler::get_user_timezone( std::string * timezone, user_id_t user_id )
{
    auto user = user_man_->find__unlocked( user_id );

    if( user.is_empty() )
        return false;

    * timezone = user.get_field( user_manager::User::TIMEZONE ).arg_s;

    return true;
}

double Handler::get_minimal_basket_size() const
{
    static const double MIN_BASKET_SIZE = 13.0; // TODO: make it configurable, SKV 19521

    return MIN_BASKET_SIZE;
}

bool Handler::is_minimal_basket_size_reached( double sum ) const
{
    auto min = get_minimal_basket_size();

    if( sum >= min )
        return true;

    return false;
}

double Handler::calculate_earning( double sum )
{
    double res = ( sum / 1.30 ) * .15;

    return res;
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::AddRideRequest  & r )
{
    // private: no mutex lock

    std::string     timezone;

    if( get_user_timezone( & timezone, session_user_id ) == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "cannot obtain user's timezone" );
    }

    std::string     error_msg;

    if( validate( & error_msg, r.ride, timezone ) == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    try
    {
        auto user = user_man_->find__unlocked( session_user_id );

        assert( user.is_empty() == false );

        std::string shopper_name = user.get_field( user_manager::User::FIRST_NAME ).arg_s + " " + user.get_field( user_manager::User::LAST_NAME ).arg_s;

        auto delivery_time  = time_adj_->to_utc( r.ride.delivery_time, timezone );

        id_t id = 0;
        std::string error_msg;

        auto b = order_db_->create_and_add_ride( & id, r.ride, delivery_time, shopper_name, session_user_id, & error_msg );

        if( b == false )
        {
            return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
        }

        return shopndrop_protocol::create_AddRideResponse( id );
    }
    catch( std::exception & e )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "cannot create task: " + std::string( e.what() ) + "." );
    }
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::GetRideRequest & r )
{
    std::string     timezone;

    if( get_user_timezone( & timezone, session_user_id ) == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "cannot obtain user's timezone" );
    }

    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    auto ride = order_db_->find_ride__unlocked( r.ride_id );

    if( ride == nullptr )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "ride not found" );
    }

    shopndrop_protocol::Ride res;

    obj_gen_->to_Ride( & res, * ride, timezone );

    return shopndrop_protocol::create_GetRideResponse( res );
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::CancelRideRequest & r )
{
    std::string error_msg;

    auto b = order_db_->cancel_ride( r.ride_id, session_user_id, & error_msg );

    if( b == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    return shopndrop_protocol::create_CancelRideResponse();
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const user_management_protocol::GetUserInfoRequest & r )
{
    // private: no mutex lock

    auto user = user_man_->find__unlocked( r.user_id );

    if( user.is_empty() )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "user id " + std::to_string( r.user_id ) + " not found" );
    }

    auto * res = user_management_protocol::create_GetUserInfoResponse(
        user.get_user_id(),
        //to_gender( user.gender ),
        user_management_protocol::gender_e::MALE,
        user.get_field( user_manager::User::FIRST_NAME ).arg_s,
        user.get_field( user_manager::User::LAST_NAME ).arg_s,
        user.get_field( user_manager::User::COMPANY_NAME ).arg_s,
        user.get_field( user_manager::User::EMAIL ).arg_s,
        "",
        user.get_field( user_manager::User::PHONE ).arg_s,
        "",
        user.get_field( user_manager::User::TIMEZONE ).arg_s );

    return res;
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::AddOrderRequest & r )
{
    std::string     error_msg;
    uint32_t        delivery_time;
    user_id_t       shopper_id;

    if( validate( & error_msg, & delivery_time, & shopper_id, r ) == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    auto shopper = user_man_->find__unlocked( shopper_id );

    if( shopper.is_empty() )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "shopper with id " + std::to_string( shopper_id ) + " not found" );
    }

    std::string shopper_name = shopper.get_field( user_manager::User::FIRST_NAME ).arg_s + " " + shopper.get_field( user_manager::User::LAST_NAME ).arg_s;

    try
    {
        double sum;
        double weight;

        goodies_db_->calculate_price_weight( r.shopping_list, & sum, & weight );

        if( is_minimal_basket_size_reached( sum ) == false )
        {
            return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "basket size is smaller than minimal size (" + std::to_string( sum ) + " < " + std::to_string( get_minimal_basket_size() ) + ")" );
        }

        auto earning = calculate_earning( sum );

        id_t order_id = 0;

        auto b = order_db_->create_and_add_order( & order_id, r.ride_id, r.shopping_list, r.delivery_address, sum, weight, earning, delivery_time, shopper_name, session_user_id, & error_msg );

        if( b == false )
        {
            return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "cannot create order: " + error_msg );
        }

        return shopndrop_protocol::create_AddOrderResponse( order_id );
    }
    catch( std::exception & e )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "cannot create order: " + std::string( e.what() ) + "." );
    }
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::CancelOrderRequest & r )
{
    return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "not implemented yet" );
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::AcceptOrderRequest & r )
{
    std::string error_msg;

    auto b = order_db_->accept_order( r.order_id, session_user_id, true, & error_msg );

    if( b == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    return shopndrop_protocol::create_AcceptOrderResponse();
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::DeclineOrderRequest & r )
{
    std::string error_msg;

    auto b = order_db_->accept_order( r.order_id, session_user_id, false, & error_msg );

    if( b == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    return shopndrop_protocol::create_DeclineOrderResponse();
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::MarkDeliveredOrderRequest & r )
{
    std::string error_msg;

    auto b = order_db_->mark_delivered_order( r.order_id, session_user_id, & error_msg );

    if( b == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    return shopndrop_protocol::create_MarkDeliveredOrderResponse();
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_protocol::RateShopperRequest & r )
{
    std::string error_msg;

    auto b = order_db_->rate_shopper( r.order_id, r.stars, session_user_id, & error_msg );

    if( b == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    return shopndrop_protocol::create_RateShopperResponse();
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_web_protocol::GetProductItemListRequest & r )
{
    auto pids = goodies_db_->get_all_product_items();

    auto res = shopndrop_web_protocol::create_GetProductItemListResponse( pids );

    return res;
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_web_protocol::GetShoppingRequestInfoRequest & r )
{
    std::vector<shopndrop_web_protocol::ShoppingRequestInfo> requests;

    std::string error_msg;

    auto b = order_db_->get_shopping_info_requests( & requests, r.ride_id, session_user_id, & error_msg );

    if( b == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, error_msg );
    }

    return shopndrop_web_protocol::create_GetShoppingRequestInfoResponse( requests );
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_web_protocol::GetShoppingListWithTotalsRequest & r )
{
    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    auto shopping_list = order_db_->find_shopping_list__unlocked( r.shopping_list_id );

    if( shopping_list == nullptr )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "shopping list not found" );
    }

    shopndrop_web_protocol::ShoppingListWithTotals slwt;

    goodies_db_->convert_to_detailed( & slwt.shopping_list, & slwt.price, & slwt.weight, shopping_list->get_shopping_list() );

    return shopndrop_web_protocol::create_GetShoppingListWithTotalsResponse( slwt );
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_web_protocol::GetDashScreenUserRequest & r )
{
    std::string     timezone;

    if( get_user_timezone( & timezone, session_user_id ) == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "cannot obtain user's timezone" );
    }

    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    db::OrderDB::VectorRide rides;
    db::OrderDB::VectorOrder orders;
    std::string error_msg;

    order_db_->get_info_for_user__unlocked( & rides, & orders, r.position, session_user_id, & error_msg );

    shopndrop_web_protocol::DashScreenUser dash_screen;

    obj_gen_->to_DashScreenUser( & dash_screen, rides, orders, epoch_now_utc(), timezone );

    return shopndrop_web_protocol::create_GetDashScreenUserResponse( dash_screen );
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const shopndrop_web_protocol::GetDashScreenShopperRequest & r )
{
    std::string     timezone;

    if( get_user_timezone( & timezone, session_user_id ) == false )
    {
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, "cannot obtain user's timezone" );
    }

    auto & mutex = order_db_->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    db::OrderDB::VectorRide rides;
    db::OrderDB::VectorOrder orders;
    std::string error_msg;

    order_db_->get_info_for_shopper__unlocked( & rides, & orders, session_user_id, & error_msg );

    shopndrop_web_protocol::DashScreenShopper dash_screen;

    obj_gen_->to_DashScreenShopper( & dash_screen, rides, orders, epoch_now_utc(), timezone );

    return shopndrop_web_protocol::create_GetDashScreenShopperResponse( dash_screen );
}

generic_protocol::BackwardMessage* Handler::handle( user_id_t session_user_id, const user_reg_protocol::RegisterUserRequest & r )
{
    dummy_log_info( log_id_, "registering new lead, user id %u, %s", session_user_id, user_reg_protocol::str_helper::to_string( r.lead ).c_str() );

    return lead_db_->handle( session_user_id, r );
}


} // namespace shopndrop
