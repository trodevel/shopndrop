/*

Convenience wrappers for logger.

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

// $Revision: 11347 $ $Date:: 2019-05-13 #$ $Author: serge $

#ifndef SHOPNDROP__LOG_WRAP_H
#define SHOPNDROP__LOG_WRAP_H

#include "utils/dummy_logger.h"      // dummy_logi_

#define LOG_FATAL( _fmt, ... )      dummy_log_fatal( get_log_id(), _fmt, ##__VA_ARGS__ )
#define LOG_ERROR( _fmt, ... )      dummy_log_error( get_log_id(), _fmt, ##__VA_ARGS__ )
#define LOG_WARN( _fmt, ... )       dummy_log_warn( get_log_id(),  _fmt, ##__VA_ARGS__ )
#define LOG_INFO( _fmt, ... )       dummy_log_info( get_log_id(),  _fmt, ##__VA_ARGS__ )
#define LOG_DEBUG( _fmt, ... )      dummy_log_debug( get_log_id(), _fmt, ##__VA_ARGS__ )
#define LOG_TRACE( _fmt, ... )      dummy_log_trace( get_log_id(), _fmt, ##__VA_ARGS__ )

#define LOGI_FATAL( _fmt, ... )     dummy_logi_fatal( get_log_id(), get_job_id(), _fmt, ##__VA_ARGS__ )
#define LOGI_ERROR( _fmt, ... )     dummy_logi_error( get_log_id(), get_job_id(), _fmt, ##__VA_ARGS__ )
#define LOGI_WARN( _fmt, ... )      dummy_logi_warn( get_log_id(),  get_job_id(), _fmt, ##__VA_ARGS__ )
#define LOGI_INFO( _fmt, ... )      dummy_logi_info( get_log_id(),  get_job_id(), _fmt, ##__VA_ARGS__ )
#define LOGI_DEBUG( _fmt, ... )     dummy_logi_debug( get_log_id(), get_job_id(), _fmt, ##__VA_ARGS__ )
#define LOGI_TRACE( _fmt, ... )     dummy_logi_trace( get_log_id(), get_job_id(), _fmt, ##__VA_ARGS__ )

#endif // SHOPNDROP__LOG_WRAP_H
