#include<iostream>
#include<boost/asio.hpp>
#include<boost/asio/steady_timer.hpp>
using  std::cout;
using std::endl;

void callback(const boost::system::error_code&){
    cout<<"hello world"<<endl;
}

void callback2(const boost::system::error_code&){
    cout<<"hello world2"<<endl;
}


int main()
{
    
    boost::asio::io_service io;
    //注册事件
    boost::asio::deadline_timer t1(io,boost::posix_time::seconds(5));
    t1.async_wait(callback);   //非阻塞等待

    //注册第二个事件
    boost::asio::deadline_timer t2(io,boost::posix_time::seconds(3));
    t2.async_wait(callback2);
    //新的时间函数
    boost::asio::steady_timer st(io);
    st.expires_from_now(boost::asio::chrono::seconds(5));
    st.wait();

    cout<<"start"<<endl;
    io.run();// while循环   当消息池中没有事件了，就会返回         相当于callbock是在run中执行的
    cout<<"end"<<endl;
    return 0;
}