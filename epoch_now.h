/*

Wrapper for now_utc()

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

// $Revision: 11047 $ $Date:: 2019-05-03 #$ $Author: serge $

#ifndef LIB_SHOPNDROP_EPOCH_NOW_UTC_H
#define LIB_SHOPNDROP_EPOCH_NOW_UTC_H

#include <cstdint>          // uint32_t
#include <string>           // std::string

namespace shopndrop {

uint32_t epoch_now_utc();
uint32_t epoch_now_local();
std::string epoch_to_simple_string( uint32_t ts );

std::string epoch_to_YYYYMMDD( uint32_t ts );
std::string epoch_to_HHMM( uint32_t ts );

} // namespace shopndrop

#endif // LIB_SHOPNDROP_EPOCH_NOW_UTC_H
