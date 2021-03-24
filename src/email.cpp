#include "email.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include "mybio.hpp"
#include <functional>

void add_r_n(char *str, size_t &len) {
    str[len] = '\r';
    str[len + 1] = '\n';
    str[len += 2] = '\0';
}

Email::Email() {
}

Email::~Email() {
}

Email &Email::setToEmailAddress(const std::string &addr) {
    toEmailAddress = addr;
    isAdjust = false;
    return *this;
}

Email &Email::setServerHost(const std::string &host) {
    serverHost = host;
    return *this;
}

Email &Email::setServerPort(int port) {
    serverPort = port;
    return *this;
}

Email &Email::setFromEmailAddress(const std::string &addr) {
    fromEmailAddress = addr;
    isAdjust = false;
    return *this;
}

Email &Email::setFromEmailPassword(const std::string &password) {
    fromEmailPassword = password;
    return *this;
}

Email &Email::setSubject(const std::string &subject) {
    this->subject = subject;
    isAdjust = false;
    return *this;
}

Email &Email::setContent(const std::string &content) {
    this->content = content;
    isAdjust = false;
    return *this;
}

Email &Email::setContent(std::string &&content) {
    this->content = move(content);
    isAdjust = false;
    return *this;
}

bool Email::isVaild() {
    return !toEmailAddress.empty() && !serverHost.empty() && serverPort != 0
           && !fromEmailAddress.empty() && !fromEmailPassword.empty()
           && !subject.empty() && !content.empty();
}

bool Email::send(Protocol protocol) {
    if (!isVaild()) {
        std::cerr << "Error: Email not valid\n";
        return false;
    }
    if (!isAdjust) {
        isAdjust = true;
        // clang-format off
        content = "From: <" + fromEmailAddress + ">\r\n"
                    "To: <" + toEmailAddress + ">\r\n"
                    "Subject: " + subject + "\r\n\r"
                    + content;
        // clang-format on
    }
    switch (protocol) {
        case Protocol::SMTP:
            return send_SMTP();
            break;

        default:
            return false;
            break;
    }
    return false;
}

bool Email::send_SMTP() {
    using namespace boost::asio;
    using namespace ip;
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::function;
    constexpr size_t MAX_RECV = 512;
    constexpr size_t MAX_SEND = 1500;
    static char recv_buf[MAX_RECV + 3];
    static char send_buf[MAX_SEND + 3];
    io_context io;
    tcp::resolver resolver(io);
    tcp::resolver::results_type endpoints =
        resolver.resolve(serverHost, "SMTP");
    tcp::socket socket(io);

    boost::system::error_code err;

    // 建立连接
    try {
        connect(socket, endpoints);
        const size_t len = socket.read_some(buffer(recv_buf), err);
        if (err == error::eof) {
            cerr << "socket error == eof\n";
        } else if (err) {
            throw boost::system::system_error(err);
        } else {
            cout << "Connect server: " << recv_buf << endl;
        }
    } catch (std::exception &e) { cerr << e.what() << endl; }

    // 发送一次数据(send_buf) 并接收回复(recv_buf)
    const auto SMTP_query = [&](const function<void()> &log_send,
                                const function<void()> &log_recv) {
        write(socket, buffer(send_buf, strlen(send_buf)), err);
        if (err) {
            cerr << "Error: " << err.message() << endl;
        }
        // socket.write_some(buffer(send_buf), err);
        log_send();
        socket.read_some(buffer(recv_buf), err);
        if (err) {
            cerr << "Error: " << err.message() << endl;
        }
        log_recv();
        memset(send_buf, 0, sizeof(send_buf));
        memset(recv_buf, 0, sizeof(recv_buf));
    };

    // 用base64加密，再加上\r\n，存入send_buf
    const auto encode = [&](const string_view content) {
        size_t len =
            base64Encode(content.data(), content.size(), false, send_buf);
        add_r_n(send_buf, len);
    };

    // EHLO
    sprintf_s(send_buf, "EHLO Codesire-dzf\r\n");
    SMTP_query(
        [] { cout << "EHLO ... " << endl; },
        [=] { cout << "EHLO receive: " << recv_buf << endl; });

    // AUTH LOGIN
    sprintf_s(send_buf, "AUTH LOGIN\r\n");
    SMTP_query(
        [] { cout << "Auth ... " << endl; },
        [=] { cout << "Auth Login Receive: " << recv_buf << endl; });

    // USER (from email address)
    encode(fromEmailAddress);
    SMTP_query(
        [=] { cout << "Base64 Username: " << send_buf << endl; },
        [=] { cout << "User Login Receive: " << recv_buf << endl; });

    // PASSWORD
    encode(fromEmailPassword);
    SMTP_query(
        [=] { cout << "Base64 Password: " << send_buf << endl; },
        [=] { cout << "Send Password Receive: " << recv_buf << endl; });

    // MAIL FROM
    sprintf_s(send_buf, "MAIL FROM: <%s>\r\n", fromEmailAddress.data());
    SMTP_query(
        [] { cout << "Mail From ..." << endl; },
        [=] { cout << "set Mail From Receive: " << recv_buf << endl; });

    // RCPT TO 第一个收件人
    sprintf_s(send_buf, "RCPT TO:<%s>\r\n", toEmailAddress.data());
    SMTP_query(
        [] { cout << "Send To ..." << endl; },
        [=] { cout << "set Send To Receive: " << recv_buf << endl; });

    // DATA 准备开始发送邮件内容
    sprintf_s(send_buf, "DATA\r\n");
    SMTP_query(
        [] { cout << "Prepare to send ..." << endl; },
        [=] { cout << "Prepare Receive: " << recv_buf << endl; });

    // 发送邮件内容
    sprintf_s(send_buf, "%s\r\n.\r\n", content.data());
    SMTP_query(
        [] { cout << "Sending content ..." << endl; },
        [=] { cout << "Send Mail Receive: " << recv_buf << endl; });

    // QUIT
    sprintf_s(send_buf, "QUIT\r\n");
    SMTP_query(
        [] { cout << "Quit ..." << endl; },
        [=] { cout << "Quit Receive: " << recv_buf << endl; });

    socket.close();

    return true;
}