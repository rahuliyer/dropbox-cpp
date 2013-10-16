/*
* Copyright (c) 2013 Rahul Iyer
* All rights reserved.
* 
* Redistribution and use in source and binary forms are permitted provided that
* the above copyright notice and this paragraph are duplicated in all such forms
* and that any documentation, advertising materials, and other materials related
* to such distribution and use acknowledge that the software was developed by 
* Rahul Iyer.  The name of Rahul Iyer may not be used to endorse or promote 
* products derived from this software without specific prior written permission.
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef __DROPBOX_METADATA_TYPE_H__
#define __DROPBOX_METADATA_TYPE_H__

#include "DropboxException.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <sys/types.h>
#include <string>
#include <vector>

namespace dropbox {

const size_t DEFAULT_FILE_LIMIT = 10;

typedef struct DropboxMetadata {
  std::string         path_;
  std::string         sizeStr_;
  size_t              sizeBytes_;
  bool                isDir_;
  std::string         mimeType_;
  bool                isDeleted_;
  std::string         rev_;
  std::string         hash_;
  bool                thumbExists_;
  std::string         icon_;
  std::string         clientMtime_;
  std::string         root_;

  static void readFromJson(boost::property_tree::ptree& pt,
      DropboxMetadata& m) {
    using namespace boost::property_tree;
    using namespace boost::property_tree::json_parser;
    using namespace std;

    try {
      // Mandatory fields
      m.path_ = pt.get<string>("path");
      m.sizeStr_ = pt.get<string>("size");
      m.sizeBytes_ = pt.get<size_t>("bytes");
      m.icon_ = pt.get<string>("icon");
      m.root_ = pt.get<string>("root");

      // Optional fields
      m.rev_ = pt.get<string>("rev", "");
      m.hash_ = pt.get<string>("hash", "");
      m.clientMtime_ = pt.get<string>("client_mtime", "");

      auto boolParser = [&](string field, string defaultVal, bool& val) {
        string strval = pt.get<string>(field, defaultVal);
        if (strval.compare("true") == 0) {
          val = true;
        } else if (strval.compare("false") == 0) {
          val = false;
        } else {
          stringstream s;
          s << "Bad '" << field << "' value: " << strval;
          throw DropboxException(MALFORMED_RESPONSE, s.str());
        }
      };

      boolParser("is_dir", "false", m.isDir_);
      boolParser("is_deleted", "false", m.isDeleted_);
      boolParser("thumb_exists", "false", m.thumbExists_);

      m.mimeType_ = pt.get<string>("mime_type", "");
    } catch (exception& e) {
      throw DropboxException(MALFORMED_RESPONSE, e.what());
    }
  }

  static void readMetadataListFromJson(boost::property_tree::ptree& pt,
      std::vector<DropboxMetadata>& list) {
    using namespace boost::property_tree;
    using namespace boost::property_tree::json_parser;
    using namespace std;

    BOOST_FOREACH(ptree::value_type& v, pt) {
      DropboxMetadata m;
      readFromJson(v.second, m);
      list.push_back(m);
    }
  }
} DropboxMetadata;
}
#endif
