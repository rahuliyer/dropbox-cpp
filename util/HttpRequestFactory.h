#ifndef __HTTP_REQUEST_FACTORY_H__
#define __HTTP_REQUEST_FACTORY_H__

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
  static HttpRequestFactory* createFactory();
  HttpRequest* createHttpRequest(std::string url, 
    HttpRequestMethod method=HttpGetRequest);

  void increaseRequestCount();
  void decreaseRequestCount();

  ~HttpRequestFactory();

private:
  HttpRequestFactory();

  std::atomic<int>    numRequests_;
};
}
#endif
