/*

Order DB.

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

#include "db_order_db.h"                     // self

#include <fstream>                      // std::ofstream
#include <sstream>                      // std::ostringstream
#include <algorithm>                    // std::sort

#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"      // dummy_log
#include "utils/utils_assert.h"      // ASSERT
#include "utils/rename_and_backup.h" // utils::rename_and_backup

#include "shopndrop_protocol/object_initializer.h"    // shopndrop_protocol
#include "shopndrop_web_protocol/object_initializer.h"    // shopndrop_web_protocol
#include "generic_protocol/object_initializer.h"   // generic_protocol::create_ErrorResponse
#include "shopndrop_protocol/str_helper.h"   // shopndrop_protocol::str_helper

#include "epoch_now.h"                  // epoch_now_utc()
#include "utils/regex_match.h"          // utils::regex_match()
#include "utils/match_filter.h"         // utils::match_filter()
#include "log_wrap.h"                   // LOG_TRACE

#define MODULENAME      "OrderDB"

namespace shopndrop {

namespace db {

OrderDB::OrderDB():
    log_id_( 0 ),
    log_id_ride_( 0 ),
    log_id_order_( 0 ),
    user_man_( nullptr ),
    //obj_gen_( nullptr ),
    is_status_loaded_( false ),
    last_order_id_( 0 )
{
}

OrderDB::~OrderDB()
{
    if( is_status_loaded_ )
    {
        //save_status();
    }
    else
    {
        dummy_log_warn( MODULENAME, "status will not be saved" );
    }

    for( auto e : map_id_to_order_ )
    {
        delete e.second;
    }
}

bool OrderDB::init(
        const Config                        & config,
        unsigned int                        log_id,
        unsigned int                        log_id_ride,
        unsigned int                        log_id_order,
        user_manager::UserManager           * user_man/*,
        ObjGenerator                        * obj_gen*/ )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    config_             = config;
    log_id_             = log_id;
    log_id_ride_        = log_id_ride;
    log_id_order_       = log_id_order;
    user_man_           = user_man;

    is_status_loaded_   = true;

    return true;
}

id_t OrderDB::get_next_id()
{
    MUTEX_SCOPE_LOCK( mutex_ );

    return get_next_id__intern();
}

id_t OrderDB::get_next_id__intern()
{
    return ++last_order_id_;
}

bool OrderDB::find_user_id_by_order_id( user_id_t * user_id, id_t order_id ) const
{
    MUTEX_SCOPE_LOCK( mutex_ );

    auto it = map_id_to_order_.find( order_id );

    if( it == map_id_to_order_.end() )
    {
        return false;
    }

    * user_id = it->second->get_attrib().user_id;

    return true;
}

bool OrderDB::create_and_add_ride( id_t * ride_id, const shopndrop_protocol::RideSummary & ride_summary, uint32_t delivery_time, const std::string & shopper_name, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "create_and_add_ride: user_id %u", user_id );

    MUTEX_SCOPE_LOCK( mutex_ );

    auto id = get_next_id__intern();

    auto ride = new Ride( id, user_id, log_id_ride_, ride_summary, delivery_time, shopper_name );

    auto b = add_ride( id, ride, user_id, error_msg );

    if( b == false )
    {
        delete ride;
        return false;
    }

    * ride_id = id;

    return true;
}

bool OrderDB::add_ride( id_t id, Ride * ride, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "add_ride: ride_id %u, user_id %u", id, user_id );

    auto b = map_id_to_ride_.insert( std::make_pair( id, ride ) ).second;

    if( b == false )
    {
        * error_msg = "ride " + std::to_string( id ) + " already exists";
        return false;
    }

    return true;
}

bool OrderDB::create_and_add_order(
        id_t                * order_id,
        id_t                ride_id,
        const shopndrop_protocol::ShoppingList & shopping_list,
        const shopndrop_protocol::Address & delivery_address,
        double              sum,
        double              weight,
        double              earning,
        uint32_t            delivery_time,
        const std::string   & shopper_name,
        user_id_t           user_id,
        std::string         * error_msg )
{
    LOG_TRACE( "create_and_add_order: user_id %u, ride_id %u, shopper_name '%s'", user_id, ride_id, shopper_name );

    MUTEX_SCOPE_LOCK( mutex_ );

    auto shopping_list_id = get_next_id__intern();

    auto shopping_list_i    = new ShoppingList( shopping_list_id, user_id, shopping_list );

    auto b = add_shopping_list( shopping_list_id, shopping_list_i, user_id, error_msg );

    if( b == false )
    {
        return false;
    }

    auto id = get_next_id__intern();

    auto order    = new Order( id, user_id, log_id_order_, ride_id, shopping_list_id, delivery_address );

    init_cache( & order->get_cache(), sum, weight, earning, delivery_time, shopper_name );

    b = add_order( id, order, user_id, error_msg );

    if( b == false )
    {
        delete order;
        return false;
    }

    * order_id = id;

    return add_pending_order_to_ride( id, ride_id, user_id, error_msg );
}

bool OrderDB::cancel_ride( id_t ride_id, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "cancel_ride: ride_id %u, user_id %u", ride_id, user_id );

    MUTEX_SCOPE_LOCK( mutex_ );

    auto ride = find_ride__unlocked( ride_id );

    // do not rely on PermChecker, it only should check permissions, SKV 19517
    if( ride == nullptr )
    {
        * error_msg = "ride " + std::to_string( ride_id ) + " doesn't exist";
        return false;
    }

    auto & ride_raw = ride->get_ride();

    if( ride_raw.is_open == false )
    {
        * error_msg = "ride " + std::to_string( ride_id ) + " is already closed";

        return false;
    }

    if( ride_raw.accepted_order_id != 0 )
    {
        auto order = find_order__unlocked( ride_raw.accepted_order_id );

        assert( order );

        order->cancel_ride( error_msg );
    }

    // no need to cancel pending orders, as they have to be already declined, SKV 19520

    ride->cancel_ride();

    return true;
}

void OrderDB::accept_order_by_id( id_t order_id, bool should_accept )
{
    auto order = find_order__unlocked( order_id );

    assert( order );

    order->accept_order( should_accept );
}

bool OrderDB::accept_order( id_t order_id, user_id_t user_id, bool should_accept, std::string * error_msg )
{
    LOG_TRACE( "accept_order: order_id %u, user_id %u", order_id, user_id );

    MUTEX_SCOPE_LOCK( mutex_ );

    VectorRide rides;

    find_open_rides_with_unaccepted_orders_for_user( & rides, user_id );

    if( rides.empty() )
    {
        * error_msg = "no offered rides found for user " + std::to_string( user_id );

        return false;
    }

    for( auto & r : rides )
    {
        if( r->has_pending_order( order_id ) )
        {
            r->accept_order( order_id, should_accept );

            accept_order_by_id( order_id, should_accept );

            // decline other pending orders, SKV 19520

            if( should_accept )
            {
                std::vector<id_t> pending_order_ids;

                r->get_pending_order_ids( & pending_order_ids );

                LOG_TRACE( "accept_order: order_id %u, ride_id %u: decline other %u pending order(s)", order_id, r->get_attrib().id, pending_order_ids.size() );

                // decline other pending orders
                for( auto p : pending_order_ids )
                {
                    accept_order_by_id( p, false );
                }
            }

            return true;
        }
    }

    * error_msg = "no pending order " + std::to_string( order_id ) + " found for user " + std::to_string( user_id );

    return false;
}

bool OrderDB::mark_delivered_order( id_t order_id, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "mark_delivered_order: order_id %u, user_id %u", order_id, user_id );

    MUTEX_SCOPE_LOCK( mutex_ );

    auto ride = find_ride_with_accepted_order_for_user( order_id, user_id );

    if( ride == nullptr )
    {
        * error_msg = "no ride with accepted order id " + std::to_string( order_id ) + " found for user " + std::to_string( user_id );

        return false;
    }

    auto order = find_order__unlocked( order_id );

    assert( order );

    auto & raw_order = order->get_order();

    switch( raw_order.state )
    {
        case shopndrop_protocol::order_state_e::ACCEPTED_WAITING_DELIVERY:
            ride->mark_delivered_order();
            order->mark_delivered_order();
            return true;
            break;
        case shopndrop_protocol::order_state_e::DELIVERED_WAITING_FEEDBACK:
            * error_msg = "order " + std::to_string( order_id ) + " is already marked as delivered for user " + std::to_string( user_id );
            return false;
            break;
        case shopndrop_protocol::order_state_e::IDLE_WAITING_ACCEPTANCE:
        default:
            break;
    }

    * error_msg = "not possible for order in state " + shopndrop_protocol::str_helper::to_string( raw_order.state );

    return false;
}

bool OrderDB::rate_shopper( id_t order_id, uint32_t stars, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "rate_shopper: order_id %u, user_id %u", order_id, user_id );

    MUTEX_SCOPE_LOCK( mutex_ );

    auto order = find_order__unlocked( order_id );

    assert( order );        // must exist, PermChecker is responsible for filtering

    auto & raw_order = order->get_order();

    if( raw_order.is_open == false )
    {
        * error_msg = "order " + std::to_string( order_id ) + " is already closed";
        return false;
    }

    switch( raw_order.state )
    {
        case shopndrop_protocol::order_state_e::IDLE_WAITING_ACCEPTANCE:
            * error_msg = "order " + std::to_string( order_id ) + " is not accepted yet";
            return false;
            break;
        case shopndrop_protocol::order_state_e::ACCEPTED_WAITING_DELIVERY:
            * error_msg = "order " + std::to_string( order_id ) + " is not delivered yet";
            return false;
            break;
        case shopndrop_protocol::order_state_e::DELIVERED_WAITING_FEEDBACK:
            order->rate_shopper( stars );
            return true;
            break;
        default:
            * error_msg = "not possible for order in state " + shopndrop_protocol::str_helper::to_string( raw_order.state );
            break;
    }

    return false;
}

void OrderDB::get_info_for_shopper__unlocked( VectorRide * rides, VectorOrder * orders, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "get_info_for_shopper__unlocked: user_id %u", user_id );

    find_rides_for_user( rides, user_id, true );

    for( auto & r : * rides )
    {
        auto & raw_ride = r->get_ride();

        if( raw_ride.accepted_order_id != 0 )
        {
            auto order = find_order__unlocked( raw_ride.accepted_order_id );

            assert( order );

            orders->push_back( order );
        }
    }
}

void OrderDB::get_info_for_user__unlocked( VectorRide * rides, VectorOrder * orders, const shopndrop_protocol::GeoPosition & position, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "get_info_for_user__unlocked: position %s, user_id %u", shopndrop_protocol::str_helper::to_string( position ).c_str(), user_id );

    find_open_rides_with_unaccepted_orders_near_position( rides, position, user_id );

    find_orders_for_user( orders, user_id );
}

bool OrderDB::get_shopping_info_requests( std::vector<shopndrop_web_protocol::ShoppingRequestInfo> * requests, id_t ride_id, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "get_shopping_info_requests: ride_id %u, user_id %u", ride_id, user_id );

    MUTEX_SCOPE_LOCK( mutex_ );

    auto ride = find_ride__unlocked( ride_id );

    // do not rely on PermChecker, it only should check permissions, SKV 19517
    if( ride == nullptr )
    {
        * error_msg = "ride " + std::to_string( ride_id ) + " doesn't exist";
        return false;
    }

    std::vector<id_t> pending_order_ids;

    ride->get_pending_order_ids( & pending_order_ids );

    LOG_TRACE( "get_shopping_info_requests: ride_id %u: found %u pending orders", ride_id, pending_order_ids.size() );

    for( auto o : pending_order_ids )
    {
        auto order = find_order__unlocked( o );

        assert( order );

        auto & cache = order->get_cache();

        shopndrop_web_protocol::ShoppingRequestInfo sri;

        shopndrop_web_protocol::initialize( & sri, o, cache.sum, cache.earning, cache.weight, order->get_order().delivery_address );

        requests->push_back( sri );
    }

    return true;
}

void OrderDB::init_cache( db::Order::Cache * cache, double sum, double weight, double earning, uint32_t delivery_time, const std::string & shopper_name )
{
    cache->sum              = sum;
    cache->weight           = weight;
    cache->earning          = earning;
    cache->delivery_time    = delivery_time;
    cache->shopper_name     = shopper_name;
}

bool OrderDB::does_fit( const shopndrop_protocol::GeoPosition & positionA, const shopndrop_protocol::GeoPosition & positionB )
{
    double delta = (double)positionA.plz - (double)positionB.plz;

    double delta2 = delta * delta;

    double dist = sqrt( delta2 );

    if( dist < 1000 )
        return true;

    return false;
}

bool OrderDB::add_shopping_list( id_t id, ShoppingList * shopping_list, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "add_shopping_list__unlocked: user_id %u", user_id );

    auto b = map_id_to_shopping_list_.insert( std::make_pair( id, shopping_list ) ).second;

    if( b == false )
    {
        * error_msg = "shopping_list " + std::to_string( id ) + " already exists";
        return false;
    }

    LOG_DEBUG( "add_shopping_list__unlocked: user_id %u, shopping_list_id %u", user_id, id );

    return true;
}

bool OrderDB::add_order( id_t id, Order * order, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "add_order__unlocked: user_id %u", user_id );

    auto b = map_id_to_order_.insert( std::make_pair( id, order ) ).second;

    if( b == false )
    {
        * error_msg = "order " + std::to_string( id ) + " already exists";
        return false;
    }

    LOG_DEBUG( "add_order__unlocked: user_id %u, order_id %u", user_id, id );

    return true;
}

bool OrderDB::add_pending_order_to_ride( id_t order_id, id_t ride_id, user_id_t user_id, std::string * error_msg )
{
    LOG_TRACE( "add_pending_order_to_ride: order_id %u, ride_id %u, user_id %u", order_id, ride_id, user_id );

    auto ride = find_ride__unlocked( ride_id );

    assert( ride );

    if( user_id == ride->get_attrib().user_id )
    {
        * error_msg = "pending order ("  + std::to_string( ride_id ) + ") has the same user id ("  + std::to_string( user_id ) + ") as the ride " + std::to_string( ride_id );
        return false;
    }

    ride->add_pending_order( order_id );

    return true;
}

void OrderDB::find_rides_for_user( VectorRide * res, user_id_t user_id, bool should_user_id_match ) const
{
    for( auto & r : map_id_to_ride_ )
    {
        if( should_user_id_match )
        {
            if( r.second->get_attrib().user_id == user_id )
                res->push_back( r.second );
        }
        else
        {
            if( r.second->get_attrib().user_id != user_id )
                res->push_back( r.second );
        }
    }
}

void OrderDB::find_orders_for_user( VectorOrder * res, user_id_t user_id ) const
{
    for( auto & r : map_id_to_order_ )
    {
        if( r.second->get_attrib().user_id == user_id )
            res->push_back( r.second );
    }
}

void OrderDB::find_open_rides_with_unaccepted_orders_for_user( VectorRide * res, user_id_t user_id ) const
{
    VectorRide temp;
    find_rides_for_user( & temp, user_id, true );

    for( auto & r: temp )
    {
        auto & ride = r->get_ride();

        if( ride.is_open )
        {
            if( ride.accepted_order_id == 0 )
                res->push_back( r );
        }
    }
}

void OrderDB::find_open_rides_with_unaccepted_orders_near_position( VectorRide * res, const shopndrop_protocol::GeoPosition & position, user_id_t user_id ) const
{
    VectorRide temp;
    find_rides_for_user( & temp, user_id, false );

    for( auto & r: temp )
    {
        auto & ride = r->get_ride();

        if( ride.is_open )
        {
            if( ride.accepted_order_id == 0 )
            {
                if( does_fit( ride.summary.position, position ) )
                    res->push_back( r );
            }
        }
    }
}

Ride * OrderDB::find_ride_with_accepted_order_for_user( id_t order_id, user_id_t user_id ) const
{
    VectorRide temp;
    find_rides_for_user( & temp, user_id, true );

    for( auto & r: temp )
    {
        auto & ride = r->get_ride();

        if( ride.is_open )
        {
            if( ride.accepted_order_id == order_id )
                return r;
        }
    }

    return nullptr;
}

const Ride * OrderDB::find_ride__unlocked( id_t ride_id ) const
{
    auto it = map_id_to_ride_.find( ride_id );

    if( it == map_id_to_ride_.end() )
        return nullptr;

    return it->second;
}

Ride * OrderDB::find_ride__unlocked( id_t ride_id )
{
    auto it = map_id_to_ride_.find( ride_id );

    if( it == map_id_to_ride_.end() )
        return nullptr;

    return it->second;
}

Order * OrderDB::find_order__unlocked( id_t order_id )
{
    auto it = map_id_to_order_.find( order_id );

    if( it == map_id_to_order_.end() )
        return nullptr;

    return it->second;
}

const ShoppingList * OrderDB::find_shopping_list__unlocked( id_t shopping_list_id ) const
{
    auto it = map_id_to_shopping_list_.find( shopping_list_id );

    if( it == map_id_to_shopping_list_.end() )
        return nullptr;

    return it->second;
}

std::mutex      & OrderDB::get_mutex() const
{
    return mutex_;
}

bool OrderDB::get_user_timezone( std::string * timezone, user_id_t user_id ) const
{
    auto user = user_man_->find__unlocked( user_id );

    if( user.is_empty() )
        return false;

    * timezone = user.get_field( user_manager::User::TIMEZONE ).arg_s;

    return true;
}

uint32_t OrderDB::get_log_id() const
{
    return log_id_;
}

} // namespace db

} // namespace shopndrop
