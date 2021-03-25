#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "email.hpp"
#include "router.hpp"
#include "private_config.h"
using namespace std;

using namespace boost::asio;

Email email;

void checkIP(const boost::system::error_code &ec, steady_timer *t) {
    static string lastIP = "0.0.0.0";
    static bool isFirst = true;
    if (isFirst) {
        ifstream fin("ip.txt");
        if (fin.good() && !fin.eof()) fin >> lastIP;
        fin.close();
    }
    Router router;
    string currentIP = router.getIP();
    if (lastIP != currentIP) {
        cout << "IP地址变化：" << lastIP << " -> " << currentIP << endl;
        if (SEND_ON_FIRST_TIME || !isFirst) {
            email.setContent(currentIP);
            email.send(Email::Protocol::SMTP);
        }
        lastIP = currentIP;
        isFirst = false;
        ofstream fout("ip.txt");
        fout << currentIP;
        fout.close();
    } else {
        // cout << "IP地址稳定" << endl;
    }
    t->expires_after(boost::asio::chrono::seconds(CHECK_INTERVAL_IN_SECOND));
    t->async_wait(boost::bind(checkIP, boost::asio::placeholders::error, t));
}

int main() {
#ifdef here_is_Email_tutorial
    email.setToEmailAddress(TO_EMAIL)
        .setFromEmailAddress(FROM_EMAIL)
        .setFromEmailPassword(FROM_PASSWORD)
        .setServerHost(SERVER_HOST)
        .setServerPort(SERVER_PORT)
        .setSubject("Hello World!")
        .setContent("I am dzf. Nice to meet you.")
        .send(Email::Protocol::SMTP);
#endif

    email.setToEmailAddress(TO_EMAIL)
        .setFromEmailAddress(FROM_EMAIL)
        .setFromEmailPassword(FROM_PASSWORD)
        .setServerHost(SERVER_HOST)
        .setServerPort(SERVER_PORT)
        .setSubject("IP地址变化");

    io_context io;
    steady_timer t(io, boost::asio::chrono::seconds(1));
    t.async_wait(boost::bind(checkIP, boost::asio::placeholders::error, &t));
    io.run();

    return 0;
}