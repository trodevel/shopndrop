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

// $Revision: 11048 $ $Date:: 2019-05-03 #$ $Author: serge $

#include "epoch_now.h"                  // self

#include "time_now.h"                   // now_utc()
#include "utils/boost_epoch.h"       // utils::from_epoch_sec

namespace shopndrop {

uint32_t epoch_now_utc()
{
    return utils::to_epoch( now_utc() );
}

uint32_t epoch_now_local()
{
    return utils::to_epoch( now_local() );
}

std::string epoch_to_simple_string( uint32_t ts )
{
    return boost::posix_time::to_simple_string( utils::from_epoch_sec( ts ) );
}

std::string epoch_to_YYYYMMDD( uint32_t ts )
{
    auto pt = utils::from_epoch_sec( ts );

    boost::gregorian::date date = pt.date();

    std::ostringstream os;

    os << std::setfill( '0' );

    os << std::setw( 4 ) << date.year() << std::setw( 2 ) << date.month().as_number() << std::setw( 2 ) << date.day();

    return os.str();
}

std::string epoch_to_HHMM( uint32_t ts )
{
    auto pt = utils::from_epoch_sec( ts );

    std::ostringstream os;

    os << std::setfill( '0' ) << std::setw( 2 ) << pt.time_of_day().hours()
            << std::setfill( '0' ) << std::setw( 2 ) << pt.time_of_day().minutes();

    return os.str();
}


} // namespace shopndrop
