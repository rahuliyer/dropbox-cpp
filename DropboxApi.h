#ifndef __DROPBOX_API_H__
#define __DROPBOX_API_H__

#include "util/OAuth.h"
#include "util/HttpRequestFactory.h"

#include "DropboxException.h"
#include "DropboxAccountInfo.h"

#include <string>
#include <memory>
#include <mutex>

namespace dropbox {
class DropboxApi {
public:
  DropboxApi(const std::string appKey, const std::string appSecret);
  DropboxApi(const std::string appKey, 
    const std::string appSecret,
    const std::string accessToken,
    const std::string tokenSecret);

  void authenticate(std::function<void(const std::string reqToken, 
    const std::string reqSecret)>);

  void setAccessToken(std::string token, std::string secret);

  DropboxErrorCode getAccountInfo(AccountInfo&);

private:
  std::mutex                      stateLock_;
  std::unique_ptr<oauth::OAuth>   oauth_;
  http::HttpRequestFactory*       httpFactory_;
};
}
#endif
