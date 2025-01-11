#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

void send_msgs(tcp::socket& socket) {
    boost::system::error_code error;
    std::string testmsg;

    while (true) {
        //std::cout << "Enter your msg: ";
        std::getline(std::cin, testmsg);
        std::cout << std::endl;
    
        if (testmsg.empty()) continue;

        boost::asio::write(socket, boost::asio::buffer(testmsg + "\n"), error);

        if (error) {
            std::cerr << "Failed to send message: " << error.message() << std::endl;
            break;
        }
    }
}

void receive_msgs(tcp::socket& socket) {
    boost::system::error_code error;

    while (true) {
        std::string message;
        boost::asio::streambuf buffer;

        boost::asio::read_until(socket, buffer, "\n", error);

        if (error == boost::asio::error::eof)
            break; // Connection closed cleanly by peer
        else if (error)
            throw boost::system::system_error(error); // Other

        std::istream is(&buffer);
        std::getline(is, message);

        std::cout << message << std::endl;
    }
}

int main() {
    try {
        constexpr unsigned short PORT = 12345;

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", std::to_string(PORT));
        tcp::socket socket(io_context);

        // Connect to the server
        boost::asio::connect(socket, endpoints);


        // Start the receive thread
        std::thread receiver(receive_msgs, std::ref(socket));

        // Handle sending messages
        send_msgs(socket);

        // Join the receive thread
        receiver.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
