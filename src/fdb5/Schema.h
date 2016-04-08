/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Schema.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Mar 2016

#ifndef fdb5_Schema_H
#define fdb5_Schema_H

#include <string>
#include <map>

#include "eckit/memory/NonCopyable.h"

namespace fdb5 {

class KeywordHandler;

//----------------------------------------------------------------------------------------------------------------------

class Schema : public std::vector<std::string>,
               private eckit::NonCopyable {

public: // methods

    Schema();
    
    virtual ~Schema();

    virtual const KeywordHandler& lookupHandler(const std::string& keyword) const;

    friend std::ostream& operator<<(std::ostream& s, const Schema& x);

protected: // methods

    virtual void print( std::ostream& out ) const = 0;

private: // members

    typedef std::map<std::string, KeywordHandler*> HandlerMap;

    mutable HandlerMap handlers_;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace fdb5

#endif
