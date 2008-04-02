/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/source/susetags/MediaPatchesMetadataParser.cc
 *
*/
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include "zypp/base/Logger.h"
#include "zypp/base/Exception.h"
#include "zypp/base/PtrTypes.h"
#include "zypp/base/String.h"

#include "zypp/parser/tagfile/TagFileParser.h"
#include "zypp/source/susetags/MediaPatchesMetadataParser.h"
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
namespace source
{ /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
namespace susetags
{ /////////////////////////////////////////////////////////////////

/*
  File:  media
  Location  /media.N/ directory on media
  Content  two or more lines of ASCII as follows
  <vendor>
  <YYYYMMDDHHMMSS>
  [<media count>]
  [<media flags>]
  [<media names>]

  Currently defined flags:
   
  doublesided 
  media is double sided, YaST will ask for 'front side' for odd-numbered media and 'back side' for even-numbered media.
  The default is single-sided media.

  <media names> may define alternate strings to use when asking to insert a certain media.
   They are defined as <key><whitespace><value> pairs, separated by \n.
   
*/

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : Parser::parse
//	METHOD TYPE : void
//
void MediaPatchesMetadataParser::parse( const Pathname & file_r, MediaPatchesEntry &entry_r )
{
  std::ifstream file(file_r.asString().c_str());
  if (!file)
  {
    ZYPP_THROW(Exception("Can't read patches file "+file_r.asString()));
  }
  std::string buffer;
  // read vendor
  getline(file, buffer);

  regex rx("^([\\S]+)( (.*))?$");
  boost::smatch what;

  if (boost::regex_match(buffer, what, rx))
  {
    //dumpRegexpResults(what);
    entry_r.dir = Pathname(what[1]);
    entry_r.comment = what[3];
  }
  else
  {
    // throw exception?
  }

  while (file && !file.eof())
  {
    getline(file, buffer);
    rx = boost::regex("^(.+)-(.+)$");
    if (boost::regex_match(buffer, what, rx, boost::match_extra))
    {
      zypp::parser::tagfile::dumpRegexpResults(what);
      entry_r.products.insert(std::pair<std::string, std::string>(what[1],what[2]));
    }
    else
    {
      // throw exception?
    }

  }
}

/////////////////////////////////////////////////////////////////
} // namespace tagfile
///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
} // namespace parser
///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////