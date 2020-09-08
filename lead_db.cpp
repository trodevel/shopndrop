/*

Lead DB.

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

#include "lead_db.h"                    // self

#include <sstream>                      // std::ostringstream

#include "utils/mutex_helper.h"         // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"         // dummy_log

#include "user_reg_protocol/object_initializer.h" // user_reg_protocol
#include "user_reg_protocol/csv_helper.h" // user_reg_protocol

#define MODULENAME      "LeadDB"

namespace shopndrop {

LeadDB::LeadDB()
{
}

LeadDB::~LeadDB()
{
}

bool LeadDB::init(
        const Config                        & config )
{
    static const unsigned rotation_interval_min = 30 * 24 * 60; // 30 days

    MUTEX_SCOPE_LOCK( mutex_ );

    config_             = config;

    logfile_.reset( new utils::LogfileTime( config_.lead_reg_file, rotation_interval_min ) );

    return true;
}

generic_protocol::BackwardMessage * LeadDB::handle( user_id_t session_user_id, const user_reg_protocol::RegisterUserRequest & r )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    std::ostringstream os;

    os << session_user_id << ";";

    user_reg_protocol::csv_helper::write( os, r.lead );

    logfile_->write( os.str() );

    return user_reg_protocol::create_RegisterUserResponse();
}

} // namespace shopndrop
