#include <MidiParser/Error.hpp>

mp::Error::Error(const mp::Error::ErrorType error_type) :
error_type(error_type)
{
}

mp::Error::ErrorType mp::Error::what() const
{
	return error_type;
}
