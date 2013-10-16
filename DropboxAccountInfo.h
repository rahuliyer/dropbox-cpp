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

#ifndef __DROPBOX_ACCOUNT_INFO_H__
#define __DROPBOX_ACCOUNT_INFO_H__

#include <string>

#include <sys/types.h>
namespace dropbox {

struct DropboxQuotaInfo {
  uint64_t    shared;
  uint64_t    quota;
  uint64_t    normal;
};

class DropboxAccountInfo {
public:
  DropboxAccountInfo() { }
  DropboxAccountInfo(std::string& json);

  void            readJson(std::string&);

  std::string         getReferralLink() const;
  std::string         getDisplayName() const;
  std::string         getUid() const;
  std::string         getCountry() const;
  std::string         getEmail() const;
  DropboxQuotaInfo    getQuotaInfo() const;

private:
  static void     readFromJson(DropboxAccountInfo*, std::string& json);

  std::string           referralLink_;
  std::string           displayName_;
  std::string           uid_;
  std::string           country_;
  std::string           email_;
  DropboxQuotaInfo      quotaInfo_;
};
}
#endif
