#pragma once

#include <deque>
#include <set>
////////////////////////////////////////////////////
// This structure defines a single card in the playing card deck.
// We have some enumerated types to make it easy for us to see
// as programmers what's going on.
// Author: Mark Mikulec, 2010.

enum eCardSuit
{	
	NO_SUIT,
	Diamonds,
	Clubs,
	Hearts,
	Spades,
	MAX_SUIT	// not a real value, for array storage
};

enum eCardFace
{
	NO_FACE,	// for empty initialization
	Two,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,
	Ten,
	Jack,
	Queen,
	King,
	Ace,
	MAX_FACE	// not a real value, for array storage
};

struct sCard
{	
	eCardFace	Face;
	eCardSuit	Suit;

	sCard()
		: Face(NO_FACE)
		, Suit(NO_SUIT)		
	{
	}

	sCard(eCardFace face, eCardSuit suit)
		: Face(face)
		, Suit(suit)		
	{
	}

	// STL compare based on the face of the card
	bool operator<(const sCard& b) const
	{
		if(this->Face == b.Face) return this->Suit > b.Suit;

		return this->Face > b.Face;		// highest card first
	}
};

typedef std::deque<sCard>	CardSet;
typedef std::set<sCard>::iterator	CardSetI;
