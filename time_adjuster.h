/*

TimeAdjuster.

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

// $Revision: 13531 $ $Date:: 2020-08-20 #$ $Author: serge $

#ifndef SHOPNDROP__TIME_ADJUSTER_H
#define SHOPNDROP__TIME_ADJUSTER_H

#include "basic_objects/protocol.h"    // basic_objects::LocalTime
#include "utils/boost_timezone.h"           // utils::TimeZoneConverter

namespace shopndrop {

class TimeAdjuster
{
public:

    TimeAdjuster();

    bool init( utils::TimeZoneConverter * tzc );

    uint32_t to_utc( const basic_objects::LocalTime & t, const std::string & timezone );

    basic_objects::TimeRange to_utc( const basic_objects::LocalTimeRange & ltr, const std::string & timezone );

    basic_objects::LocalTime to_local( uint32_t t, const std::string & timezone );

    basic_objects::LocalTime * to_local( basic_objects::LocalTime * res, uint32_t t, const std::string & timezone );

private:

private:

    utils::TimeZoneConverter    * tzc_;
};

} // namespace shopndrop

#endif // SHOPNDROP__TIME_ADJUSTER_H
