#include<windows.h>
#include<stdio.h>
#include<WinSock.h>
#include<iostream>

#pragma comment(lib,"ws2_32")

using namespace std;

struct Base64Date6
{
    unsigned int d4 : 6;
    unsigned int d3 : 6;
    unsigned int d2 : 6;
    unsigned int d1 : 6;
};

//  协议中加密部分使用的是base64方法
char ConvertToBase64(char c6);
void EncodeBase64(char*dbuf, char*buf128, int len);
void SendMail(char*email, char*body, char* hostname, short port, char *username, char *password);
int  OpenSocket(struct sockaddr *addr);

char *x[] = {"D:\\devGames\\iptrail\\build\\src\\Debug\\ref.exe",
"513374673@qq.com",
    "smtp.163.com",
    "25",
    "ps_v52_0@163.com",
    "ZOCGINLBQNKDZHVB",
    "subject","./test.txt"};

int main(int argc, char *argv[])
{
    argc = 8;
    argv = x;
    char *EmailTo = argv[1];
    char *host = argv[2];
    char *port = argv[3];
    char *user = argv[4];
    char *pass = argv[5];
    char *subject = argv[6];
    char *filename = argv[7];

    char *EmailContents = new char[20000];

    char *From = new char[128];
    sprintf(From, "From: <%s>\r\n", user);
    char *To = new char[128];
    sprintf(To, "To: <%s>\r\n", EmailTo);
    char *sub = new char[128];
    sprintf(sub, "Subject: %s\r\n\r", subject);

    short porttemp = 0;
    for (int i = 0; i < (int)strlen(port); i++)
    {
        porttemp = porttemp * 10 + port[i] - '0';
    }
    FILE *fp;
    fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("File path error\n");
        return false;
    }

    //打开传送内容的文件夹并读取到缓冲区
    fseek(fp, 0L, SEEK_END);
    int fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    cout << fileSize << endl;
    char *content = (char*)malloc(sizeof(char) * fileSize);
    fread(content, 1, fileSize, fp);
    cout << content << endl;
    fclose(fp);

    sprintf(EmailContents, "%s%s%s%s", From, To, sub, content);
    cout << EmailContents << endl;
    SendMail(EmailTo, EmailContents, host, porttemp, user, pass);
    system("PAUSE");
    return 0;
}

// 发送邮件
void SendMail(char*email, char*body, char* hostname, short port, char *username, char *password)
{
    int sockfd = { 0 };
    char buf[1500] = { 0 };
    char rbuf[1500] = { 0 };
    char login[128] = { 0 };
    char pass[128] = { 0 };
    WSADATA WSAData;
    struct sockaddr_in their_addr = { 0 };
    WSAStartup(MAKEWORD(2, 2), &WSAData);
    memset(&their_addr, 0, sizeof(their_addr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    hostent*hptr = gethostbyname(hostname);
    memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);

    printf("IP of %s is : %d:%d:%d:%d\n",
        hostname,
        their_addr.sin_addr.S_un.S_un_b.s_b1,
        their_addr.sin_addr.S_un.S_un_b.s_b2,
        their_addr.sin_addr.S_un.S_un_b.s_b3,
        their_addr.sin_addr.S_un.S_un_b.s_b4);

    // 连接邮箱服务器，如果连接后没有响应，则2 秒后重新连接
    sockfd = OpenSocket((struct sockaddr *)&their_addr);
    memset(rbuf, 0, 1500);
    while (recv(sockfd, rbuf, 1500, 0) == 0)
    {
        cout << "reconnect..." << endl;
        Sleep(2);
        sockfd = OpenSocket((struct sockaddr *)&their_addr);
        memset(rbuf, 0, 1500);
    }

    cout << rbuf << endl;
    
    // EHLO
    memset(buf, 0, 1500);
    sprintf(buf, "EHLO HYL-PC\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "EHLO REceive: " << rbuf << endl;

    // AUTH LOGIN
    memset(buf, 0, 1500);
    sprintf(buf, "AUTH LOGIN\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "Auth Login Receive: " << rbuf << endl;

    // USER
    memset(buf, 0, 1500);
    sprintf(buf, username);//user
    memset(login, 0, 128);
    EncodeBase64(login, buf, strlen(buf));
    sprintf(buf, "%s\r\n", login);
    send(sockfd, buf, strlen(buf), 0);
    cout << "Base64 UserName: " << buf << endl;
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "User Login Receive: " << rbuf << endl;

    // PASSWORD
    sprintf(buf, password);//password
    memset(pass, 0, 128);
    EncodeBase64(pass, buf, strlen(buf));
    sprintf(buf, "%s\r\n", pass);
    send(sockfd, buf, strlen(buf), 0);
    cout << "Base64 Password: " << buf << endl;
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "Send Password Receive: " << rbuf << endl;

    // MAIL FROM
    memset(buf, 0, 1500);
    sprintf(buf, "MAIL FROM: <%s>\r\n", username);
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "set Mail From Receive: " << rbuf << endl;

    // RCPT TO 第一个收件人
    sprintf(buf, "RCPT TO:<%s>\r\n", email);
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "Tell Sendto Receive: " << rbuf << endl;

    // DATA 准备开始发送邮件内容
    sprintf(buf, "DATA\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "Send Mail Prepare Receive: " << rbuf << endl;

    // 发送邮件内容，\r\n.\r\n内容结束标记
    sprintf(buf, "%s\r\n.\r\n", body);
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "Send Mail Receive: " << rbuf << endl;

    // QUIT
    sprintf(buf, "QUIT\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    cout << "Quit Receive: " << rbuf << endl;

    //清理工作
    closesocket(sockfd);
    WSACleanup();
    return;
}

//打开TCP Socket连接
int OpenSocket(struct sockaddr *addr)
{
    int sockfd = 0;
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd <0)
    {
        cout << "Open sockfd(TCP) error!" << endl;
        exit(-1);
    }
    if (connect(sockfd, addr, sizeof(struct sockaddr))<0)
    {
        cout << "Connect sockfd(TCP) error!" << endl;
        exit(-1);
    }
    return sockfd;
}

// base64的实现
void EncodeBase64(char *dbuf, char *buf128, int len)
{
    struct Base64Date6 *ddd = NULL;
    int i = 0;
    char buf[256] = { 0 };
    char *tmp = NULL;
    char cc = '\0';
    memset(buf, 0, 256);
    strcpy(buf, buf128);
    for (i = 1; i <= len / 3; i++)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct Base64Date6*)tmp;
        dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i - 1) * 4 + 3] = ConvertToBase64((unsigned int)ddd->d4);
    }
    if (len % 3 == 1)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct Base64Date6*)tmp;
        dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = '=';
        dbuf[(i - 1) * 4 + 3] = '=';
    }
    if (len % 3 == 2)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct Base64Date6*)tmp;
        dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i - 1) * 4 + 3] = '=';
    }
    return;
}

char ConvertToBase64(char uc)
{
    if (uc <26)
    {
        return 'A' + uc;
    }
    if (uc <52)
    {
        return 'a' + (uc - 26);
    }
    if (uc <62)
    {
        return '0' + (uc - 52);
    }
    if (uc == 62)
    {
        return '+';
    }
    return '/';
}

/*
#include <iostream>
#include <boost/asio.hpp>

bool test_internet(boost::asio::io_context& io, boost::asio::ip::tcp::endpoint& new_endpoint);

int main(int, char**) {
    using namespace boost;
    asio::io_context io;
    asio::ip::tcp::endpoint ep;
    test_internet(io, ep);

    return 0;
}


bool test_internet(boost::asio::io_context& io, boost::asio::ip::tcp::endpoint& new_endpoint) {
    using namespace boost::asio;
    using std::cerr;
    using std::endl;
    ip::tcp::resolver resolver(io);
    ip::tcp::resolver::query query("www.baidu.com", "http");
    ip::tcp::resolver::iterator it = resolver.resolve(query), end;
    if (it == end) return false;
    ip::tcp::endpoint test_endpoint = *it;
    cerr << test_endpoint.address() << ' ' << test_endpoint.port() << endl;
    ip::tcp::socket test_socket(io);
    test_socket.connect(test_endpoint);
    cerr << test_socket.local_endpoint().address().to_string() << endl;
    return true;
}

*/