#include <exception>

namespace oauth {

enum OAuthErrorCode {
  HttpRequestFailed,
  MalformedOAuthResponse,
  UnsupportedMethod,
};

class OAuthException : public std::exception {
public:
  OAuthException(OAuthErrorCode errcode, std::string errStr) : 
    errCode_(errcode), errStr_(errStr) {
  }

  virtual const char* what() const throw() {
    return errStr_.c_str();
  }

  OAuthErrorCode errorCode() {
    return errCode_;
  }

private:
  OAuthErrorCode      errCode_;
  std::string         errStr_;
};
}
