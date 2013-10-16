#ifndef __OAUTH_H__
#define __OAUTH_H__

#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"

#include <memory>
#include <map>
#include <string>

namespace oauth {
// Currently only supports plaintext
enum OAuthSecurityMethod {
  OAuthSecPlaintext,
};

class OAuth {
public:
  /**
   * Create an OAuth instance
   *
   * @param consumer_key      The API key/app id for your app
   * @param consumer_secret   The API secret for your app
   * @param version           OAuth version
   * @param method            The OAUth security method defined in
   *                          OAuthSecurityMethod
   */
  OAuth(const std::string consumer_key,
    const std::string consumer_secret,
    const std::string version = "1.0",
    OAuthSecurityMethod method = OAuthSecPlaintext);

  /**
   * Get the request token for the app. This corresponds to the
   * "Obtaining an unauthorized request token" stage of the OAuth specification
   * For more information see:
   * http://oauth.net/core/1.0/#auth_step1
   *
   * @param url   The service provider url to use to fetch the request token
   */
  void            fetchRequestToken(std::string url);

  /**
   * Get the access token used to authenticate API calls. This corresponds to
   * the "Obtaining an access token" stage in the OAuth specification. For
   * more information see:
   * http://oauth.net/core/1.0/#auth_step3
   *
   * NOTE: This api call assumes that the "Obtaining user authorization" step
   * in the OAuth specification (http://oauth.net/core/1.0/#auth_step2) is
   * complete. Since this step isn't standard across service providers, it is
   * assumed that this step is done out of band.
   *
   * @param url     The service provider url to fetch the access token
   */
  void            fetchAccessToken(std::string url);

  /**
   * Return the request token obtained via fetchRequestToken
   *
   * @return string   The request token obtained
   */
  std::string     getRequestToken() const;

  /**
   * Return the request token secret obtained via fetchRequestToken
   *
   * @return string   The request token secret
   */
  std::string     getRequestTokenSecret() const;

  /**
   * Return the access token obtained via fetchAccessToken
   *
   * @return string   The access token obtained
   */
  std::string     getAccessToken() const;

  /**
   * Return the access token secret obtained via fetchAccessToken
   *
   * @return string   The access token secret
   */
  std::string     getAccessTokenSecret() const;

  /**
   * Set the access token. Use this method when you have a preauthenticated
   * access token that you want to use to make API calls
   *
   * @param string    accessToken  The pre-authenticated access token
   */
  void            setAccessToken(std::string accessToken);

  /**
   * Set the access token secret. Use this method when you have a
   * preauthenticated access token secret that you want to use to make API calls
   *
   * @param string    tokenSecret  The pre-authenticated access token
   */
  void            setAccessTokenSecret(std::string tokenSecret);

  /**
   * Adds an OAuth authentication header to the supplied HTTP request. Use thus
   * method to add authentication to the service provider api calls you make
   *
   * @param   HttpRequest* request  The HttpRequest to add the authentication
   *                                header to
   */
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
