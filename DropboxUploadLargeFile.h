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

#ifndef __DROPBOX_UPLOAD_LARGE_FILE_H__
#define __DROPBOX_UPLOAD_LARGE_FILE_H__

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace dropbox {

class DropboxUploadLargeFileRequest {
public:
  DropboxUploadLargeFileRequest(const std::string path,
    std::function<size_t(uint8_t*, size_t, size_t)> cb, // 4MB
    const bool overwrite = true,
    const std::string parent_rev = "",
    size_t chunkSize = (1UL << 22),
    size_t offset = 0) :
      path_(path),
      dataCb_(cb),
      overwrite_(overwrite),
      parentRev_(parent_rev),
      chunkSize_(chunkSize),
      offset_(offset) {
  }

  void setOverwrite(bool overwrite) {
    overwrite_ = overwrite;
  }

  void setParentRev(const std::string parent_rev) {
    parentRev_ = parent_rev;
  }

  void setOffset(size_t offset) {
    offset_ = offset;
  }

  std::string getPath() const {
    return path_;
  }

  bool shouldOverwrite() const {
    return overwrite_;
  }

  std::string getParentRev() const {
    return parentRev_;
  }

  size_t getChunkSize() const {
    return chunkSize_;
  }

  size_t getOffset() const {
    return offset_;
  }

  size_t getData(uint8_t* data, size_t offset, size_t size) const {
    return dataCb_(data, offset, size);
  }

private:
  const std::string   path_;
  std::function<size_t(uint8_t*, size_t, size_t)> dataCb_;

  bool                overwrite_;
  std::string         parentRev_;
  size_t              chunkSize_;
  size_t              offset_;
};

class DropboxUploadLargeFileResponse {
public:
  static DropboxUploadLargeFileResponse readFromJson(std::string response) {
    using namespace boost::property_tree;
    using namespace boost::property_tree::json_parser;
    using namespace std;

    stringstream s;
    s << response;

    ptree pt;
    read_json(s, pt);

    string upId = pt.get<string>("upload_id");
    size_t offset = pt.get<size_t>("offset");
    string expiry = pt.get<string>("expires");

    return DropboxUploadLargeFileResponse(upId, offset, expiry);
  }

  std::string getUploadId() const {
    return uploadId_;
  }

  size_t getOffset() const {
    return offset_;
  }

  std::string getExpiry() const {
    return expiry_;
  }

private:
  DropboxUploadLargeFileResponse(std::string upId,
    size_t offset,
    std::string expiry) :
      uploadId_(upId),
      offset_(offset),
      expiry_(expiry) {
  }

  const std::string   uploadId_;
  const size_t        offset_;
  const std::string   expiry_;
};
}
#endif
