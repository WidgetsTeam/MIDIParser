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

	int getEventsQuantity();

	const Event& operator[](unsigned int index);
private:
	std::vector<Event> events;
};

}

#endif
