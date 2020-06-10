#include"chat_message.h"
#include"structHeader.h"
#include<boost/asio.hpp>
#include<deque>
#include<iostream>
#include<thread>
#include<cstdlib>
#include<cassert>
using boost::asio::ip::tcp;

using chat_message_queue = std::deque<chat_message>;

class chat_client{
public:
    chat_client(boost::asio::io_service&io_service,tcp::resolver::iterator endpoint_iterator)
    :io_service_(io_service)
    ,socket_(io_service)
    {   
        do_connect(endpoint_iterator);
    }
    void write(const chat_message&msg ){
            io_service_.post([this,msg](){
                bool write_in_progress = !write_msgs_.empty();
                write_msgs_.push_back(msg);
                if(!write_in_progress){
                    do_write();
                }
            });
    }
    void close()
    {
        io_service_.post([this](){
            socket_.close();
        });
    }
private:
    void do_connect(tcp::resolver::iterator endpoint_iterator){
        boost::asio::async_connect(socket_,endpoint_iterator,[this](boost::system::error_code ec,tcp::resolver::iterator ){
            if(!ec){
                do_read_header();
            }
        });
    }

    void do_read_header(){
            boost::asio::async_write(socket_,boost::asio::buffer(read_msg_.data(),chat_message::header_length),
            [this](boost::system::error_code ec,std::size_t ){
                if(!ec){
                    if(read_msg_.body_length()==sizeof(RoomInformation)&&
                            read_msg_.type()==MT_ROOM_INFO){
                                const RoomInformation *info = reinterpret_cast<const RoomInformation*>(read_msg_.body());
                                std::cout<<"client:";
                                assert(info->name.nameLen<=sizeof(info->name.name));
                                std::cout.write(info->name.name,info->name.nameLen);
                                std::cout<<"says:";
                                assert(info->chat.infoLen<=sizeof(info->chat.information));
                                std::cout.write(info->chat.information,info->chat.infoLen);
                                std::cout<<"\n";
                            }
                do_read_header();
                }else{
                    socket_.close();
                }
            });
    }
    void do_write(){
        boost::asio::async_write(socket_,boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
            [this](boost::system::error_code ec,std::size_t ){
                if(!ec){
                    write_msgs_.pop_front();
                    if(!write_msgs_.empty()){
                        do_write();
                    }
                }else{
                    socket_.close();
                }
            });
    }
private:
    boost::asio::io_service &io_service_;
    tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};

int main(int argc,char*argv[])
{
    try
    {
        if(argc!=3){
            std::cerr<<"Usage:chat_client <host><post>\n";
            return 1;
        }
        boost::asio::io_service io_service;
        tcp::resolver ressolver(io_service);
        auto endpoint_iterator = ressolver.resolve({argv[1],argv[2]});
        chat_client c(io_service,endpoint_iterator);
        
        std::thread t([&io_service](){
            io_service.run();
        });

        char line[chat_message::max_body_length+1];

        while(std::cin.getline(line,chat_message::max_body_length+1)){
            chat_message msg;
            auto type = 0;
            std::string input(line,line+std::strlen(line));
            std::string output;
            if(parseMessage(input,&type,output)){
                msg.setMessage(type,output.data(),output.size());
                c.write(msg);
                std::cout<<"write message for server"<<output.size()<<std::endl;
            }
        }
        c.close();
        t.join();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}