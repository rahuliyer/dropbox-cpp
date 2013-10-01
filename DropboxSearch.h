#ifndef __DROPBOX_SEARCH_H__
#define __DROPBOX_SEARCH_H__

#include <DropboxMetadataType.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class DropboxSearchRequest {
public:
  DropboxSearchRequest(std::string path,
    std::string query,
    bool include_deleted,
    size_t limit = 1000) :
      path_(path),
      query_(query),
      includeDeleted_(include_deleted),
      limit_(limit) {
  }

  std::string getSearchPath() const {
    return path_;
  }

  std::string getSearchQuery() const {
    return query_;
  }

  size_t getResultLimit() const {
    return limit_;
  }

  bool shouldIncludeDeleted() const {
    return includeDeleted_;
  }

private:
  const std::string   path_;
  const std::string   query_;
  const bool          includeDeleted_;
  const size_t        limit_;
};

class DropboxSearchResult {
public:
  DropboxSearchResult() {
  }

  static DropboxSearchResult readFromJson(const std::string& json) {
    using namespace std;
    using namespace dropbox;
    using namespace boost::property_tree;
    using namespace boost::property_tree::json_parser;

    stringstream ss;
    ss << json;

    ptree pt;
    read_json(ss, pt);

    vector<DropboxMetadata> v;
    DropboxMetadata::readMetadataListFromJson(pt, v);

    return DropboxSearchResult(v);
  }

  std::vector<dropbox::DropboxMetadata> const& getResults() const {
    return results_;
  }

private:
  DropboxSearchResult(const std::vector<dropbox::DropboxMetadata>& res) :
    results_(res) {
  }

  std::vector<dropbox::DropboxMetadata>  results_;
};

#endif
