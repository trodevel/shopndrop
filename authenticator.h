/*

Authenticator.

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

#ifndef SHOPNDROP_AUTHENTICATOR_H
#define SHOPNDROP_AUTHENTICATOR_H

#include <mutex>            // std::mutex
#include <map>              // std::map
#include <cstdint>          // uint32_t

#include "session_manager/i_authenticator.h" // session_manager::IAuthenticator
#include "user_manager/user_manager.h"       // user_manager::UserManager

namespace shopndrop {

class Authenticator: public session_manager::IAuthenticator
{
public:

    Authenticator();

    bool init(
            user_manager::UserManager * user_man );

    // interface session_manager::IAuthenticator
    virtual bool is_authenticated( uint32_t user_id, const std::string & password ) const;

private:

    // Config
    user_manager::UserManager   * user_man_;
};

} // namespace shopndrop

#endif // SHOPNDROP_AUTHENTICATOR_H
