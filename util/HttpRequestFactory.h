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

#ifndef __HTTP_REQUEST_FACTORY_H__
#define __HTTP_REQUEST_FACTORY_H__

/**
 * A simple wrapper around curl. This class is a singleton that ensures that
 * the curl_global_init() function is called only once.
 */

#include <atomic>
#include <string>

namespace http {

enum HttpRequestMethod {
  HttpGetRequest,
  HttpPostRequest,
  HttpPutRequest,
  HttpDeleteRequest,
};

class HttpRequest;

class HttpRequestFactory {
public:
  /**
   * Create an instance of the singleton. The memory is internally managed,
   * so DO NOT free the pointer returned.
   *
   * @return  Pointer to the singleton
   */
  static HttpRequestFactory* createFactory();

  /**
   * Create a HttpRequest. The pointer returned is not managed and must be
   * freed
   *
   * @param url       url to make the request to
   * @param method    type of Http request (GET/POST etc.)a
   *
   * @return  HttpRequest pointer
   */
  HttpRequest* createHttpRequest(std::string url,
    HttpRequestMethod method=HttpGetRequest);

  /**
   * Increases the counter of number of allocated http requests from this
   * factory
   *
   * @return  void
   */
  void increaseRequestCount();

  /**
   * Increases the counter of number of allocated http requests from this
   * factory.
   *
   * @return  void
   */
  void decreaseRequestCount();

  /**
   * Note: This will assert if the number of outstanding requests is non-zero
   * at the time of destruction
   *
   * @return  void
   */
  ~HttpRequestFactory();

private:
  HttpRequestFactory();

  std::atomic<int>    numRequests_;
};
}
#endif
