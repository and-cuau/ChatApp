#include <array>
#include <iostream>
#include <boost/asio.hpp>
#include <mutex>

std::mutex mtx;

using boost::asio::ip::udp;

void send_msgs(udp::socket& socket, udp::endpoint& receiver_endpoint) {
    std::string msg;
    for (;;) {
        std::array<char, 1> send_buf = {{0}};
        std::cout << "debug2" << std::endl;
        std::cin >> msg;
        mtx.lock();
        socket.send_to(boost::asio::buffer(msg), receiver_endpoint);
        mtx.unlock();
        std::cout << "debug3" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    constexpr unsigned short PORT = 12347;
    try {
        std::cout << "Test start of main" << std::endl;

        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        udp::endpoint receiver_endpoint =
            *resolver.resolve(udp::v4(), "127.0.0.1", std::to_string(PORT)).begin();

        udp::socket socket(io_context);
        socket.open(udp::v4());
        // socket.bind(udp::endpoint(udp::v4(), PORT));  // Bind the socket to the local port

        std::cout << "debug1" << std::endl;

        // std::thread sender(send_msgs, std::ref(socket), std::ref(receiver_endpoint)); // SEND

        // Loop to receive data
        for (int i = 0; i < 1; i++) {
            std::array<char, 128> recv_buf;
            udp::endpoint sender_endpoint;

            try {
                size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
                // Process received data
                std::cout << "Received " << len << " bytes from "
                          << sender_endpoint.address().to_string() << ":"
                          << sender_endpoint.port() << "\n"
                          << std::string(recv_buf.data(), len) << std::endl;
            } catch (const boost::system::system_error& e) {
                std::cerr << "Error receiving data: " << e.what() << std::endl;
            }
        }

        //sender.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    

    std::cout << "Test end of main" << std::endl;
    return 0;
}
