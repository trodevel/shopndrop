/*

DB Task.

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

#ifndef SHOPNDROP_DB_RIDE_H
#define SHOPNDROP_DB_RIDE_H

#include "shopndrop_protocol/protocol.h"     // shopndrop_protocol::GeoPosition
#include "db_obj_attribution.h"     // ObjAttribution

#include <set>

namespace shopndrop {

namespace db {

class Ride
{

public:

    Ride(
            id_t                                    id,
            user_id_t                               user_id,
            uint32_t                                log_id,
            const shopndrop_protocol::RideSummary   & summary,
            uint32_t                                delivery_time,
            const std::string                       & shopper_name );

    const ObjAttribution & get_attrib() const;

    const shopndrop_protocol::Ride & get_ride() const;

    uint32_t get_delivery_time() const;
    const std::string & get_shopper_name() const;

    bool add_pending_order( id_t order_id );

    bool has_pending_order( id_t order_id ) const;

    void get_pending_order_ids( std::vector<id_t> * pending_order_ids ) const;

    void accept_order( id_t order_id, bool should_accept );
    void mark_delivered_order();
    void cancel_ride();

private:

    uint32_t get_log_id() const;
    uint32_t get_job_id() const;

private:

    uint32_t        log_id_;

    ObjAttribution  attrib_;

    shopndrop_protocol::Ride     ride_;

    uint32_t        delivery_time_;

    std::string     cache_shopper_name_;

    std::set<id_t>  pending_order_ids_;
};

} // namespace db

} // namespace shopndrop

#endif // SHOPNDROP_DB_RIDE_H
