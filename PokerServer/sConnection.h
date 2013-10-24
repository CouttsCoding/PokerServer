#pragma once
#include <vector>
#include <winsock.h>
#include <queue>
#include "iNetMessage.h"
#define BUFFER_SIZE	1024



struct outMessage
{
	int isShared;
	iNetMessage* message;
	
};


struct sConnection
{
public:
	sConnection::sConnection();
	sConnection(SOCKET newSocket);
	~sConnection();

	bool ReadData();
	bool WriteData();
	bool nextInQueue();
	void Shutdown();

	SOCKET	Socket;
	char	ReadBuffer[BUFFER_SIZE];
	int		ReadSize;
	char	WriteBuffer[BUFFER_SIZE];
	int		WriteSize;
	std::queue<outMessage> outQueue;
};

typedef std::vector<sConnection>	ConnectionVector;
typedef ConnectionVector::iterator	ConnectionVectorI;
