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

#ifndef SHOPNDROP_DB_OBJ_GENERATOR_H
#define SHOPNDROP_DB_OBJ_GENERATOR_H

#include "shopndrop_web_protocol/protocol.h"  // shopndrop_protocol::Ride
#include "db_ride.h"                    // db::Ride
#include "db_order.h"                   // db::Order
#include "time_adjuster.h"              // TimeAdjuster

namespace shopndrop {

class ObjGenerator
{
public:

    ObjGenerator();

    bool init(
            TimeAdjuster                            * time_adj );

    const shopndrop_protocol::Ride * to_Ride(
            shopndrop_protocol::Ride        * res,
            const db::Ride                  & ride,
            const std::string               & timezone );

    const shopndrop_web_protocol::DashScreenUser * to_DashScreenUser(
            shopndrop_web_protocol::DashScreenUser * res,
            const std::vector<db::Ride*>    & rides,
            const std::vector<db::Order*>   & orders,
            uint32_t                        current_time,
            const std::string               & timezone );

    const shopndrop_web_protocol::DashScreenShopper * to_DashScreenShopper(
            shopndrop_web_protocol::DashScreenShopper * res,
            const std::vector<db::Ride*>    & rides,
            const std::vector<db::Order*>   & orders,
            uint32_t                        current_time,
            const std::string               & timezone );

private:

    const shopndrop_protocol::RideSummary * to_RideSummary(
            shopndrop_protocol::RideSummary * res,
            const db::Ride                  & ride,
            const std::string               & timezone );

    const shopndrop_web_protocol::RideSummaryWithShopper * to_RideSummaryWithShopper(
            shopndrop_web_protocol::RideSummaryWithShopper * res,
            const db::Ride                  & ride,
            const std::string               & timezone );

    const shopndrop_web_protocol::RideWithId * to_RideWithId(
            shopndrop_web_protocol::RideWithId * res,
            const db::Ride                  & ride,
            const std::string               & timezone );

    const shopndrop_web_protocol::AcceptedOrderUser * to_AcceptedOrderUser(
            shopndrop_web_protocol::AcceptedOrderUser * res,
            const db::Order                 & order,
            const std::string               & timezone );

    const shopndrop_web_protocol::AcceptedOrderShopper * to_AcceptedOrderShopper(
            shopndrop_web_protocol::AcceptedOrderShopper * res,
            const db::Order                 & order,
            const std::string               & timezone );

private:

    TimeAdjuster                            * time_adj_;
};

} // namespace shopndrop

#endif // SHOPNDROP_DB_OBJ_GENERATOR_H
