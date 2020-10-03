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

#include "session_manager/session_manager.h"    // session_manager::SessionManager
#include "config_reader/config_reader.h"     // config_reader::ConfigReader
#include "http_server_wrap/config.h"         // http_server_wrap::Server
#include "user_reg_email/config.h"          // user_reg_email::Config
#include "core.h"                               // shopndrop::Core::Config

namespace shopndrop {

void init_logs( std::string * filename, uint32_t * rotation_interval, const config_reader::ConfigReader & cr );
void init_config( Core::Config * cfg, const config_reader::ConfigReader & cr );
void init_scheduler( uint32_t * granularity_ms, const config_reader::ConfigReader & cr );

} // namespace shopndrop
