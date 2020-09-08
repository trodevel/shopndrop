/*

Goodies DB.

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

#ifndef SHOPNDROP__GOODIES_DB_H
#define SHOPNDROP__GOODIES_DB_H

#include <string>                   // std::string
#include <map>                      // std::map
#include <set>                      // std::set
#include <mutex>                    // std::mutex
#include <memory>                   // std::unique_ptr
#include <vector>                   // std::vector

#include "shopndrop_web_protocol/protocol.h" // shopndrop_protocol::ProductItem

namespace shopndrop {

class GoodiesDB
{
public:
    struct Config
    {
        std::string status_file;
    };

public:

    GoodiesDB();
    ~GoodiesDB();

    bool init(
            const std::string & db_file );

    std::vector<shopndrop_web_protocol::ProductItemWithId> get_all_product_items() const;

    const shopndrop_protocol::ProductItem * find_product_item( id_t id ) const;
    const shopndrop_protocol::ProductItem * find_product_item__unlocked( id_t id ) const;

    bool validate( const shopndrop_protocol::ShoppingList & shopping_list, std::string * error_msg );

    void calculate_price_weight( const shopndrop_protocol::ShoppingList & shopping_list, double * price, double * weight );

    void convert_to_detailed( shopndrop_web_protocol::ShoppingListWithProduct * res, double * price, double * weight, const shopndrop_protocol::ShoppingList & shopping_list );

private:

    bool load();

private:

    typedef std::map< id_t, shopndrop_protocol::ProductItem >     MapIdToProductItem;

private:
    mutable std::mutex          mutex_;

    std::string                 db_file_;

    MapIdToProductItem          map_id_to_product_item_;
};

} // namespace shopndrop

#endif // SHOPNDROP__GOODIES_DB_H
