#ifndef __DROPBOX_EXCEPTION_H__
#define __DROPBOX_EXCEPTION_H__

#include <exception>
#include <string>

namespace dropbox {

enum DropboxErrorCode {
  CURL_ERROR = -1,
  MALFORMED_RESPONSE = -2,
  SUCCESS = 200,
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
