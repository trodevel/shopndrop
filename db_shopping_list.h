/*

DB ShoppingList.

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

#ifndef SHOPNDROP__DB_SHOPPING_LIST_H
#define SHOPNDROP__DB_SHOPPING_LIST_H

#include "shopndrop_protocol/protocol.h"     // shopndrop_protocol::ShoppingList
#include "db_obj_attribution.h"     // ObjAttribution

namespace shopndrop {

namespace db {

class ShoppingList
{
public:
    ShoppingList( id_t id, user_id_t user_id, const shopndrop_protocol::ShoppingList & shopping_list );

    const shopndrop_protocol::ShoppingList & get_shopping_list() const;

private:

    ObjAttribution  attrib_;

    shopndrop_protocol::ShoppingList    shopping_list_;
};

} // namespace db

} // namespace shopndrop

#endif // SHOPNDROP__DB_SHOPPING_LIST_H
