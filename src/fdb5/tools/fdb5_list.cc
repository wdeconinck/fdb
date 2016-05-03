/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/option/SimpleOption.h"
#include "eckit/option/CmdArgs.h"

#include "fdb5/tools/FDBTool.h"
#include "fdb5/database/Index.h"
#include "fdb5/rules/Schema.h"
#include "fdb5/toc/TocHandler.h"
#include "fdb5/toc/TocDB.h"

#include "fdb5/database/Field.h"

using namespace std;
using namespace eckit;
using namespace fdb5;

//----------------------------------------------------------------------------------------------------------------------

class ListVisitor : public EntryVisitor {
  public:
    ListVisitor(const Key &dbKey,
                const fdb5::Schema &schema,
                const eckit::option::CmdArgs &args) :
        dbKey_(dbKey),
        schema_(schema),
        args_(args) {
    }

  private:
    virtual void visit(const Index &index,
                       const std::string &indexFingerprint,
                       const std::string &fieldFingerprint,
                       const Field &field);

    const Key &dbKey_;
    const fdb5::Schema &schema_;
    const eckit::option::CmdArgs &args_;
};

void ListVisitor::visit(const Index &index,
                        const std::string &indexFingerprint,
                        const std::string &fieldFingerprint,
                        const Field &field) {

    Key key(fieldFingerprint, schema_.ruleFor(dbKey_, index.key()));

    std::cout << dbKey_ << index.key() << key;

    bool location = false;
    args_.get("location", location);

    if (location) {
        std::cout << " " << field.location();
    }

    std::cout << std::endl;

}

//----------------------------------------------------------------------------------------------------------------------

class FDBList : public FDBTool {

  public: // methods

    FDBList(int argc, char **argv) : FDBTool(argc, argv) {
        options_.push_back(new eckit::option::SimpleOption<bool>("location", "Also print the location of each field"));

    }

  private: // methods

    virtual void usage(const std::string &tool);
    virtual void process(const eckit::PathName &path, const option::CmdArgs &args);
};

void FDBList::usage(const std::string &tool) {

    eckit::Log::info() << std::endl
                       << "Usage: " << tool << " [--location] --match=k1=v1,k2=v2..." << std::endl
                       << "       " << tool << " [--location] [path1|request1] [path2|request2] ..." << std::endl
                       << "Example:" << std::endl
                       << tool << " --match=expver=0069 will list all databases matching the expver provided" << std::endl
                       << tool << " /tmp/fdb/od:0001:oper:20160428:1200:g will list this directory" << std::endl
                       << tool << " class=od,expver=0001,stream=oper,date=20160428,time=1200,domain=g will list the same directory"
                       << std::endl;
    FDBTool::usage(tool);
}

void FDBList::process(const eckit::PathName &path, const option::CmdArgs &args) {

    Log::info() << "Listing " << path << std::endl;

    fdb5::TocHandler handler(path);
    Key key = handler.databaseKey();
    Log::info() << "Database key " << key << std::endl;

    fdb5::Schema schema(path / "schema");

    std::vector<Index *> indexes = handler.loadIndexes();
    ListVisitor visitor(key, schema, args);

    for (std::vector<Index *>::const_iterator i = indexes.begin(); i != indexes.end(); ++i) {
        (*i)->entries(visitor);
    }

    handler.freeIndexes(indexes);
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
    FDBList app(argc, argv);
    return app.start();
}

