#include "cPokerServerProtocol.h"
#include "sConnection.h"
#include "netMessages.h"
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////
// In the constructor is where we "register" all our valid incoming
// messages we want the protocol to listen for. So therefore we only
// need to register client and duplex messages by inserting them into
// our map.
cPokerServerProtocol::cPokerServerProtocol(cPokerGame* game)
	: mGame(game)
{
	mValidMessages[sJoinNetMessage::s_Id] = new sJoinNetMessage();
	mValidMessages[sPingNetMessage::s_Id] = new sPingNetMessage();
	mValidMessages[sBetMessage::s_Id] = new sBetMessage();
	mValidMessages[cNewGameMessage::s_Id] = new cNewGameMessage();
	mValidMessages[cDisconnectNetMessage::s_Id] = new cDisconnectNetMessage();
}

cPokerServerProtocol::~cPokerServerProtocol()
{
}

////////////////////////////////////////////////////////////////////
// This is where all the action happens. Here we look at the data
// that was filled in the buffer of the connection, and then decide
// to do something with it.
void cPokerServerProtocol::ProcessMessage(iNetMessage* message, sConnection conn)
{
	
	switch(message->Id)
	{
	case sJoinNetMessage::s_Id:
		cout << "Received client player join packet!" << endl;

			if(this->mGame->gameState == WAIT && this->mGame->playerCount < 3 )// check uf the game is full, and and if it is between rounds
			{
				this->mGame->addPlayer(conn);
				sRespondMessage resp;
				resp.ID = this->mGame->playerCount;
				this->mGame->mNetwork->sendMessage(&conn, &resp);
			}
			else if(!mGame->nextRound)// check if the game is between rounds, and check if any spots are availible
			{
				int id = mGame->checkConnected(conn);
				if(id != -2)// no spots open
				{
					sRespondMessage resp;
					resp.ID = id;
					this->mGame->mNetwork->sendMessage(&conn, &resp);
				}
				else// spot open
				{
					sRespondMessage resp;
					resp.ID = id;
					this->mGame->mNetwork->sendMessage(&conn, &resp);
				}
			}
			else// no room, try again later
			{
				sRespondMessage resp;
				resp.ID = -2;
				this->mGame->mNetwork->sendMessage(&conn, &resp);
			}

		
		
		break;
	case sPingNetMessage::s_Id:
		cout << "Received ping packet!" << endl;
		
		// STUDENT TODO: Do something with this packet, but remember,
		// it's going to be deleted just outside of this packet!
		//////////////////////////////////////////////////////////////////
		break;

		case cDisconnectNetMessage::s_Id:// player disconnected
			{
				mGame->PlayerLeft(conn);
			}

		
		break;

	
	case cNewGameMessage::s_Id:// existing players choose to play agian
		{
			cNewGameMessage* ngame = (cNewGameMessage*)message;
			mGame->StartNewRound(ngame->pID);
			
		}
		break;


		case sBetMessage::s_Id:// some one has bet or folded.
		sBetMessage* bet = (sBetMessage*)message;

		mGame->Bet(bet->pID, bet->bet);
		
		break;

	}
}