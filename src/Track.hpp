#ifndef TRACK_HPP
#define TRACK_HPP

#include <vector>

#include "Event.hpp"

namespace mp
{

class Track
{
public:
	friend class File;

	int          getEventsQuantity()                  const;

	const Event& operator[](const unsigned int index) const;
private:

	std::vector<Event> events;
};

}

#endif
