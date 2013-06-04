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
    OAuthSecurityMethod method = OAuthSecPlaintext);

  void            fetchRequestToken(std::string url);
  void            fetchAccessToken(std::string url);

  std::string     getRequestToken();
  std::string     getRequestTokenSecret();

  std::string     getAccessToken();
  std::string     getAccessTokenSecret();

  void            setAccessToken(std::string);
  void            setAccessTokenSecret(std::string);

  void            addOAuthAccessHeader(http::HttpRequest*);

private:
  void addOAuthHeader(http::HttpRequest*, std::string, std::string);
  void getTokenAndSecret(std::string&, std::string&, std::string&);
  void splitParams(std::string&, std::map<std::string, std::string>&);

  const std::string           consumerKey_;
  const std::string           consumerSecret_;
  OAuthSecurityMethod         securityMethod_;
  std::string                 oauthVersion_;

  http::HttpRequestFactory*   requestFactory_;
  
  std::string                 requestToken_;
  std::string                 requestSecret_;

  std::string                 accessToken_;
  std::string                 accessSecret_;
};
}

