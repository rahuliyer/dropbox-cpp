#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

/**
 * A simple wrapper around the curl "easy" functions
 */
#include "HttpRequestFactory.h"

#include <sys/types.h>

#include <curl/curl.h>

#include <string>
#include <map>
#include <memory>

namespace http {

class HttpRequest {
public:
  /**
   * Construct a HttpRequest
   *
   * @param     factory   The HttpRequestFactory that created this request
   * @param     url       The url to hit
   * @param     method    The Http method (GET/PUT etc.) to use
   */
  HttpRequest(HttpRequestFactory* factory, std::string url, 
    HttpRequestMethod method);
  
  /**
   * Set the request method
   *
   * @param     method    Http request method (GET/POST etc.)
   *
   * @return    void
   */
  void                            setMethod(HttpRequestMethod);

  /**
   * Get the HttpRequestMethod being used
   *
   * @return    HttpRequestMethod
   */
  HttpRequestMethod               getMethod() const;

  /**
   * Adds a parameter to the http request. All params are consolidated
   * like:
   * param1=value1&param2=value2...
   * before the request is sent.
   *
   * @param     param   Name of the param
   * @param     value   Value of the param
   *
   * @return    void
   */
  void                            addParam(const std::string& param, 
                                    const std::string& value);

  /**
   * Converts the integer value for a parameter to a string and adds it
   * to the request
   *
   * @param     param   Name of the param
   * @param     value   Integer value for the param
   *
   * @return    void
   */
   void                           addIntegerParam(const std::string& param,
                                    const int val);

  /**
   * add a Range for Http Get requests
   *
   * @param     start   start of the range
   * @param     end     end of the range, last byte included
   */
   void                           addRange(uint64_t start, uint64_t end);

  /**
   * Returns the params in a map of params to values
   *
   * @return    map<string, string>   map of the params and values
   */
  const std::map<std::string, 
    std::string>&                 getParams() const;

  /**
   * Adds a header to the http request. The header is serialized as:
   * <header>: <value>
   * at the time of sending the http request
   *
   * @param     header    The name of the header (e.g Authorization)
   * @param     value     Value of that header
   *
   * @return    void
   */
  void                            addHeader(const std::string& header, 
                                    const std::string& value);

  /**
   * Get headers as a map of header names to values
   *
   * @return    map<string, string>    Map of header names to values
   */
  const std::map<std::string,
    std::string>&                 getHeaders() const;

  /**
   * Adds data to the request. This is typically used when uploading files
   * using a PUT request. The data is *NOT* copied and it is the responsibility
   * of the owner to make sure the pointer provided as an argument to this
   * function is valid until the execution of the request completes.
   *
   * @param     data      The data to be uploaded
   * @param     size      Size of the data being uploaded
   *
   * @return    void
   */
  void                            setRequestData(uint8_t* const data,
                                    const size_t size);

  /**
   * Dispatch the http request
   *
   * @return    int   The error code returned by curl. 0 on success, 
   *                  non zero on failure. 
   *                  Check /usr/include/curl/curl.h for values
   */
  int                             execute();

  /**
   * Get the Http response code for the executed http request
   *
   * @return    long    Http response code
   */
  long                            getResponseCode() const;

  /**
   * Get the response returned by the server
   *
   * @return    uint8_t* Bytes returned as the http response
   */
  uint8_t*                        getResponse() const;

  /**
   * The size of the http response
   *
   * @return    size_t   Number of bytes in the http response
   */
  size_t                          getResponseSize() const;

  /**
   * The http headers set in the response returned as a map of header name
   * to value
   *
   * @return    map<string, string>   map of header names to values
   */
  const std::map<std::string, 
    std::string>&                 getResponseHeaders() const;

  /**
   * Callback function to be called when data has been received
   * This function is called by curl when there is data it has successfully 
   * read.
   *
   * @param     char*   buf     Data that was received
   * @param     size_t  nmemb   Number of members
   * @param     size_t  size    Size of the member
   * @param     void*   cookie  Any cookie we wish to pass. In this case
   *                            it's a pointer to the HttpRequest itself
   *
   * @return    size_t  The number of bytes consumed. If this is not 
   *                    the same as nmemb * size, curl will throw an error.
   */
  static size_t                   writeFunction(char*, size_t, size_t, void*);

  /**
   * Callback function to be called when header data has been received
   * This function is called by curl when there is header data it has 
   * successfully read.
   *
   * @param     char*   buf     Data that was received
   * @param     size_t  nmemb   Number of members
   * @param     size_t  size    Size of the member
   * @param     void*   cookie  Any cookie we wish to pass. In this case
   *                            it's a pointer to the HttpRequest itself
   *
   * @return    size_t  The number of bytes consumed. If this is not 
   *                    the same as nmemb * size, curl will throw an error.
   */
  static size_t                   headerFunction(char*, size_t, size_t, void*);
  
  /**
   * Callback function to be called to get data that needs to be send on 
   * HTTP PUT.
   * This function is called by curl when it needs to get the data to be sent 
   * on the HTTP PUT. 
   *
   * @param     char*   buf     Data to be sent
   * @param     size_t  nmemb   Number of members
   * @param     size_t  size    Size of the member
   * @param     void*   cookie  Any cookie we wish to pass. In this case
   *                            it's a pointer to the HttpRequest itself
   *
   * @return    size_t  The number of bytes sent
   */
  static size_t                   readFunction(void*, size_t, size_t, void*);

  ~HttpRequest();
private:

  HttpRequestFactory* const                 factory_;
  const std::string                         url_;
  HttpRequestMethod                         method_;

  std::map<std::string, std::string>        params_;
  std::map<std::string, std::string>        headers_;

  bool                                      hasRange_;
  uint64_t                                  rangeStart_;
  uint64_t                                  rangeEnd_;

  size_t                                    requestDataSize_;
  size_t                                    requestDataOffset_;
  uint8_t*                                  requestData_;

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
