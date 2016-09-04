#include "Event.hpp"

mp::Event::Event()
{
}

mp::Event::Name mp::Event::getEventName() const
{
	return name;
}

mp::Event::Type mp::Event::getEventType() const
{
	return type;
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

unsigned int mp::Event::getDataQuantity() const
{
	return data.size();
}

int mp::Event::getChannel() const
{
	return channel;
}

char mp::Event::operator[](const unsigned int index) const
{
	return data[index];
}