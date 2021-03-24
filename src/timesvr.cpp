#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <string>
#include <ctime>
#include <iomanip>

std::string_view get_daytime_time() {
    using namespace std;
    constexpr size_t len = 128;
    static char str[len];
    time_t t = time(nullptr);
    strftime(str, 126, "%F %T", localtime(&t));
    return string_view(str, len);
}

int main() {
    using namespace std;
    using namespace boost::asio;

    try {
        io_context io;
        ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 13));

        while (1) {
            ip::tcp::socket socket(io);
            acceptor.accept(socket);
            boost::system::error_code error;
            boost::asio::write(socket, buffer(get_daytime_time()), error);
        }
    } catch (const std::exception &e) { std::cerr << e.what() << '\n'; }

    cout << get_daytime_time() << endl;
    return 0;
}