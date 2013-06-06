#ifndef __DROPBOX_ACCOUNT_INFO_H__
#define __DROPBOX_ACCOUNT_INFO_H__

#include <string>

#include <sys/types.h>
namespace dropbox {

struct QuotaInfo {
  uint64_t    shared;
  uint64_t    quota;
  uint64_t    normal;
};

class AccountInfo {
public:
  AccountInfo(std::string& json);

  std::string     getReferralLink() const;
  std::string     getDisplayName() const;
  std::string     getUid() const;
  std::string     getCountry() const;
  std::string     getEmail() const;
  QuotaInfo       getQuotaInfo() const;

private:
  std::string           referralLink_;
  std::string           displayName_;
  std::string           uid_;
  std::string           country_;
  std::string           email_;
  QuotaInfo             quotaInfo_;
};
}
#endif
