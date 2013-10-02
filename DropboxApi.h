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
#include "DropboxUploadLargeFile.h"
#include "DropboxSearch.h"

#include <string>
#include <memory>
#include <mutex>

namespace dropbox {

#define DROPBOX_ROOT "dropbox"
#define SANDBOX_ROOT "sandbox"

class DropboxApi {
public:
  /**
   * Create an instance of the Dropbox API class
   *
   * @param appKey        Your app key
   * @param appSecret     Your app secret
   */
  DropboxApi(const std::string appKey, const std::string appSecret);

  /**
   * Create an instance of the Dropbox API class with an access token
   *
   * @param appKey        Your app key
   * @param appSecret     Your app secret
   * @param accessToken   Access token for the user's account
   * @param tokenSecret   Access token secret for the user's account
   */
  DropboxApi(const std::string appKey, 
    const std::string appSecret,
    const std::string accessToken,
    const std::string tokenSecret);

  /**
   * Get an access token and an access token secret for the given user
   *
   * @param cb            Callback function to be called to authorize the 
   *                      request token. The request token and request token
   *                      secret are passed as arguments to the callback
   *
   * @return void
   */
  void authenticate(std::function<void(const std::string reqToken, 
    const std::string reqSecret)>);

  /**
   * Set the access token and access token secret if you already have one
   * 
   * @param token         The access token
   * @param secret        The access token secret
   *
   * @return void
   */
  void setAccessToken(std::string token, std::string secret);

  /**
   * Get the access token for the user
   * @param none
   *
   * @return access token
   */
  std::string getAccessToken();

  /**
   * Get the access token secret for the user
   * @param none
   *
   * @return access token secret
   */
  std::string getAccessTokenSecret();

  /**
   * Set the root directory for the client. Valid values are DROPBOX_ROOT and 
   * SANDBOX_ROOT.
   *
   * @param root            The root directory
   *
   * @return void
   */
  void setRoot(const std::string);

  /**
   * Get account info for the user. This method calls the /account/info method
   * of the core API.
   *
   * @param info            Output param in which the account info is returned
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode getAccountInfo(DropboxAccountInfo& info);

  /**
   * Get file data. This method calls the /files (GET) method of the core API
   *
   * @param req             DropboxGetFileRequest object containing request 
   *                        params
   * @param res             Output parameter; an object of type
   *                        DropboxGetFileResponse that has the response
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode getFile(DropboxGetFileRequest& req,
    DropboxGetFileResponse& res);

  /**
   * Get the metadata for a file. This call can also be used to get the
   * children if the specified file is a directory. This method calls the 
   * /metadata method of the core API.
   *
   * @param req             An object of type DropboxMetadataRequest that has 
   *                        the params for the request
   * @param res             Output param of type DropboxMetadataResponse that
   *                        holds the result
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode getFileMetadata(DropboxMetadataRequest& req, 
    DropboxMetadataResponse& res);

  /**
   * Get the revisions for a file. This method calls the /revisions method of
   * the core API.
   *
   * @param path            Path of the file relative to the root
   * @param numRevisions    Number of revisions desired. Max is 1000; 
   *                        default is 10
   * @param revs            Output param of type DropboxRevisions that holds
   *                        the result
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode getRevisions(std::string path, 
    size_t numRevisions, 
    DropboxRevisions& revs);

  /**
   * Restore a file to a given revision. This method calls the /restore method
   * of the core API.
   *
   * @param path            Path of the file relative to the root
   * @param rev             The id of the revision to restore the file to
   * @param m               Output param of type DropboxMetadata that holds the
   *                        metadata of the revision restored to.
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode restoreFile(std::string path, std::string rev,
    DropboxMetadata& m);

  /**
   * Delete a given file or directory. This method calls the /delete method of
   * the core API.
   *
   * @param path            Path of the file relative to the root
   * @param m               Output param of type DropboxMetadata that holds the
   *                        metadata of the file that was deleted
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode deleteFile(std::string path, DropboxMetadata& m);
 
  /**
   * Make a copy of a file. This method calls the /copy method of the core API
   *
   * @param from            The path of the source file relative to root
   * @param to              The path of the destination file relative to root
   * @param m               Output param of type DropboxMetadata that contains
   *                        the metadata of the destination file
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode copyFile(std::string from, std::string to,
    DropboxMetadata& m);

  /**
   * Move a file or directory. This method calls the /move method in the core
   * API.
   *
   * @param from            The path of the source file relative to root
   * @param to              The path of the destination file relative to root
   * @param m               Output param of type DropboxMetadata that contains
   *                        the metadata of the destination file
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode moveFile(std::string from, std::string to,
    DropboxMetadata& m);

  /**
   * Create a folder. This method calls the /create_folder method of the core
   * API.
   *
   * @param path            Path of the folder relative to root
   * @param m               Output param of type DropboxMetadata that contains
   *                        the metadata of the folder created
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode createFolder(const std::string path, DropboxMetadata& m);

  /**
   * Upload a file. This calls the /files_put method of the core API.
   * 
   * @param req             Object of type DropboxUploadFileRequest that 
   *                        contains the params for the request
   * @param res             Output param of type DropboxMetadata that contains
   *                        the metadata of the uploaded file
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode uploadFile(const DropboxUploadFileRequest& req,
    DropboxMetadata& res);

  /**
   * Upload a large file. Dropbox defines "large" to be files greater than
   * 150 MB in size. This uses the /chunked_upload and /commit_chunked_upload
   * methods of the core API
   *
   * @param req             Object of type DropboxUploadLargeFileRequest that 
   *                        contains the params for the request
   * @param res             Output param of type DropboxMetadata that contains
   *                        the metadata of the uploaded file
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode uploadLargeFile(const DropboxUploadLargeFileRequest& req,
    DropboxMetadata& res);

  /**
   * Search for files matching a given string pattern. This calls the /search
   * method in the core API.
   * 
   * @param req             An object of type DropboxSearchRequest that contains
   *                        the params of for the search request
   * @param res             Output param of type DropboxSearchResult that has
   *                        the search results
   *
   * @return Error code for the operation. See DropboxErrorCode for values
   */
  DropboxErrorCode search(const DropboxSearchRequest&, DropboxSearchResult&);

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
