#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "HttpRequestFactory.h"

#include <sys/types.h>

#include <curl/curl.h>

#include <string>
#include <map>
#include <memory>

namespace http {

class HttpRequest {
public:
  HttpRequest(HttpRequestFactory* factory, std::string url, 
    HttpRequestMethod method);
  
  void                            setMethod(HttpRequestMethod);
  HttpRequestMethod               getMethod();

  void                            addParam(const std::string& param, 
                                    const std::string& value);
  const std::map<std::string, 
    std::string>&                 getParams();

  void                            addHeader(const std::string& header, 
                                    const std::string& value);
  const std::map<std::string,
    std::string>&                 getHeaders();

  int                             execute();

  long                            getResponseCode();
  uint8_t*                        getResponse();
  size_t                          getResponseSize();
  const std::map<std::string, 
    std::string>&                 getResponseHeaders();

  static size_t                   writeFunction(char*, size_t, size_t, void*);
  static size_t                   headerFunction(char*, size_t, size_t, void*);
  
  ~HttpRequest();
private:

  HttpRequestFactory*                       factory_;
  std::string                               url_;
  HttpRequestMethod                         method_;

  std::map<std::string, std::string>        params_;
  std::map<std::string, std::string>        headers_;

  bool                                      shouldNullTerminateResponse_;
  size_t                                    responseSize_;
  std::unique_ptr<uint8_t, void(*)(void *)> response_;
  long                                      responseCode_;
  std::map<std::string, std::string>        responseHeaders_;

  std::unique_ptr<CURL,
    void(*)(CURL*)>                         curl_;
  std::unique_ptr<struct curl_slist,
    void(*)(struct curl_slist*)>            slist_;
};
}
#endif
