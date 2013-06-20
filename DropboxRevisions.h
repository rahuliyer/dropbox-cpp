#ifndef __DROPBOX_REVISIONS_H__
#define __DROPBOX_REVISIONS_H__

#include "DropboxMetadataType.h"

#include <vector>

namespace dropbox {

class DropboxRevisions {
public:
  void                        readFromJson(std::string& json);
  std::vector<DropboxMetadata>&    getRevisions();

private:
  std::vector<DropboxMetadata>       revisions_;
};
}
#endif
