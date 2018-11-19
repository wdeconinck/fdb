/*
 * (C) Copyright 2018- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   November 2018

#ifndef fdb5_api_visitor_ListVisitor_H
#define fdb5_api_visitor_ListVisitor_H

#include "fdb5/api/visitors/QueryVisitor.h"
#include "fdb5/api/helpers/ListIterator.h"

namespace fdb5 {
namespace api {
namespace visitor {

/// @note Helper classes for LocalFDB

//----------------------------------------------------------------------------------------------------------------------

struct ListVisitor : public QueryVisitor<ListElement> {

public:
    using QueryVisitor::QueryVisitor;

    void visitDatum(const Field& field, const Key& key) override {
        ASSERT(currentDatabase_);
        ASSERT(currentIndex_);

        queue_.emplace(ListElement({currentDatabase_->key(), currentIndex_->key(), key},
                                      field.sharedLocation()));
    }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace visitor
} // namespace api
} // namespace fdb5

#endif
