#pragma once

// STUDENT TODO: Place all messages in the protocol that originate
// on the server and go to the client here.
//////////////////////////////////////////////////////////////////

// Message format starts with s for "Server"


//struct TableState
struct sTableMessage : public iNetMessage 
{
	const static int s_Id = MAKEID('s', 'U', 'D', 'T');	// For "client player join"

	int pot;
	//int lastBet;
	int actionTo;
	//char* message;
	//int hands[20];
	//cHand hands[4];
	int playerCount;
	int handCount;
	int hands[32];
	int playerMoney[4];
	int pToCall[4];

	// Default c'tor
	sTableMessage()
		: iNetMessage(s_Id)
	{
	}
	// d'tor
	virtual ~sTableMessage()
	{
		
	}


	int WriteTo(char* buffer)
	{
		buffer  += iNetMessage::WriteTo(buffer);
		memcpy(buffer, &pot, sizeof(int)*45);
		//int newhand = -1;
		//int suit = 0;
		//int face = 0;
		//int toCall = 0;
		//
		//std::string Text = "";

		//iNetMessage::WriteTo(buffer);

		//memcpy(buffer, &pot, 4);
		////memcpy(buffer, &lastBet, 4);
		//memcpy(buffer, &actionTo, 4);
		//memcpy(buffer, &playerCount, 4);	// pot actionTo playercount  handCount (cards in hand) playermoney
		//

		//
		//for(int p = 0; p < 4; p++)
		//{
		//	if(p > this->playerCount)
		//	{
		//		int placeHolder = 0;
		//		for(int e = 0; e < 13; e++)
		//			memcpy(buffer, &placeHolder, 4);	

		//	}
		//	else
		//	{
		//		handCount = hands[p].getHandSize();
		//		memcpy(buffer, &handCount, 4);

		//		for(int h = 1; h < 5; h++)
		//		{
		//			suit = hands[p].CardSuitAtIndex(h);
		//			face = hands[p].CardValueAtIndex(h);
		//			memcpy(buffer, &suit, 4);
		//			memcpy(buffer, &face, 4);
		//		}
		//		toCall = lastBet - pLastBet[p];
		//		memcpy(buffer, &toCall, 4);
		//		memcpy(buffer, &playerMoney[p], 4);
		//	}
		//}
		//strcat(buffer, Text);
		//return the length of the buffer
		return GetMinimumNetworkSize();
	}

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize() + (

			(
			sizeof(int)*45
			)



			);	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new sTableMessage();
	}
};



struct sHandMessage : public iNetMessage 
{
	const static int s_Id = MAKEID('s', 'U', 'D', 'H');	// For "client player join"

	//int pId;
	int handCount;
	//cHand hand;
	int hand[10];


	// Default c'tor
	sHandMessage()
		: iNetMessage(s_Id)
	{
	}
	// d'tor
	virtual ~sHandMessage()
	{
		
	}


	int WriteTo(char* buffer)
	{
		//int newhand = -1;
		//int suit = -1;
		//int face = -1;
		//std::string Text = "";

		buffer += iNetMessage::WriteTo(buffer);

		memcpy(buffer, &handCount, sizeof(int)*11);

		//handCount = hand.getHandSize();
		//memcpy(buffer, &handCount, 4);

		//for(int h = 0; h < handCount; h++)
		//{
		//	suit = hand.CardSuitAtIndex(h);
		//	face = hand.CardValueAtIndex(h);
		//	memcpy(buffer, &suit, 4);
		//	memcpy(buffer, &face, 4);
		//}

		////strcat(buffer, Text);
		////return the length of the buffer
		return GetMinimumNetworkSize();
	}

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize() + (

			sizeof(int)*11



			);	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new sHandMessage();
	}
};




//struct TableState
struct sEndGameMessage : public iNetMessage 
{
	const static int s_Id = MAKEID('s', 'E', 'G', 'M');	// For "client player join"

	/*cHand hands[4];*/
	int winID;
	int winHand;
	int playerCount;	
	int hands[40];
	int playerMoney[4];

	

	// Default c'tor
	sEndGameMessage()
		: iNetMessage(s_Id)
	{
	}
	// d'tor
	virtual ~sEndGameMessage()
	{	
	}


	int WriteTo(char* buffer)
	{
		buffer += iNetMessage::WriteTo(buffer);
		memcpy(buffer, &winID, sizeof(int)*51);


		//int newhand = -1;
		//int suit = -1;
		//int face = -1;
		//std::string Text = "";

		//buffer += iNetMessage::WriteTo(buffer);

		//memcpy(buffer, &winID, 4);
		//memcpy(buffer, &winHand, 4);

		//memcpy(buffer, &playerCount, 4);

		//
		//for(int p = 0; p < playerCount; p++)
		//{
		//	memcpy(buffer, &handCount, 4);

		//	for(int h = 0; h < handCount; h++)
		//	{
		//		suit = hands[p].CardSuitAtIndex(h);
		//		face = hands[p].CardValueAtIndex(h);
		//		memcpy(buffer, &suit, 4);
		//		memcpy(buffer, &face, 4);
		//	}

		//	memcpy(buffer, &newhand, 4);// Replace this with players money.
		//}
		//strcat(buffer, Text);
		//return the length of the buffer
		return GetMinimumNetworkSize();
	}

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize() + (

			sizeof(int)*47);	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new sEndGameMessage();
	}
};






//struct TableState
struct sRespondMessage : public iNetMessage 
{
	const static int s_Id = MAKEID('s', 'R', 'S', 'P');	// For "client player join"

	int ID;

	
	// Default c'tor
	sRespondMessage()
		: iNetMessage(s_Id)
		, ID(-1)
	{
	}
	// d'tor
	virtual ~sRespondMessage()
	{
		
	}


	int WriteTo(char* buffer)
	{
		buffer += iNetMessage::WriteTo(buffer);
		
		memcpy(buffer, &ID, 4);	// THIS IS DANGEROUS! But we're pros, so this works

		//return the length of the buffer
		return GetMinimumNetworkSize();
	}

	int GetMinimumNetworkSize() const
	{
		return iNetMessage::GetMinimumNetworkSize() + sizeof(int);	// FOr at least null terminating zero
	}

	iNetMessage* Spawn() const
	{	
		return new sRespondMessage();
	}
};
