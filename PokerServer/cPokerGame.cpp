#include "cPokerGame.h"
#include <string.h>
#include <algorithm>
#include <ctime>

bool cPokerGame::_seeded = false;


cPokerGame::cPokerGame(cNetworkManager* network)
	:mNetwork(network)
{
	this->betCount = 0;
	this->playerCount = -1;
	this->gameState = WAIT;
	this->actionTo = 0;
	this->gameStarted = false;
	this->betRecieved = false;
	this->lastBet = -2;
	this->startPlayer= 0;
	this->foldCount = 0;
	memset(mPlayers, 0, sizeof(cPokerPlayer*) * NUM_PLAYERS);
	

	for(int i = 0; i < 4; i++)
	{

		this->mPlayers[i] = new cPokerPlayer();
	}// player placeholder


	
	this->Shuffle();// shuffle the deck
}


void cPokerGame::Shuffle()
{
	sCard _card;

	this->mDeck.clear();
	for( int i = 1; i <= 4; ++i )// populates the deck with cards
	{
		for( int j = 1; j <= 13; ++j )
		{
			_card = sCard( eCardFace(j), eCardSuit(i));
			this->mDeck.push_back(_card);
		}
	}
	//this->Draw();

	if( cPokerGame::_seeded )// gives rand a seed
	{
		cPokerGame::_seedRNG();
	}

	std::random_shuffle(this->mDeck.begin(), this->mDeck.end() );
	

}

void cPokerGame::_seedRNG(void)
{
	srand((int)(time(0)));

	_seeded = true;
}

cPokerGame::~cPokerGame()
{
}

sCard cPokerGame::Draw()// draws a card from the deck
{
	
	sCard ret = this->mDeck[this->mDeck.size()-1];
	this->mDeck.pop_back();
	
	return ret;

}


void cPokerGame::addPlayer(sConnection conn)// a player has joined the game
{
	this->playerCount++;
	this->mPlayers[this->playerCount] = new cPokerPlayer();
	this->mPlayers[this->playerCount]->socket = conn.Socket;
	this->mPlayers[this->playerCount]->in = true;
	this->mPlayers[this->playerCount]->connected = true;
}

void cPokerGame::addWaiting(sConnection conn)
{
	
	/*this->mWaiting.push_back(new cPokerPlayer());
	this->mWaiting[this->mWaiting.size()]->socket = conn.Socket;*/
}

void cPokerGame::StartNewRound(int playerId)// a player as confirmed they are in for another round
{
	this->mPlayers[playerId]->nextRound = true;
	
	this->mPlayers[playerId]->myHand.newHand();
	for(int i = 0; i < 4; i++)
	{
		if(this->mPlayers[i]->nextRound == false)
			return;
	}
	this->nextRound = true;
}


int cPokerGame::checkConnected(sConnection conn)// checks if any spots are availible between rounds
{
	int i;
	for( i = 0; i < 4; i++)
	{
		if(!this->mPlayers[i]->connected)
		{
			delete this->mPlayers[i];
			this->mPlayers[i] = new cPokerPlayer();
			this->mPlayers[i]->socket = conn.Socket;
			this->mPlayers[i]->nextRound = true;
			break;
		}
	}

	this->mPlayers[i]->myHand.newHand();
	for(int n = 0; n < 4; n++)
	{
		if(this->mPlayers[n]->nextRound == false)
			return i;
	}
	this->nextRound = true;

	return i;
}


// change to contain the bets of each player for te betting round so that those who have already bet will know how much to add.
bool cPokerGame::Bet(int playerId, int bet)
{
	this->betRecieved = true;
	if(this->lastBet == -2 && bet != -1)//first bet of the round
	{
		this->currentPot += bet;
		this->lastBet = bet;
		this->EOBetting = playerId;
		this->mPlayers[playerId]->money -= bet;
		//this->hasRaised = true;
		this->nextRound = true;
		this->pLastBet[playerId] = (this->pLastBet[playerId] == 0) ? bet : this->pLastBet[playerId]+bet ;

		while(true)
		{
			if(this->actionTo+1 > 3)// gives action to the next player who is in.
			{
				this->actionTo = 0;
			}
			else
			{
				this->actionTo++;
			}
			if(this->mPlayers[this->actionTo]->in)
			{
				break;
			}
		}

	}
	else if(bet == 0)// some one has callled
	{
		this->currentPot += this->lastBet;
		this->pLastBet[playerId] = (this->pLastBet[playerId] == 0) ? this->lastBet : this->pLastBet[playerId]+this->lastBet ;
		this->mPlayers[playerId]->money -= this->lastBet;
		while(true)
		{
			if(this->actionTo+1 > 3)// gives action to the next player who is in.
			{
				this->actionTo = 0;
			}
			else
			{
				this->actionTo++;
			}
			if(this->mPlayers[this->actionTo]->in)
			{
				break;
			}
		}
	}
	else if (bet == -1)// some one has folded
	{
		cPokerGame::PlayerFold(playerId);

		
		if(this->foldCount >= 3)// if 3 players have folded, give the pot to the last player in
		{
			this->nextRound = false;
			this->mPlayers[this->actionTo]->money += this->currentPot;

				this->gameState = ::WAIT;
				sEndGameMessage* table = new sEndGameMessage();
				table->winID = this->actionTo;
				table->winHand = -1;
				table->playerCount = this->playerCount+1;

				int index = 0;
				int pIndex = 0;
				for(int h = 0; h < 40; h+=2 )
				{
					if(index == 5)
					{
						index = 0;
						pIndex++;
					}
					if(this->mPlayers[pIndex]->in)
					{
						table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
						table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
					}
					else
					{
						table->hands[h] = 0;
						table->hands[h+1] = 0;
					}
					index++;
				}


				

				for(int i = 0; i <= this->playerCount; i++)
				{
					//table.hands[i] = this->mPlayers[i]->myHand;
					table->playerMoney[i] = this->mPlayers[i]->money;
					//table.pLastBet[i] = this->pLastBet[i];
					//table.lastAction[i] = this->mPlayers[i]->lastAction;
				}




				this->mNetwork->BroadcastMessage(table);
		
		}
	}
	else if( this->lastBet < bet )//some one has raised
	{
		this->currentPot += bet;
		this->lastBet = bet;
		this->EOBetting = playerId;
		this->mPlayers[playerId]->money -= bet;
		this->pLastBet[playerId] = (this->pLastBet[playerId] == 0) ? bet : this->pLastBet[playerId]+bet;


		while(true)
		{
			if(this->actionTo+1 > 3)
			{
				this->actionTo = 0;
			}
			else
			{
				this->actionTo++;
			}
			if(this->mPlayers[this->actionTo]->in)
			{
				break;
			}
		}

	}
	return false;
}



void cPokerGame::PlayerFold(int playerId)// player has folded
{
	//this->mPlayers[playerId]->myHand.newHand();
	this->mPlayers[playerId]->in = false;
	this->foldCount++;
	while(true)
		{
			if(this->actionTo+1 > 3)
			{
				this->actionTo = 0;
			}
			else
			{
				this->actionTo++;
			}

			if(this->mPlayers[this->actionTo]->in && this->mPlayers[this->actionTo]->connected)
			{
				if(playerId == this->startPlayer)
				{
					this->startPlayer = this->actionTo;
				}
				break;
			}
	}
	
}

void cPokerGame::PlayerLeft(sConnection conn)// player has left the game
{
	int i;
	for(i = 0 ; i < 4; i++)
	{
		if(this->mPlayers[i]->socket == conn.Socket)
		{
			this->mPlayers[i]->in = false;
			this->mPlayers[i]->connected = false;
			this->foldCount++;
			break;
		}
	}

	if( i == this->actionTo)
	{
		
		while(true)
		{
			if(this->actionTo+1 > 3)// gives action to the next player who is in.
			{
				this->actionTo = 0;
			}
			else
			{
				this->actionTo++;
			}

			if(this->mPlayers[this->actionTo]->in && this->mPlayers[this->actionTo]->connected)
			{
				if(i == this->startPlayer)
				{
					this->startPlayer = this->actionTo;
				}


				sTableMessage* table = new sTableMessage();
				table->actionTo = this->actionTo;
				//table.handCount = this->handCount;
				table->playerCount = this->playerCount+1;
				table->pot = this->currentPot;
				table->handCount = this->mPlayers[0]->myHand.getHandSize()-1;
		

				int index = 1;
				int pIndex = 0;
				for(int h = 0; h < 32; h+=2 )
				{
					if(index == 5)
					{
						index = 1;
						pIndex++;
					}
					if(this->mPlayers[pIndex]->in)
					{
						table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
						table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
					}
					else
					{
						table->hands[h] = 0;
						table->hands[h+1] = 0;
					}
						
					index++;
				}
		


				for(int i = 0; i < 4; i++)
				{
					table->playerMoney[i] = this->mPlayers[i]->money;
					if(this->mPlayers[i]->in &&  this->lastBet != -2)
					{
						table->pToCall[i] = this->lastBet - this->pLastBet[i];
					}
					else
					{
						table->pToCall[i] = 0;
					}
				}


	
	
				this->mNetwork->BroadcastMessage(table);

				break;
			}
		}
				

	}


	if(this->foldCount >= 3)// checks if 3 players have left/folded and gives the pot to the last player in.
	{
		this->nextRound = false;
		this->mPlayers[this->actionTo]->money += this->currentPot;

			this->gameState = ::WAIT;
			sEndGameMessage* table = new sEndGameMessage();
			table->winID = this->actionTo;
			table->winHand = -1;
			table->playerCount = this->playerCount+1;

			int index = 0;
			int pIndex = 0;
			for(int h = 0; h < 40; h+=2 )
			{
				if(index == 5)
				{
					index = 0;
					pIndex++;
				}
				if(this->mPlayers[pIndex]->in)
				{
					table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
					table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
				}
				else
				{
					table->hands[h] = 0;
					table->hands[h+1] = 0;
				}
				index++;
			}


				

			for(int i = 0; i <= this->playerCount; i++)
			{
				//table.hands[i] = this->mPlayers[i]->myHand;
				table->playerMoney[i] = this->mPlayers[i]->money;
				//table.pLastBet[i] = this->pLastBet[i];
				//table.lastAction[i] = this->mPlayers[i]->lastAction;
			}




			this->mNetwork->BroadcastMessage(table);
	
		}


}



void cPokerGame::Deal()// deals the first 2 cards
{
	if(this->gameStarted)// if the game was started before, then this is just a new round
	{
		this->gameState = ::BET;
		this->betCount = 0;
		this->currentPot = 4;
		this->actionTo = 0;
		this->lastBet = -2;
		this->pLastBet[0] = 0;
		this->pLastBet[1] = 0;
		this->pLastBet[2] = 0;
		this->pLastBet[3] = 0;
		this->hasRaised = false;
		this->EOBetting = -1;
		this->foldCount = 0;
		this->startPlayer = 0;
		

		for(int i = 0; i < 4; i++)// clears the hand and draws new cards
		{
			this->mPlayers[i]->in = true;
			this->mPlayers[i]->nextRound = false;
			this->mPlayers[i]->myHand.newHand();
			this->mPlayers[i]->myHand.Deal(this->Draw());
			this->mPlayers[i]->myHand.Deal(this->Draw());
		}

	


		
		



		for(int i = 0; i < 4; i++)
		{
			if(this->mPlayers[i]->myHand.CardValueAtIndex(1) < this->mPlayers[this->startPlayer]->myHand.CardValueAtIndex(1))
			{
				this->startPlayer = i;
			}
			else if(this->mPlayers[i]->myHand.CardValueAtIndex(1) == this->mPlayers[this->startPlayer]->myHand.CardValueAtIndex(1))
			{
				if(this->mPlayers[i]->myHand.CardSuitAtIndex(1) < this->mPlayers[this->startPlayer]->myHand.CardSuitAtIndex(1))
				{
					this->startPlayer = i;
				}
			}
		}
		this->actionTo = this->startPlayer;

	
		sTableMessage* table = new sTableMessage();
		table->actionTo = this->startPlayer;
		//table.handCount = this->handCount;
		table->playerCount = this->playerCount+1;
		table->pot = this->currentPot;
		table->handCount = this->mPlayers[0]->myHand.getHandSize()-1;
		

		int index = 1;
		int pIndex = 0;
		for(int h = 0; h < 32; h+=2 )
		{
			if(index == 5)
			{
				index = 1;
				pIndex++;
			}
		
			table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
			table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
			index++;
		}
		


		for(int i = 0; i < 4; i++)
		{
			this->mPlayers[i]->money--;
			table->playerMoney[i] = this->mPlayers[i]->money;
			table->pToCall[i] = 0;//this->lastBet - this->pLastBet[i];
		}


	
	
		this->mNetwork->BroadcastMessage(table);
		this->gameStarted = true;

		for(int i = 0; i < 4; i++)
		{
			sHandMessage* handUpdate = new sHandMessage();

			handUpdate->handCount = this->mPlayers[i]->myHand.getHandSize();
			
			index = 0;
			for(int h = 0; h < 10; h+=2 )
			{

				handUpdate->hand[h] = this->mPlayers[i]->myHand.CardSuitAtIndex(index);
				handUpdate->hand[h+1] = this->mPlayers[i]->myHand.CardValueAtIndex(index);
				index++;
			}
			this->mNetwork->sendMessageToQueue(this->mPlayers[i]->socket, handUpdate);


		}

	}

	
	else// a new game will be started
	{
		this->gameState = ::BET;
		this->currentPot = 4;
		this->actionTo = 0;
		this->lastBet = -2;
		this->pLastBet[0] = 0;
		this->pLastBet[1] = 0;
		this->pLastBet[2] = 0;
		this->pLastBet[3] = 0;
		this->hasRaised = false;
		this->EOBetting = -1;
		this->foldCount = 0;
		this->startPlayer = 0;
		

		for(int i = 0; i < 4; i++)
		{
			this->mPlayers[i]->nextRound = false;
			this->mPlayers[i]->myHand.Deal(this->Draw());
			this->mPlayers[i]->myHand.Deal(this->Draw());
		}


		
		



		for(int i = 0; i < 4; i++)
		{
			if(this->mPlayers[i]->myHand.CardValueAtIndex(1) < this->mPlayers[this->startPlayer]->myHand.CardValueAtIndex(1))
			{
				this->startPlayer = i;
			}
			else if(this->mPlayers[i]->myHand.CardValueAtIndex(1) == this->mPlayers[this->startPlayer]->myHand.CardValueAtIndex(1))
			{
				if(this->mPlayers[i]->myHand.CardSuitAtIndex(1) < this->mPlayers[this->startPlayer]->myHand.CardSuitAtIndex(1))
				{
					this->startPlayer = i;
				}
			}
		}
		this->actionTo = this->startPlayer;



	

		
		sTableMessage* table = new sTableMessage();
		table->actionTo = this->startPlayer;
		//table.handCount = this->handCount;
		table->playerCount = this->playerCount+1;
		table->pot = this->currentPot;
		table->handCount = this->mPlayers[0]->myHand.getHandSize()-1;
		

		int index = 1;
		int pIndex = 0;
		for(int h = 0; h < 32; h+=2 )
		{
			if(index == 5)
			{
				index = 1;
				pIndex++;
			}
			
			table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
			table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
			index++;
		}
		


		for(int i = 0; i < 4; i++)
		{
			this->mPlayers[i]->money--;
			table->playerMoney[i] = this->mPlayers[i]->money;
			table->pToCall[i] = 0;//this->lastBet - this->pLastBet[i];
		}


	
	
		this->mNetwork->BroadcastMessage(table);
		this->gameStarted = true;

		for(int i = 0; i < 4; i++)
		{
			sHandMessage* handUpdate = new sHandMessage();
			handUpdate->handCount = this->mPlayers[i]->myHand.getHandSize();
			index = 0;
			for(int h = 0; h < 10; h+=2 )
			{
				handUpdate->hand[h] = this->mPlayers[i]->myHand.CardSuitAtIndex(index);
				handUpdate->hand[h+1] = this->mPlayers[i]->myHand.CardValueAtIndex(index);
				index++;
			}
			this->mNetwork->sendMessageToQueue(this->mPlayers[i]->socket, handUpdate);


		}

	}





}






void cPokerGame::checkState()
{
	switch(this->gameState)
	{
		case ::WAIT:
			{
				//check for players waiting to join and add them.

				if(this->playerCount > 2 && this->nextRound)
				{
					this->gameState = ::DEAL;

				}

				break;
			}
		case ::DEAL:
			{
				// deal cards and get the round started.
				this->Deal();
				break;
			}
		case ::ANTE:
			{
				// wait till all players have placed first bet.
				break;
			}
		case ::BET:
			{
				// wait till the betting is finished.
				

				if(this->actionTo == this->EOBetting)// betting finished
				{
					this->betCount++;
					
					this->gameState = (this->betCount > 3) ? ::FINAL : ::DRAW;
					
					
				}
				else if ( this->betRecieved )// current player has bet, move to nect player
				{




					this->betRecieved = false;

					sTableMessage* table = new sTableMessage();
					table->actionTo = this->actionTo;
					//table.handCount = this->handCount;
					table->playerCount = this->playerCount+1;
					table->pot = this->currentPot;
					table->handCount = this->mPlayers[0]->myHand.getHandSize()-1;
		

					int index = 1;
					int pIndex = 0;
					for(int h = 0; h < 32; h+=2 )
					{
						if(index == 5)
						{
							index = 1;
							pIndex++;
						}
						if(this->mPlayers[pIndex]->in)// loads hands into the message sent to the players
						{
							table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
							table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
						}
						else
						{
							table->hands[h] = 0;
							table->hands[h+1] = 0;
						}
						
						index++;
					}
		


					for(int i = 0; i < 4; i++)
					{
						table->playerMoney[i] = this->mPlayers[i]->money;
						if(this->mPlayers[i]->in &&  this->lastBet != -2)
						{
							table->pToCall[i] = this->lastBet - this->pLastBet[i];
						}
						else
						{
							table->pToCall[i] = 0;
						}
					}


	
	
					this->mNetwork->BroadcastMessage(table);

				}
				break;
			}
		case ::DRAW:
			{
				this->betRecieved = false;
				//Draw a card for each player and send out the data, update the game state and give action to player 0.

				

				if(this->betCount  < 4)// check if all cards have been delt, for the final round of betting
				{
					for(int i = 0; i <= this->playerCount; i++)
					{
						if(this->mPlayers[i]->in)
						{
							this->mPlayers[i]->myHand.Deal(this->Draw());
						}
					}
				}
				//this->drawCount++;
				//this->handCount++;
				this->actionTo = this->startPlayer;
				this->lastBet = -2;
				this->pLastBet[0] = 0;
				this->pLastBet[1] = 0;
				this->pLastBet[2] = 0;
				this->pLastBet[3] = 0;
				this->EOBetting = -1;
				this->gameState = ::BET;


				
				
				sTableMessage* table = new sTableMessage();
				table->actionTo = this->actionTo;
				//table.handCount = this->handCount;
				table->playerCount = this->playerCount+1;
				table->pot = this->currentPot;
				table->handCount = this->mPlayers[0]->myHand.getHandSize()-1;
		

				int index = 1;
				int pIndex = 0;
				for(int h = 0; h < 32; h+=2 )
				{
					if(index == 5)
					{
						index = 1;
						pIndex++;
					}
					if(this->mPlayers[pIndex]->in)
					{
					table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
					table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
					}
					else
					{
						table->hands[h] = 0;
						table->hands[h+1] = 0;
					}
					index++;
				}
		


				for(int i = 0; i < 4; i++)
				{
					table->playerMoney[i] = this->mPlayers[i]->money;
					table->pToCall[i] = 0;//this->lastBet - this->pLastBet[i];
				}


	
	
				this->mNetwork->BroadcastMessage(table);


				for(int i = 0; i < 4; i++)
				{
					if(this->mPlayers[i]->in)
					{
						sHandMessage* handUpdate = new sHandMessage();
					
						handUpdate->handCount = this->mPlayers[i]->myHand.getHandSize();
						//this->mNetwork->sendMessage(&this->mPlayers[i]->conn, &handUpdate);
						index = 0;
						for(int h = 0; h < 10; h+=2 )
						{
							handUpdate->hand[h] = this->mPlayers[i]->myHand.CardSuitAtIndex(index);
							handUpdate->hand[h+1] = this->mPlayers[i]->myHand.CardValueAtIndex(index);
							index++;
						}
						this->mNetwork->sendMessageToQueue(this->mPlayers[i]->socket, handUpdate);
					}

				}

				
				
				
				break;
			}

		case ::FINAL:
			{
				// send table state to players including each players face down card, the id of the winning player, winning had, and updated money.
				this->nextRound = false;
				int winningHand = 0;
				int winningId;
				int currentHand[4];
				for(int i = 0; i <= this->playerCount; i++)
				{
					if(this->mPlayers[i]->in)
					{
						currentHand[i] = this->mPlayers[i]->myHand.GetValue();
					}
					else
					{
						currentHand[i] = -1;
					}
					if(currentHand[i] > winningHand)
					{
						winningHand = currentHand[i];
						winningId = i;
					}
				}
				
				this->mPlayers[winningId]->money += this->currentPot;

				this->gameState = ::WAIT;
				sEndGameMessage* table = new sEndGameMessage();
				table->winID = winningId;
				table->winHand = winningHand;
				table->playerCount = this->playerCount+1;

				int index = 0;
				int pIndex = 0;
				for(int h = 0; h < 40; h+=2 )
				{
					if(index == 5)
					{
						index = 0;
						pIndex++;
					}
					if(this->mPlayers[pIndex]->in)
					{
						table->hands[h] = this->mPlayers[pIndex]->myHand.CardSuitAtIndex(index);
						table->hands[h+1] = this->mPlayers[pIndex]->myHand.CardValueAtIndex(index);
					}
					else
					{
						table->hands[h] = 0;
						table->hands[h+1] = 0;
					}
					index++;
				}


				

				for(int i = 0; i <= this->playerCount; i++)
				{
					//table.hands[i] = this->mPlayers[i]->myHand;
					table->playerMoney[i] = this->mPlayers[i]->money;
					//table.pLastBet[i] = this->pLastBet[i];
					//table.lastAction[i] = this->mPlayers[i]->lastAction;
				}




				this->mNetwork->BroadcastMessage(table);
				break;
			}

	}

}