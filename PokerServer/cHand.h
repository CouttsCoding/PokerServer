#pragma once

#include "sCard.h"

////////////////////////////////////////////////////
// Defines a hand. The first index in the array will
// be the private card, until the showdown.
// Author: Mark Mikulec, 2010.

class cHand
{
public:
	cHand();
	void Deal(const sCard& newCard);
	int GetValue() const;
	int GetSuitValue() const;
	int CardSuitAtIndex(int index);
	int CardValueAtIndex(int index);
	bool operator<(const cHand& b) const;

	int getHandSize();
	void newHand();

	static const int ROYAL_FLUSH	= 900;
	static const int STRAIGHT_FLUSH	= 800;
	static const int FOUR_KIND		= 700;
	static const int FULL_HOUSE		= 600;
	static const int FLUSH			= 500;
	static const int STRAIGHT		= 400;
	static const int THREE_KIND		= 300;
	static const int TWO_PAIR		= 200;
	static const int ONE_PAIR		= 100;

	static void UnitTests();

private:
	sCard	mHand[5];
	int		mNumberOfCards;
};

typedef std::set<cHand>		HandSet;
typedef HandSet::iterator	HandSetI;