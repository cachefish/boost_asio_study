#ifndef _SERI_H__
#define _SERI_H__
#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>

class SBindName
{
    friend class boost::serialization::access;
public:
    SBindName(std::string name):m_bindName(std::move(name))
    {

    }
    SBindName(){}
    const std::string &bindName()const{return m_bindName;}
private:
    template<class Archive>
    void serialize(Archive &ar,const unsigned int version)
    {
        ar&m_bindName;
    }
private:
    std::string m_bindName;
};

class SChatInfo
{
    friend class boost::serialization::access;
public:
    SChatInfo(std::string info):m_chatInformation(std::move(info))
    {

    }
    SChatInfo(){}
    const std::string &chatInformation()const{return m_chatInformation;}
private:
    template<class Archive>
    void serialize(Archive &ar,const unsigned int version){
        ar&m_chatInformation;
    }
private:
    std::string m_chatInformation;
};



class SRoomInfo
{
    friend class boost::serialization::access;
public:
    SRoomInfo(){}
    SRoomInfo(std::string name,std::string info)
    :m_bind(std::move(name))
    ,m_chat(std::move(info))
    {

    }
    const std::string&name()const{return m_bind.bindName();}
    const std::string&information()const{return m_chat.chatInformation();}
private:
    template<class Archive>
    void serialize(Archive&ar,const unsigned int version)
    {
        ar&m_bind;
        ar&m_chat;
    }
private:
    SBindName m_bind;
    SChatInfo  m_chat;
};






#endif