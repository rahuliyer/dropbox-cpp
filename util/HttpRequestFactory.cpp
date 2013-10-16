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

#include "HttpRequestFactory.h"
#include "HttpRequest.h"

#include <mutex>
#include <cassert>
#include <memory>

#include <curl/curl.h>

using namespace http;
using namespace std;

HttpRequestFactory::HttpRequestFactory() {
  numRequests_.store(0);

  if (curl_global_init(CURL_GLOBAL_DEFAULT)) {
    throw std::bad_alloc();
  }
}

HttpRequestFactory* HttpRequestFactory::createFactory() {
  static mutex lock;
  static unique_ptr<HttpRequestFactory> factory;

  lock_guard<mutex> g(lock);

  HttpRequestFactory* ptr = NULL;
  if (factory.get() == NULL) {
    try {
      ptr = new HttpRequestFactory();
    } catch(...) { }

    factory.reset(ptr);
  }

  return factory.get();
}

HttpRequest* HttpRequestFactory::createHttpRequest(string url,
    HttpRequestMethod method) {
  return new HttpRequest(this, url, method);
}

void HttpRequestFactory::increaseRequestCount() {
  numRequests_++;
}

void HttpRequestFactory::decreaseRequestCount() {
  numRequests_--;
}

HttpRequestFactory::~HttpRequestFactory() {
  assert(numRequests_.load() == 0);

  curl_global_cleanup();
}

