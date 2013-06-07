#include "DropboxAccountInfo.h"
#include "DropboxException.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>

using namespace dropbox;
using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

void AccountInfo::readFromJson(AccountInfo* info, string& json) {
  try {
    stringstream ss;
    ss << json;

    ptree pt;
    read_json(ss, pt);

    info->referralLink_ = pt.get<string>("referral_link");
    info->displayName_ = pt.get<string>("display_name");
    info->uid_ = pt.get<string>("uid");
    info->country_ = pt.get<string>("country");
    info->email_ = pt.get<string>("email");

    info->quotaInfo_.shared = pt.get<uint64_t>("quota_info.shared");
    info->quotaInfo_.quota = pt.get<uint64_t>("quota_info.quota");
    info->quotaInfo_.normal = pt.get<uint64_t>("quota_info.normal");
  } catch (exception& e) {
    throw DropboxException(MALFORMED_RESPONSE, e.what());
  }
}

AccountInfo::AccountInfo(string& json) {
  readFromJson(this, json);
}

void AccountInfo::readJson(string& json) {
  readFromJson(this, json);
}

string AccountInfo::getReferralLink() const {
  return referralLink_;
}

string AccountInfo::getDisplayName() const {
  return displayName_;
}

string AccountInfo::getUid() const {
  return uid_;
}

string AccountInfo::getCountry() const {
  return country_;
}

string AccountInfo::getEmail() const {
  return email_;
}

QuotaInfo AccountInfo::getQuotaInfo() const {
  return quotaInfo_;
}
