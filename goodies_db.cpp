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

#include "goodies_db.h"                 // self

#include <fstream>                      // std::ofstream
#include <sstream>                      // std::ostringstream
#include <algorithm>                    // std::sort

#include "utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"      // dummy_log
#include "utils/utils_assert.h"      // ASSERT


#define MODULENAME      "GoodiesDB"

namespace shopndrop {

GoodiesDB::GoodiesDB()
{
}

GoodiesDB::~GoodiesDB()
{
}

bool GoodiesDB::init(
        const std::string & db_file )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    db_file_    = db_file;

    if( load() == false )
        return false;

//    is_status_loaded_   = true;

    return true;
}

bool GoodiesDB::load()
{
    map_id_to_product_item_ =
    {
            { 13, { "Kaffee Auslese",           "Packung",  5.79, 0.5 } },
            { 14, { "Dallmayr Kaffee",          "Packung",  6.29, 0.5 } },
            { 15, { "Bärenmarke Kondensmilch",  "Dose",     0.89, 0.34 } },
            { 16, { "Jacobs Gold",              "Dose",     7.49, 0.2 } },
            { 17, { "Nescafe Classic",          "Dose",     4.39, 0.1 } },
            { 21, { "Brot",                     "Stück",    1.49, 0.5 } },
            { 22, { "Roggenvollkornbrot",       "Stück",    0.79, 0.5 } },
            { 23, { "Sonnenkernbrot",           "Stück",    1.39, 0.5 } },
            { 31, { "Apfel",                    "kg",       2.49, 1.5 } },
            { 32, { "Gurke",                    "Stück",    0.49, 0.2 } },
            { 33, { "Banane",                   "Stück",    0.89, 0.2 } },
            { 34, { "Paprika Rot",              "Packung",  1.59, 0.5 } },
            { 35, { "Karotten",                 "Schale",   1.69, 1.0 } },
            { 36, { "Tomaten",                  "Packung",  2.79, 0.5 } },
            { 37, { "Orangen",                  "Netz",     1.99, 2.0 } },
            { 41, { "Nutella",                  "Dose",     1.99, 0.3 } },
            { 42, { "Corny Riegel",             "Riegel",   1.59, 0.15 } },
            { 43, { "Haribo Color-Rado",        "Packung",  1.19, 0.36 } },
            { 44, { "Ritter Sport Voll-Nuss",   "Tafel",    1.49, 0.1 } },
            { 45, { "Ritter Sport Marzipan",    "Tafel",    1.19, 0.1 } },
            { 46, { "Milka Alpenmilch",         "Tafel",    0.99, 0.1 } },
            { 50, { "Spaghetti",                "Packung",  0.79, 0.5 } },
            { 60, { "Erdnüsse",                 "Packung",  1.79, 0.125 } },
            { 71, { "Milch",                    "Packung",  1.09, 1.0 } },
            { 72, { "Käse Gouda jung",          "Packung",  2.06, 0.4 } },
            { 73, { "Käseaufschnitt",           "Packung",  1.39, 0.25 } },
            { 74, { "Süssrahmbutter",           "Packung",  2.49, 0.25 } },
    };

    return true;
}

std::vector<shopndrop_web_protocol::ProductItemWithId> GoodiesDB::get_all_product_items() const
{
    std::vector<shopndrop_web_protocol::ProductItemWithId> res;

    for( auto & e : map_id_to_product_item_ )
    {
        res.push_back( { e.first, e.second } );
    }

    return res;
}

const shopndrop_protocol::ProductItem * GoodiesDB::find_product_item( id_t id ) const
{
    MUTEX_SCOPE_LOCK( mutex_ );

    return find_product_item__unlocked( id );
}

const shopndrop_protocol::ProductItem * GoodiesDB::find_product_item__unlocked( id_t id ) const
{
    auto it = map_id_to_product_item_.find( id );

    if( it == map_id_to_product_item_.end() )
        return nullptr;

    return & it->second;
}

bool GoodiesDB::validate( const shopndrop_protocol::ShoppingList & shopping_list, std::string * error_msg )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    for( auto & e : shopping_list.items )
    {
        auto product_item = find_product_item__unlocked( e.product_item_id );

        if( product_item == nullptr )
        {
            * error_msg = "invalid product item id " + std::to_string( e.product_item_id );
            return false;
        }
    }

    return true;
}

void GoodiesDB::calculate_price_weight( const shopndrop_protocol::ShoppingList & shopping_list, double * price, double * weight )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    double p = 0;
    double w = 0;

    for( auto & e : shopping_list.items )
    {
        auto product_item = find_product_item__unlocked( e.product_item_id );

        assert( product_item != nullptr );

        p += product_item->price * e.amount;
        w += product_item->weight * e.amount;
    }

    * price     = p;
    * weight    = w;
}


void GoodiesDB::convert_to_detailed( shopndrop_web_protocol::ShoppingListWithProduct * res, double * price, double * weight, const shopndrop_protocol::ShoppingList & shopping_list )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    double p = 0;
    double w = 0;

    for( auto & e : shopping_list.items )
    {
        auto product_item = find_product_item__unlocked( e.product_item_id );

        assert( product_item != nullptr );

        shopndrop_web_protocol::ShoppingItemWithProduct si;

        si.product_item     = * product_item;
        si.shopping_item    = e;

        res->items.push_back( si );

        p += product_item->price * e.amount;
        w += product_item->weight * e.amount;
    }

    * price    = p;
    * weight   = w;
}

} // namespace shopndrop
