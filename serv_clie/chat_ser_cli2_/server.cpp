#include"chat_message.h"
#include<cstdlib>
#include<boost/asio.hpp>
#include<deque>
#include<iostream>
#include<list>
#include<memory>
#include<set>
#include<utility>
using boost::asio::ip::tcp;

using chat_message_queue = std::deque<chat_message>;
using chat_message_queue2 = std::list<chat_message>;

class chat_session;
using chat_session_ptr = std::shared_ptr<chat_session>;

class chat_room{
public:
    void join(chat_session_ptr);
    void leave(chat_session_ptr);

    void deliver(const chat_message&);

private:
    std::set<chat_session_ptr> participants_;
    enum{max_recent_msgs = 100};
    chat_message_queue  recent_msgs_;
};

class chat_session:public std::enable_shared_from_this<chat_session>{
public:
    chat_session(tcp::socket socket,chat_room&room)
    :socket_(std::move(socket))
    ,room_(room)
    {

    }
    void start(){
        room_.join(shared_from_this());
        do_read_header();
    }

    void deliver(const chat_message&msg){
        bool write_in_progress = !write_msg_.empty();
        write_msg_.push_back(msg);
        if(!write_in_progress){
            do_write();
        }
    }

private:
    void do_read_header(){
        auto self(shared_from_this());
        boost::asio::async_read(
            socket_,boost::asio::buffer(read_msg_.data(),chat_message::header_length),
            [this,self](boost::system::error_code ec,size_t ){
                if(!ec&&read_msg_.decode_header()){
                    do_read_body();
                }else{
                    room_.leave(shared_from_this());
                }
            }
        );

    } 
    void do_read_body(){
        auto self(shared_from_this());
        boost::asio::async_read(
            socket_,boost::asio::buffer(read_msg_.body(),read_msg_.body_length()),
            [this,self](boost::system::error_code ec,std::size_t ){
                if(!ec){
                handleMessage();
                do_read_header();
            }else {
                room_.leave(shared_from_this());
            }
            });
    }
    void handleMessage(){
        if(read_msg_.type()==MT_BIND_NAME){
            const BindName*bind = reinterpret_cast<const BindName*>(read_msg_.body());
            m_name.assign(bind->name,bind->name+bind->nameLen);
        }else if(read_msg_.type()==MT_CHAT_INFO){
            const ChatInformation*chat = reinterpret_cast<const ChatInformation*>(read_msg_.body());
            m_chatInformation.assign(chat->information,chat->information+chat->infoLen);
            auto rinfo = buildRoomInfo();
            chat_message msg;
            msg.setMessage(MT_ROOM_INFO,&rinfo,sizeof(rinfo));
            room_.deliver(msg);
        }else{
            //not vaild msg do nothing
        }

    }
     void do_write()
    {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_,boost::asio::buffer(write_msg_.front().data(),
            write_msg_.front().length()),
            [this,self](boost::system::error_code ec,std::size_t ){
                if(!ec){
                     write_msg_.pop_front();
                    if(!write_msg_.empty()){
                        do_write();
                    }
                }else{
                    room_.leave(shared_from_this());
                }
            }
        );
    }

    RoomInformation buildRoomInfo()const{
            RoomInformation info;
            info.name.nameLen = m_name.size();
            std::memcpy(info.name.name,m_name.data(),m_name.size());
            info.chat.infoLen = m_chatInformation.size();
            std::memcpy(info.chat.information,m_chatInformation.data(),
            m_chatInformation.size());
            return info;
    }
private:
    tcp::socket socket_;
    chat_room &room_;
    chat_message read_msg_;
    chat_message_queue write_msg_;
    std::string m_name;
    std::string m_chatInformation;
};

void chat_room::join(chat_session_ptr participant)
{
    participants_.insert(participant);
    for(const auto &msg:recent_msgs_){
        participant->deliver(msg);
    }
}
void chat_room::leave(chat_session_ptr participant)
{
    participants_.erase(participant);
}

void chat_room::deliver(const chat_message&msg)
{
    recent_msgs_.push_back(msg);
    while(recent_msgs_.size()>max_recent_msgs)
    {
        recent_msgs_.pop_front();
    }
    for(auto &participant:participants_)
    {
        participant->deliver(msg);
    }
}

class chat_server
{
public:
    chat_server(boost::asio::io_service &io_service,const tcp::endpoint &endpoint)
    :acceptor_(io_service,endpoint)
    ,socket_(io_service)
    {   
        do_accept();
    }
private:
    void do_accept(){
        acceptor_.async_accept(socket_,[this](boost::system::error_code ec){
            if(!ec){
                auto session = std::make_shared<chat_session>(std::move(socket_),room_);
                session->start();
            }
            do_accept();
        });
    }

private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    chat_room room_;
};

int main(int argc,char*argv[])
{
    try
    {
        if(argc<2){
            std::cerr<<"Usage chat_server<port >.."<<std::endl;
            return 1;
        }
            boost::asio::io_service io_service;

            std::list<chat_server>  servers;
            for(int i =1;i<argc;++i){
                tcp::endpoint endpoint(tcp::v4(),std::atoi(argv[i]));
                servers.emplace_back(io_service,endpoint);
            }
            io_service.run();
        }
    
    catch(const std::exception& e)
    {
        std::cerr << "Exception:"<<e.what() << '\n';
    }
    return 0;
}