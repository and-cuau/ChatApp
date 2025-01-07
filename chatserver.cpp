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
void handle_msgs(std::shared_ptr<tcp::socket> socket); // Pass shared_ptr
std::string make_daytime_string();
std::string intro();
std::queue<std::string> q;
std::mutex mtx;
std::vector<std::thread> threads;

void testConnection(){ 
    constexpr unsigned short PORT = 12345;
    try{
        boost::asio::io_context io_context;
        // A tcp::acceptor object to listen for new connections. Set to listen on TCP port 12345, IP version 4.
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));
        int thread_ind = 0;
        for(;;){
            // Shared pointer to dynamically allocated socket
            auto socket = std::make_shared<tcp::socket>(io_context); // Use shared_ptr here

            std::cout << "awaiting connection...." << std::endl;
            acceptor.accept(*socket);  // Dereference the shared pointer to pass to acceptor

            // Pass the shared_ptr to the thread
            threads.push_back(std::thread(handle_msgs, socket)); // Pass the shared_ptr directly
            thread_ind++;
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

void handle_msgs(std::shared_ptr<tcp::socket> socket) {  // Accept shared_ptr
    boost::system::error_code ignored_error;
    
    for(;;){
        std::string message(30, '\0'); // 1024
        boost::system::error_code error;
        socket->receive(boost::asio::buffer(message), 0, error);  // Dereference shared_ptr to use socket
        
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
        std::cout << "Added to queue: " << q.back() << std::endl;
        mtx.unlock();
    }
}

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

std::string intro()
{
    std::string mode;
    std::cout << "\nWelcome to Andre's Tic-Tac-Toe. Please select a game mode. Enter '1' for single player mode (Play against an AI). Select '2' for two player mode.\n";
    std::cin >> mode;
    return mode;
}

using boost::asio::ip::tcp;
int main(int argc, char* argv[]) { 

    std::string response;
    std::cout << "\nServer has been initialized successfully and is now ready to accept connections.\n";

    testConnection();

    return 0; 
}
