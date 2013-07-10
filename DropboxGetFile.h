#ifndef __DROPBOX_GET_FILE_H__
#define __DROPBOX_GET_FILE_H__

#include <string>
#include <memory>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "DropboxMetadata.h"

namespace dropbox {

class DropboxGetFileRequest {
public:
  DropboxGetFileRequest(std::string path, std::string rev="") : 
    path_(path), rev_(rev), hasRange_(false) {
  }

  void setRange(uint64_t offset, uint64_t length) {
    offset_ = offset;
    length_ = length;
    hasRange_ = true;
  }

  std::string getPath() const {
    return path_;
  }

  std::string getRev() const {
    return rev_;
  }

  bool hasRange() const {
    return hasRange_;
  }

  uint64_t getOffset() const {
    return offset_;
  }

  uint64_t getLength() const {
    return length_;
  }

private:
  std::string         path_;
  std::string         rev_;
  bool                hasRange_;
  uint64_t            offset_;
  uint64_t            length_;
};

class DropboxGetFileResponse {
public:
  DropboxGetFileResponse() { }

  void setData(uint8_t* data, uint64_t len) {
    data_.reset(new uint8_t[len]);
    memcpy(data_.get(), data, len);
    length_ = len;
  }

  void setMetadata(std::string& s) {
    using namespace boost::property_tree;
    using namespace boost::property_tree::json_parser;

    std::stringstream ss;
    ss << s;

    ptree pt;
    read_json(ss, pt);
    DropboxMetadata::readFromJson(pt, metadata_);
  }

  const uint8_t* const getData() const {
    return data_.get();
  }

  uint64_t getDataLength() const {
    return length_;
  }

  DropboxMetadata getMetadata() const {
    return metadata_;
  }

private:
  std::unique_ptr<uint8_t>    data_;
  uint64_t                    length_;
  DropboxMetadata             metadata_;
};

}
#endif
