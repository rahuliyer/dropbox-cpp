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

#ifndef __DROPBOX_EXCEPTION_H__
#define __DROPBOX_EXCEPTION_H__

#include <exception>
#include <string>

namespace dropbox {

enum DropboxErrorCode {
  CURL_ERROR = -1,
  MALFORMED_RESPONSE = -2,
  IO_ERROR = -3,
  SUCCESS = 200,
  PARTIAL_CONTENT = 206,
  NOT_MODIFIED = 304,
  TOO_MANY_FILES = 406,
};

class DropboxException : public std::exception {
public:
  DropboxException(DropboxErrorCode code, std::string str) :
    errCode_(code), errStr_(str) {
  }

  virtual const char* what() const throw() {
    return errStr_.c_str();
  }

  DropboxErrorCode getErrorCode() const {
    return errCode_;
  }

private:
  DropboxErrorCode      errCode_;
  std::string           errStr_;
};
}

#endif
