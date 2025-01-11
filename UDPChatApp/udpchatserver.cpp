#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <ctime>
#include <thread>
#include <chrono>
#include <vector>
#include <sys/types.h>
#include <queue>

#include <boost/asio.hpp>
using boost::asio::ip::udp;

#include <cstring>

void testConnection();
void handle_msgs(std::shared_ptr<udp::socket> socket_ptr, int connect_num); // Pass shared_ptr
std::string make_daytime_string();
std::string intro();
std::queue<std::string> q;
std::mutex mtx;
std::vector<std::thread> threads;
std::vector<udp::socket> sockets;
//std::vector<std::shared_ptr<boost::asio::ip::udp::socket>> socket_ptrs;
std::shared_ptr<boost::asio::ip::udp::socket> socket_ptrs;


void testConnection(){ 
    boost::system::error_code ignored_error;
    //constexpr unsigned short PORT = 12345;

    try{
        boost::asio::io_context io_context;
        unsigned short port = 12347; // Can be incremented
     
        int connect_num = 0;

        for(int i = 0; i < 1; i++){
            // Shared pointer to dynamically allocated socket
            auto socket_ptr = std::make_shared<udp::socket>(io_context, udp::endpoint(udp::v4(), port)); // Use shared_ptr here. NEW: udpendpoint
            // socket_ptrs.push_back(socket_ptr);
            // socket_ptr->non_blocking(false);

            //std::string connect_num_str = std::to_string(connect_num);// no actual need

            std::string message(1, '\0'); // 1024
            
            //boost::asio::write(*(socket_ptr), boost::asio::buffer(connect_num_str), ignored_error); no actual need for this
            
            // Pass the shared_ptr to the thread
            threads.push_back(std::thread(handle_msgs, socket_ptr, connect_num)); // Pass the shared_ptr directly
            connect_num++;
            // port++;
        }
        // Join all threads
        for (auto& t : threads) {
            t.join();
        }
        std::cout << "All threads have finished handling messages.\n";

    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void handle_msgs(std::shared_ptr<udp::socket> socket_ptr, int connect_num) {  // Accept shared_ptr
    boost::system::error_code ignored_error;
    
    for(;;){
        std::string message(30, '\0'); // 1024

        // std::array<char, 1> recv_buf;
        udp::endpoint remote_endpoint;

        boost::system::error_code error;

        std::cout << "Awaiting datagram...." << std::endl;

        socket_ptr->receive_from(boost::asio::buffer(message), remote_endpoint, 0, error); // NEW method and NEW inputs
        
        // message = std::to_string(connect_num) + ": " + message;
        std::cout << "TEST SERVER-SIDE MESSAGE: " << message<< std::endl;

        if (error) {
            if (error == boost::asio::error::eof) {
                std::cout << "Connection closed by client.\n";
                break; // Client closed connection gracefully
            } else if (error == boost::asio::error::would_block) {
                std::cout << "Socket would block. Data not yet available.\n";
                continue; // Retry if in non-blocking mode
            } else {
                std::cerr << "Error during receive: " << error.message() << std::endl;
                break; // Break on other errors
            }
        }

        // if (message.find("end") != std::string::npos) {
        //     break;
        // }
        
        // Lock and push to queue
        mtx.lock();
       // q.push(message);
        // std::cout << "Added to queue: " << q.back() << std::endl;
        mtx.unlock();



        boost::system::error_code ignored_error;
        socket_ptr->send_to(boost::asio::buffer(message),
        remote_endpoint, 0, ignored_error);

        // int num_sock_ptrs = static_cast<int>(socket_ptrs.size());

        //  socket.send_to(boost::asio::buffer(message),
        //  remote_endpoint, 0, ignored_error);

        // for (int i = 0; i < num_sock_ptrs; i++){
        //     boost::asio::write(*(socket_ptrs.at(i)), boost::asio::buffer(message), ignored_error);
        // }
    }
}


using boost::asio::ip::udp;
int main(int argc, char* argv[]) { 

    std::string response;
    std::cout << "\nServer has been initialized successfully and is now ready to accept connections.\n";

    testConnection();

    return 0; 
}