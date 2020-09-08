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

// $Revision: 13757 $ $Date:: 2020-09-08 #$ $Author: serge $

#include "time_adjuster.h"              // self

#include "basic_objects/object_initializer.h" // basic_objects::init_LocalTime
#include "basic_objects/converter.h"    // basic_objects::to_val
#include "utils/utils_assert.h"         // ASSERT
#include "utils/boost_epoch.h"          // utils::from_epoch_sec
#include "utils/dummy_logger.h"         // dummy_log
#include "utils/copy_member.h"          // COPY_MEMBER
#include "epoch_now.h"                  // epoch_to_simple_string()

#define MODULENAME      "TimeAdjuster"

namespace shopndrop {

TimeAdjuster::TimeAdjuster():
    tzc_( nullptr )
{
}

bool TimeAdjuster::init( utils::TimeZoneConverter * tzc )
{
    ASSERT( tzc );

    tzc_    = tzc;

    return true;
}

basic_objects::TimeRange TimeAdjuster::to_utc( const basic_objects::LocalTimeRange & ltr, const std::string & timezone )
{
    basic_objects::TimeRange res;

    res.from    = to_utc( ltr.from, timezone );
    res.to      = to_utc( ltr.to, timezone );

    return res;
}

basic_objects::LocalTime * to_LocalTime( basic_objects::LocalTime * res, const boost::posix_time::ptime & ptime )
{
    auto hh     = ptime.time_of_day().hours();
    auto mm     = ptime.time_of_day().minutes();
    auto ss     = ptime.time_of_day().seconds();

    auto date   = ptime.date();

    auto year   = date.year();
    auto month  = date.month().as_number();
    auto day    = date.day();

    basic_objects::initialize( res, year, month, day, hh, mm, ss );

    return res;
}

basic_objects::LocalTime * TimeAdjuster::to_local( basic_objects::LocalTime * res, uint32_t t, const std::string & timezone )
{
    auto pt = utils::from_epoch_sec( t );

    auto pt_res = tzc_->utc_to_local( pt, timezone );

    to_LocalTime( res, pt_res );

    return res;
}

basic_objects::LocalTime TimeAdjuster::to_local( uint32_t t, const std::string & timezone )
{
    basic_objects::LocalTime res;

    to_local( & res, t, timezone );

    return res;
}

boost::posix_time::ptime to_ptime( const basic_objects::LocalTime & lt )
{
    boost::posix_time::ptime res(
            boost::gregorian::date( lt.year, lt.month, lt.day ),
            boost::posix_time::time_duration( lt.hh, lt.mm, lt.ss ));

    return res;
}

uint32_t TimeAdjuster::to_utc( const basic_objects::LocalTime & lt, const std::string & timezone )
{
    auto v = basic_objects::to_val( lt );

    if( v == 0 )
        return 0;

    auto pt = to_ptime( lt );

    auto pt_res = tzc_->local_to_utc( pt, timezone );

    auto res = utils::to_epoch( pt_res );

    return res;
}

} // namespace shopndrop
