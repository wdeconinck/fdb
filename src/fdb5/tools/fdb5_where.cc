/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/option/CmdArgs.h"
#include "fdb5/tools/FDBTool.h"
#include "fdb5/toc/TocDB.h"
#include "eckit/parser/StringTools.h"
#include "eckit/option/SimpleOption.h"

using namespace std;
using namespace eckit;
using namespace fdb5;

//----------------------------------------------------------------------------------------------------------------------

class FDBWhere : public FDBTool {
public: // methods

    FDBWhere(int argc, char **argv) : FDBTool(argc, argv) {
        options_.push_back(new eckit::option::SimpleOption<std::string>("pattern", "Provide a pattern to match 'class:stream:expver', e.g. --pattern=od:.*:0001"));

    }

private: // methods

    virtual void usage(const std::string &tool);
    virtual int numberOfPositionalArguments() const { return 0; }
    virtual void process(const eckit::PathName&, const eckit::option::CmdArgs& args);
    virtual void finish(const eckit::option::CmdArgs& args);


};

void FDBWhere::usage(const std::string &tool) {

    eckit::Log::info() << std::endl
                       << "Usage: " << tool << std::endl
                       << "       " << tool << "--pattern pattern" << std::endl
                       << "       " << tool << "--match k1=v1,k2=v2"  << std::endl
                       << "       " << tool << "path1|request1 [path2|request2] ..." << std::endl;
    FDBTool::usage(tool);
}

void FDBWhere::process(const eckit::PathName& path, const eckit::option::CmdArgs& args)  {
    std::cout << path << std::endl;
}

void FDBWhere::finish(const eckit::option::CmdArgs& args) {


    if (args.count() == 0) {
        std::string pattern = ".*";
        args.get("pattern", pattern);

        std::vector<eckit::PathName> roots = TocDB::roots(pattern);
        for (std::vector<eckit::PathName>::const_iterator i = roots.begin(); i != roots.end(); ++i) {
            std::cout << *i << std::endl;
        }
    }


}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
    FDBWhere app(argc, argv);
    return app.start();
}
