#include "Track.hpp"

mp::Track::Track()
{

}

int mp::Track::getEventsQuantity()
{
	return 0;
}

const mp::Event& mp::Track::operator[](unsigned int index)
{
	return events[index];
}
