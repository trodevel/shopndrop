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

// $Revision: 12061 $ $Date:: 2019-09-27 #$ $Author: serge $

#include "authenticator.h"              // self

#include "utils/dummy_logger.h"      // dummy_log
#include "utils/utils_assert.h"      // ASSERT

#include "password_hasher/login_to_id_converter.h"      // password_hasher::convert_password_to_hash

#define MODULENAME      "shopndrop::Authenticator"

namespace shopndrop {

Authenticator::Authenticator():
    user_man_( nullptr )
{
}

bool Authenticator::init(
        user_manager::UserManager * user_man )
{
    ASSERT( user_man );

    user_man_   = user_man;

    return true;
}

// interface session_manager::IAuthenticator
bool Authenticator::is_authenticated( uint32_t user_id, const std::string & password ) const
{
    auto u = user_man_->find__unlocked( user_id );

    if( u.is_empty() )
    {
        dummy_log_info( MODULENAME, "is_authenticated: unknown user id %u", user_id );

        return false;
    }

    auto password_hash = password_hasher::convert_password_to_hash( password );

    if( u.get_password_hash() == password_hash )
    {
        dummy_log_info( MODULENAME, "is_authenticated: authenticated user id %u", user_id );

        return true;
    }

    dummy_log_info( MODULENAME, "is_authenticated: invalid password hash for user id %u", user_id );

    return false;
}

} // namespace shopndrop
