#pragma once
#include <string>

class Email {
  private:
    std::string toEmailAddress;
    std::string serverHost;
    int serverPort = 0;
    std::string fromEmailAddress;
    std::string fromEmailPassword;
    std::string subject;
    std::string content;
    bool isAdjust = false;

  public:
    enum class Protocol { SMTP };

  public:
    Email();
    ~Email();

    Email &setToEmailAddress(const std::string &);
    Email &setServerHost(const std::string &);
    Email &setServerPort(int port);
    Email &setFromEmailAddress(const std::string &);
    Email &setFromEmailPassword(const std::string &);
    Email &setSubject(const std::string &);
    Email &setContent(const std::string &);
    Email &setContent(std::string &&);
    bool send(Protocol);
    bool isVaild();

  private:
    bool send_SMTP();
    
};