/*

Config Extractor.

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

// $Revision: 13938 $ $Date:: 2020-10-03 #$ $Author: serge $

#include "config_extractor.h"       // self

namespace shopndrop {

#define GET_VALUE( _v, _s, _toe )               cr.get_value( & cfg-> _v, _s, #_v, _toe )
#define GET_VALUE_CONVERTED( _v, _s, _toe )     cr.get_value_converted( & cfg-> _v, _s, #_v, _toe )

void init_logs( std::string * filename, uint32_t * rotation_interval_min, const config_reader::ConfigReader & cr )
{
    const std::string section( "logs" );

    cr.get_value( filename, section, "filename", true );
    cr.get_value_converted( rotation_interval_min, section, "rotation_interval_min", true );
}

void init_config( Core::Config * cfg, const config_reader::ConfigReader & cr )
{
    const std::string section( "core" );

    GET_VALUE( db_status_file  , section, true );
    GET_VALUE( request_log     , section, true );
    GET_VALUE_CONVERTED( request_log_rotation_interval_min, section, true );
    GET_VALUE( users_db_file, section, true );
    GET_VALUE( user_reg_email_credentials_file,         section, true );
    GET_VALUE( timezone_file   , section, true );

    GET_VALUE( goodies_db_file,             section, true );
}

void init_scheduler( uint32_t * granularity_ms, const config_reader::ConfigReader & cr )
{
    const std::string section( "scheduler" );

    cr.get_value_converted( granularity_ms  , section, "granularity_ms", true );
}

} // namespace shopndrop
