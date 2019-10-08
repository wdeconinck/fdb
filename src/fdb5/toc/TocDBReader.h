/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   TocDBReader.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Mar 2016

#ifndef fdb5_TocDBReader_H
#define fdb5_TocDBReader_H

#include "fdb5/toc/TocDB.h"

namespace fdb5 {

//----------------------------------------------------------------------------------------------------------------------

/// DB that implements the FDB on POSIX filesystems

class TocDBReader : public TocDB {

public: // methods

    TocDBReader(const Key& key, const fdb5::Config& config);
    TocDBReader(const eckit::PathName& directory, const fdb5::Config& config);

    virtual ~TocDBReader();

    virtual std::vector<Index> indexes(bool sorted) const;

private: // methods

    void loadIndexesAndRemap();
    virtual bool selectIndex(const Key &key);
    virtual void deselectIndex();

    virtual bool open();
    virtual void close();

    virtual void axis(const std::string &keyword, eckit::StringSet &s) const;

    virtual eckit::DataHandle *retrieve(const Key &key) const;

    virtual void print( std::ostream &out ) const;

private: // members

    Key currentIndexKey_;

    // Indexes matching current key. If there is a key remapping for a mounted
    // SubToc, then this is stored alongside
    std::vector<std::pair<Index, Key>> matching_;

    // All indexes
    // If there is a key remapping for a mounted SubToc, this is stored alongside
    std::vector<std::pair<Index, Key>> indexes_;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace fdb5

#endif
