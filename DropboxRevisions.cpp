#include "DropboxRevisions.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace dropbox;
using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

void DropboxRevisions::readFromJson(string& json) {
  try {
    stringstream ss;
    ss << json;

    ptree pt;
    read_json(ss, pt);

    BOOST_FOREACH(ptree::value_type& v, pt) {
      DropboxMetadata m;
      DropboxMetadata::readFromJson(v.second, m);

      revisions_.push_back(m);
    }
  } catch (exception& e) {
    throw;
  }
}

vector<DropboxMetadata>& DropboxRevisions::getRevisions() {
  return revisions_;
}
