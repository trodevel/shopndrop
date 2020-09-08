/*

DB Ride.

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

// $Revision: 11560 $ $Date:: 2019-05-21 #$ $Author: serge $

#include "db_ride.h"        // self
#include "log_wrap.h"       // LOGI

#include <cassert>

namespace shopndrop {

namespace db {

Ride::Ride(
        id_t                                    id,
        user_id_t                               user_id,
        uint32_t                                log_id,
        const shopndrop_protocol::RideSummary   & summary,
        uint32_t                                delivery_time,
        const std::string                       & shopper_name ):
        log_id_( log_id ),
        attrib_( id, user_id )

{
    LOGI_INFO( "created" );

    ride_.is_open           = true;
    ride_.summary           = summary;
    ride_.accepted_order_id = 0;
    ride_.resolution        = shopndrop_protocol::ride_resolution_e::UNDEF;

    delivery_time_          = delivery_time;
    cache_shopper_name_     = shopper_name;
}

const ObjAttribution & Ride::get_attrib() const
{
    return attrib_;
}

const shopndrop_protocol::Ride & Ride::get_ride() const
{
    return ride_;
}

uint32_t Ride::get_delivery_time() const
{
    return delivery_time_;
}

const std::string & Ride::get_shopper_name() const
{
    return cache_shopper_name_;
}

bool Ride::add_pending_order( id_t order_id )
{
    LOGI_DEBUG( "add_pending_order: order_id %u", order_id );

    pending_order_ids_.insert( order_id );

    LOGI_DEBUG( "number of pending orders: %u", pending_order_ids_.size() );

    return true;
}

bool Ride::has_pending_order( id_t order_id ) const
{
    return pending_order_ids_.count( order_id ) > 0;
}

void Ride::get_pending_order_ids( std::vector<id_t> * pending_order_ids ) const
{
    pending_order_ids->clear();

    for( auto & e : pending_order_ids_ )
    {
        pending_order_ids->push_back( e );
    }
}

void Ride::accept_order( id_t order_id, bool should_accept )
{
    LOGI_DEBUG( "accept_order: order_id %u, should_accept %u", order_id, (unsigned)(should_accept) );

    assert( ride_.is_open );
    assert( ride_.accepted_order_id == 0 );

    auto _n = pending_order_ids_.erase( order_id );

    assert( _n > 0 );

    if( should_accept )
    {
        ride_.accepted_order_id = order_id;
    }
}

void Ride::mark_delivered_order()
{
    LOGI_DEBUG( "mark_delivered_order: is_open_ %u, accepted_order_id_ %u", (unsigned)(ride_.is_open), ride_.accepted_order_id );

    assert( ride_.is_open );
    assert( ride_.accepted_order_id != 0 );

    ride_.is_open       = false;
    ride_.resolution    = shopndrop_protocol::ride_resolution_e::EXPIRED_OR_COMPLETED;
}

void Ride::cancel_ride()
{
    LOGI_DEBUG( "cancel_ride: is_open_ %u, accepted_order_id_ %u, num of pending orders %u", (unsigned)(ride_.is_open), ride_.accepted_order_id, ride_.pending_order_ids.size() );

    assert( ride_.is_open );

    ride_.is_open       = false;
    ride_.resolution    = shopndrop_protocol::ride_resolution_e::CANCELLED;
}

uint32_t Ride::get_log_id() const
{
    return log_id_;
}

uint32_t Ride::get_job_id() const
{
    return attrib_.id;
}

} // namespace db

} // namespace shopndrop

