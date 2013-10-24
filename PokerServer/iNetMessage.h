#pragma once
#include <map>

#define MAKEID(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

struct iNetMessage
{
	int	Id;

	iNetMessage(int messageId)
		: Id(messageId)
	{
	}

	virtual ~iNetMessage()
	{
	}

	virtual int GetMinimumNetworkSize() const
	{
		return sizeof(int);
	}

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	// Here's a neat design pattern; the spawner. Call the same generic method,
	// and get something custom back depending on what is derived! Also known
	// more common as a factory method in the Java world.

	// MARK's TIP: MmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmMm

	virtual iNetMessage* Spawn() const = 0;
	
	////////////////////////////////////////////////////////////////////
	// When reading from the buffer, return the number of bytes read. Make
	// sure you call this parent method as the first line in the derived class!
	virtual int ReadFrom(char* buffer)
	{
		int*	id	= reinterpret_cast<int*>(buffer);
		
		Id = *id;

		return sizeof(int);
	}

	////////////////////////////////////////////////////////////////////
	// When writing from the buffer, return the number of bytes read. Make
	// sure you call this parent method as the first line in the derived class!
	virtual int WriteTo(char* buffer)
	{
		*reinterpret_cast<int*>(buffer) = Id;

		return sizeof(int);
	}
};

typedef std::map<int, const iNetMessage*>	NetMessageMap;
typedef NetMessageMap::const_iterator		NetMessageMapCI;
