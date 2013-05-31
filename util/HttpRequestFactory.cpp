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

