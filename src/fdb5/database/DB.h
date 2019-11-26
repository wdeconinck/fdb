/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   DB.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Mar 2016

#ifndef fdb5_DB_H
#define fdb5_DB_H

#include "eckit/types/Types.h"

#include "fdb5/config/Config.h"
#include "fdb5/database/Catalogue.h"
#include "fdb5/database/Key.h"
#include "fdb5/database/Store.h"

namespace eckit {
class DataHandle;
}

namespace fdb5 {

class Schema;
class DbStats;

enum class ControlAction : uint16_t;
class ControlIdentifiers;

//----------------------------------------------------------------------------------------------------------------------

class DB final : public eckit::OwnedLock {

public: // methods

    static std::unique_ptr<DB> buildReader(const Key &key, const fdb5::Config& config = fdb5::Config());
    static std::unique_ptr<DB> buildWriter(const Key &key, const fdb5::Config& config = fdb5::Config());
    static std::unique_ptr<DB> buildReader(const eckit::URI& uri, const fdb5::Config& config = fdb5::Config());
    static std::unique_ptr<DB> buildWriter(const eckit::URI& uri, const fdb5::Config& config = fdb5::Config());

    std::string dbType() const;

    const Key& key() const;
    const Schema& schema() const;

    void axis(const std::string &keyword, eckit::StringSet &s) const;
    eckit::DataHandle *retrieve(const Key &key) const;
    void archive(const Key &key, const void *data, eckit::Length length);

    bool open();
    void flush();
    void close();

    bool exists() const;

    void dump(std::ostream& out, bool simple=false, const eckit::Configuration& conf = eckit::LocalConfiguration()) const;

    bool selectIndex(const Key &key);
    void deselectIndex();

    virtual DbStats stats() const;

    // for ToC tools
    void hideContents();
    eckit::URI uri() const;
    void overlayDB(const DB& otherDB, const std::set<std::string>& variableKeys, bool unmount);
    void reconsolidateIndexesAndTocs();

    // Control access properties of the DB
    void control(const ControlAction& action, const ControlIdentifiers& identifiers) const;
    // TODO: *Locked to be implemented by a single enquire()
    virtual bool retrieveLocked() const;
    virtual bool archiveLocked() const;
    virtual bool listLocked() const;
    virtual bool wipeLocked() const;

protected: // methods

    friend std::ostream &operator<<(std::ostream &s, const DB &x);
    void print( std::ostream &out ) const;

private: // members

    DB(const Key &key, const fdb5::Config& config, bool read);
    DB(const eckit::URI &uri, const fdb5::Config& config, bool read);

    Store& store() const;

    bool buildByKey_ = false;
    std::unique_ptr<Catalogue> catalogue_;
    mutable std::unique_ptr<Store> store_ = nullptr;
};

//----------------------------------------------------------------------------------------------------------------------

class DBVisitor : private eckit::NonCopyable {
public:

    virtual ~DBVisitor();
    virtual void operator() (DB& db) = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace fdb5

#endif
