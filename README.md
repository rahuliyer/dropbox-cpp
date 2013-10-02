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

Using the library
-----------------
The API is defined in DropboxApi.h. The DropboxApi class is the core of the library. To get started, instantiate an object of the class:
```
DropboxApi d("my_app_token", "my_app_secret");
```

To get an access token and secret, you need to call the authenticate() function. This function takes a callback as an argument. This callback is called with the OAuth request token and secret as params. A simple callback example is given below:
```
class Foo {
public:
  Foo() { }

  void bar(string token, string secret) {
    cout << "Token: " << token << endl;
    cout << "Secret: " << secret << endl;

    cout << "Go to https://www.dropbox.com/1/oauth/authorize?oauth_token="
      << token << " to authorize" << endl;
    cin.get();
  }
};

function<void(string, string)> fn = bind(&Foo::bar, &f, std::placeholders::_1, std::placeholders::_2);
d.authenticate(fn);
```

Alternatively, if you already have an access token and secret, you can set those instead
```
d.setAccessToken("my_access_token", "my_access_token_secret");
```

Now that you have an authenticated instance of the library, you can start making api calls. Here's an example of getting the account info for the authenticated user:
```
DropboxAccountInfo ac2;
DropboxErrorCode code = d.getAccountInfo(ac2);
```

For more information, look at DropboxApi.h
