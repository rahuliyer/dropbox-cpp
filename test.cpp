#include "util/OAuth.h"
#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"
#include "DropboxAccountInfo.h"

#include <iostream>
#include <cstring>
#include <sstream>

using namespace std;
using namespace http;
using namespace oauth;
using namespace dropbox;

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
  string response = string((char *)r->getResponse(), r->getResponseSize());
  cout << "Response: " << response << endl;

  AccountInfo ac(response);

  cout << endl;
  cout << "refferal link: " << ac.getReferralLink() << endl;
  cout << "display_name: " << ac.getDisplayName() << endl;
  cout << "uid: " << ac.getUid() << endl;
  cout << "country: " << ac.getCountry() << endl;
  cout << "email: " << ac.getEmail() << endl;
  cout << "Quota info:" << endl;
  cout << "\tshared: " << ac.getQuotaInfo().shared << endl;
  cout << "\tquota: " << ac.getQuotaInfo().quota << endl;
  cout << "\tnormal: " << ac.getQuotaInfo().normal << endl;

  delete r;

  return 0;
}

