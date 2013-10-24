#pragma once

#include "cHand.h"
//#include "sConnection.h"


class cPokerPlayer
{
public:
	cPokerPlayer();
	~cPokerPlayer();

	cHand myHand;
	int money;
	int index;	//if needed
	bool in;	// if the player has folded or not
	bool connected;
	bool nextRound; // if the player is in for antoher round.
	//sConnection* conn;
	int socket;

};
