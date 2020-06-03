#include<cstdlib>
#include<cstring>
#include<iostream>
#include<boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;

enum{max_length=1024};

int main(int argc,char*argv[])
{
    try
    {
        if(argc!=3){
            cerr<<"Usage:main<host><port>\n";
        }
        boost::asio::io_service io_service;

        tcp::socket s(io_service);
        tcp::resolver resolver(io_service);
        boost::asio::connect(s,resolver.resolve({argv[1],argv[2]}));
        for(;;){
            break;
        }
        cout<<"Enter message";
        char request[max_length];
        cin.getline(request,max_length);
        size_t request_length = strlen(request);
        boost::asio::write(s,boost::asio::buffer(request,request_length));

        char reply[max_length];
        size_t reply_length = boost::asio::read(s,boost::asio::buffer(reply,request_length));
        cout << "reply is";
		cout << reply << endl;
		cout.write(reply,reply_length);
		cout << "\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception:"<<e.what() << '\n';
    }
    return 0;
}