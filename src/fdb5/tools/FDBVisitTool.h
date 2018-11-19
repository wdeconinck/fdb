/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   FDBInspect.h
/// @author Simon Smart
/// @date   November 2018

#ifndef fdb5_tools_FDBVisitTool_H
#define fdb5_tools_FDBVisitTool_H

#include "fdb5/tools/FDBTool.h"
#include "fdb5/api/helpers/FDBToolRequest.h"

namespace fdb5 {
namespace tools {

//----------------------------------------------------------------------------------------------------------------------

class FDBVisitTool : public FDBTool {

protected: // methods

    FDBVisitTool(int argc, char **argv, std::string minimunKeys = std::string());
    virtual ~FDBVisitTool() override;

    virtual void usage(const std::string& tool) const override;

    virtual void init(const eckit::option::CmdArgs &args) override;

    bool fail() const;

    std::vector<FDBToolRequest> requests() const;

private: // members

    // minimum set of keys needed to execute a query
    // these are used for safety to restrict the search space for the visitors
    std::vector<std::string> minimumKeys_;

    std::vector<std::string> requests_;

    // Fail on errors?
    bool fail_;

    bool all_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace tools
} // namespace fdb5

#endif
