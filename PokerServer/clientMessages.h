#pragma once

// STUDENT TODO: Place all messages in the protocol that originate
// on the client and go to the server here.
//////////////////////////////////////////////////////////////////

// Message format starts with c for "Client"

struct sJoinNetMessage : public iNetMessage
{
	const static int s_Id = MAKEID('c', 'P', 'J', 'N');	// For "client player join"


	sJoinNetMessage()
		: iNetMessage(s_Id)
	{
	}

	virtual ~sJoinNetMessage()
	{
	}

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	// Why do I explicitly set the size using math? Why not just do plain old
	// sizeof(*this)? Because of padding!!

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize();	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new sJoinNetMessage();
	}

	int ReadFrom(char* buffer)
	{
		buffer += iNetMessage::ReadFrom(buffer);

		//int len = strlen(buffer) + 1;

		//Name = new char[len];	// Because we know the next member is a string
		//strcpy(Name, buffer);

		return 4 ;//+ len;			// Don't forget the 4 bytes we read in from the parent
	}

	// We don't derive the WriteTo() because we never since this struct to the client!!
};
struct cDisconnectNetMessage : public iNetMessage
{
	const static int s_Id = MAKEID('c', 'D', 'S', 'C');	// For "client player join"


	cDisconnectNetMessage()
		: iNetMessage(s_Id)
	{
	}

	virtual ~cDisconnectNetMessage()
	{
	}

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	// Why do I explicitly set the size using math? Why not just do plain old
	// sizeof(*this)? Because of padding!!

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize();	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new cDisconnectNetMessage();
	}

	int ReadFrom(char* buffer)
	{
		buffer += iNetMessage::ReadFrom(buffer);

		//int len = strlen(buffer) + 1;

		//Name = new char[len];	// Because we know the next member is a string
		//strcpy(Name, buffer);

		return 4 ;//+ len;			// Don't forget the 4 bytes we read in from the parent
	}

	// We don't derive the WriteTo() because we never since this struct to the client!!
};

struct cNewGameMessage : public iNetMessage
{
	const static int s_Id = MAKEID('c', 'P', 'A', 'M');	// For "client player join"

	int pID;

	cNewGameMessage()
		: iNetMessage(s_Id)
	{
	}

	virtual ~cNewGameMessage()
	{
	}

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	// Why do I explicitly set the size using math? Why not just do plain old
	// sizeof(*this)? Because of padding!!

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize() + sizeof(int);	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new cNewGameMessage();
	}

	int ReadFrom(char* buffer)
	{
		buffer += iNetMessage::ReadFrom(buffer);
		memcpy(&pID, buffer, sizeof(int));

		return GetMinimumNetworkSize();//+ len;			// Don't forget the 4 bytes we read in from the parent
	}

	// We don't derive the WriteTo() because we never since this struct to the client!!
};





struct sBetMessage : public iNetMessage
{
	const static int s_Id = MAKEID('c', 'B', 'E', 'T');	// For "client player join"

	int pID;
	int bet;

	sBetMessage()
		: iNetMessage(s_Id)
		, pID(-1)
		, bet(0)
	{
	}

	virtual ~sBetMessage()
	{
	}

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	// Why do I explicitly set the size using math? Why not just do plain old
	// sizeof(*this)? Because of padding!!

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize() + sizeof(int) * 2;	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new sBetMessage();
	}

	int ReadFrom(char* buffer)
	{
		buffer += iNetMessage::ReadFrom(buffer);
		memcpy(&pID, buffer, sizeof(int)*2);
		





		//int len = strlen(buffer) + 1;

		//Name = new char[len];	// Because we know the next member is a string
		//strcpy(Name, buffer);

		return GetMinimumNetworkSize() ;//+ len;			// Don't forget the 4 bytes we read in from the parent
	}

	// We don't derive the WriteTo() because we never since this struct to the client!!
};