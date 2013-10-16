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

#ifndef __OAUTH_EXCEPTION_H__
#define __OAUTH_EXCEPTION_H__

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

#endif
