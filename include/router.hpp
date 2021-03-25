#pragma once

#include "private_config.h"
#include <string>

class Router {
  public:
    Router();
    void reLogin();
    std::string getIP();

  private:

    const std::string key = ENC_KEY;
    const std::string iv = ENC_IV;
    const std::string diviceId = ENC_DEVICE_ID;
    std::string nonce;
    std::string oldPwd;
    void initNonce();
    void initOldPwd();
    std::string token;
};

