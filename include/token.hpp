#pragma once

#include "private_config.h"
#include <string>

class Token {
  public:
    Token();
    void login();

  private:
    struct LoginPost {
        string username;
        string password;
        int logtype;
        string nonce;
    };
    const std::string key = ENC_KEY;
    const std::string iv = ENC_IV;
    const std::string diviceId = ENC_DEVICE_ID;
    std::string nonce;
    std::string oldPwd;
    void initNonce();
    void initOldPwd();
};