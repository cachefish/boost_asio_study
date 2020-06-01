#include<iostream>
#include<boost/asio.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
 //date_time 主要处理时间 找到特定的时间点 //posix_time主要处理时间段
int main()
{
    //io_server作为整个连接代码和底层操作的桥梁
    boost::asio::io_service io;
    boost::asio::deadline_timer   t(io,boost::posix_time::seconds(5));  //boost事件相关，用于延时5s
    t.wait();   //阻塞等待
    std::cout<<"Hello world"<<std::endl;
    return 0;
}
