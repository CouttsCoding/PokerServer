// PokerServer.cpp : Defines the entry point for the console application.

#include "cHand.h"
#include "cNetworkManager.h"
#include "cPokerServerProtocol.h"
#include "assert.h"
#include "duplexMessages.h"

int main(int argc, char* argv[])
{
	cHand::UnitTests();	// Some unit tests to check my code!!

	int			returnValue	= 0;
	cNetworkManager network;

	Assert(sizeof(int) == 4);
	
	// STUDENT TODO: Make it so that the server can listen on any ip!!
	//////////////////////////////////////////////////////////////////
	cPokerGame game(&network);


	returnValue = network.Startup("0.0.0.0", 1234, new cPokerServerProtocol(&game));

	if(returnValue != 0) return returnValue;

	int ticker	= 0;
	int count	= 1;	// odd from server

	while(true)
	{
		network.Run();

		// STUDENT TODO: Replace the following ticker/ping code with game logic.
		//////////////////////////////////////////////////////////////////



		game.checkState();


		
	}
	network.Shutdown();
	
	return 0;
}

