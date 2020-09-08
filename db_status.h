/*

Job DB - Status.

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

// $Revision: 11045 $ $Date:: 2019-05-03 #$ $Author: serge $

#ifndef SHOPNDROP_DB_STATUS_H
#define SHOPNDROP_DB_STATUS_H

#include <vector>                   // std::vector

#include "db_task.h"                // Job

namespace shopndrop {

namespace db {

struct Status
{
    typedef std::vector< db::Task * > VectTask;

    job_id_t    last_task_id;
    VectTask    tasks;
};

} // namespace db

} // namespace shopndrop

#endif // SHOPNDROP_DB_STATUS_H
