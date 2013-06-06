#include "HttpRequestFactory.h"
#include "HttpRequest.h"
#include "OAuthException.h"
#include "OAuth.h"

#include <sstream>

using namespace oauth;
using namespace std;
using namespace http;

OAuth::OAuth(string key,
  string secret, string version, OAuthSecurityMethod method) :
    consumerKey_(key), 
    consumerSecret_(secret), 
    securityMethod_(method),
    oauthVersion_(version),
    requestFactory_(HttpRequestFactory::createFactory()) {
}

void OAuth::splitParams(string& response, map<string, string>& params) {
  size_t idx = 0;
  size_t len  = response.size();

  do {
    size_t ampPos = response.find("&", idx);
    if (ampPos == string::npos) {
      ampPos = len;
    }

    string paramStr = response.substr(idx, ampPos - idx);
    size_t eqPos = paramStr.find("=");
    if (eqPos == string::npos) {
      throw OAuthException(MalformedOAuthResponse, "Malformed OAuth response");
    }

    string key = paramStr.substr(0, eqPos);
    string value = paramStr.substr(eqPos + 1);

    params[key] = value;

    idx = ampPos + 1;
  } while(idx < len);
}

void OAuth::getTokenAndSecret(string& response, string& token, string& secret) {
  map<string, string> params;

  splitParams(response, params);

  auto i = params.find("oauth_token");
  if (i == params.end()) {
    throw OAuthException(MalformedOAuthResponse, "Malformed OAuth response");
  }

  token = i->second;

  i = params.find("oauth_token_secret");
  if (i == params.end()) {
    throw OAuthException(MalformedOAuthResponse, "Malformed OAuth response");
  }

  secret = i->second;
}

void OAuth::addOAuthHeader(HttpRequest* r, string token, string secret) const {
  stringstream ss;

  ss << "OAuth oauth_version=\"" << oauthVersion_ << "\", ";
  ss << "oauth_consumer_key=\"" << consumerKey_ << "\", ";

  if (token.compare("")) {
    ss << "oauth_token=\"" << token << "\", ";
  }

  string signatureMethod;
  string signature;
  switch (securityMethod_) {
    case OAuthSecPlaintext:
      {
        signatureMethod="PLAINTEXT";
        stringstream sst;
        sst << consumerSecret_ << "&" << secret;
        signature = sst.str();
      }
      break;
    default:
      throw OAuthException(UnsupportedMethod, 
        "Only Plaintext signature supported");  
  }
  
  ss << "oauth_signature_method=\"" << signatureMethod << "\", ";
  ss << "oauth_signature=\"" << signature << "\"";

  string header = ss.str();
  r->addHeader("Authorization", header);
} 

void OAuth::fetchRequestToken(string url) {
  unique_ptr<HttpRequest> r(requestFactory_->createHttpRequest(url)); 

  r->setMethod(HttpPostRequest);
 
  addOAuthHeader(r.get(), "", "");

  if (r->execute() || r->getResponseCode() != 200) {
    stringstream ss;
    ss << "Got http error " << r->getResponseCode();

    throw OAuthException(HttpRequestFailed, ss.str());
  }

  string response((char *)r->getResponse(), r->getResponseSize());
  getTokenAndSecret(response, requestToken_, requestSecret_);
}

void OAuth::fetchAccessToken(string url) {
  unique_ptr<HttpRequest> r(requestFactory_->createHttpRequest(url)); 

  r->setMethod(HttpPostRequest);
 
  addOAuthHeader(r.get(), requestToken_, requestSecret_);

  if (r->execute() || r->getResponseCode() != 200) {
    stringstream ss;
    ss << "Got http error " << r->getResponseCode();

    throw OAuthException(HttpRequestFailed, ss.str());
  }

  string response((char *)r->getResponse(), r->getResponseSize());
  getTokenAndSecret(response, accessToken_, accessSecret_);
}

string OAuth::getRequestToken() const {
  return requestToken_;
}

string OAuth::getRequestTokenSecret() const {
  return requestSecret_;
}

string OAuth::getAccessToken() const {
  return accessToken_;
}

string OAuth::getAccessTokenSecret() const {
  return accessSecret_;
}

void OAuth::setAccessToken(string token) {
  accessToken_ = token;
}

void OAuth::setAccessTokenSecret(string secret) {
  accessSecret_ = secret;
}

void OAuth::addOAuthAccessHeader(HttpRequest* r) const {
  addOAuthHeader(r, accessToken_, accessSecret_);
}
