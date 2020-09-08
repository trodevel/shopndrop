/*

DB Order.

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

#include "db_order.h"   // self

#include <cassert>

#include "log_wrap.h"                       // LOGI
#include "shopndrop_protocol/str_helper.h"  // StrHelper

namespace shopndrop {

namespace db {


Order::Order(
        id_t                                    id,
        user_id_t                               user_id,
        uint32_t                                log_id,
        id_t                                    ride_id,
        id_t                                    shopping_list_id,
        const shopndrop_protocol::Address       & delivery_address ):
        log_id_( log_id ),
        attrib_( id, user_id )
{
    LOGI_INFO( "created: ride_id %u, shopping_list_id %u", ride_id, shopping_list_id );

    order_.is_open          = true;
    order_.ride_id          = ride_id;
    order_.shopping_list_id = shopping_list_id;
    order_.delivery_address = delivery_address;
    order_.state            = shopndrop_protocol::order_state_e::IDLE_WAITING_ACCEPTANCE;
    order_.resolution       = shopndrop_protocol::order_resolution_e::UNDEF;
}

const ObjAttribution & Order::get_attrib() const
{
    return attrib_;
}

Order::Cache & Order::get_cache()
{
    return cache_;
}

const Order::Cache & Order::get_cache() const
{
    return cache_;
}

const shopndrop_protocol::Order & Order::get_order() const
{
    return order_;
}

bool Order::cancel_ride( std::string * error_msg )
{
    LOGI_DEBUG( "cancel_ride: " );

    if( order_.is_open == false )
    {
        * error_msg = "cannot cancel ride, because order " + std::to_string( attrib_.id ) + " is already closed";
        return false;
    }

    switch( order_.state )
    {
        case shopndrop_protocol::order_state_e::IDLE_WAITING_ACCEPTANCE:
            close_order( shopndrop_protocol::order_resolution_e::RIDE_CANCELLED );
            return true;
            break;
        case shopndrop_protocol::order_state_e::ACCEPTED_WAITING_DELIVERY:
            close_order( shopndrop_protocol::order_resolution_e::RIDE_CANCELLED );
            return true;
            break;
        case shopndrop_protocol::order_state_e::DELIVERED_WAITING_FEEDBACK:
            * error_msg = "cannot cancel ride, because accepted order " + std::to_string( attrib_.id ) + " was already delivered";
            return false;
            break;
        default:
            break;
    }

    * error_msg = "unexpected in state " + shopndrop_protocol::str_helper::to_string( order_.state );

    return false;
}

bool Order::accept_order( bool should_accept )
{
    LOGI_DEBUG( "accept_order: %u", (unsigned)should_accept );

    assert( order_.is_open );
    assert( order_.state == shopndrop_protocol::order_state_e::IDLE_WAITING_ACCEPTANCE );

    if( should_accept )
    {
        next_state( shopndrop_protocol::order_state_e::ACCEPTED_WAITING_DELIVERY );
    }
    else
    {
        close_order( shopndrop_protocol::order_resolution_e::DECLINED_BY_SHOPPER );
    }

    return true;
}

void Order::mark_delivered_order()
{
    LOGI_DEBUG( "mark_delivered_order: is_open_ %u", (unsigned)(order_.is_open) );

    assert( order_.is_open );
    assert( order_.state == shopndrop_protocol::order_state_e::ACCEPTED_WAITING_DELIVERY );

    next_state( shopndrop_protocol::order_state_e::DELIVERED_WAITING_FEEDBACK );
}

void Order::rate_shopper( uint32_t stars )
{
    LOGI_DEBUG( "rate_shopper: stars %u, is_open_ %u", stars, (unsigned)(order_.is_open) );

    assert( order_.is_open );
    assert( order_.state == shopndrop_protocol::order_state_e::DELIVERED_WAITING_FEEDBACK );

    close_order( shopndrop_protocol::order_resolution_e::DELIVERED );
}

uint32_t Order::get_log_id() const
{
    return log_id_;
}

uint32_t Order::get_job_id() const
{
    return attrib_.id;
}

void Order::next_state( shopndrop_protocol::order_state_e state )
{
    LOGI_DEBUG( "switched state %s --> %s", shopndrop_protocol::str_helper::to_string( order_.state ).c_str(), shopndrop_protocol::str_helper::to_string( state ).c_str() );

    order_.state = state;
}

void Order::close_order( shopndrop_protocol::order_resolution_e resolution )
{
    LOGI_INFO( "closed: resolution %s", shopndrop_protocol::str_helper::to_string( resolution ).c_str() );

    assert( order_.is_open == true );

    order_.is_open      = false;
    order_.resolution   = resolution;
}


} // namespace db

} // namespace shopndrop

