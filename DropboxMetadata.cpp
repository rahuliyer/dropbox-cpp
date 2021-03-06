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

#include "DropboxMetadata.h"
#include "DropboxException.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <sstream>

using namespace std;
using namespace dropbox;
using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

DropboxMetadataRequest::DropboxMetadataRequest(const string path,
    const bool includeChildren,
    const bool includeDeleted) : path_(path),
      includeChildren_(includeChildren),
      includeDeleted_(includeDeleted) {
  fileLimit_ = DEFAULT_FILE_LIMIT;
  hash_ = "";
  rev_ = "";
}

void DropboxMetadataRequest::setLimit(const size_t limit) {
  fileLimit_ = limit;
}

size_t DropboxMetadataRequest::getLimit() const {
  return fileLimit_;
}

void DropboxMetadataRequest::setHash(const string hash) {
  hash_ = hash;
}

string DropboxMetadataRequest::getHash() const {
  return hash_;
}

void DropboxMetadataRequest::setRev(const string rev) {
  rev_ = rev;
}

string DropboxMetadataRequest::getRev() const {
  return rev_;
}

string DropboxMetadataRequest::path() const {
  return path_;
}

bool DropboxMetadataRequest::includeDeleted() const {
  return includeDeleted_;
}

bool DropboxMetadataRequest::includeChildren() const {
  return includeChildren_;
}

DropboxMetadataResponse::DropboxMetadataResponse() {
}

void DropboxMetadataResponse::readJson(const string& json) {
  try {
    stringstream ss;
    ss << json;

    ptree pt;
    read_json(ss, pt);

    DropboxMetadata::readFromJson(pt, metadata_);
    if (pt.count("contents") == 0) {
      return;
    }

    DropboxMetadata::readMetadataListFromJson(
      pt.get_child("contents"), children_);
  } catch (exception& e) {
    throw DropboxException(MALFORMED_RESPONSE, e.what());
  }
}

DropboxMetadata& DropboxMetadataResponse::getMetadata() {
  return metadata_;
}

const vector<DropboxMetadata>& DropboxMetadataResponse::getChildren() const {
  return children_;
}

