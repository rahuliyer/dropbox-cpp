#include "util/OAuth.h"
#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"
#include "DropboxAccountInfo.h"
#include "DropboxApi.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <functional>

using namespace std;
using namespace std::placeholders;
using namespace http;
using namespace oauth;
using namespace dropbox;

class Foo {
public:
  Foo() { }

  void bar(string token, string secret) {
    cout << "Token: " << token << endl;
    cout << "Secret: " << secret << endl;

    char a;
    cin >> a;
  }
};

int main(int argc, char** argv) {
#if 0
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
#endif

  DropboxApi d("2tvqhnmuke5p8h7", "gvqvzr324d5wnrz");
  Foo f;

  if (argc != 3) {
    function<void(string, string)> fn = bind(&Foo::bar, &f, _1, _2);
    d.authenticate(fn);
  } else {
    d.setAccessToken(argv[1], argv[2]);
  }

  AccountInfo ac2;
  cout << "Response: " << d.getAccountInfo(ac2) << endl;
  cout << endl;
  cout << "refferal link: " << ac2.getReferralLink() << endl;
  cout << "display_name: " << ac2.getDisplayName() << endl;
  cout << "uid: " << ac2.getUid() << endl;
  cout << "country: " << ac2.getCountry() << endl;
  cout << "email: " << ac2.getEmail() << endl;
  cout << "Quota info:" << endl;
  cout << "\tshared: " << ac2.getQuotaInfo().shared << endl;
  cout << "\tquota: " << ac2.getQuotaInfo().quota << endl;
  cout << "\tnormal: " << ac2.getQuotaInfo().normal << endl;

  return 0;
}

