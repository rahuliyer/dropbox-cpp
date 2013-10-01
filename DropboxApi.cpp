#include "DropboxApi.h"

#include "util/HttpRequest.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <cassert>

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

string DropboxApi::getAccessToken() {
  lock_guard<mutex> g(stateLock_);
  return oauth_->getAccessToken();
}
  
string DropboxApi::getAccessTokenSecret() {
  lock_guard<mutex> g(stateLock_);
  return oauth_->getAccessTokenSecret();
}

void DropboxApi::setRoot(const string root) {
  lock_guard<mutex> g(stateLock_);
  root_ = root;
}

DropboxErrorCode DropboxApi::execute(shared_ptr<HttpRequest> r) {
  int ret;

  {
    lock_guard<mutex> g(stateLock_);
    oauth_->addOAuthAccessHeader(r.get());
  }

  if ((ret = r->execute())) {
    stringstream ss;
    ss << "Curl error (code = " << ret << ")";

    throw DropboxException(CURL_ERROR, ss.str());
  }

  return (DropboxErrorCode)r->getResponseCode();
}

DropboxErrorCode DropboxApi::getAccountInfo(DropboxAccountInfo& info) {
  shared_ptr<HttpRequest> r(
    httpFactory_->createHttpRequest("https://api.dropbox.com/1/account/info"));

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

DropboxErrorCode DropboxApi::copyOrMove(const string from, 
    const string to,
    const string op,
    DropboxMetadata& m) {
  stringstream ss;
  ss << "https://api.dropbox.com/1/fileops/" << op;

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));
  
  r->addParam("root", root_);
  r->addParam("from_path", from);
  r->addParam("to_path", to);

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

DropboxErrorCode DropboxApi::copyFile(string from,
    string to,
    DropboxMetadata& m) {
  return copyOrMove(from, to, "copy", m);
}

DropboxErrorCode DropboxApi::moveFile(string from,
    string to,
    DropboxMetadata& m) {
  return copyOrMove(from, to, "move", m);
}

DropboxErrorCode DropboxApi::createFolder(const string path,
    DropboxMetadata& m) {
  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(
    "https://api.dropbox.com/1/fileops/create_folder"));
  
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

DropboxErrorCode DropboxApi::getFile(DropboxGetFileRequest& req,
    DropboxGetFileResponse& res) {
  stringstream ss;
  ss << "https://api-content.dropbox.com/1/files/" << root_ << "/" 
    << req.getPath();

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));
  if (req.getRev().compare("")) {
    r->addParam("rev", req.getRev());
  }

  if (req.hasRange()) {
    r->addRange(req.getOffset(), req.getOffset() + req.getLength() - 1);
  }

  DropboxErrorCode code = execute(r);
  if (code != SUCCESS && code != PARTIAL_CONTENT) {
    return code;
  }

  res.setData(r->getResponse(), r->getResponseSize());

  map<string, string> respHeaders = r->getResponseHeaders();
  res.setMetadata(respHeaders["x-dropbox-metadata"]);

  return code;
}

DropboxErrorCode DropboxApi::uploadFile(const DropboxUploadFileRequest& req,
    DropboxMetadata& m) {
  stringstream ss;
  ss << "https://api-content.dropbox.com/1/files_put/" << root_ << "/" 
    << req.getPath();

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));
  r->setMethod(HttpPutRequest);

  if (req.shouldOverwrite()) {
    r->addParam("overwrite", "true");
  } else {
    r->addParam("overwrite", "false");
  }

  if (req.getParentRev().compare("")) {
    r->addParam("parent_rev", req.getParentRev());
  }

  assert(req.getUploadData());

  r->setRequestData(req.getUploadData(), req.getUploadDataSize());

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

DropboxErrorCode DropboxApi::uploadLargeFile(
    const DropboxUploadLargeFileRequest& req,
    DropboxMetadata& m) {
  string uploadId = "";
  size_t offset = req.getOffset();
  size_t size = 0;
  unique_ptr<uint8_t, void(*)(void*)> data(
    (uint8_t*)malloc(req.getChunkSize()), free);

  if (!data.get()) {
    throw std::bad_alloc();
  }

  do {
    shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(
      "https://api-content.dropbox.com/1/chunked_upload"));
    r->setMethod(HttpPutRequest);
    r->addIntegerParam("offset", offset);

    if (uploadId.compare("")) {
      r->addParam("upload_id", uploadId);
    }

    size = req.getData(data.get(), offset, req.getChunkSize());
    if (!size) {
      continue;
    }

    if (size < 0) {
      throw DropboxException(IO_ERROR, "Error receiving file data");
    }

    r->setRequestData(data.get(), size);
     
    DropboxErrorCode code = execute(r);
    if (code != SUCCESS) {
      return code;
    }

    string response((char *)r->getResponse(), r->getResponseSize());

    DropboxUploadLargeFileResponse res =
      DropboxUploadLargeFileResponse::readFromJson(response);
    uploadId = res.getUploadId();
    offset = res.getOffset();
  } while (size != 0);

  stringstream ss;
  ss << "https://api-content.dropbox.com/1/commit_chunked_upload/" << root_
    << "/" << req.getPath();

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));
  r->setMethod(HttpPostRequest);

  if (req.shouldOverwrite()) {
    r->addParam("overwrite", "true");
  } else {
    r->addParam("overwrite", "false");
  }

  if (req.getParentRev().compare("")) {
    r->addParam("parent_rev", req.getParentRev());
  }

  r->addParam("upload_id", uploadId);

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

DropboxErrorCode DropboxApi::search(const DropboxSearchRequest& req,
    DropboxSearchResult& res) {
  stringstream ss;
  ss << "https://api.dropbox.com/1/search/" << root_ << "/" 
    << req.getSearchPath();

  shared_ptr<HttpRequest> r(httpFactory_->createHttpRequest(ss.str()));

  r->addParam("query", req.getSearchQuery());
  r->addIntegerParam("file_limit", req.getResultLimit());

  if (req.shouldIncludeDeleted()) {
    r->addParam("include_deleted", "true");
  } else {
    r->addParam("include_deleted", "false");
  }

  DropboxErrorCode code = execute(r);

  if (code != SUCCESS) {
    return code;
  }

  string response((char *)r->getResponse(), r->getResponseSize());
  res = DropboxSearchResult::readFromJson(response);

  return code;
}
