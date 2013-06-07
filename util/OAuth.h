#ifndef __OAUTH_H__
#define __OAUTH_H__

#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"

#include <memory>
#include <map>
#include <string>

namespace oauth {
enum OAuthSecurityMethod {
  OAuthSecPlaintext,
};

class OAuth {
public:
  OAuth(const std::string consumer_key, 
    const std::string consumer_secret,
    const std::string version = "1.0",
    OAuthSecurityMethod method = OAuthSecPlaintext);

  void            fetchRequestToken(std::string url);
  void            fetchAccessToken(std::string url);

  std::string     getRequestToken() const;
  std::string     getRequestTokenSecret() const;

  std::string     getAccessToken() const;
  std::string     getAccessTokenSecret() const;

  void            setAccessToken(std::string);
  void            setAccessTokenSecret(std::string);

  void            addOAuthAccessHeader(http::HttpRequest*) const;

private:
  void addOAuthHeader(http::HttpRequest*, std::string, std::string) const;
  void getTokenAndSecret(std::string&, std::string&, std::string&);
  void splitParams(std::string&, std::map<std::string, std::string>&);

  const std::string                 consumerKey_;
  const std::string                 consumerSecret_;
  const OAuthSecurityMethod         securityMethod_;
  const std::string                 oauthVersion_;

  http::HttpRequestFactory* const   requestFactory_;
  
  std::string                       requestToken_;
  std::string                       requestSecret_;

  std::string                       accessToken_;
  std::string                       accessSecret_;
};
}

#endif
