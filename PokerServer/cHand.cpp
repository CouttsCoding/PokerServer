
#include "cHand.h"
#include "assert.h"

/////////////////////////////////////////////////////////
// Implementation of the cHand class.
// Author: Mark Mikulec, 2010.

/////////////////////////////////////////////////////////
// Default constructor - magically, the cards in my hand
// are initialized to NO_FACE, NO_SUIT.
cHand::cHand()
	: mNumberOfCards(0)
{	
	sCard blankCard = sCard(eCardFace(0),eCardSuit(0));// placeholder card.
	for(int i = 0; i < 5; i++)
	{
		mHand[i] = blankCard;
	}
}

/////////////////////////////////////////////////////////
// Give this hand a card!
void cHand::Deal(const sCard& newCard)
{
	Assert(mNumberOfCards < 5);	// Can only have 5 cards in your hand!!

	mHand[mNumberOfCards] = newCard;
	mNumberOfCards++;
}

/////////////////////////////////////////////////////////
// Calculate the current value of the hand and return it.
int cHand::GetValue() const
{
	// Here's a trick - use the enum values (integers) to rate the cards. Since two is 0 and ace is 12,
	// the highest possible hand ia "raw" value of cards is 59. (4 aces and 1 king)

	// So if we add a 100 for every rank of hand type, we can guarantee to evaluate a hand correctly.

	std::set<sCard> sortedHand;						// Sets sort automatically on insertion! Easy!

	for(int i = 0; i < 5; i++)
	{
		sortedHand.insert(mHand[i]);
	}

	bool		flush			= true;				// Start by assuming it's true
	bool		straight		= true;
	int			i				= 0;
	int			totalValue		= 0;
	eCardSuit	firstCardsSuit	= mHand[0].Suit;
	int			kinds[MAX_FACE];					// Used to keep track of common cards

	memset(kinds, 0, sizeof(kinds));				// We want to always initialize our data

	for(CardSetI card = sortedHand.begin(); card != sortedHand.end(); card++)
	{
		totalValue += card->Face;
		flush &= (firstCardsSuit == mHand[i].Suit); // Are all the cards of the same suit?
		i++;

		CardSetI nextCard = card;
		nextCard++;

		if(nextCard != sortedHand.end())			// Are the cards in sequential order (straight?)
		{
			straight &= ((card->Face - nextCard->Face) == 1);
		}

		kinds[card->Face]++;						// We got one of these...
	}

	// Starting from the greatest hand type to the worst, see if this hand meets the criteria

	if(totalValue == (Ace + King + Queen + Jack + Ten) && straight && flush)	
	{
		totalValue += ROYAL_FLUSH;
	}
	else if(straight && flush)
	{
		totalValue += STRAIGHT_FLUSH;
	}
	else
	{
		int ofAKind[5];	// index represents number of cards in that group

		memset(ofAKind, 0, sizeof(ofAKind));

		for(int face = Two; face < MAX_FACE; face++)// Iterate through all card types looking for 4-of-a-kind
		{
			ofAKind[kinds[face]]++;
		}

		if(ofAKind[4] == 1)
		{
			totalValue += FOUR_KIND;			// Found one!		
		}
		else if(ofAKind[3] == 1 && ofAKind[2] == 1)
		{
			totalValue += FULL_HOUSE;
		}
		else if(flush)
		{
			totalValue += FLUSH;
		}
		else if(straight)
		{
			totalValue += STRAIGHT;
		}
		else if(ofAKind[3] == 1)
		{
			totalValue += THREE_KIND;
		}
		else if(ofAKind[2] == 2)
		{
			totalValue += TWO_PAIR;
		}
		else if(ofAKind[2] == 1)
		{
			totalValue += ONE_PAIR;
		}
	}

	return totalValue;
}

/////////////////////////////////////////////////////////
// Used for breaking ties if the hand value is equal! This
// is not 100% accurate, since it'll take all cards into account
// even if they aren't relevant, but it works  for this exercise.
int cHand::GetSuitValue() const
{
	int value = 0;

	for(int i = 0; i < 5; i++)
	{
		value += static_cast<int>(mHand[i].Suit);
	}

	return value;
}

/////////////////////////////////////////////////////////
// Slow since we are calling cHand GetValue alot! But it works...
bool cHand::operator<(const cHand& b) const
{
	int myValue		= this->GetValue();
	int theirValue	= b.GetValue();

	if(myValue == theirValue) return this->GetSuitValue() > b.GetSuitValue();

	return myValue > theirValue;	// highest hand first
}

/////////////////////////////////////////////////////////
// Make sure my logic for hand values work.
void cHand::UnitTests()
{
	cHand player[7];

	player[0].Deal(sCard(Nine, Clubs));
	player[0].Deal(sCard(Ten, Clubs));
	player[0].Deal(sCard(Ten, Hearts));
	player[0].Deal(sCard(Ten, Spades));
	player[0].Deal(sCard(Three, Spades));
	Assert(player[0].GetValue() == 337);	// 3 of a kind

	player[1].Deal(sCard(Jack, Hearts));
	player[1].Deal(sCard(Four, Hearts));
	player[1].Deal(sCard(Two, Hearts));
	player[1].Deal(sCard(Seven, Hearts));
	player[1].Deal(sCard(Five, Hearts));

	Assert(player[1].GetValue() == 524);	// hearts flush

	player[2].Deal(sCard(Eight, Hearts));
	player[2].Deal(sCard(Jack, Clubs));
	player[2].Deal(sCard(Seven, Diamonds));
	player[2].Deal(sCard(Nine, Hearts));
	player[2].Deal(sCard(Ten, Clubs));

	Assert(player[2].GetValue() == 440);	// jack-high straight

	player[3].Deal(sCard(King, Hearts));
	player[3].Deal(sCard(Jack, Clubs));
	player[3].Deal(sCard(Seven, Diamonds));
	player[3].Deal(sCard(Nine, Hearts));
	player[3].Deal(sCard(Ten, Clubs));

	Assert(player[3].GetValue() == 45);		// king-high

	player[4].Deal(sCard(King, Hearts));
	player[4].Deal(sCard(Jack, Clubs));
	player[4].Deal(sCard(Ace, Diamonds));
	player[4].Deal(sCard(Nine, Hearts));
	player[4].Deal(sCard(Ace, Clubs));

	Assert(player[4].GetValue() == 156);	// pair of aces

	player[5].Deal(sCard(Two, Spades));
	player[5].Deal(sCard(Seven, Clubs));
	player[5].Deal(sCard(Seven, Diamonds));
	player[5].Deal(sCard(Two, Hearts));
	player[5].Deal(sCard(Five, Spades));

	Assert(player[5].GetValue() == 218);	// 2-pair,2 's & 7's

	player[6].Deal(sCard(King, Spades));
	player[6].Deal(sCard(Ten, Spades));
	player[6].Deal(sCard(Queen, Spades));
	player[6].Deal(sCard(Jack, Spades));
	player[6].Deal(sCard(Ace, Spades));

	Assert(player[6].GetValue() == 955);	// The mighty royal flush!!

	HandSet sortMe;

	for(int i = 0; i < 7; i++)
	{
		sortMe.insert(player[i]);
	}

	// Will be sorted now - assert if it's proper order

	int i = 0;

	for(HandSetI hand = sortMe.begin(); hand != sortMe.end(); hand++)
	{
		switch(i)
		{
		case 0:
			Assert(hand->GetValue() == 955);
			break;
		case 1:
			Assert(hand->GetValue() == 524);
			break;
		case 2:
			Assert(hand->GetValue() == 440);
			break;
		case 3:
			Assert(hand->GetValue() == 337);
			break;
		case 4:
			Assert(hand->GetValue() == 218);
			break;
		case 5:
			Assert(hand->GetValue() == 156);
			break;
		case 6:
			Assert(hand->GetValue() == 45);
			break;
		}

		i++;
	}
}



int cHand::CardSuitAtIndex(int index)
{
	return this->mHand[index].Suit;
}

int cHand::CardValueAtIndex(int index)
{
	return this->mHand[index].Face;
}


int cHand::getHandSize()
{
	return this->mNumberOfCards;
}

void cHand::newHand()
{
	this->mNumberOfCards = 0;
	sCard placeHolder = sCard(eCardFace(0), eCardSuit(0));
	for(int i = 0; i < 5; i++)
	{
		this->mHand[i] = placeHolder;
	}
}