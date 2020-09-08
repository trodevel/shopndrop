/*

Wrapper for boost universal_time(), local_time()

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

#ifndef LIB_SHOPNDROP_TIME_NOW_H
#define LIB_SHOPNDROP_TIME_NOW_H

#include <boost/date_time/posix_time/posix_time.hpp>    // boost::posix_time::ptime

namespace shopndrop {

inline boost::posix_time::ptime now_utc()
{
    return boost::posix_time::second_clock::universal_time();
}

inline boost::posix_time::ptime now_local()
{
    return boost::posix_time::second_clock::local_time();
}


} // namespace shopndrop

#endif // LIB_SHOPNDROP_TIME_NOW_H
