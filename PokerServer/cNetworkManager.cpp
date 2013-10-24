#include "cNetworkManager.h"
#include "assert.h"
#include "clientMessages.h"
#include <iostream>

using namespace std;

cNetworkManager::cNetworkManager()
	: mListeningSocket(0)
	, mConnections()
	, mProtocol(NULL)
{
}

cNetworkManager::~cNetworkManager()
{
	delete mProtocol;
}

// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

// Why did I not use the constructor and destructor here? Winsock startup can
// fail. And you'll notice, there is no way of sending return codes to the 
// caller without using exceptions. Exceptions aren't bad, I'm just old school
// and prefer not to use them. ALso, system level classes like this in larger
// applications (like real games - you use something called a *singleton*
// (look it up) to access these systems. And wrapping singletons in exceptions
// are a pain on initialization.

// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm


////////////////////////////////////////////////////////////////////
// Initialize WinSock and bind the listener socket.
int cNetworkManager::Startup(const char* ip, int port, iNetworkProtocol* protocol)
{
	mProtocol = protocol;

	WSAData winsockData;

	if(WSAStartup(MAKEWORD(2, 2), &winsockData) != 0)
	{
		cerr << "An error occured." << endl;
		WSACleanup();
        return 255;
    }

	cout << "Establishing the listener on " << ip << ":" << port << "..." << endl;

    SetupListener(ip, htons(port));

    if(mListeningSocket == INVALID_SOCKET)
	{
        cout << endl << "establish listener" << endl;
        return 3;
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////
// This function should be familiar to you guys. It binds the socket
// tp a socket connections and then listens on a particular port.
void cNetworkManager::SetupListener(const char* ip, int port)
{
	u_long nInterfaceAddr = inet_addr(ip);

    if (nInterfaceAddr != INADDR_NONE)
	{
        mListeningSocket = socket(AF_INET, SOCK_STREAM, 0);

        if(mListeningSocket != INVALID_SOCKET)
		{
            sockaddr_in sinInterface;
            sinInterface.sin_family = AF_INET;
            sinInterface.sin_addr.s_addr = nInterfaceAddr;
            sinInterface.sin_port = port;

            if(bind(mListeningSocket, (sockaddr*)&sinInterface, sizeof(sockaddr_in)) != SOCKET_ERROR)
			{
                listen(mListeningSocket, SOMAXCONN);                 
            }
            else
			{
                cerr << "bind() failed" << endl;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////
// Run the network system by accepting new connections, reading and 
// writing in data.
void cNetworkManager::Run()
{

	this->nextInQueue();
    sockaddr_in sinRemote;
	int nAddrSize = sizeof(sinRemote);

	fd_set readFDs, writeFDs, errorFDs;
	SetupFDSets(readFDs, writeFDs, errorFDs);

	timeval wait;

	wait.tv_sec = 0;
	wait.tv_usec = 10000;		// Wait only 10 milliseconds for any information to come before letting go

	if(select(0, &readFDs, &writeFDs, &errorFDs, &wait) > 0)
	{	
		if(FD_ISSET(mListeningSocket, &readFDs))	// Something happened on one of the sockets. Was it the listener socket?...
		{
			SOCKET sd = accept(mListeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
			
			if(sd != INVALID_SOCKET)
			{
				// Tell user we accepted the socket, and add it to our connecition list
				cout << "Accepted connection from " << inet_ntoa(sinRemote.sin_addr) << ":" << ntohs(sinRemote.sin_port) << ", socket " << sd << "." << endl;
				
				mConnections.push_back(sConnection(sd));

				if ((mConnections.size() + 1) > 64)
				{
					// For the background on this check, see www.tangentsoft.net/wskfaq/advanced.html#64sockets the +1 is to account for the listener socket.
					cout << "WARNING: More than 63 client connections accepted.  This will not work reliably on some Winsock stacks!" << endl;
				}
				
				u_long nNoBlock = 1;
				ioctlsocket(sd, FIONBIO, &nNoBlock);		// Mark the socket as non-blocking
			}
			else
			{
				cerr << "accept() failed" << endl;
				return;
			}
		}
		else if (FD_ISSET(mListeningSocket, &errorFDs))
		{
			int err;
			int errlen = sizeof(err);

			getsockopt(mListeningSocket, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
			cerr << "Exception on listening socket: " << err << endl;
			return;
		}
		
		ConnectionVectorI it = mConnections.begin();		// ...Or was it one of the client sockets?
		while(it != mConnections.end())
		{



			bool		bOK			= true;
			const char* pcErrorType = 0;
			
			if (FD_ISSET(it->Socket, &errorFDs))			// See if this socket's flag is set in any of the FD sets.
			{
				bOK = false;
				pcErrorType = "General socket error";
				FD_CLR(it->Socket, &errorFDs);
			}
			else
			{
				if(FD_ISSET(it->Socket, &readFDs))
				{
					cout << "Socket " << it->Socket << " became readable; handling it." << endl;
					bOK = it->ReadData();
					pcErrorType = "Read error";
					FD_CLR(it->Socket, &readFDs);
				}

				if(FD_ISSET(it->Socket, &writeFDs))
				{
					cout << "Socket " << it->Socket << " became writable; handling it." << endl;
					bOK = it->WriteData();
					//std::cout << it->WriteBuffer << std::endl;
					pcErrorType = "Write error";
					FD_CLR(it->Socket, &writeFDs);
					Sleep(50);
				}
			}

			if(!bOK)										// Something bad happened on the socket, or the client closed its half of the connection.  Shut the conn down and remove it from the list.
			{	
				int err		= 0;
				int errlen	= sizeof(err);

				getsockopt(it->Socket, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);

				cDisconnectNetMessage* dis = new cDisconnectNetMessage(); 
				mProtocol->ProcessMessage(dis, *it._Ptr);

				if(err != NO_ERROR)
				{
					cerr << "Socket option error: " << pcErrorType << err << endl;
				}

				it->Shutdown();

				mConnections.erase(it);
				it = mConnections.begin();
			}
			else
			{
				ReadMessage(*it);	// Process the data in the packet and then go on to next connection							
				++it;
			}
		}
    }
}

//// SetupFDSets ///////////////////////////////////////////////////////
// Set up the three FD sets used with select() with the sockets in the
// connection list.  Also add one for the listener socket, if we have
// one.
void cNetworkManager::SetupFDSets(fd_set& readFDs, fd_set& writeFDs, fd_set& errorFDs) 
{
    FD_ZERO(&readFDs);
    FD_ZERO(&writeFDs);
    FD_ZERO(&errorFDs);

    // Add the listener socket to the read and except FD sets, if there is one.
    if(mListeningSocket != INVALID_SOCKET)
	{
        FD_SET(mListeningSocket, &readFDs);
        FD_SET(mListeningSocket, &errorFDs);
    }

    // Add client connections
    ConnectionVectorI it = mConnections.begin();

    while(it != mConnections.end())
	{

		if(it->ReadSize < BUFFER_SIZE)
		{
			// There's space in the read buffer, so pay attention to
			// incoming data.
			FD_SET(it->Socket, &readFDs);
		}

        if (it->WriteSize > 0)
		{
            // There's data still to be sent on this socket, so we need
            // to be signalled when it becomes writable.
            FD_SET(it->Socket, &writeFDs);
        }

        FD_SET(it->Socket, &errorFDs);
        ++it;
    }
}

////////////////////////////////////////////////////////////////////
// Figure out what net messages are coming down the wire by looking
// at the list inside the network protocol 
int cNetworkManager::ReadMessage(sConnection& conn)
{	
	int processed	= 0;

	while(conn.ReadSize >= sizeof(int))	// make sure we at least have the id of the message
	{	
		int*					id				= reinterpret_cast<int*>(conn.ReadBuffer + processed);	// Treat it as an integer for speed
		const NetMessageMap&	validMessages	= mProtocol->GetValidMessages();
		NetMessageMapCI			netMsgIt		= validMessages.find(*id);

		if(netMsgIt == validMessages.end())	// Check with the protocol if such a packet exists
		{
			cerr << "Ignoring message (size " << conn.ReadSize << ") - message type not found: "<< (conn.ReadBuffer + conn.ReadSize) << endl;
			conn.ReadSize = 0;
		}
		else
		{
			int messageSize = netMsgIt->second->GetMinimumNetworkSize();

			if(conn.ReadSize >= messageSize)							// we have the whole message, so spawn a new one
			{				
				iNetMessage* newMsg = netMsgIt->second->Spawn();

				int actualSize = newMsg->ReadFrom(conn.ReadBuffer + processed);

				conn.ReadSize -= actualSize;
				processed += actualSize;

				mProtocol->ProcessMessage(newMsg, conn);

				delete newMsg;										// delete it right after we are done!
			}
			else													// not enough data, break and wait for next read
			{
				return processed;
			}
		}
	}

	return 0;
}

void cNetworkManager::BroadcastMessage(iNetMessage* toBroadcast)
{
	for(ConnectionVectorI conn = mConnections.begin(); conn != mConnections.end(); conn++)
	{
		outMessage toPush;
		toPush.isShared = 1;
		toPush.message = toBroadcast;
		//// If this assert trips, that means our write buffer is backed up.
		//// It's a hint that there is something wrong. But this assert isn't bullet proof,
		////  and it won't  catch when variable messages overflow the data! Be careful!

		//Assert(BUFFER_SIZE - conn.WriteSize > toBroadcast->GetMinimumNetworkSize());

		//conn.WriteSize += toBroadcast->WriteTo(conn.WriteBuffer + conn.WriteSize);

		//Assert(conn.WriteSize <= BUFFER_SIZE);	// This assert will catch any additional overflow, assuming your WriteTo() function return is correct
		conn->outQueue.push(toPush);
	}
	int bob = 0;
}


//places the current message into a queue to be sent later in preoper order.
void cNetworkManager::sendMessageToQueue(int socket, iNetMessage* toSend)//, iNetMessage* toSend)
{

	outMessage toPush;
	toPush.isShared = 0;
	toPush.message = toSend;
	bool notFound = true;
	//add seach for the connection object with the specified socket and add the message to it.
	ConnectionVectorI it = mConnections.begin();		// ...Or was it one of the client sockets?
	while(it != mConnections.end())
	{
		if(it->Socket == socket)
		{
			it->outQueue.push(toPush);
			//notFound = false;
			break;
		}
		it++;
	}


	//conn.outQueue.push(toPush);
}


//send the current message to the connection.
void cNetworkManager::sendMessage(sConnection* conn, iNetMessage* toSend)//, iNetMessage* toSend)
{
		Assert(BUFFER_SIZE - conn->WriteSize > toSend->GetMinimumNetworkSize());

		conn->WriteSize += toSend->WriteTo(conn->WriteBuffer + conn->WriteSize);

		Assert(conn->WriteSize <= BUFFER_SIZE);	// This assert will catch any additional overflow, assuming your WriteTo() function return is correct

		conn->WriteData();

}



// loads the next message form the Queue for each connection
void cNetworkManager::nextInQueue()
{
	bool isShared = false;
	
	for(ConnectionVectorI conn = mConnections.begin(); conn != mConnections.end(); conn++)
	{
		if(conn->outQueue.size() > 0)// does the queue have any messages?
		{
			isShared = conn->nextInQueue();
		}
	}
	if(isShared)// if a shared message was sent, remove it now
	{
		ConnectionVectorI conn = mConnections.begin();
		delete conn->outQueue.front().message;
		for(; conn != mConnections.end(); conn++)
		{
			conn->outQueue.pop();
		}
	}
}





void cNetworkManager::Shutdown()
{
	// STUDENT TODO: For the student to finish! Look at BasicServer on
	// how to shutdown Winsock properly.
	//////////////////////////////////////////////////////////////////
	WSACleanup();
}