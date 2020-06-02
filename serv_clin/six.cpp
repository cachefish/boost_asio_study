#include<iostream>
#include<boost/array.hpp>
#include<boost/asio.hpp>

using boost::asio::ip::tcp;

//同步阻塞的客户端
int main(int argc,char*argv[])
{
    try{
        if(argc!=2){
            std::cerr<<"Usage:client<host>"<<std::endl;
            return 1;
        }
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);//用来解析io_service地址
        tcp::resolver::query query(argv[1],"daytime");//host地址  daytime tcp端口号
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query); //返回一个迭代器 是一个可用的地址


        tcp::socket socket(io_service); //开启socket事件
        boost::asio::connect(socket,endpoint_iterator);
        for(;;){
            boost::array<char,128> buf;
            boost::system::error_code error;
            size_t len = socket.read_some(boost::asio::buffer(buf),error);

            if(error==boost::asio::error::eof){  //文件结束
                break;
            }else if(error){
                throw boost::system::system_error(error);
            }
            std::cout.write(buf.data(),len);     //输出内容
        }
    }
    catch(std::exception&e){
        std::cerr<<e.what()<<std::endl;
    }
    system("pause");
    return 0;
}