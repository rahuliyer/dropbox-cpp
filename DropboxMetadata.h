#ifndef __DROPBOX_METADATA_H__
#define __DROPBOX_METADATA_H__

#include "DropboxMetadataType.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <string>
#include <vector>

namespace dropbox {

class DropboxMetadataRequest {
public:
  DropboxMetadataRequest(const std::string path,
    const bool includeChildren = false,
    const bool includeDeleted = false);

  void          setLimit(const size_t);
  size_t        getLimit() const;

  void          setHash(const std::string);
  std::string   getHash() const;

  void          setRev(const std::string);
  std::string   getRev() const;

  std::string   path() const;
  bool          includeDeleted() const;
  bool          includeChildren() const;

private:
  const std::string         path_;
  size_t                    fileLimit_;
  std::string               hash_;
  bool                      includeChildren_;
  bool                      includeDeleted_;
  std::string               rev_;
};

class DropboxMetadataResponse {
public:
  DropboxMetadataResponse();
  
  void                                  readJson(const std::string&);
  DropboxMetadata&                      getMetadata();
  const std::vector<DropboxMetadata>&   getChildren() const;
  
private:
  void    readMetadataFromJson(boost::property_tree::ptree&, DropboxMetadata&);

  DropboxMetadata                 metadata_;
  std::vector<DropboxMetadata>    children_;
};
}
#endif
