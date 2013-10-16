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

#include "DropboxAccountInfo.h"
#include "DropboxException.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>

using namespace dropbox;
using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

void DropboxAccountInfo::readFromJson(DropboxAccountInfo* info, string& json) {
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

DropboxAccountInfo::DropboxAccountInfo(string& json) {
  readFromJson(this, json);
}

void DropboxAccountInfo::readJson(string& json) {
  readFromJson(this, json);
}

string DropboxAccountInfo::getReferralLink() const {
  return referralLink_;
}

string DropboxAccountInfo::getDisplayName() const {
  return displayName_;
}

string DropboxAccountInfo::getUid() const {
  return uid_;
}

string DropboxAccountInfo::getCountry() const {
  return country_;
}

string DropboxAccountInfo::getEmail() const {
  return email_;
}

DropboxQuotaInfo DropboxAccountInfo::getQuotaInfo() const {
  return quotaInfo_;
}
