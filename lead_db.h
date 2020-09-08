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

// $Revision: 13757 $ $Date:: 2020-09-08 #$ $Author: serge $

#ifndef SHOPNDROP__LEAD_DB_H
#define SHOPNDROP__LEAD_DB_H

#include <mutex>                    // std::mutex
#include <memory>                   // std::unique_ptr

#include "user_reg_protocol/protocol.h"    // shopndrop_web_protocol::GetRideStatusRequest

#include "utils/logfile_time.h"                     // utils::LogfileTime

#include "types.h"                  // user_id_t

namespace shopndrop_protocol
{
class SayFeedbackRequest_Map;
}

namespace generic_protocol
{
class BackwardMessage;
}

namespace shopndrop {

class LeadDB
{
public:
    struct Config
    {
        std::string lead_reg_file;
    };

public:

    LeadDB();
    ~LeadDB();

    bool init(
            const Config                        & config );

    generic_protocol::BackwardMessage * handle( user_id_t session_user_id, const user_reg_protocol::RegisterUserRequest & r );

private:

private:
    mutable std::mutex          mutex_;

    Config                      config_;

    std::unique_ptr<utils::LogfileTime>    logfile_;
};

} // namespace shopndrop

#endif // SHOPNDROP__LEAD_DB_H
