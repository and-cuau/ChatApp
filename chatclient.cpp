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

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <iostream>

#include <cstring>





std::string intro()
{
    std::string mode;
    std::cout << "Test";
    std::cin >> mode;
    return mode;
}

int launchExecutables()
{
    // Executable filenames
    std::string exe1 = "./tttserver";
    std::string exe2 = "./tttclient";

    // Launch the first executable
    int result1 = std::system(exe1.c_str());
    if (result1 != 0) {
        std::cerr << "Failed to launch executable 1.\n";
        return 1;
    }

    // Launch the second executable
    int result2 = std::system(exe2.c_str());
    if (result2 != 0) {
        std::cerr << "Failed to launch executable 2.\n";
        return 1;
    }

    return 0;
}


void new_connection() {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    constexpr unsigned short PORT = 12345;

    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    
    // socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("192.168.1.100"), "12345"));

    tcp::resolver::results_type endpoints =
    resolver.resolve("127.0.0.1", std::to_string(PORT));

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    boost::system::error_code error;

    
    // std::string message(1024, '\0');
    std::string testmsg;

    for(;;){
        std::cout << "Enter your msg: ";
        std::cin >> testmsg;
        socket.send(boost::asio::buffer(testmsg), 0, error);
    }

   // size_t len = socket.read_some(boost::asio::buffer(message), error);
   
}

void receive(){

//      for (;;)
//     {
//       // std::array<char, 128> buf;
//       std::string message(1024, '\0');

//       boost::system::error_code error;

//       size_t len = socket.read_some(boost::asio::buffer(message), error);

//       if (error == boost::asio::error::eof)
//         break; // Connection closed cleanly by peer.
//       else if (error)
//         throw boost::system::system_error(error); // Some other error.

//       std::cout.write(message, len);
//     }
//   catch (std::exception& e)
//   {
//     std::cerr << e.what() << std::endl;
//   }

}

int main() { 

    std::string mode;

    new_connection();



    return 0;
    
}