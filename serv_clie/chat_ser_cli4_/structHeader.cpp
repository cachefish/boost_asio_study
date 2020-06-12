#include "structHeader.h"
#include "SerilizationObject.h"
#include "JsonObject.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
template <typename T> std::string seriliaze(const T &obj) {
  std::stringstream ss;
  boost::archive::text_oarchive oa(ss);
  oa & obj;
  return ss.str();
}

bool parseMessage3(const std::string &input, int *type,
                   std::string &outbuffer) {
  auto pos = input.find_first_of(" ");
  if (pos == std::string::npos)
    return false;
  if (pos == 0)
    return false;
	// "BindName ok" -> substr -> BindName
  auto command = input.substr(0, pos);
  if (command == "BindName") {
    // we try to bind name
    std::string name = input.substr(pos + 1);
    if (name.size() > 32)
      return false;
    if (type)
      *type = MT_BIND_NAME;
		ptree tree;
		tree.put("name", name);
		outbuffer = ptreeToJsonString(tree);
    //outbuffer = seriliaze(SBindName(std::move(name)));
    return true;
  } else if (command == "Chat") {
    // we try to chat
    std::string chat = input.substr(pos + 1);
    if (chat.size() > 256)
      return false;
		ptree tree;
		tree.put("information", chat);
		outbuffer = ptreeToJsonString(tree);
		//outbuffer = seriliaze(SChatInfo(std::move(chat)));
    if (type)
      *type = MT_CHAT_INFO;
    return true;
  }
  return false;
}

bool parseMessage2(const std::string &input, int *type,
                   std::string &outbuffer) {
  auto pos = input.find_first_of(" ");
  if (pos == std::string::npos)
    return false;
  if (pos == 0)
    return false;
	// "BindName ok" -> substr -> BindName
  auto command = input.substr(0, pos);
  if (command == "BindName") {
    // we try to bind name
    std::string name = input.substr(pos + 1);
    if (name.size() > 32)
      return false;
    if (type)
      *type = MT_BIND_NAME;
    //SBindName bindInfo(std::move(name));
    outbuffer = seriliaze(SBindName(std::move(name)));
    return true;
  } else if (command == "Chat") {
    // we try to chat
    std::string chat = input.substr(pos + 1);
    if (chat.size() > 256)
      return false;
		outbuffer = seriliaze(SChatInfo(std::move(chat)));
//    ChatInformation info;
//    info.infoLen = chat.size();
//    std::memcpy(&(info.information), chat.data(), chat.size());
//    auto buffer = reinterpret_cast<const char *>(&info);
//    outbuffer.assign(buffer, buffer + sizeof(info));
    if (type)
      *type = MT_CHAT_INFO;
    return true;
  }
  return false;
}
// cmd messagebody
bool parseMessage(const std::string &input, int *type, std::string &outbuffer) {
  // input should be cmd body
  auto pos = input.find_first_of(" ");
  if (pos == std::string::npos)
    return false;
  if (pos == 0)
    return false;
	// "BindName ok" -> substr -> BindName
  auto command = input.substr(0, pos);
  if (command == "BindName") {
    // we try to bind name
    std::string name = input.substr(pos + 1);
    if (name.size() > 32)
      return false;
    if (type)
      *type = MT_BIND_NAME;
    BindName bindInfo;
    bindInfo.nameLen = name.size();
    std::memcpy(&(bindInfo.name), name.data(), name.size());
    auto buffer = reinterpret_cast<const char *>(&bindInfo);
    outbuffer.assign(buffer, buffer + sizeof(bindInfo));
    return true;
  } else if (command == "Chat") {
    // we try to chat
    std::string chat = input.substr(pos + 1);
    if (chat.size() > 256)
      return false;
    ChatInformation info;
    info.infoLen = chat.size();
    std::memcpy(&(info.information), chat.data(), chat.size());
    auto buffer = reinterpret_cast<const char *>(&info);
    outbuffer.assign(buffer, buffer + sizeof(info));
    if (type)
      *type = MT_CHAT_INFO;
    return true;
  }
  return false;
}

