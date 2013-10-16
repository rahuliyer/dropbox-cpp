/*
* Copyright (c) 2013 Rahul Iyer
* All rights reserved.
* 
* Redistribution and use in source and binary forms are permitted provided that
* the above copyright notice and this paragraph are duplicated in all such forms
* and that any documentation, advertising materials, and other materials related
* to such distribution and use acknowledge that the software was developed by 
* Rahul Iyer.  The name of Rahul Iyer may not be used to endorse or promote 
* products derived from this software without specific prior written permission.
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

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
