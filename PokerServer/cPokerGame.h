#pragma once
#include "cPokerPlayer.h"
#include "cNetworkManager.h"
#include "netMessages.h"
#define	NUM_PLAYERS	4


enum eGameState
{
	WAIT,
	DEAL,
	ANTE,
	DRAW,
	BET,
	FINAL
};


class cPokerGame
{
public:
	cPokerGame(cNetworkManager* network);
	~cPokerGame();

	void addWaiting(sConnection conn);
	void addPlayer(sConnection conn);
	sCard Draw();
	int playerCount;
	void Start();
	void checkState();
	bool Bet(int playerId, int bet);
	void PlayerFold(int playerId);
	void PlayerLeft(sConnection conn);
	void Deal();
	void StartNewRound(int playerId);
	int checkConnected(sConnection conn);
	void Shuffle();

	static void _seedRNG();


	static bool _seeded;
	eGameState gameState;
	cNetworkManager* mNetwork;
	bool nextRound;

private:
	CardSet	mDeck;
	cPokerPlayer*	mPlayers[NUM_PLAYERS];
	std::deque<cPokerPlayer*> mWaiting;
	int currentPot;
	bool hasRaised;
	int lastToBet;
	int EOBetting;
	int lastBet;
	int actionTo;
	int handCount;
	int pLastBet[4];
	bool gameStarted;
	bool betRecieved;
	int betCount;
	int startPlayer;
	
	

	int foldCount;
	

};
