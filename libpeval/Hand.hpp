#include "Constants.hpp"
#include "helpers.hpp"

class Hand {

public:
	Hand()										= default;
	Hand(const Hand& hand)						= default;
	Hand(Hand&& hand) noexcept					= default;
	Hand& operator=(const Hand& hand)			= default;
	Hand& operator=(Hand&& hand) noexcept		= default;
	auto operator<=>(const Hand& hand) const	= default;

};