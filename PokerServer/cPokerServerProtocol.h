#pragma once
#include "inetworkprotocol.h"
#include "sConnection.h"
#include "cPokerGame.h"
class cPokerServerProtocol : public iNetworkProtocol
{
public:
	cPokerServerProtocol();
	cPokerServerProtocol( cPokerGame* game);
	virtual ~cPokerServerProtocol();
	void ProcessMessage(iNetMessage* message, sConnection conn);

	cPokerGame* mGame;
};
