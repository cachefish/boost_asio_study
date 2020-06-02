#include<iostream>
#include<boost/asio.hpp>
#include<functional>
#include<memory>
#include<vector>
#include<boost/date_time/posix_time/posix_time.hpp>
//关于用bind和lambda绑定成员函数

class printer
{
public:
    printer(boost::asio::io_service&io):timer_(io,boost::posix_time::seconds(1)),count_(0)
    {
        //timer_.async_wait(std::bind(&printer::print,this));
        timer_.async_wait([this](const auto&error){
                if(error==boost::asio::error::operation_aborted){
                    std::cout<<"cancel now\n";
                }else{ 
                        this->print();
                }           
        });
    }
    ~printer()
    {
            std::cout<<"final count is"<<count_<<std::endl;
    }
    void print()
    {
        if(count_<5)
        {
            std::cout<<count_<<std::endl;
            ++count_;

            timer_.expires_at(timer_.expires_at()+boost::posix_time::seconds(1));
          //  timer_.async_wait(std::bind(&printer::print,this));
            timer_.async_wait([this](const auto&error){             //()中需要有const auto&err，否则会出现绑定的标签不一致
                if(error==boost::asio::error::operation_aborted){
                    std::cout<<"cancel now\n";
                }else{ 
                        this->print();
                }    
        });
        }
    }
private:
    boost::asio::deadline_timer timer_;
    int count_;
};

int main()
{
    boost::asio::io_service io;
    printer p(io);
    io.run();
    return  0;
}