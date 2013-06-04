#include "util/OAuth.h"
#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"

#include <iostream>
#include <cstring>

using namespace std;
using namespace http;
using namespace oauth;

int main(int argc, char** argv) {
  OAuth o("2tvqhnmuke5p8h7", "gvqvzr324d5wnrz");

  if (argc != 3) {
    o.fetchRequestToken("https://api.dropbox.com/1/oauth/request_token");
    cout << "Token: " << o.getRequestToken() << endl;
    cout << "Secret: " << o.getRequestTokenSecret() << endl;

    char c;
    cin >> c;

    o.fetchAccessToken("https://api.dropbox.com/1/oauth/access_token");
    cout << "Token: " << o.getAccessToken() << endl;
    cout << "Secret: " << o.getAccessTokenSecret() << endl;
  } else {
    o.setAccessToken(argv[1]);
    o.setAccessTokenSecret(argv[2]);
  }

  HttpRequestFactory* f = HttpRequestFactory::createFactory();
  HttpRequest* r = 
    f->createHttpRequest("https://api.dropbox.com/1/account/info");
  
  o.addOAuthAccessHeader(r);

  r->execute();
  cout << "Response code: " << r->getResponseCode() << endl;
  cout << "Response: " << 
    string((char *)r->getResponse(), r->getResponseSize()) << endl;
 
  delete r;

  return 0;
}

