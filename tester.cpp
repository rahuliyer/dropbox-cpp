#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <cassert>

#include "DropboxAccountInfo.h"
#include "DropboxMetadata.h"
#include "DropboxApi.h"

using namespace std;
using namespace std::placeholders;
using namespace dropbox;

// Globals
const string TEST_DIR = "/testdir";
const size_t SIZE = (1 << 20);
const size_t LARGE_SIZE = 2 * (1 << 20) + 2;
DropboxApi* d;

class AuthorizationHelper {
public:
  AuthorizationHelper() { }

  void authorize(string token, string secret) {
    cout << "Request token: " << token << endl;
    cout << "Request token secret: " << secret << endl;

    cout << "Go to https://www.dropbox.com/1/oauth/authorize?oauth_token="
      << token << " to authorize" << endl;
    cout << "Hit any key to continue after authorization" << endl;
    cin.get();
  }
};

TEST(AccountInfoTestCase, AccountInfoTest) {
  AccountInfo ac;
  DropboxErrorCode code = d->getAccountInfo(ac);

  EXPECT_EQ(SUCCESS, code);
  EXPECT_STREQ(getenv("DROPBOX_ACCOUNT_NAME"), ac.getDisplayName().c_str());
  EXPECT_STREQ(getenv("DROPBOX_ACCOUNT_EMAIL"), ac.getEmail().c_str());
}

class BaseDropboxTestCase : public ::testing::Test {
protected:
  static void SetUpTestCase() {
    DropboxMetadata m;
    DropboxErrorCode code = d->createFolder(TEST_DIR, m);
    assert(code == SUCCESS);
  }

  static void TearDownTestCase() {
    DropboxMetadata m;
    DropboxErrorCode code = d->deleteFile(TEST_DIR, m);
    assert(code == SUCCESS);
  }
};

class DropboxFolderTestCase : public BaseDropboxTestCase {
public:
  void SetUp() {
    folderName_ = TEST_DIR + "/testdir";
    code_ = d->createFolder(folderName_, md_);
  }

  void TearDown() {
  }

  string folderName_;
  DropboxErrorCode code_;
  DropboxMetadata md_;
};

TEST_F(DropboxFolderTestCase, CreateFolderTest) {
  EXPECT_EQ(SUCCESS, code_);
  EXPECT_STREQ(folderName_.c_str(), md_.path_.c_str());
  EXPECT_EQ(true, md_.isDir_);
  EXPECT_NE(true, md_.isDeleted_);
}

TEST_F(DropboxFolderTestCase, DeleteFolderTest) {
  DropboxMetadata m;
  DropboxErrorCode code = d->deleteFile(folderName_, m);
  EXPECT_EQ(SUCCESS, code);
  EXPECT_STREQ(folderName_.c_str(), m.path_.c_str());
  EXPECT_EQ(true, m.isDir_);
  EXPECT_EQ(true, m.isDeleted_);
}

class DropboxFileTestCase : public BaseDropboxTestCase {
public:
  static uint8_t* getRandomData(size_t size) {
    int fd = open("/dev/urandom", O_RDONLY);
    assert(fd > 0);
    
    uint8_t* p = (uint8_t *)malloc(size);
    assert(p);

    size_t rem = size;
    size_t offset = 0;
    while (rem) {
      int ret = read(fd, p + offset, rem);
      assert(ret > 0);
      offset += ret;
      rem -= ret;
    }

    close(fd);

    return p;
  }

  void SetUp() {
    fileName_ = TEST_DIR + "/testfile";
    DropboxUploadFileRequest up_req(fileName_);
    data_ = getRandomData(SIZE);
    up_req.setUploadData(data_, SIZE);

    code_ = d->uploadFile(up_req, md_);
  }

  void TearDown() {
    free(data_);
  }

  uint8_t* data_;
  string fileName_;
  DropboxErrorCode code_;
  DropboxMetadata md_;
};

TEST_F(DropboxFileTestCase, UploadFileTest) {
  EXPECT_EQ(code_, SUCCESS);
  EXPECT_STREQ(fileName_.c_str(), md_.path_.c_str());
  EXPECT_NE(true, md_.isDir_);
  EXPECT_NE(true, md_.isDeleted_);
  EXPECT_EQ(SIZE, md_.sizeBytes_);
}

TEST_F(DropboxFileTestCase, NonOverWriteTestCase) {
  DropboxUploadFileRequest up_req(fileName_);
  up_req.setUploadData(getRandomData(SIZE), SIZE);
  up_req.setOverwrite(false);

  DropboxMetadata m;
  DropboxErrorCode code = d->uploadFile(up_req, m);

  EXPECT_EQ(code, SUCCESS);
  EXPECT_STRNE(fileName_.c_str(), m.path_.c_str());
  EXPECT_NE(true, m.isDir_);
  EXPECT_NE(true, m.isDeleted_);
  EXPECT_EQ(SIZE, m.sizeBytes_);
}

TEST_F(DropboxFileTestCase, CopyFileTestCase) {
  string copy_filename = fileName_ + ".bk";
  DropboxMetadata m;
  DropboxErrorCode code = d->copyFile(fileName_, copy_filename, m);

  EXPECT_EQ(code, SUCCESS);
  EXPECT_STREQ(copy_filename.c_str(), m.path_.c_str());
  EXPECT_EQ(md_.isDir_, m.isDir_);
  EXPECT_EQ(md_.isDeleted_, m.isDeleted_);
  EXPECT_EQ(md_.sizeBytes_, m.sizeBytes_);
}

TEST_F(DropboxFileTestCase, MoveFileTestCase) {
  string copy_filename = fileName_ + ".bk2";
  DropboxMetadata m;
  DropboxErrorCode code = d->moveFile(fileName_, copy_filename, m);

  EXPECT_EQ(code, SUCCESS);
  EXPECT_STREQ(copy_filename.c_str(), m.path_.c_str());
  EXPECT_EQ(md_.isDir_, m.isDir_);
  EXPECT_EQ(md_.isDeleted_, m.isDeleted_);
  EXPECT_EQ(md_.sizeBytes_, m.sizeBytes_);
}

TEST_F(DropboxFileTestCase, GetFileTestCase) {
  DropboxMetadata m;
  DropboxGetFileRequest gfreq(fileName_);
  DropboxGetFileResponse gfres;

  DropboxErrorCode code = d->getFile(gfreq, gfres);
  m = gfres.getMetadata();

  EXPECT_EQ(code, SUCCESS);
  EXPECT_EQ(SIZE, gfres.getDataLength());
  EXPECT_EQ(0, memcmp(data_, gfres.getData(), SIZE));
  EXPECT_STREQ(fileName_.c_str(), m.path_.c_str());
  EXPECT_EQ(md_.isDir_, m.isDir_);
  EXPECT_EQ(md_.isDeleted_, m.isDeleted_);
  EXPECT_EQ(md_.sizeBytes_, m.sizeBytes_);
}

TEST_F(DropboxFileTestCase, PartialGetFileTestCase) {
  DropboxMetadata m;
  DropboxGetFileRequest gfreq(fileName_);
  DropboxGetFileResponse gfres;
  size_t offset = 1177;
  size_t len = 6656;

  gfreq.setRange(offset, len);
  DropboxErrorCode code = d->getFile(gfreq, gfres);
  m = gfres.getMetadata();

  EXPECT_EQ(code, PARTIAL_CONTENT);
  EXPECT_EQ(len, gfres.getDataLength());
  EXPECT_EQ(0, memcmp(data_ + offset, gfres.getData(), len));
  EXPECT_STREQ(fileName_.c_str(), m.path_.c_str());
  EXPECT_EQ(md_.isDir_, m.isDir_);
  EXPECT_EQ(md_.isDeleted_, m.isDeleted_);
  EXPECT_EQ(md_.sizeBytes_, m.sizeBytes_);
}

TEST_F(DropboxFileTestCase, DeleteFileTest) {
  DropboxMetadata m;
  DropboxErrorCode code = d->deleteFile(fileName_, m);
  EXPECT_EQ(SUCCESS, code);
  EXPECT_STREQ(fileName_.c_str(), m.path_.c_str());
  EXPECT_NE(true, m.isDir_);
  EXPECT_EQ(true, m.isDeleted_);
}

class DropboxLargeFileTestCase : public BaseDropboxTestCase {
public:
  void SetUp() {
    fileName_ = TEST_DIR + "/largetestfile";
    data_ = DropboxFileTestCase::getRandomData(LARGE_SIZE);
    size_t rem_size = LARGE_SIZE;
    auto cb = [&](uint8_t* buf, size_t off, size_t size) {
      size_t fetched_size = 0;

      if (size < rem_size) {
        fetched_size = size;
      } else {
        fetched_size = rem_size;
      }
      rem_size -= fetched_size;
      
      memcpy(buf, data_ + off, fetched_size);

      return fetched_size;
    };

    DropboxUploadLargeFileRequest req(fileName_, cb, true, "", SIZE, 0);
    code_ = d->uploadLargeFile(req, md_);
  }
  
  void TearDown() {
    free(data_);
  }

  uint8_t*          data_;
  string            fileName_;
  DropboxErrorCode  code_;
  DropboxMetadata   md_;
};

TEST_F(DropboxLargeFileTestCase, UploadFileTest) {
  EXPECT_EQ(code_, SUCCESS);
  EXPECT_STREQ(fileName_.c_str(), md_.path_.c_str());
  EXPECT_NE(true, md_.isDir_);
  EXPECT_NE(true, md_.isDeleted_);
  EXPECT_EQ(LARGE_SIZE, md_.sizeBytes_);
}

TEST_F(DropboxLargeFileTestCase, NonOverWriteTestCase) {
  auto cb = [&](uint8_t* buf, size_t offset, size_t sz) {
    if (offset + sz < LARGE_SIZE) {
      return sz;
    } else {
      return LARGE_SIZE - offset;
    }
  };

  DropboxUploadLargeFileRequest req(fileName_, cb,
    false, "", LARGE_SIZE, 0);

  DropboxMetadata m;
  DropboxErrorCode code = d->uploadLargeFile(req, m);

  EXPECT_EQ(code, SUCCESS);
  EXPECT_STRNE(fileName_.c_str(), m.path_.c_str());
  EXPECT_NE(true, m.isDir_);
  EXPECT_NE(true, m.isDeleted_);
  EXPECT_EQ(LARGE_SIZE, m.sizeBytes_);
}

TEST_F(DropboxLargeFileTestCase, GetFileTestCase) {
  DropboxMetadata m;
  DropboxGetFileRequest gfreq(fileName_);
  DropboxGetFileResponse gfres;

  DropboxErrorCode code = d->getFile(gfreq, gfres);
  m = gfres.getMetadata();

  EXPECT_EQ(code, SUCCESS);
  EXPECT_EQ(LARGE_SIZE, gfres.getDataLength());
  EXPECT_EQ(0, memcmp(data_, gfres.getData(), LARGE_SIZE));
  EXPECT_STREQ(fileName_.c_str(), m.path_.c_str());
  EXPECT_EQ(md_.isDir_, m.isDir_);
  EXPECT_EQ(md_.isDeleted_, m.isDeleted_);
  EXPECT_EQ(md_.sizeBytes_, m.sizeBytes_);
}

class DropboxMetadataOpsTestCase : public BaseDropboxTestCase {
public:
  void SetUp() {
    fileName_ = TEST_DIR + "/testfile";
    DropboxUploadFileRequest up_req(fileName_);
    data_ = DropboxFileTestCase::getRandomData(SIZE);
    up_req.setUploadData(data_, SIZE);

    code_ = d->uploadFile(up_req, md_);
    assert(code_ == SUCCESS);

    code_ = d->copyFile(fileName_, fileName_ + "_1", md_);

    code_ = d->copyFile(fileName_, fileName_ + "_2", md_);

    code_ = d->deleteFile(fileName_ + "_2", md_);
  }

  void TearDown() {
    free(data_);
  }

  uint8_t* data_;
  string fileName_;
  DropboxErrorCode code_;
  DropboxMetadata md_;
};

TEST_F(DropboxMetadataOpsTestCase, MetadataTestCase) {
  DropboxMetadataRequest req(TEST_DIR);
  DropboxMetadataResponse res;

  d->getFileMetadata(req, res);
  DropboxMetadata m = res.getMetadata();

  EXPECT_EQ(TEST_DIR, m.path_);
  EXPECT_TRUE(m.isDir_);
  EXPECT_EQ("dropbox", m.root_);
}

TEST_F(DropboxMetadataOpsTestCase, MetadataListingTestCase) {
  DropboxMetadataRequest req(TEST_DIR, true);
  DropboxMetadataResponse res;

  d->getFileMetadata(req, res);
  vector<DropboxMetadata>& children = res.getChildren();

  EXPECT_EQ(2UL, children.size());

  for (auto m : children) {
    EXPECT_FALSE(m.isDir_);
    EXPECT_FALSE(m.isDeleted_);
  }
}

TEST_F(DropboxMetadataOpsTestCase, MetadataIncludeDeletesListingTestCase) {
  DropboxMetadataRequest req(TEST_DIR, true, true);
  DropboxMetadataResponse res;

  d->getFileMetadata(req, res);
  vector<DropboxMetadata>& children = res.getChildren();

  EXPECT_LT(3UL, children.size());

  for (auto m : children) {
    if (m.path_.compare(fileName_ + "_2") == 0) {
      EXPECT_TRUE(m.isDeleted_);
    }
  }
}

TEST_F(DropboxMetadataOpsTestCase, RevisionsTest) {
  DropboxRevisions revs;

  d->getRevisions(fileName_ + "_2", 10, revs);

  EXPECT_LE(1UL, revs.getRevisions().size());
}

class DropboxTestEnvironment : public ::testing::Environment {
public:
  void SetUp() {
    char* api_key = getenv("DROPBOX_API_KEY");
    char* api_secret = getenv("DROPBOX_API_SECRET");

    assert(api_key);
    assert(api_secret);

    d = new DropboxApi(api_key, api_secret);

    char* auth_token = getenv("DROPBOX_AUTH_TOKEN");
    char* auth_secret = getenv("DROPBOX_AUTH_TOKEN_SECRET");

    if (auth_token != NULL) {
      d->setAccessToken(auth_token, auth_secret);
    } else {
      AuthorizationHelper h;
      auto fn = bind(&AuthorizationHelper::authorize, &h,
        std::placeholders::_1, std::placeholders::_2);
      d->authenticate(fn);

      cout << "Access token: " << d->getAccessToken() << endl;
      cout << "Access token secret: " << d->getAccessTokenSecret() << endl;
    }
  }

  void TearDown() {
    delete d;
  }
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new DropboxTestEnvironment());

  return RUN_ALL_TESTS();
}
