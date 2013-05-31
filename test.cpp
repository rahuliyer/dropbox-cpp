#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"

#include <iostream>
#include <cstring>

using namespace std;
using namespace http;

int main() {
  HttpRequestFactory* f = HttpRequestFactory::createFactory();

  HttpRequest* request = f->createHttpRequest("https://api.dropbox.com/1/oauth/request_token");
  request->setMethod(HttpPostRequest);
  request->addHeader("Authorization", "OAuth oauth_version=\"1.0\", oauth_signature_method=\"PLAINTEXT\", oauth_consumer_key=\"2tvqhnmuke5p8h7\", oauth_signature=\"gvqvzr324d5wnrz&\"");
  request->nullTerminateResponse(true);
  cerr << "execute: " << request->execute() << endl;
  cerr << "code: " << request->getResponseCode() << endl;
  cerr << "size: " << request->getResponseSize() << endl;

  string response((char *)request->getResponse());
  cerr << "Response:" << endl;
  cerr << response << endl;

  cerr << "Response headers:" << endl;

  for (auto i : request->getResponseHeaders()) {
    cerr << i.first << ": " << i.second << endl;
  }

  delete request;

  return 0;
}

