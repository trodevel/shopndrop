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

#include "thunk.h"    // self

#include <boost/algorithm/string/predicate.hpp>     // boost::algorithm::starts_with

#include "utils/mutex_helper.h"          // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"          // dummy_log

#include "basic_parser/malformed_request.h"             // basic_parser::MalformedRequest
#include "generic_request/request_decoder.h"         // generic_request::decode_request
#include "generic_request/request.h"                 // generic_request::Request
#include "generic_request/parser.h"          // generic_request::Parser
#include "generic_protocol/csv_helper.h"        // generic_protocol::csv_helper
#include "generic_request/str_helper.h"              // generic_request::StrHelper
#include "generic_protocol/object_initializer.h"           // generic_protocol::create_ErrorResponse
#include "user_reg_protocol/parser.h"        // user_reg_protocol::parser
#include "user_reg_protocol/csv_helper.h"  // user_reg_protocol::csv_helper
#include "user_management_protocol/parser.h"        // user_management_protocol::parser
#include "user_management_protocol/csv_helper.h"  // user_management_protocol::csv_helper
#include "shopndrop_protocol/parser.h"          // shopndrop_web_protocol::parser
#include "shopndrop_protocol/csv_helper.h"    // shopndrop_protocol::csv_helper
#include "shopndrop_web_protocol/parser.h"          // shopndrop_web_protocol::parser
#include "shopndrop_web_protocol/csv_helper.h"    // shopndrop_web_protocol::CsvResponseEncoder

#include "handler_thunk.h"              // HandlerThunk
#include "perm_checker.h"               // PermChecker

#define MODULENAME      "shopndrop::Thunk"

namespace shopndrop {

Thunk::Thunk():
    perm_checker_( nullptr ),
    handler_thunk_( nullptr )
{
}

bool Thunk::init(
        PermChecker         * perm_checker,
        HandlerThunk        * hander,
        const std::string   & request_log,
        uint32_t            request_log_rotation_interval_min )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    if( !perm_checker || !hander )
        return false;

    perm_checker_   = perm_checker;
    handler_thunk_  = hander;

    logfile_.reset( new utils::LogfileTime( request_log, request_log_rotation_interval_min ) );

    return true;
}

const std::string Thunk::handle( restful_interface::method_type_e type, const std::string & path, const std::string & body, const std::string & origin )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    try
    {
        return handle__( type, path, body, origin );
    }
    catch( basic_parser::MalformedRequest & e )
    {
        dummy_log_info( MODULENAME, "malformed request '%s'", e.what() );

        std::unique_ptr<generic_protocol::BackwardMessage> resp( generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::INVALID_ARGUMENT, std::string( "malformed request: " ) + e.what() ) );

        auto res = shopndrop_protocol::csv_helper::to_csv( *resp );

        log_response( origin, res );

        return res;
    }
    catch( std::exception & e )
    {
        dummy_log_error( MODULENAME, "exception '%s'", e.what() );

        std::unique_ptr<generic_protocol::BackwardMessage> resp( generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::RUNTIME_ERROR, ( std::string( "cannot process request: " ) + e.what() ) ) );

        auto res = shopndrop_protocol::csv_helper::to_csv( *resp );

        log_response( origin, res );

        return res;
    }
}

std::string Thunk::handle__( restful_interface::method_type_e type, const std::string & path, const std::string & body, const std::string & origin )
{
    // private: no mutex lock

    std::string s = to_string( type, path, body );

    dummy_log_info( MODULENAME, "got request '%s'", s.c_str() );

    generic_request::Request r = generic_request::Parser::to_request( s );

    std::string parsed_s = generic_request::StrHelper::to_string( r );

    log_request( origin, parsed_s );

    dummy_log_debug( MODULENAME, "parsed = %s", parsed_s.c_str() );

    generic_request::Request rd = generic_request::decode_request( r );

    std::unique_ptr<basic_parser::Object> req( user_reg_protocol::parser::to_forward_message( rd ) );

    if( req != nullptr )
    {
        std::unique_ptr<const generic_protocol::BackwardMessage> resp( handle( req.get() ) );

        auto res = user_reg_protocol::csv_helper::to_csv( *resp );

        log_response( origin, res );

        return res;
    }

    req.reset( user_management_protocol::parser::to_forward_message( rd ) );

    if( req != nullptr )
    {
        std::unique_ptr<const generic_protocol::BackwardMessage> resp( handle( req.get() ) );

        auto res = user_management_protocol::csv_helper::to_csv( *resp );

        log_response( origin, res );

        return res;
    }

    req.reset( shopndrop_web_protocol::parser::to_forward_message( rd ) );

    if( req != nullptr )
    {
        std::unique_ptr<const generic_protocol::BackwardMessage> resp( handle( req.get() ) );

        std::string res = shopndrop_web_protocol::csv_helper::to_csv( *resp );

        log_response( origin, res );

        return res;
    }

    std::unique_ptr<const generic_protocol::BackwardMessage> resp( generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::INVALID_ARGUMENT, "cannot parse" ) );

    auto res = generic_protocol::csv_helper::to_csv( *resp );

    log_response( origin, res );

    return res;
}

generic_protocol::BackwardMessage* Thunk::handle( const basic_parser::Object * req )
{
    user_id_t session_user_id = 0;

    if( perm_checker_->is_authenticated( & session_user_id, req ) == false )
        return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::INVALID_OR_EXPIRED_SESSION, "invalid or expired session id" );

    if( perm_checker_->is_allowed( session_user_id, req ) )
        return handler_thunk_->handle( session_user_id, req );

    return generic_protocol::create_ErrorResponse( generic_protocol::ErrorResponse_type_e::NOT_PERMITTED, "no rights to execute request" );
}

std::string Thunk::to_string( restful_interface::method_type_e type, const std::string & path, const std::string & body )
{
    std::string res;

    if( boost::algorithm::starts_with( path, "/api/" ) )
    {
        res = "CMD=" + path.substr( 5 );
    }
    else
    {
        res = path;
    }

    res += "&" + body;

    return res;
}

void Thunk::log_request( const std::string & origin, const std::string & s ) const
{
    logfile_->write( "REQ " + origin + " " + s );
}

void Thunk::log_response( const std::string & origin, const std::string & s ) const
{
    logfile_->write( "RESP " + origin + " " + s );
}

} // namespace shopndrop
