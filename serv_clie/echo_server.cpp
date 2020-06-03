#include<cstdlib>
#include<iostream>
#include<memory>
#include<utility>
#include<boost/asio.hpp>

using boost::asio::ip::tcp;

class Session:public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket):socket_(std::move(socket)){

    }
    void start(){do_read();}
private:
    void do_read()
    {
        auto self = shared_from_this();
        socket_.async_read_some(
            boost::asio::buffer(data_,max_length),
            [this,self](boost::system::error_code ec,std::size_t length){
                if(!ec){
                    do_write(length);
                }
            });
    }
    
    void do_write(std::size_t length){
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_,boost::asio::buffer(data_,length),
            [this,self](boost::system::error_code ec,std::size_t){
                if(!ec){
                    do_read();
                }
            }
        );
    }
private:
    tcp::socket socket_;
    enum{max_length=1024};
    char data_[max_length];
};

class Server
{
public:
    Server(boost::asio::io_service&io_service,short port)
    :acceptor_(io_service,tcp::endpoint(tcp::v4(),port)),
    socket_(io_service)
    {

    }

    void start()
    {
        do_accept();
    }
private:
    void do_accept(){
        acceptor_.async_accept(socket_,[this](boost::system::error_code ec){
                if(!ec){
                    auto newSession = std::make_shared<Session>(std::move(socket_));
                    newSession->start();
                }
                do_accept();
        });
    }
private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;

};

int main(int argc,char*argv[])
{
    try
    {
        if(argc!=2){
            std::cerr<<"Usage:async_tcp_echo_server<port>\n";
            return 1;
        }
        boost::asio::io_service io_service;
        Server s(io_service,std::atoi(argv[1]));
        s.start();

        io_service.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}