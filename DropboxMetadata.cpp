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

void DropboxMetadataResponse::readMetadataFromJson(ptree& pt,
    DropboxMetadata& m) {
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
}  

void DropboxMetadataResponse::readJson(const string& json) {
  try {
    stringstream ss;
    ss << json;

    ptree pt;
    read_json(ss, pt);
    
    readMetadataFromJson(pt, metadata_);

    BOOST_FOREACH(ptree::value_type& v, pt.get_child("contents")) {
      DropboxMetadata m;
      readMetadataFromJson(v.second, m);
      children_.push_back(m);
    }
  } catch (exception& e) {
    throw DropboxException(MALFORMED_RESPONSE, e.what());
  }
}

DropboxMetadata& DropboxMetadataResponse::getMetadata() {
  return metadata_;
}

vector<DropboxMetadata>& DropboxMetadataResponse::getChildren() {
  return children_;
}

