#include "Track.hpp"

mp::Track::Track()
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
