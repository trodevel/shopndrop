/*

db::ObjGenerator.

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

#include "db_obj_generator.h"           // self

#include "utils/utils_assert.h"      // ASSERT
#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK

#include "shopndrop_protocol/object_initializer.h"    // shopndrop_web_protocol

#define MODULENAME      "ObjGenerator"

namespace shopndrop {

ObjGenerator::ObjGenerator():
    time_adj_( nullptr )
{
}

bool ObjGenerator::init(
        TimeAdjuster                            * time_adj )
{
    ASSERT( time_adj );

    time_adj_           = time_adj;

    return true;
}

const shopndrop_protocol::Ride * ObjGenerator::to_Ride(
            shopndrop_protocol::Ride        * res,
            const db::Ride                  & ride,
            const std::string               & timezone )
{
    * res  = ride.get_ride();

    time_adj_->to_local( & res->summary.delivery_time, ride.get_delivery_time(), timezone );

    ride.get_pending_order_ids( & res->pending_order_ids );

    return res;
}

const shopndrop_web_protocol::DashScreenUser * ObjGenerator::to_DashScreenUser(
        shopndrop_web_protocol::DashScreenUser * res,
        const std::vector<db::Ride*>    & rides,
        const std::vector<db::Order*>   & orders,
        uint32_t                        current_time,
        const std::string               & timezone )
{
    time_adj_->to_local( & res->current_time, current_time, timezone );

    for( auto & r : rides )
    {
        shopndrop_web_protocol::RideSummaryWithShopper t;

        to_RideSummaryWithShopper( & t, * r, timezone );

        res->rides.push_back( t );
    }

    for( auto & r : orders )
    {
        shopndrop_web_protocol::AcceptedOrderUser t;

        to_AcceptedOrderUser( & t, * r, timezone );

        res->orders.push_back( t );
    }

    return res;
}

const shopndrop_web_protocol::DashScreenShopper * ObjGenerator::to_DashScreenShopper(
        shopndrop_web_protocol::DashScreenShopper * res,
        const std::vector<db::Ride*>    & rides,
        const std::vector<db::Order*>   & orders,
        uint32_t                        current_time,
        const std::string               & timezone )
{
    time_adj_->to_local( & res->current_time, current_time, timezone );

    for( auto & r : rides )
    {
        shopndrop_web_protocol::RideWithId t;

        to_RideWithId( & t, * r, timezone );

        res->rides.push_back( t );
    }

    for( auto & r : orders )
    {
        shopndrop_web_protocol::AcceptedOrderShopper t;

        to_AcceptedOrderShopper( & t, * r, timezone );

        res->orders.push_back( t );
    }

    return res;
}


const shopndrop_protocol::RideSummary * ObjGenerator::to_RideSummary(
        shopndrop_protocol::RideSummary * res,
        const db::Ride                  & ride,
        const std::string               & timezone )
{
    * res  = ride.get_ride().summary;

    time_adj_->to_local( & res->delivery_time, ride.get_delivery_time(), timezone );

    return res;
}

const shopndrop_web_protocol::RideSummaryWithShopper * ObjGenerator::to_RideSummaryWithShopper(
        shopndrop_web_protocol::RideSummaryWithShopper * res,
        const db::Ride                  & ride,
        const std::string               & timezone )
{
    res->ride_id        = ride.get_attrib().id;
    res->shopper_name   = ride.get_shopper_name();

    to_RideSummary( & res->ride, ride, timezone );

    return res;
}

const shopndrop_web_protocol::RideWithId * ObjGenerator::to_RideWithId(
        shopndrop_web_protocol::RideWithId * res,
        const db::Ride                  & ride,
        const std::string               & timezone )
{
    res->ride_id        = ride.get_attrib().id;

    to_Ride( & res->ride, ride, timezone );

    return res;
}

const shopndrop_web_protocol::AcceptedOrderUser * ObjGenerator::to_AcceptedOrderUser(
        shopndrop_web_protocol::AcceptedOrderUser * res,
        const db::Order                 & order,
        const std::string               & timezone )
{
    res->order_id       = order.get_attrib().id;

    auto & cache        = order.get_cache();

    time_adj_->to_local( & res->delivery_time, cache.delivery_time, timezone );

    res->order          = order.get_order();
    res->sum            = cache.sum;
    res->shopper_name   = cache.shopper_name;

    return res;
}


const shopndrop_web_protocol::AcceptedOrderShopper * ObjGenerator::to_AcceptedOrderShopper(
        shopndrop_web_protocol::AcceptedOrderShopper * res,
        const db::Order                 & order,
        const std::string               & timezone )
{
    auto & cache = order.get_cache();

    res->order_id       = order.get_attrib().id;
    time_adj_->to_local( & res->delivery_time, cache.delivery_time, timezone );
    res->order          = order.get_order();
    res->sum            = cache.sum;
    res->earning        = cache.earning;
    res->weight         = cache.weight;

    return res;
}

} // namespace shopndrop
