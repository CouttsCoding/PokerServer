#include "sConnection.h"
#include "assert.h"
#include <iostream>

using namespace std;


sConnection::sConnection()
{
}

sConnection::sConnection(SOCKET newSocket)
	: Socket(newSocket)
	, ReadSize(0)
	, WriteSize(0)
{
	memset(ReadBuffer, 0, sizeof(char) * BUFFER_SIZE);
	memset(WriteBuffer, 0, sizeof(char) * BUFFER_SIZE);
}

sConnection::~sConnection(void)
{
}

////////////////////////////////////////////////////////////////////////
// Data came in on a client socket, so read it into the buffer.  Returns
// false on failure, or when the client closes its half of the
// connection.  (WSAEWOULDBLOCK doesn't count as a failure.)
bool sConnection::ReadData() 
{
    int numBytes = recv(Socket, ReadBuffer + ReadSize, BUFFER_SIZE - ReadSize, 0);

    if (numBytes == 0)
	{
        cout << "Socket " << Socket << " was closed by the client. Shutting down." << endl;
        return false;
    }
    else if (numBytes == SOCKET_ERROR)
	{
        // Something bad happened on the socket.  It could just be a
        // "would block" notification, or it could be something more
        // serious.  Let caller handle the latter case.  WSAEWOULDBLOCK
        // can happen after select() says a socket is readable under
        // Win9x: it doesn't happen on WinNT/2000 or on Unix.
        int err;
        int errlen = sizeof(err);
        getsockopt(Socket, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
        return (err == WSAEWOULDBLOCK);
    }

    ReadSize += numBytes;	// We read some bytes.  Advance the buffer size counter.
    
    return true;
}


////////////////////////////////////////////////////////////////////////
// The connection is writable, so send any pending data.  Returns
// false on failure.  (WSAEWOULDBLOCK doesn't count as a failure.)
bool sConnection::WriteData() 
{
    int numBytes = send(Socket, WriteBuffer, WriteSize, 0);

    if (numBytes == SOCKET_ERROR)
	{
        // Something bad happened on the socket.  Deal with it.
        int err;
        int errlen = sizeof(err);
        getsockopt(Socket, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
        return (err == WSAEWOULDBLOCK);
    }
    
    if(numBytes == WriteSize)
	{   
        WriteSize = 0;	// Everything got sent, so take a shortcut on clearing buffer.
		memset(WriteBuffer, 0, sizeof(char) * BUFFER_SIZE); // clears the write buffer
    }
    else
	{
        WriteSize -= numBytes;								// We sent part of the buffer's data.  Remove that data from the buffer
        memmove(WriteBuffer, WriteBuffer + numBytes, WriteSize);
    }
    
    return true;
}




bool sConnection::nextInQueue()// loads the next message in the connections out going queue
{
	// If this assert trips, that means our write buffer is backed up.
		// It's a hint that there is something wrong. But this assert isn't bullet proof,
		//  and it won't  catch when variable messages overflow the data! Be careful!
	

		//// If this assert trips, that means our write buffer is backed up.
		//// It's a hint that there is something wrong. But this assert isn't bullet proof,
		////  and it won't  catch when variable messages overflow the data! Be careful!
		
	Assert(BUFFER_SIZE - this->WriteSize > this->outQueue.front().message->GetMinimumNetworkSize());

		this->WriteSize += this->outQueue.front().message->WriteTo(this->WriteBuffer + this->WriteSize);

		Assert(this->WriteSize <= BUFFER_SIZE);	// This assert will catch any additional overflow, assuming your WriteTo() function return is correct

		if(this->outQueue.front().isShared == 0)// if a client specific message is sent, after loading it into the buffer, delte it and remove it form the queue
		{
			delete this->outQueue.front().message;
			this->outQueue.pop();
			return false;
		}
		return true;
	
}



void sConnection::Shutdown()
{
	// STUDENT TODO: For the student to finish! Look at BasicServer on
	// how to shutdown a single connection properly.
	//////////////////////////////////////////////////////////////////
	WSACleanup();
}