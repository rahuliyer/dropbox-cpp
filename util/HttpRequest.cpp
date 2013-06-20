#include "HttpRequest.h"

#include <curl/curl.h>

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cstring>
#include <cassert>

using namespace http;
using namespace std;

HttpRequest::HttpRequest(HttpRequestFactory* factory, 
    string url, 
    HttpRequestMethod method) : factory_(factory), 
      url_(url), 
      method_(method), 
      response_(NULL, free),
      curl_(curl_easy_init(), curl_easy_cleanup), 
      slist_(NULL, curl_slist_free_all) {
  factory_->increaseRequestCount();
}

void HttpRequest::setMethod(HttpRequestMethod method) {
  method_ = method;
}

HttpRequestMethod HttpRequest::getMethod() const {
  return method_;
}

void HttpRequest::addParam(const string& param, const string& value) {
  params_[param] = value;
}

void HttpRequest::addIntegerParam(const string& param, const int value) {
  stringstream ss;

  ss << value;

  addParam(param, ss.str());
}

const map<string, string>& HttpRequest::getParams() const {
  return params_;
}

void HttpRequest::addHeader(const string& header, const string& value) {
  headers_[header] = value;
}

const map<string, string>& HttpRequest::getHeaders() const {
  return headers_;
}

size_t HttpRequest::writeFunction(char* buf, size_t size, size_t n, void *p) {
  size_t numBytes = size * n;
  HttpRequest* r = (HttpRequest *)p;

  if (!r->responseSize_) {
    r->responseSize_ = numBytes;
    r->response_.reset((uint8_t *)std::malloc(r->responseSize_));
  } else {
    r->responseSize_ += numBytes;
    r->response_.reset((uint8_t *)std::realloc(r->response_.get(), 
      r->responseSize_));
  }

  memcpy(r->response_.get(), buf, numBytes);

  return numBytes;
}

size_t HttpRequest::headerFunction(char* buf, size_t sz, size_t n, void *p) {
  size_t numBytes = sz * n;
  HttpRequest* r = (HttpRequest *)p;

  // Trim out the \r\n and any trailing null bytes
  size_t size = numBytes - 1;
  while (size) {
    if (buf[size] == '\r' || buf[size] == '\n' || buf[size] ==0) {
      --size;
    } else {
      break;
    }
  }

  if (!size) {
    return numBytes;
  }

  string s(buf, size + 1);
  size_t pos = s.find(":");

  string header;
  string val;
  if (pos != string::npos) {
    header = s.substr(0, pos);
    val = s.substr(pos + 1);
  } else {
    header = s;
    val = "";
  }


  r->responseHeaders_[header] = val;
  
  return numBytes;
}

int HttpRequest::execute() {
  int ret = 0;

  responseSize_ = 0;

  // Set the headers
  vector<string> headerstrings;
  for (auto i : headers_) {
    stringstream ss;
    ss << i.first << ": " << i.second;

    headerstrings.push_back(ss.str());
  }

  for (auto i : headerstrings) {
    slist_.reset(curl_slist_append(slist_.get(), i.c_str()));
  }

  if ((ret = curl_easy_setopt(curl_.get(), CURLOPT_HTTPHEADER, slist_.get()))) {
    return ret;
  }

  // Set the params
  string paramList;
  bool empty = true;
  if (method_ == HttpGetRequest || method_ == HttpPostRequest) {
    for (auto i : params_) {
      stringstream ss;
      ss << i.first << "=" << i.second;
      
      if (empty) {
        empty = false;
        paramList += ss.str();
      } else {
        paramList += "&";
        paramList += ss.str();
      }
    }
  }

  string url = url_;
  // Set the Http method
  switch (method_) {
    case HttpGetRequest:
      url += "?";
      url += paramList;
      break;

    case HttpPostRequest:
      if ((ret = curl_easy_setopt(curl_.get(), CURLOPT_POST, 1))) {
        return ret;
      }

      if ((ret = curl_easy_setopt(curl_.get(), 
          CURLOPT_POSTFIELDS, 
          paramList.c_str()))) {
        return ret;
      }
      break;

    case HttpPutRequest:
      if ((ret = curl_easy_setopt(curl_.get(), CURLOPT_PUT, 1))) {
        return ret;
      }
      break;

    default:
      return -1;
  }

  // Set the URL
  if ((ret = curl_easy_setopt(curl_.get(), CURLOPT_URL, url.c_str())) 
      != CURLE_OK) {
    return ret;
  }

  // Set the write and header functions and data
  if ((ret = curl_easy_setopt(curl_.get(), 
      CURLOPT_WRITEFUNCTION, 
      &HttpRequest::writeFunction))) {
    return ret;
  }

  if ((ret = curl_easy_setopt(curl_.get(), 
      CURLOPT_WRITEDATA, 
      this))) {
    return ret;
  }

  if ((ret = curl_easy_setopt(curl_.get(), 
      CURLOPT_HEADERFUNCTION, 
      &HttpRequest::headerFunction))) {
    return ret;
  }

  if ((ret = curl_easy_setopt(curl_.get(), 
      CURLOPT_HEADERDATA, 
      this))) {
    return ret;
  }

  // Go!!
  if ((ret = curl_easy_perform(curl_.get()))) {
    return ret;
  }

  if ((ret = curl_easy_getinfo(curl_.get(), 
      CURLINFO_RESPONSE_CODE, &responseCode_)) != CURLE_OK) {
    return ret;
  }

  return 0;
}

long HttpRequest::getResponseCode() const {
  return responseCode_;
}

uint8_t* HttpRequest::getResponse() const {
  return response_.get();
}

size_t HttpRequest::getResponseSize() const {
  return responseSize_;
}

const map<string, string>& HttpRequest::getResponseHeaders() const {
  return responseHeaders_;
}

HttpRequest::~HttpRequest() {
  factory_->decreaseRequestCount();
}
