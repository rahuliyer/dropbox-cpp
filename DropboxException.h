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
