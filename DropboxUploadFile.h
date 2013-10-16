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

#ifndef __DROPBOX_UPLOAD_FILE_H__
#define __DROPBOX_UPLOAD_FILE_H__

#include <string>

namespace dropbox {

class DropboxUploadFileRequest {
public:
  DropboxUploadFileRequest(const std::string path,
    const bool overwrite = true,
    const std::string parent_rev = "") :
      path_(path),
      overwrite_(overwrite),
      parentRev_(parent_rev),
      data_(NULL),
      dataSize_(0) {
  }

  void setOverwrite(bool overwrite) {
    overwrite_ = overwrite;
  }

  void setParentRev(const std::string parent_rev) {
    parentRev_ = parent_rev;
  }

  void setUploadData(uint8_t* const data, size_t size) {
    data_ = data;
    dataSize_ = size;
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

  uint8_t* getUploadData() const {
    return data_;
  }

  size_t getUploadDataSize() const {
    return dataSize_;
  }

private:
  const std::string   path_;
  bool                overwrite_;
  std::string         parentRev_;
  uint8_t*            data_;
  size_t              dataSize_;
};
}
#endif
