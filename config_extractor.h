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

// $Revision: 11057 $ $Date:: 2019-05-06 #$ $Author: serge $

#include "config_reader/config_reader.h"     // config_reader::ConfigReader
#include "http_server_wrap/config.h"         // http_server_wrap::Server
#include "session_manager/manager.h"         // session_manager::Manager
#include "core.h"                               // shopndrop::Core::Config

namespace shopndrop {

void init_logs( std::string * filename, uint32_t * rotation_interval, const config_reader::ConfigReader & cr );
void init_config( http_server_wrap::Config * cfg, const config_reader::ConfigReader & cr );
void init_config( Core::Config * cfg, const config_reader::ConfigReader & cr );
void init_scheduler( uint32_t * granularity_ms, const config_reader::ConfigReader & cr );
void init_config( session_manager::Manager::Config * cfg, const config_reader::ConfigReader & cr );
void init_config( LeadDB::Config * cfg, const config_reader::ConfigReader & cr );

} // namespace shopndrop
