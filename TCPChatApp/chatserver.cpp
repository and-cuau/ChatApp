#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <queue>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <iostream>

#include <cstring>

void testConnection();
void handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num); // Pass shared_ptr
std::string make_daytime_string();
std::string intro();
std::queue<std::string> q;
std::mutex mtx;
std::vector<std::thread> threads;
std::vector<tcp::socket> sockets;
std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> socket_ptrs;


void testConnection(){ 
    boost::system::error_code ignored_error;
    constexpr unsigned short PORT = 12345;
    try{
        boost::asio::io_context io_context;
        // A tcp::acceptor object to listen for new connections. Set to listen on TCP port 12345, IP version 4.
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));
        int connect_num = 0;
        for(;;){
            // Shared pointer to dynamically allocated socket
            auto socket_ptr = std::make_shared<tcp::socket>(io_context); // Use shared_ptr here
            socket_ptrs.push_back(socket_ptr);

            std::cout << "awaiting connection...." << std::endl;
            acceptor.accept(*socket_ptr);  // Dereference the shared pointer to pass to acceptor

            //std::string connect_num_str = std::to_string(connect_num);// no actual need

            std::string message(1, '\0'); // 1024
            
            //boost::asio::write(*(socket_ptr), boost::asio::buffer(connect_num_str), ignored_error); no actual need for this
            
            // Pass the shared_ptr to the thread
            threads.push_back(std::thread(handle_msgs, socket_ptr, connect_num)); // Pass the shared_ptr directly
            connect_num++;
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

void handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num) {  // Accept shared_ptr
    boost::system::error_code ignored_error;
    
    for(;;){
        std::string message(30, '\0'); // 1024
        boost::system::error_code error;
        socket_ptr->receive(boost::asio::buffer(message), 0, error);  // De-reference shared_ptr to use socket
        
        message = std::to_string(connect_num) + ": " + message;
        // std::cout << "TEST SERVER-SIDE MESSAGE: " << message<< std::endl;


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

        if (message.find("end") != std::string::npos) {
            break;
        }
        
        // Lock and push to queue
        mtx.lock();
        q.push(message);
        // std::cout << "Added to queue: " << q.back() << std::endl;
        mtx.unlock();

        int num_sock_ptrs = static_cast<int>(socket_ptrs.size());

        for (int i = 0; i < num_sock_ptrs; i++){
            boost::asio::write(*(socket_ptrs.at(i)), boost::asio::buffer(message), ignored_error);
        }


    }
}


using boost::asio::ip::tcp;
int main(int argc, char* argv[]) { 

    std::string response;
    std::cout << "\nServer has been initialized successfully and is now ready to accept connections.\n";

    testConnection();

    return 0; 
}
