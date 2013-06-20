#include "DropboxApi.h"

#include "util/HttpRequest.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>

using namespace dropbox;
using namespace oauth;
using namespace http;
using namespace std;

using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

DropboxApi::DropboxApi(string appKey, string appSecret) {
  httpFactory_ = HttpRequestFactory::createFactory();

  lock_guard<mutex> g(stateLock_);
  oauth_.reset(new OAuth(appKey, appSecret));
  root_ = DROPBOX_ROOT;
}

DropboxApi::DropboxApi(string appKey, 
    string appSecret, 
    string accessToken, 
    string tokenSecret) {
  httpFactory_ = HttpRequestFactory::createFactory();

  lock_guard<mutex> g(stateLock_);
  oauth_.reset(new OAuth(appKey, appSecret));
  oauth_->setAccessToken(accessToken);
  oauth_->setAccessTokenSecret(tokenSecret);
}

void DropboxApi::authenticate(function<void(const string, const string)> cb) {
  lock_guard<mutex> g(stateLock_);

  oauth_->fetchRequestToken("https://api.dropbox.com/1/oauth/request_token");
  
  cb(oauth_->getRequestToken(), oauth_->getRequestTokenSecret());

  oauth_->fetchAccessToken("https://api.dropbox.com/1/oauth/access_token");
}

void DropboxApi::setAccessToken(string token, string secret) {
  lock_guard<mutex> g(stateLock_);
  oauth_->setAccessToken(token);
  oauth_->setAccessTokenSecret(secret);
}

void DropboxApi::setRoot(const string root) {
  lock_guard<mutex> g(stateLock_);
  root_ = root;
}

DropboxErrorCode DropboxApi::execute(shared_ptr<HttpRequest> r) {
  int ret;
  if ((ret = r->execute())) {
    stringstream ss;
    ss << "Curl error (code = " << ret << ")";

    throw DropboxException(CURL_ERROR, ss.str());
  }

  return (DropboxErrorCode)r->getResponseCode();
}

DropboxErrorCode DropboxApi::getAccountInfo(AccountInfo& info) {
  shared_ptr<HttpRequest> r(
    httpFactory_->createHttpRequest("https://api.dropbox.com/1/account/info"));

  {
    lock_guard<mutex> g(stateLock_);
    oauth_->addOAuthAccessHeader(r.get());
  }

  DropboxErrorCode code = execute(r);

  if (code == SUCCESS) {
    string response((char *)r->getResponse(), r->getResponseSize());
    info.readJson(response);
  }

  return code;
}

DropboxErrorCode DropboxApi::getFileMetadata(DropboxMetadataRequest& req,
    DropboxMetadataResponse& res) {
  stringstream ss;
  ss << "https://api.dropbox.com/1/metadata/" << root_ << "/" << req.path();

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));

  {
    lock_guard<mutex> g(stateLock_);
    oauth_->addOAuthAccessHeader(r.get());
  }

  r->setMethod(HttpGetRequest);

  r->addIntegerParam("file_limit", req.getLimit());

  if (req.getHash().compare("")) {
    r->addParam("hash", req.getHash());
  }
  
  if (req.includeChildren()) {
    r->addParam("list", "true");
  } else {
    r->addParam("list", "false");
  }

  if (req.includeDeleted()) {
    r->addParam("include_deleted", "true");
  } else {
    r->addParam("include_deleted", "false");
  }

  if (req.getRev().compare("")) {
    r->addParam("rev", req.getRev());
  }

  DropboxErrorCode code = execute(r);
  if (code != SUCCESS) {
    return code;
  }

  string response((char *)r->getResponse(), r->getResponseSize());
  res.readJson(response);

  return code;
}

DropboxErrorCode DropboxApi::getRevisions(string path,
    size_t numRevisions, DropboxRevisions& revs) {
  stringstream ss;

  ss << "https://api.dropbox.com/1/revisions/" << root_ << "/" << path;

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));

  {
    lock_guard<mutex> g(stateLock_);
    oauth_->addOAuthAccessHeader(r.get());
  }

  r->setMethod(HttpGetRequest);

  if (numRevisions) {
    r->addIntegerParam("rev_limit", numRevisions);
  }

  DropboxErrorCode code = execute(r);
  if (code != SUCCESS) {
    return code;
  }

  string response((char *)r->getResponse(), r->getResponseSize());
  revs.readFromJson(response);

  return code;
}

DropboxErrorCode DropboxApi::restoreFile(string path, 
    string rev, DropboxMetadata& m) {
  stringstream ss;

  ss << "https://api.dropbox.com/1/restore/" << root_ << "/" << path;

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));

  {
    lock_guard<mutex> g(stateLock_);
    oauth_->addOAuthAccessHeader(r.get());
  }

  r->setMethod(HttpPostRequest);
  r->addParam("rev", rev);

  DropboxErrorCode code = execute(r);
  if (code != SUCCESS) {
    return code;
  }

  string response((char *)r->getResponse(), r->getResponseSize());

  stringstream s;
  s << response;

  ptree pt;
  read_json(s, pt);

  DropboxMetadata::readFromJson(pt, m);

  return code;
}

DropboxErrorCode DropboxApi::deleteFile(string path, DropboxMetadata& m) {
  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(
    "https://api.dropbox.com/1/fileops/delete"));

  {
    lock_guard<mutex> g(stateLock_);
    oauth_->addOAuthAccessHeader(r.get());
  }

  r->addParam("root", root_);
  r->addParam("path", path);

  DropboxErrorCode code = execute(r);
  if (code != SUCCESS) {
    return code;
  }
  
  string response((char *)r->getResponse(), r->getResponseSize());

  stringstream s;
  s << response;

  ptree pt;
  read_json(s, pt);

  DropboxMetadata::readFromJson(pt, m);

  return code;
}
