#include <iostream>
#include "email.hpp"
#include "router.hpp"
#include "private_config.h"
using namespace std;

int main() {
    Email email;

#ifdef PRIVATE_CONFIG1
    email.setToEmailAddress(TO_EMAIL)
        .setFromEmailAddress(FROM_EMAIL)
        .setFromEmailPassword(FROM_PASSWORD)
        .setServerHost(SERVER_HOST)
        .setServerPort(SERVER_PORT)
        .setSubject("Hello World!")
        .setContent("I am dzf. Nice to meet you.")
        .send(Email::Protocol::SMTP);
#endif

    Router router;

    return 0;
}