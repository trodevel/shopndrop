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

// $Revision: 13757 $ $Date:: 2020-09-08 #$ $Author: serge $

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

void init_config( http_server_wrap::Config * cfg, const config_reader::ConfigReader & cr )
{
    const std::string section( "http_server" );

    GET_VALUE( server_cert,         section, true );
    GET_VALUE( server_key,          section, true );
    cr.get_value_converted( & cfg->port,        section, "port", true );
    cr.get_value_converted( & cfg->max_threads, section, "max_threads", true );
    cr.get_value_converted( & cfg->max_clients, section, "max_clients", true );
}

void init_config( Core::Config * cfg, const config_reader::ConfigReader & cr )
{
    const std::string section( "core" );

    cr.get_value( & cfg->db_status_file  , section, "db_status_file", true );
    cr.get_value( & cfg->request_log     , section, "request_log", true );
    cr.get_value_converted( & cfg->request_log_rotation_interval_min, section, "request_log_rotation_interval_min", true );
    cr.get_value( & cfg->credentials_file, section, "credentials_file", true );
    cr.get_value( & cfg->timezone_file   , section, "timezone_file", true );

    GET_VALUE( goodies_db_file,             section, true );
}

void init_scheduler( uint32_t * granularity_ms, const config_reader::ConfigReader & cr )
{
    const std::string section( "scheduler" );

    cr.get_value_converted( granularity_ms  , section, "granularity_ms", true );
}

void init_config( session_manager::Manager::Config * cfg, const config_reader::ConfigReader & cr )
{
    const std::string section( "session_manager" );

    cr.get_value_converted( & cfg->expiration_time,         section, "expiration_time_min",     true );
    cr.get_value_converted( & cfg->max_sessions_per_user,   section, "max_sessions_per_user",   true );
    cr.get_value_converted( & cfg->postpone_expiration,     section, "postpone_expiration",     true );
}

void init_config( LeadDB::Config * cfg, const config_reader::ConfigReader & cr )
{
    const std::string section( "lead_db" );

    GET_VALUE( lead_reg_file,           section, true );
}

} // namespace shopndrop
