#include "Track.hpp"

mp::Track::Track() :
events_quantity(0)
{

}

int mp::Track::getEventsQuantity() const
{
	return events_quantity;
}

const mp::Event& mp::Track::operator[](const int index) const
{
	return events[index];
}
