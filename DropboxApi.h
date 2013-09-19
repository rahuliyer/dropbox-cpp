#ifndef __DROPBOX_API_H__
#define __DROPBOX_API_H__

#include "util/OAuth.h"
#include "util/HttpRequestFactory.h"
#include "util/HttpRequest.h"

#include "DropboxException.h"
#include "DropboxAccountInfo.h"
#include "DropboxMetadata.h"
#include "DropboxRevisions.h"
#include "DropboxGetFile.h"
#include "DropboxUploadFile.h"

#include <string>
#include <memory>
#include <mutex>

namespace dropbox {

#define DROPBOX_ROOT "dropbox"
#define SANDBOX_ROOT "sandbox"

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

  void setRoot(const std::string);

  DropboxErrorCode getAccountInfo(AccountInfo&);

  DropboxErrorCode getFile(DropboxGetFileRequest&, DropboxGetFileResponse&);

  DropboxErrorCode getFileMetadata(DropboxMetadataRequest&, 
    DropboxMetadataResponse&);

  DropboxErrorCode getRevisions(std::string path, 
    size_t numRevisions, 
    DropboxRevisions&);

  DropboxErrorCode restoreFile(std::string path, std::string, DropboxMetadata&);

  DropboxErrorCode deleteFile(std::string path, DropboxMetadata&);
  
  DropboxErrorCode copyFile(std::string from, std::string to, DropboxMetadata&);
  DropboxErrorCode moveFile(std::string from, std::string to, DropboxMetadata&);

  DropboxErrorCode createFolder(const std::string path, DropboxMetadata&);

  DropboxErrorCode uploadFile(const DropboxUploadFileRequest&,
    DropboxMetadata&);

private:
  DropboxErrorCode  copyOrMove(const std::string,
    const std::string,
    const std::string,
    DropboxMetadata&);
  DropboxErrorCode  execute(std::shared_ptr<http::HttpRequest>);

  std::string                     root_;
  std::mutex                      stateLock_;
  std::unique_ptr<oauth::OAuth>   oauth_;
  http::HttpRequestFactory*       httpFactory_;
};
}
#endif
