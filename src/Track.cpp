#include <MidiParser/Track.hpp>

int mp::Track::getEventsQuantity() const
{
	return events.size();
}

const mp::Event& mp::Track::operator[](const unsigned int index) const
{
	return events[index];
}
