//时间服务器
#include<ctime>
#include<iostream>
#include<string>
#include<boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std;
    auto now = time(nullptr);
    return ctime(&now);
}

int main()
{
    try
    {
        /* code */
        boost::asio::io_service io_service;

        tcp::acceptor acceptor(io_service,tcp::endpoint(tcp::v4(),13));
        std::cout<<"start service\n";
        for(;;){
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            
            auto message = make_daytime_string();

            boost::system::error_code ignored_error;
            boost::asio::write(socket,boost::asio::buffer(message),ignored_error);
        }
        std::cout<<"bye\n";

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
