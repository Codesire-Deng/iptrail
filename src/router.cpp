#include "router.hpp"
#include <ctime>
#include <openssl/evp.h>
#include <boost/json.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>

using std::string;

namespace RouterRequest {
    struct LoginPost {
        string username;
        string password;
        int logtype;
        string nonce;

        string nonceTransform() const {
            string ret;
            for (char c : nonce)
                if (c == ':') ret += "%3A";
                else
                    ret += c;
            return ret;
        }

        string to_string() const {
            return "username=" + username + "&password=" + password
                   + "&logtype=" + std::to_string(logtype)
                   + "&nonce=" + nonceTransform();
        }
    };

    namespace json = boost::json;

    void tag_invoke(json::value_from_tag, json::value &jv, const LoginPost &c) {
        jv = {
            {"username", c.username},
            {"password", c.password},
            {"logtype", c.logtype},
            {"nonce", c.nonce}};
    }
} // namespace RouterRequest

Router::Router() {
    initNonce();
    initOldPwd();
    reLogin();
}

void Router::initNonce() {
    const int tim = int(time(nullptr));
    const string type = "0";
    srand(tim);
    const int random = int(double(rand()) / RAND_MAX * 10000);
    nonce = type + "_" + diviceId + "_" + std::to_string(tim) + "_"
            + std::to_string(random);
}

static char byteToHex(unsigned char c) {
    if (c < 10) return '0' + c;
    return c - 10 + 'a';
}

static string bytesToHexString(unsigned char *st, unsigned int len) {
    char *buf = new char[(len << 1) | 1];
    for (unsigned int i = 0; i < len; ++i) {
        buf[i << 1] = byteToHex(st[i] >> 4);
        buf[(i << 1) | 1] = byteToHex(st[i] & 0xF);
    }
    buf[len << 1] = '\0';
    string &&ret = string(buf);
    delete[] buf;
    return ret;
}
void Router::initOldPwd() {
    const string pwd = ENC_ORIGIN_PWD;
    static unsigned char buf[40];
    static unsigned int len;
    string s = pwd + this->key;
    EVP_Digest(s.data(), s.size(), buf, &len, EVP_sha1(), nullptr);
    s = this->nonce + bytesToHexString(buf, len);
    EVP_Digest(s.data(), s.size(), buf, &len, EVP_sha1(), nullptr);
    oldPwd = bytesToHexString(buf, len);
}

void Router::reLogin() {
    using namespace boost::asio;
    using namespace boost::beast;
    using std::cerr, std::cout, std::endl;

    io_context io;
    ip::tcp::resolver resolver(io);
    tcp_stream stream(io);
    auto const results =
        resolver.resolve(GATEWAY_IP, std::to_string(GATEWAY_PORT));
    stream.connect(results);

    const RouterRequest::LoginPost param = {"admin", oldPwd, 2, nonce};
    const string target = "/cgi-bin/luci/api/xqsystem/login";
    http::request<http::string_body> req{http::verb::post, target, 11};
    req.set(http::field::host, GATEWAY_IP);
    req.set(http::field::connection, "keep-alive");
    req.body() = param.to_string();
    req.prepare_payload();
    req.set(http::field::accept, "*/*");
    // req.set(
    //     http::field::user_agent,
    //     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
    //     "(KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36");
    req.set(http::field::content_type, "application/x-www-form-urlencoded; charset=UTF-8");
    // req.set(http::field::origin, "Origin: http://" GATEWAY_IP);
    // req.set(http::field::referer, "http://" GATEWAY_IP "/cgi-bin/luci/web");
    // req.set(http::field::accept_encoding, "gzip, deflate");
    // req.set(http::field::accept_language, "zh-CN,zh;q=0.9,en-GB;q=0.8,en;q=0.7");
    http::write(stream, req);
    flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    boost::json::value jv = boost::json::parse(res.body());
    this->token = boost::json::value_to<string>(jv.at("token"));

    stream.socket().shutdown(ip::tcp::socket::shutdown_both);
}

string Router::getIP() {
    using namespace boost::asio;
    using namespace boost::beast;
    io_context io;
    ip::tcp::resolver resolver(io);
    tcp_stream stream(io);
    auto const results =
        resolver.resolve(GATEWAY_IP, std::to_string(GATEWAY_PORT));
    stream.connect(results);

    const string target = "/cgi-bin/luci/;stok="+token+"/api/xqnetwork/pppoe_status";
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, GATEWAY_IP);
    http::write(stream, req);

    flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    // std::cerr << res.body() << std::endl;

    boost::json::value jv = boost::json::parse(res.body());
    return boost::json::value_to<string>(jv.at("ip").at("address"));
}