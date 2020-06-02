#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>

//多线程

class printer {
public:
   printer(boost::asio::io_service& io)
       : strand_(io), timer1_(io, boost::posix_time::seconds(1)),
       timer2_(io, boost::posix_time::seconds(1)), count_(0) 
   {
       //同时执行
       timer1_.async_wait(strand_.wrap([this](const auto&) {this->print1(); }));
       timer2_.async_wait(strand_.wrap([this](const auto&) {this->print2(); }));
   }

   ~printer() { std::cout << "Final count is " << count_ << std::endl; }

   void print1() {
       if (count_ < 10) {
           std::cout << "Timer 1: " << count_ << std::endl;
           ++count_;

           timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
           timer1_.async_wait(strand_.wrap([this](const auto&) {this->print1(); }));
       }
   }

   void print2() {
       if (count_ < 10) {
           std::cout << "Timer 2: " << count_ << std::endl;
           ++count_;

           timer2_.expires_at(timer2_.expires_at() + boost::posix_time::seconds(1));
           timer2_.async_wait(strand_.wrap([this](const auto&) {this->print2(); }));
       }
   }

private:
   boost::asio::io_service::strand strand_;   //用mutex实现的//保证回调事件的相关顺序
   boost::asio::deadline_timer timer1_;
   boost::asio::deadline_timer timer2_;
   int count_;
};

int main() 
{
   boost::asio::io_service io;
   printer p(io);

   std::thread t([&io] { 
       std::cout << "start scecond" << std::endl;
       io.run();
       std::cout << "start scecond finished" << std::endl;
       });
 /*  if (io.stopped()) {
       io.reset();
   }*/
   io.run();
   t.join();

   return 0;
}
