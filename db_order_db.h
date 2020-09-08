/*

Job DB.

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

#ifndef SHOPNDROP__DB_ORDER_DB_H
#define SHOPNDROP__DB_ORDER_DB_H

#include <string>                   // std::string
#include <map>                      // std::map
#include <set>                      // std::set
#include <mutex>                    // std::mutex
#include <memory>                   // std::unique_ptr
#include <vector>                   // std::vector

#include "shopndrop_web_protocol/protocol.h" // shopndrop_web_protocol::GetRideStatusRequest
#include "user_manager/user_manager.h"               // user_manager::UserManager

#include "db_ride.h"                // Ride
#include "db_order.h"               // Order
#include "db_shopping_list.h"       // ShoppingList

namespace generic_protocol
{
class BackwardMessage;
}

namespace shopndrop {

namespace db {

class OrderDB
{
public:
    struct Config
    {
        std::string status_file;
    };

    typedef std::vector< db::Ride* >                VectorRide;
    typedef std::vector< db::Order* >               VectorOrder;

public:

    OrderDB();
    ~OrderDB();

    bool init(
            const Config                        & config,
            unsigned int                        log_id,
            unsigned int                        log_id_ride,
            unsigned int                        log_id_order,
            user_manager::UserManager           * user_man/*,
            ObjGenerator                        * obj_gen */ );

    id_t get_next_id();

    bool find_user_id_by_order_id( user_id_t * user_id, id_t order_id ) const;

    bool create_and_add_ride( id_t * ride_id, const shopndrop_protocol::RideSummary & ride_summary, uint32_t delivery_time, const std::string & shopper_name, user_id_t user_id, std::string * error_msg );
    bool create_and_add_order(
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
            std::string         * error_msg );

    bool cancel_ride( id_t ride_id, user_id_t user_id, std::string * error_msg );
    bool accept_order( id_t order_id, user_id_t user_id, bool should_accept, std::string * error_msg );
    bool mark_delivered_order( id_t order_id, user_id_t user_id, std::string * error_msg );
    bool rate_shopper( id_t order_id, uint32_t stars, user_id_t user_id, std::string * error_msg );

    void get_info_for_shopper__unlocked( VectorRide * rides, VectorOrder * orders, user_id_t user_id, std::string * error_msg );
    void get_info_for_user__unlocked( VectorRide * rides, VectorOrder * orders, const shopndrop_protocol::GeoPosition & position, user_id_t user_id, std::string * error_msg );

    bool get_shopping_info_requests( std::vector<shopndrop_web_protocol::ShoppingRequestInfo> * requests, id_t ride_id, user_id_t user_id, std::string * error_msg );

    const Ride * find_ride__unlocked( id_t ride_id ) const;
    Ride * find_ride__unlocked( id_t ride_id );
    Order * find_order__unlocked( id_t order_id );
    const ShoppingList * find_shopping_list__unlocked( id_t shopping_list_id ) const;

    std::mutex      & get_mutex() const;

private:

    typedef std::map< id_t, db::Ride* >             MapIdToRide;
    typedef std::map< id_t, db::Order* >            MapIdToOrder;
    typedef std::map< id_t, db::ShoppingList* >     MapIdToShoppingList;
    typedef std::map< user_id_t, std::set<id_t> >   MapUserIdToOrderIds;

private:

    id_t get_next_id__intern();

    bool add_ride( id_t ride_id, Ride * ride, user_id_t user_id, std::string * error_msg );
    bool add_shopping_list( id_t shopping_list_id, ShoppingList * shopping_list, user_id_t user_id, std::string * error_msg );
    bool add_order( id_t order_id, Order * order, user_id_t user_id, std::string * error_msg );

    void accept_order_by_id( id_t order_id, bool should_accept );

    bool add_pending_order_to_ride( id_t order_id, id_t ride_id, user_id_t user_id, std::string * error_msg );

    void find_rides_for_user( VectorRide * res, user_id_t user_id, bool should_user_id_match ) const;
    void find_orders_for_user( VectorOrder * res, user_id_t user_id ) const;
    void find_open_rides_with_unaccepted_orders_for_user( VectorRide * res, user_id_t user_id ) const;
    void find_open_rides_with_unaccepted_orders_near_position( VectorRide * res, const shopndrop_protocol::GeoPosition & position, user_id_t user_id ) const;
    Ride * find_ride_with_accepted_order_for_user( id_t order_id, user_id_t user_id ) const;

    static void init_cache( db::Order::Cache * cache, double sum, double weight, double earning, uint32_t delivery_time, const std::string & shopper_name );

    static bool does_fit( const shopndrop_protocol::GeoPosition & positionA, const shopndrop_protocol::GeoPosition & positionB );

    bool get_user_timezone( std::string * timezone, user_id_t user_id ) const;

    uint32_t get_log_id() const;

private:
    mutable std::mutex          mutex_;

    Config                      config_;

    unsigned int                log_id_;
    unsigned int                log_id_ride_;
    unsigned int                log_id_order_;

    user_manager::UserManager           * user_man_;

    bool                    is_status_loaded_;
    id_t                    last_order_id_;

    MapIdToRide             map_id_to_ride_;
    MapIdToOrder            map_id_to_order_;
    MapIdToShoppingList     map_id_to_shopping_list_;
    MapUserIdToOrderIds     map_user_id_to_order_id_;
};

} // namespace db

} // namespace shopndrop

#endif // SHOPNDROP__DB_ORDER_DB_H
