#include "DropboxAccountInfo.h"
#include "DropboxException.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>

using namespace dropbox;
using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

AccountInfo::AccountInfo(string& json) {
  try {
    stringstream ss;
    ss << json;

    ptree pt;
    read_json(ss, pt);

    referralLink_ = pt.get<string>("referral_link");
    displayName_ = pt.get<string>("display_name");
    uid_ = pt.get<string>("uid");
    country_ = pt.get<string>("country");
    email_ = pt.get<string>("email");

    quotaInfo_.shared = pt.get<uint64_t>("quota_info.shared");
    quotaInfo_.quota = pt.get<uint64_t>("quota_info.quota");
    quotaInfo_.normal = pt.get<uint64_t>("quota_info.normal");
  } catch (exception& e) {
    throw DropboxException(MalformedResponse, e.what());
  }
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
