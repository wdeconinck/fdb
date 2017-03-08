/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "fdb5/database/Retriever.h"

#include "eckit/config/Resource.h"
#include "eckit/log/Plural.h"

#include "fdb5/config/MasterConfig.h"
#include "fdb5/database/NotifyWind.h"
#include "fdb5/database/MultiRetrieveVisitor.h"
#include "fdb5/io/HandleGatherer.h"
#include "fdb5/rules/Schema.h"

#include "marslib/MarsTask.h"

namespace fdb5 {

//----------------------------------------------------------------------------------------------------------------------

static void purgeDB(Key& key, DB*& db) {
    Log::debug() << "Purging DB with key " << key << std::endl;
    delete db;
}

Retriever::Retriever() :
    schema_(MasterConfig::instance().schema()),
    databases_(Resource<size_t>("fdbMaxOpenDatabases", 16), &purgeDB) {
}

Retriever::~Retriever() {
}

eckit::DataHandle *Retriever::retrieve(const MarsTask& task,
                                       const Schema&,
                                       bool sorted,
                                       const fdb5::NotifyWind& notifyee) const {

	HandleGatherer result(sorted);
    MultiRetrieveVisitor visitor(notifyee, result, databases_);
    Log::info() << "Schema: " << schema_ << std::endl;
    schema_.expand(task.request(), visitor);

    eckit::Log::userInfo() << "Retrieving " << eckit::Plural(int(result.count()), "field") << std::endl;

    return result.dataHandle();
}

eckit::DataHandle *Retriever::retrieve(const MarsTask &task) const {

    class NotifyClient : public NotifyWind {
        const MarsTask &task_;
        virtual void notifyWind() const { task_.notifyWinds(); }

      public:
        NotifyClient(const MarsTask &task) : task_(task) {}
    };

    Log::error() << "In simple retrieve" << std::endl;

    NotifyClient wind(task);
    return retrieve(task, wind);
}

eckit::DataHandle *Retriever::retrieve(const MarsTask &task, const NotifyWind& notifyee) const {

    bool sorted = false;
    std::vector<std::string> sort;
    task.request().getValues("optimise", sort);

    Log::error() << "Sorted? " << sorted << std::endl;

    if (sort.size() == 1 && sort[0] == "on") {
        sorted = true;
        eckit::Log::userInfo() << "Using optimise" << std::endl;
    }

    // TODO: this logic does not work if a retrieval spans several
    // databases with different schemas. Another SchemaHasChanged will be thrown.
    try {

        return retrieve(task, schema_, sorted, notifyee);

    } catch (SchemaHasChanged &e) {

        eckit::Log::error() << e.what() << std::endl;
        eckit::Log::error() << "Trying with old schema: " << e.path() << std::endl;

        return retrieve(task, Schema(e.path()), sorted, notifyee);
    }

}

void Retriever::print(std::ostream &out) const {
    out << "Retriever[]";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace fdb5
