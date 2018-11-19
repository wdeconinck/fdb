/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "fdb5/tools/FDBVisitTool.h"
#include "fdb5/api/FDB.h"
#include "fdb5/api/helpers/FDBToolRequest.h"

#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"

using namespace eckit;
using namespace eckit::option;

namespace fdb5 {
namespace tools {


//----------------------------------------------------------------------------------------------------------------------

class FDBWipe : public FDBVisitTool {

public: // methods

    FDBWipe(int argc, char **argv) :
        FDBVisitTool(argc, argv, "class,expver,stream,date,time"),
        doit_(false) {

        options_.push_back(new SimpleOption<bool>("doit", "Delete the files (data and indexes)"));
    }

private: // methods

    virtual void init(const CmdArgs &args);
    virtual void execute(const CmdArgs& args);
    virtual void finish(const CmdArgs &args);

private: // members
    bool doit_;
};


void FDBWipe::init(const CmdArgs &args) {

    FDBVisitTool::init(args);

    args.get("doit", doit_);
}

void FDBWipe::execute(const CmdArgs& args) {

    FDB fdb;

    for (const FDBToolRequest& request : requests()) {

        auto listObject = fdb.wipe(request, doit_);

        size_t count = 0;
        WipeElement elem;
        while (listObject.next(elem)) {

            Log::info() << "FDB owner: " << elem.owner << std::endl
                        << std::endl;

            Log::info() << "Metadata files to deleted:" << std::endl;
            for (const auto& f : elem.metadataPaths) {
                Log::info() << "    " << f << std::endl;
            }
            Log::info() << std::endl;

            Log::info() << "Data files to delete: " << std::endl;
            if (elem.dataPaths.empty()) Log::info() << " - NONE -" << std::endl;
            for (const auto& f : elem.dataPaths) {
                Log::info() << "    " << f << std::endl;
            }
            Log::info() << std::endl;

            Log::info() << "Other files to delete: " << std::endl;
            if (elem.otherPaths.empty()) Log::info() << " - NONE -" << std::endl;
            for (const auto& f : elem.otherPaths) {
                Log::info() << "    " << f << std::endl;
            }
            Log::info() << std::endl;

            count++;
        }

        if (count == 0 && fail()) {
            std::stringstream ss;
            ss << "No FDB entries found for: " << request << std::endl;
            throw FDBToolException(ss.str());
        }
    }
}


void FDBWipe::finish(const CmdArgs&) {

    if (!doit_) {
        Log::info() << std::endl
                    << "Rerun command with --doit flag to delete unused files"
                    << std::endl
                    << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace tools
} // namespace fdb5

int main(int argc, char **argv) {
    fdb5::tools::FDBWipe app(argc, argv);
    return app.start();
}
