/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Mar 2016

#ifndef fdb5_RootManager_H
#define fdb5_RootManager_H

#include "eckit/utils/Regex.h"
#include "eckit/filesystem/PathName.h"

#include "fdb5/config/Config.h"
#include "fdb5/toc/FileSpace.h"

namespace metkit { class MarsRequest; }

namespace fdb5 {

class Key;
class FileSpace;
class DbPathNamer;

//----------------------------------------------------------------------------------------------------------------------

class RootManager  {

public: // methods

    RootManager(const Config& config=Config());

    /// Uniquely selects a directory where the Key will be put or already exists
    eckit::PathName directory(const Key &key, bool store = false);

    /// Lists the roots that can be visited given a DB key
    std::vector<eckit::PathName> allRoots(const Key& key);

    /// Lists the roots that can be visited given a DB key
    std::vector<eckit::PathName> visitableRoots(const Key& key);
    std::vector<eckit::PathName> visitableRoots(const std::set<Key>& keys);
    std::vector<eckit::PathName> visitableRoots(const metkit::MarsRequest& request);

    /// Lists the roots where a DB key would be able to be written
    std::vector<eckit::PathName> writableRoots(const Key& key);

    std::string dbPathName(const Key& key);

    std::vector<std::string> possibleDbPathNames(const Key& key, const char* missing);

private: // members

    const std::vector<FileSpace> spacesTable_;
    const std::vector<DbPathNamer>& dbPathNamers_;
    Config config_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace fdb5

#endif
