/*

HandlerThunk of server requests.

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

// $Revision: 13792 $ $Date:: 2020-09-11 #$ $Author: serge $


#ifndef THUNK_H
#define THUNK_H

#include <mutex>                // std::mutex


#include "utils/logfile_time.h"                  // utils::LogfileTime
#include "restful_interface/i_handler.h"         // restful_interface::IHandler
#include "generic_protocol/protocol.h"   // generic_protocol::ForwardMessage

namespace shopndrop {

class PermChecker;
class HandlerThunk;

class Thunk: public virtual restful_interface::IHandler
{
public:
    Thunk();

    bool init(
            PermChecker         * perm_checker,
            HandlerThunk             * hander,
            const std::string   & request_log,
            uint32_t            request_log_rotation_interval_min );

    virtual const std::string handle( restful_interface::method_type_e type, const std::string & path, const std::string & body, const std::string & origin ) override;

private:
    std::string handle__( restful_interface::method_type_e type, const std::string & path, const std::string & body, const std::string & origin );

    generic_protocol::BackwardMessage* handle( const basic_parser::Object * req );

    static std::string to_string( restful_interface::method_type_e type, const std::string & path, const std::string & body );
    void log_request( const std::string & origin, const std::string & s ) const;
    void log_response( const std::string & origin, const std::string & s ) const;

private:
    mutable std::mutex          mutex_;

    PermChecker                 * perm_checker_;
    HandlerThunk                * handler_thunk_;

    std::unique_ptr<utils::LogfileTime>    logfile_;
};

} // namespace shopndrop

#endif // THUNK_H
