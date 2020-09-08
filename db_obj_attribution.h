/*

DB ObjAttribution.

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

#ifndef SHOPNDROP__DB_OBJ_ATTRIBUTION_H
#define SHOPNDROP__DB_OBJ_ATTRIBUTION_H

#include "shopndrop_protocol/protocol.h"     // shopndrop_protocol::Order
#include "types.h"                  // id_t
#include "epoch_now.h"              // epoch_now_utc()

namespace shopndrop {

namespace db {

struct ObjAttribution
{
    ObjAttribution( id_t id, user_id_t user_id ):
        id( id ),
        user_id( user_id )

    {
        creation_time   = epoch_now_utc();
    }

    id_t            id;
    user_id_t       user_id;
    uint32_t        creation_time;
};

} // namespace db

} // namespace shopndrop

#endif // SHOPNDROP__DB_OBJ_ATTRIBUTION_H
