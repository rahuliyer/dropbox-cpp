#include "util/OAuth.h"
#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"

#include "DropboxAccountInfo.h"
#include "DropboxMetadata.h"
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

void dumpMetadata(DropboxMetadata& m) {
  cout << "Path: " << m.path_ << endl << \
    "Size: " << m.sizeStr_ << endl << \
    "Size (bytes): " << m.sizeBytes_ << endl << \
    "isDir: " << m.isDir_ << endl << \
    "mime type: " << m.mimeType_ << endl << \
    "isDeleted: " << m.isDeleted_ << endl << \
    "rev: " << m.rev_ << endl << \
    "hash: " << m.hash_ << endl << \
    "thumb exists: " << m.thumbExists_ << endl << \
    "icon: " << m.icon_ << endl << \
    "client mtime: " << m.clientMtime_ << endl << \
    "root: " << m.root_ << endl << endl;
}

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
    function<void(string, string)> fn = bind(&Foo::bar, &f, std::placeholders::_1, std::placeholders::_2);
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

  cout << endl;

  DropboxMetadataRequest req("/", true, true);
  DropboxMetadataResponse res;
  d.getFileMetadata(req, res);

  dumpMetadata(res.getMetadata());

  cout << "Children: " << endl;
  for (auto i : res.getChildren()) {
    dumpMetadata(i);
  }

  DropboxRevisions revs;
  d.getRevisions("/test2", 2, revs);

  cout << "Revisions" << endl;
  for (auto i : revs.getRevisions()) {
    dumpMetadata(i);
  }

  DropboxMetadata m;
  d.restoreFile("/test2", "160ef6ddfe", m);

  cout << "restored file metadata: " << endl;
  dumpMetadata(m);

  d.copyFile("/test.txt", "/test.txt.bk", m);
  cout << "copied file metadata: " << endl;
  dumpMetadata(m);

  d.moveFile("/test.txt.bk", "/test.txt.bk2", m);
  cout << "moved file metadata: " << endl;
  dumpMetadata(m);

  sleep(10);
  d.deleteFile("/test.txt.bk2", m);

  cout << "deleted file metadata: " << endl;
  dumpMetadata(m);


  return 0;
}

