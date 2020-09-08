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

#ifndef SHOPNDROP_DB_ORDER_H
#define SHOPNDROP_DB_ORDER_H

#include "shopndrop_protocol/protocol.h"     // shopndrop_protocol::Order
#include "db_obj_attribution.h"     // ObjAttribution

namespace shopndrop {

namespace db {

class Order
{
public:

    struct Cache
    {
        double      sum;
        double      earning;
        double      weight;
        uint32_t    delivery_time;
        std::string shopper_name;
    };

public:

    Order(
            id_t                                    id,
            user_id_t                               user_id,
            uint32_t                                log_id,
            id_t                                    ride_id,
            id_t                                    shopping_list_id,
            const shopndrop_protocol::Address       & delivery_address );

    const ObjAttribution & get_attrib() const;
    Cache & get_cache();
    const Cache & get_cache() const;

    const shopndrop_protocol::Order & get_order() const;

    bool cancel_ride( std::string * error_msg );
    bool accept_order( bool should_accept );
    void mark_delivered_order();
    void rate_shopper( uint32_t stars );

private:

    uint32_t get_log_id() const;
    uint32_t get_job_id() const;

    void next_state( shopndrop_protocol::order_state_e state );
    void close_order( shopndrop_protocol::order_resolution_e resolution );

private:

    uint32_t        log_id_;

    ObjAttribution  attrib_;

    shopndrop_protocol::Order       order_;

    Cache           cache_;
};

} // namespace db

} // namespace shopndrop

#endif // SHOPNDROP_DB_ORDER_H
