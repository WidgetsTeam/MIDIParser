#include "File.hpp"

#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

mp::File::File() :
is_read(false), 
is_correct(false)
{

}

mp::File::File(const std::string& filename)
{
	open(filename);
}

void mp::File::open(const std::string& filename)
{
	is_read = true;
	is_correct = true;

	std::ifstream midi_file(filename, std::ios::binary);
	if (!midi_file.is_open())
	{
		is_read = false;
		is_correct = false;
		return;
	}
	
	std::string header_chunk_signature(4, '\0');
	midi_file.read(&header_chunk_signature[0], 4);

	if (header_chunk_signature != "MThd")
	{
		is_correct = false;
		return;
	}

	unsigned int header_chunk_data_length;
	midi_file.read(reinterpret_cast<char*>(&header_chunk_data_length), 4);
	header_chunk_data_length = changeEndian<unsigned int>(header_chunk_data_length);

	midi_file.read(reinterpret_cast<char*>(&format), 2);
	format = changeEndian<short>(format);

	midi_file.read(reinterpret_cast<char*>(&tracks_quantity), 2);
	tracks_quantity = changeEndian<unsigned short>(tracks_quantity);

	short division;
	midi_file.read(reinterpret_cast<char*>(&division), 2);
	division = changeEndian<short>(division);

	if (division >> 15 == 0)
	{
		ticks_per_quater_note = division;
		smpte_type = false;
	}
	else
	{
		smpte_byte[0] = division & 0xFF;
		division >>= 8;
		smpte_byte[1] = abs(division);
		smpte_type = true;
	}
	
	for (int i = 0; i < tracks_quantity; i++)
	{
		// ...
	}
}

void mp::File::close()
{
	is_read = false;
	is_correct = false;	
}

bool mp::File::isGood() const
{
	return is_read && is_correct;
}

bool mp::File::isRead() const
{
	return is_read;
}

bool mp::File::isCorrect() const
{
	return is_correct;
}

short mp::File::getFormat() const
{
	if (is_read && is_correct)
		return format;
	else
		abort();
}

unsigned short mp::File::getTracksQuantity() const
{
	if (is_read && is_correct)
		return tracks_quantity;
	else
		abort();
}

short mp::File::getTicksPerQuaterNote() const
{
	if (is_read && is_correct)
		return ticks_per_quater_note;
	else
		abort();
}

short mp::File::getTicksPerFrame() const
{
	if (is_read && is_correct)
		return static_cast<short>(smpte_byte[0]);
	else
		abort();
}

short mp::File::getFramesPerSecond() const
{
	if (is_read && is_correct)
		return static_cast<short>(smpte_byte[1]);
	else
		abort();
}

bool mp::File::isSmpteType() const
{
	if (is_read && is_correct)
		return smpte_type;
	else
		abort();
}

template<typename T>
T mp::File::changeEndian(T value)
{
	union
	{
		T as_number;
		unsigned char as_bytes_array[sizeof(T)];
	} conversion;

	conversion.as_number = value;
	std::reverse(conversion.as_bytes_array, conversion.as_bytes_array + sizeof(T));

	return conversion.as_number;
}