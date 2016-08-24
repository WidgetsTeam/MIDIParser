#include "Event.hpp"

mp::Event::Event()
{
}

mp::Event::Name mp::Event::getEventName() const
{
	return event_name;
}

mp::Event::Type mp::Event::getEventType() const
{
	return event_type;
}

double mp::Event::getDeltaTimeInMicroseconds() const
{
	return delta_time_microseconds;
}

int mp::Event::getDeltaTimeInTicks() const
{
	return delta_time_ticks;
}

double mp::Event::getAbsoluteTimeInMicroseconds() const
{
	return absolute_time_microseconds;
}

long long int mp::Event::getAbsoluteTimeInTicks() const
{
	return absolute_time_ticks;
}

const std::vector<char>& mp::Event::getData() const
{
	return event_data;
}

int mp::Event::getChannel() const
{
	return channel;
}
