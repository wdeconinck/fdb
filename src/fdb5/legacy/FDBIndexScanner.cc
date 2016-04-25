/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ctype.h>

#include "eckit/io/StdFile.h"
#include "eckit/io/StdPipe.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/utils/Translator.h"
#include "eckit/types/Types.h"

//#include "pointdb/FieldInfoData.h"
//#include "pointdb/FieldInfoKey.h"
//#include "pointdb/GribFileSummary.h"
//#include "pointdb/IndexFile.h"
//#include "pointdb/IndexValidator.h"

#include "fdb5/legacy/FDBIndexScanner.h"
#include "fdb5/legacy/IndexCache.h"

using namespace eckit;

namespace fdb5 {
namespace legacy {

//----------------------------------------------------------------------------------------------------------------------

struct FDBInfo {

    eckit::PathName path_;
    PathName tmp_;
    GribFileSummary summary_;

    IndexFile btree_;

    FDBInfo(const PathName& path, const PathName& tmp) : path_(path), tmp_(tmp), btree_(tmp) {}
};

//-----------------------------------------------------------------------------

FDBIndexScanner::FDBIndexScanner(IndexCache &cache, const PathName &path):
    cache_(cache),
    path_(path)
{
}

FDBIndexScanner::~FDBIndexScanner()
{
}

void FDBIndexScanner::execute()
{
    Log::info() << "scanning " << path_ << std::endl;

    PathName lsfdb1( path_ + ".lsfdb" );
    PathName lsfdb2( path_ + "lsfdb" );

    if( lsfdb1.exists() )
    {
        StdFile f( lsfdb1, "r" );
        process(f);
        return;
    }

    if( lsfdb2.exists() )
    {
        StdFile f( lsfdb2, "r" );
        process(f);
        return;
    }

    // if all else fails, lets try to run the old tool 'lsfdb' (must be in path)
    {
        std::string cmd(std::string("lsfdb ") + path_.asString());
        StdPipe f(cmd, "r");
        process(f);
    }
}

void FDBIndexScanner::process(FILE* f)
{
    Tokenizer p(":");
    Tokenizer parse("=");

    std::string ignore;
    std::string prefix;
    std::string s;

    unsigned long long len;
    unsigned long long off;
    int count = 0;

    // /ma_fdb/:od:oper:g:0001:20120617::/:fc:0000::::::::.

    std::vector<std::string> c;

	//cout << c << endl;
	//cout << path_ << endl;
	//cout << path_.baseName() << endl;
	//cout << string(path_.baseName()) << endl;

    std::string t(path_.baseName());
	//cout << "[" << t << "]" << endl;

    p(t,c);

	//cout << c.size() << endl;
	//cout << c << endl;

	//ASSERT(c.size() >= 3);



    StringDict r;
    r["type"]   = c[0];
    r["time"]   = c[1];
    r["number"] = c[2];

    c.clear();

	//cout << c << endl;
	//cout << path_ << endl;
	//cout << path_.dirName() << endl;
	//cout << path_.dirName().baseName() << endl;
    p(path_.dirName().baseName(),c);

	//cout << c << endl;

	//ASSERT(c.size() >= 5);
    r["class"]    = c[0];
    r["type"]     = c[1];
    r["domain"]   = c[2];
    r["expver"]   = c[3];
    r["date"]     = c[4];



    std::map<std::string, FDBInfo*> info;

    FieldInfoKey min = FieldInfoKey::minimum();
    FieldInfoKey max = FieldInfoKey::maximum();

    try {

        char buf[10240];

        fgets(buf, sizeof(buf), f);

        while(fgets(buf, sizeof(buf), f))
        {
            std::map<std::string, std::string> m(r);

            std::string line(buf);
    //        cout << line << endl;
            std::istringstream in(line);
            // 409802 FDB;  4558 0:6:0;  levelist=27 levtype=m parameter=155 step=6  3281188 (61798740)
            in >> ignore;
            in >> ignore;
            in >> ignore;
            in >> prefix;
            prefix = prefix.substr(0, prefix.length()-1);

            std::map<std::string,FDBInfo*>::iterator i = info.find(prefix);
            if( i == info.end() )
            {
                std::string p = path_.dirName() + "/:" + prefix + path_.baseName();
                Log::info() << p << std::endl;

                p = p.substr(0, p.length()-1);

                if( cache_.ready(p) )
                {
                    Log::info() << p << " already indexed." << std::endl;
                    return;
                }

                PathName tmp = cache_.btree(p) + ".tmp";
                if(tmp.exists())
                    tmp.unlink();

                info[prefix] = new FDBInfo(p, tmp);

                i = info.find(prefix);
            }


            for(;;)
            {
                std::vector<std::string> v;
                in >> s;

                if(isdigit(s[0]))
                {
                    len = Translator<std::string,unsigned long long>()(s);
                    in >> s;
                    off = Translator<std::string,unsigned long long>()(s.substr(1,s.length()-2));
                    break;
                }

                parse(s,v);
                m[v[0]] = v[1];
            }

            FieldInfoKey key;
            FieldInfoData data(off, len);

            key.fill(m);

			//cout << key << endl;

            ASSERT(min < key);
            ASSERT(key < max);
            ASSERT(key == key);


            (*i).second->btree_.set(key,data);
            (*i).second->summary_.add(key, 0, len);

            if((count++ % 10000) == 0)
            {
                Log::info() << count << " " << key << std::endl;
            }

        }

        for(std::map<std::string,FDBInfo*>::iterator k = info.begin(); k != info.end(); ++k)
        {
            ASSERT((*k).second);

            PathName btree = cache_.btree((*k).second->path_);
            PathName::rename((*k).second->tmp_, btree);

            Log::info() << "Done " << btree << std::endl;

            (*k).second->btree_.flush();

            pool().push(new IndexValidator(cache_, (*k).second->path_));

            (*k).second->summary_.save(cache_.summary((*k).second->path_));

            cache_.ready((*k).second->path_, (*k).second->summary_);

            delete (*k).second;
            (*k).second = 0;
        }

    }
    catch(Exception& e)
    {

        Log::error() << "** " << e.what() << " Caught in " << Here()  <<  std::endl;
        Log::error() << "** Exception is handled" << std::endl;

        for(std::map<std::string,FDBInfo*>::iterator k = info.begin(); k != info.end(); ++k)
            delete (*k).second;

        throw;

    }

    Log::info() << "Index Done .... " << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace legacy
} // namespace fdb5
