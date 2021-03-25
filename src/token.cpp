#include "token.hpp"
#include <ctime>
#include <openssl/evp.h>
#include <boost/json.hpp>

using std::string;

Token::Token() {
    initNonce();
    initOldPwd();
}

void Token::initNonce() {
    const int tim = time(nullptr);
    const string type = "0";
    srand(tim);
    const int random = double(rand()) / RAND_MAX * 10000;
    nonce = type + "_" + diviceId + "_" + std::to_string(tim) + "_"
            + std::to_string(random);
}

void Token::initOldPwd() {
    const string pwd = ENC_ORIGIN_PWD;
    static unsigned char buf[40];
    static unsigned int len;
    string s = pwd + this->key;
    EVP_Digest(s.data(), s.size(), buf, &len, EVP_sha1(), nullptr);
    s = this->nonce + bytesToHexString(buf, len);
    EVP_Digest(s.data(), s.size(), buf, &len, EVP_sha1(), nullptr);
    oldPwd = bytesToHexString(buf, len);
}

static char toHex(unsigned char c) {
    if (c < 10) return '0' + c;
    return c - 10 + 'a';
}

static string bytesToHexString(unsigned char *st, unsigned int len) {
    char *buf = new char[(len << 1) | 1];
    for (unsigned int i = 0; i < len; ++i) {
        buf[i << 1] = toHex(st[i] >> 4);
        buf[(i << 1) | 1] = toHex(st[i] & 0xF);
    }
    buf[len << 1] = '\0';
    string &&ret = string(buf);
    delete[] buf;
    return ret;
}

void Token::login() {
    LoginPost param = {"admin", oldPwd, 2, nonce};
    
}