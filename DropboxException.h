#ifndef __DROPBOX_EXCEPTION_H__
#define __DROPBOX_EXCEPTION_H__

#include <exception>

namespace dropbox {

enum DropboxErrorCode {
  MalformedResponse = -1,
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
