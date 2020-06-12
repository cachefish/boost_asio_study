#ifndef FND_STRUCT_HEADER_H
#define FND_STRUCT_HEADER_H
#include <string>
struct Header {
	int bodySize;
	int type;
};

enum MessageType {
	MT_BIND_NAME = 1, // {"name" : "abc"}
	MT_CHAT_INFO = 2, // {"information" "what i say"}
	MT_ROOM_INFO = 3, // {"name" : "abc", "information" : "what i say"}
};

// client send
struct BindName {
	char name[32];
	int nameLen;
};

// client send
struct ChatInformation {
	char information[256];
	int infoLen;
};


// serversend
struct RoomInformation {
	BindName name;
	ChatInformation chat;
};

bool parseMessage(const std::string &input, int *type, std::string &outbuffer);
bool parseMessage2(const std::string &input, int *type, std::string &outbuffer);
bool parseMessage3(const std::string& input, int * type, std::string& outbuffer);
#endif