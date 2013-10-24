#pragma once
#include "iNetMessage.h"

struct sConnection;

// Override this interface so that the data coming from the network can be used in the game's server.
// This is the glue that combines the game to the network layer.
class iNetworkProtocol
{
public:
	virtual void ProcessMessage(iNetMessage* message, sConnection conn) = 0;

	const NetMessageMap& GetValidMessages() const
	{
		return mValidMessages;
	}

protected:
	NetMessageMap	mValidMessages;	// Once instance of each valid message in the protocol, so that we can see if it's legit or not
};