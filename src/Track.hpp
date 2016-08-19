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

	Track();

	int          getEventsQuantity()                  const;

	const Event& operator[](const int index) const;
private:
	int                events_quantity;

	std::vector<Event> events;
};

}

#endif
