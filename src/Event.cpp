#include "Event.hpp"

mp::Event::Event()
{
}

mp::Event::Name mp::Event::getEventName() const
{
	return Name();
}

mp::Event::Type mp::Event::getEventType() const
{
	return Type();
}

double mp::Event::getDeltaTimeInMicroseconds() const
{
	return 0.0;
}

int mp::Event::getDeltaTimeInTicks() const
{
	return 0;
}

double mp::Event::getAbsoluteTimeInMicroseconds() const
{
	return 0.0;
}

long long int mp::Event::getAbsoluteTimeInTicks() const
{
	return 0;
}

const std::vector<char>& mp::Event::getData() const
{
	return std::vector<char>();
}

int mp::Event::getChannel() const
{
	return 0;
}
