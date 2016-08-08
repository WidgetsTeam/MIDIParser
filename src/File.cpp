#include "File.hpp"

#include <fstream>
#include <algorithm>
#include <cmath>


mp::File::File() :
file_existing(false), 
file_correct(false)
{

}

mp::File::File(const std::string& filename)
{
	open(filename);
}

void mp::File::open(const std::string& filename)
{
	file_existing = true;
	file_correct = true;

	std::ifstream midi_file(filename, std::ios::binary);
	if (!midi_file.is_open())
	{
		file_existing = false;
		file_correct = false;
		return;
	}
	
	std::string header_chunk_signature(4, '\0');
	midi_file.read(&header_chunk_signature[0], 4);

	if (header_chunk_signature != "MThd")
	{
		file_correct = false;
		return;
	}

	unsigned int header_chunk_data_length;
	midi_file.read(reinterpret_cast<char*>(&header_chunk_data_length), 4);
	header_chunk_data_length = changeEndian<unsigned int>(header_chunk_data_length);

	midi_file.read(reinterpret_cast<char*>(&midi_file_format), 2);
	midi_file_format = changeEndian<short>(midi_file_format);

	midi_file.read(reinterpret_cast<char*>(&tracks_quantity), 2);
	tracks_quantity = changeEndian<unsigned short>(tracks_quantity);

	short division;
	midi_file.read(reinterpret_cast<char*>(&division), 2);
	division = changeEndian<short>(division);
	
	if (division >> 15 == 0)
	{
		ticks_per_quater_note = division;
		division_type = 0;
	}
	else
	{
		smtpe_byte[0] = division & 0xFF;
		division >>= 8;
		smtpe_byte[1] = abs(division);
		division_type = 1;
	}
	

	for (int i = 0; i < tracks_quantity; i++)
	{
		// ...
	}
}

void mp::File::close()
{
	file_existing = false;
	file_correct = false;	
}

bool mp::File::isGood() const
{
	return file_existing && file_correct;
}

bool mp::File::isExisting() const
{
	return file_existing;
}

bool mp::File::isCorrect() const
{
	return file_correct;
}


short mp::File::getMidiFileFormat() const
{
	return midi_file_format;
}

unsigned short mp::File::getTracksQuantity() const
{
	return tracks_quantity;
}



bool mp::File::getDivisionType() const
{
	return division_type;
}

short mp::File::getTicksPerQuaterNote() const
{
	return ticks_per_quater_note;
}

short mp::File::getTicksPerFrame() const
{
	return static_cast<short>(smtpe_byte[0]);
}

short mp::File::getFramesPerSecond() const
{
	return static_cast<short>(smtpe_byte[1]);
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
	std::reverse(conversion.as_bytes_array, conversion.as_bytes_array + sizeof (T));

	return conversion.as_number;
}

