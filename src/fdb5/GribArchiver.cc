/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/DataHandle.h"
#include "eckit/log/Timer.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Progress.h"
#include "eckit/serialisation/HandleStream.h"
#include "eckit/io/MemoryHandle.h"
#include "grib_api.h"

#include "marslib/EmosFile.h"
#include "marslib/MarsRequest.h"

#include "fdb5/Key.h"
#include "fdb5/GribArchiver.h"

using namespace eckit;

namespace fdb5 {

//----------------------------------------------------------------------------------------------------------------------

GribArchiver::GribArchiver(bool completeTransfers):
    completeTransfers_(completeTransfers)
{
}

Length GribArchiver::archive(eckit::DataHandle& source)
{
    Timer timer("fdb::service::archive");

    EmosFile file(source);
    size_t len = 0;

    std::set<Key> seen;

    size_t count = 0;
    size_t total_size = 0;


    try{

        Key key;

        while( (len = gribToKey(file, key)) )
        {
            write(key, static_cast<const void *>(buffer()), len ); // finally archive it
            total_size += len;
            count++;
        }
    }
    catch(...) {
        if(completeTransfers_) {
            Log::error() << "Exception recieved. Completing transfer." << std::endl;
            // Consume rest of datahandle otherwise client retries for ever
            eckit::Buffer buffer(80*1024*1024);
            while( (len = file.readSome(buffer)) )
                /* empty */;
        }
        throw;
    }

    Log::info() << "FDB archive " << BigNum(count) << " fields,"
                << " size " << Bytes(total_size) << ","
                << " in " << Seconds(timer.elapsed())
                << " (" << Bytes(total_size, timer) << ")" <<  std::endl;

    return total_size;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace fdb5
