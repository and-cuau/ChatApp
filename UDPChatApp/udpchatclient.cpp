#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>
using boost::asio::ip::udp;

// void send_msgs(udp::socket& socket,   udp::endpoint &receiver_endpoint) {
//     boost::system::error_code error;
//     std::string testmsg; //OLD
//     std::array<char, 1> send_buf  = {{ 0 }}; //NEW
//     // socket.open(udp::v4()); // NEW

//     while (true) {
//         //std::cout << "Enter your msg: ";
//         std::getline(std::cin, testmsg);
//         std::cout << std::endl;
    
//         if (testmsg.empty()) continue;

        
//         socket.send_to(boost::asio::buffer(testmsg), receiver_endpoint); 

//         //boost::asio::write(socket, boost::asio::buffer(testmsg + "\n"), error); OLD

//         if (error) {
//             std::cerr << "Failed to send message: " << error.message() << std::endl;
//             break;
//         }
//     }
// }

void receive_msgs(udp::socket& socket) {
    boost::system::error_code error;

    //while (true) {
        // std::string message;
        // boost::asio::streambuf buffer; OLD

        std::array<char, 128> recv_buf; // NEW
        //std::string message(30, '\0');
        udp::endpoint sender_endpoint;
       
        std::cout << "before receivefrom" << std::endl;

        // try {
         size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint); // NEW // Needs figuring out
        // }
        // catch(const boost::system::system_error& e){
        //       std::cerr << "Error receiving data: " << e.what() << std::endl;
        // }

       std::cout << "after receivefrom" << std::endl;

        //std::cout.write(recv_buf.data(), len);

        // if (error == boost::asio::error::eof)
        //     break; // Connection closed cleanly by peer.
        // else if (error)
        //     throw boost::system::system_error(error); // Some other error.

        // std::istream is(&buffer); //OLD
        // std::getline(is, message);

        // std::cout << message << std::endl;
   // }
}

int main() {
    try {
        constexpr unsigned short PORT = 12345;

        boost::asio::io_context io_context;
        // tcp::resolver resolver(io_context);
        // tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", std::to_string(PORT));
        // tcp::socket socket(io_context);

        udp::resolver resolver(io_context);
        // udp::endpoint receiver_endpoint =
        // *resolver.resolve(udp::v4(), "127.0.0.1",  std::to_string(PORT)).begin();

        udp::socket socket(io_context);
        socket.open(udp::v4());


        // Start the receive thread
        std::thread receiver(receive_msgs, std::ref(socket));

        // Handle sending messages
        // send_msgs(socket, receiver_endpoint);

        // Join the receive thread
        receiver.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}