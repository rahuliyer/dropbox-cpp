dropbox-cpp
===========

dropbox-cpp is a C++ client for the Dropbox core API.

Building the library and running tests
--------------------------------------
To build the library:
```
make clean
make
```

To run the tests, copy over the file runtest.sh.skeleton to runtest.sh
```
cp runtest.sh.skeleton runtest.sh
```

Edit runtest.sh to fill in your app key and app secret in the DROPBOX_API_KEY and DROPBOX_API_SECRET variables. If you know your access token and access token secret, you can fill those in too. Then run runtest.sh

```
./runtest.sh
```

If you haven't provided an access token and secret, the test suite will attempt to get one for you. You will see something like
```
Request token: <some token>
Request token secret: <some secret>
Go to https://www.dropbox.com/1/oauth/authorize?oauth_token=<token> to authorize
Hit any key to continue after authorization
```

Visit the link above and grant permissions to the app. Then hit any key to continue. The test suite will print out the access token and secret and start executing the tests. You can enter the access token and secret into runtest.sh and you will not be prompted to authorize the app again.
```
Access token: <some token>
Access token secret: <some secret>
[----------] 1 test from AccountInfoTestCase
[ RUN      ] AccountInfoTestCase.AccountInfoTest

```

