#pragma once
#include <winsock.h>
#include "sConnection.h"
#include "iNetworkProtocol.h"

// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

// Don't put include files in here unless absolutely necessary. IE: Only when
// they are required for member variables below. 

// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm




class cNetworkManager
{
public:
	cNetworkManager();
	virtual ~cNetworkManager();

	int Startup(const char* ip, int port, iNetworkProtocol* protocol);
	void Run();
	void Shutdown();
	void BroadcastMessage(iNetMessage* toBroadcast);
	void nextInQueue();
	// STUDENT TODO: SendMessage to a specific connection method.
	//////////////////////////////////////////////////////////////////
	void sendMessage(sConnection* conn, iNetMessage* toSend);
	void sendMessageToQueue(int socket, iNetMessage* toSend);//, iNetMessage* toSend)

private:
	void SetupListener(const char* ip, int port);
	void SetupFDSets(fd_set& readFDs, fd_set& writeFDs, fd_set& errorFDs) ;
	int ReadMessage(sConnection& conn);

	SOCKET				mListeningSocket;
	ConnectionVector	mConnections;
	iNetworkProtocol*	mProtocol;
	
};
