#pragma once

// STUDENT TODO: Place all messages in the protocol that originate
// on either client or server here.
//////////////////////////////////////////////////////////////////

// Message format starts with d for "Duplex"

struct sPingNetMessage : public iNetMessage
{
	const static int s_Id = MAKEID('d', 'P', 'N', 'G');	// For "both ping"
	
	int		Count;
	bool	FromClient;

	sPingNetMessage()
		: iNetMessage(s_Id)
		, FromClient(false)
		, Count(0)
	{
	}

	virtual ~sPingNetMessage()
	{
	}

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize() + 5;	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new sPingNetMessage();
	}

	int ReadFrom(char* buffer)
	{
		buffer += iNetMessage::ReadFrom(buffer);

		memcpy(&Count, buffer, 5);				// THIS IS DANGEROUS! But we're pros, so this works

		return GetMinimumNetworkSize();			// Don't forget the 4 bytes we read in from the parent
	}

	int WriteTo(char* buffer)
	{
		buffer += iNetMessage::WriteTo(buffer);

		memcpy(buffer, &Count, 5);				// THIS IS DANGEROUS! But we're pros, so this works

		return GetMinimumNetworkSize();			// Don't forget the 4 bytes we read in from the parent
	}
};