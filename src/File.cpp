#include "File.hpp"

#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cassert>

mp::File::File() :
is_read(false), 
is_correct(false)
{

}

mp::File::File(const std::string& filename) :
File()
{
	open(filename);
}

void mp::File::open(const std::string& filename)
{
	if (filename.find(".mid", 0) == std::string::npos &&
		filename.find(".midi", 0) == std::string::npos &&
		filename.find(".MID", 0) == std::string::npos &&
		filename.find(".MIDI", 0) == std::string::npos)
	{
		return;
	}

	std::ifstream midi_file(filename, std::ios_base::binary);
	if (!midi_file.is_open())
	{
		return;
	}

	std::string header_chunk_signature(4, '\0');
	midi_file.read(&header_chunk_signature[0], 4);

	if (header_chunk_signature != "MThd" || !midi_file.good())
	{
		is_read = true;
		midi_file.close();
		return;
	}

	unsigned int header_chunk_data_length;
	midi_file.read(reinterpret_cast<char*>(&header_chunk_data_length), 4);
	if (!midi_file.good())
	{
		is_read = true;
		midi_file.close();
		return;
	}
	header_chunk_data_length = changeEndian<unsigned int>(header_chunk_data_length);

	midi_file.read(reinterpret_cast<char*>(&format), 2);
	if (!midi_file.good())
	{
		is_read = true;
		midi_file.close();
		return;
	}
	format = changeEndian<short>(format);

	midi_file.read(reinterpret_cast<char*>(&tracks_quantity), 2);
	if (!midi_file.good())
	{
		is_read = true;
		midi_file.close();
		return;
	}
	tracks_quantity = changeEndian<unsigned short>(tracks_quantity);

	short division;
	midi_file.read(reinterpret_cast<char*>(&division), 2);
	if (!midi_file.good())
	{
		is_read = true;
		midi_file.close();
		return;
	}
	division = changeEndian<short>(division);

	if (division >> 15 == 0)
	{
		ticks_per_quater_note = division;
		is_smpte_type = false;
	}
	else
	{
		smpte_byte[0] = division & 0xFF;
		division >>= 8;
		smpte_byte[1] = abs(division);
		is_smpte_type = true;
	}
	
	for (int i = 0; i < tracks_quantity; i++)
	{
		std::string track_chunk_signature(4, '\0');
		midi_file.read(&track_chunk_signature[0], 4);

		if (track_chunk_signature != "MTrk" || !midi_file.good())
		{
			is_read = true;
			midi_file.close();
			return;
		}

		unsigned int track_chunk_data_length;
		midi_file.read(reinterpret_cast<char*>(&track_chunk_data_length), 4);
		if (!midi_file.good())
		{
			is_read = true;
			midi_file.close();
			return;
		}
		track_chunk_data_length = changeEndian<unsigned int>(track_chunk_data_length);

		tracks.push_back(Track());

		/*while(true)
		{
			TODO: Loading events from track
		}*/
	}

	midi_file.close();
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
	assert(is_read && is_correct);
	return format;
}

unsigned short mp::File::getTracksQuantity() const
{
	assert(is_read && is_correct);
	return tracks_quantity;
}

short mp::File::getTicksPerQuaterNote() const
{
	assert(is_read && is_correct && !is_smpte_type);
	return ticks_per_quater_note;
}

short mp::File::getTicksPerFrame() const
{
	assert(is_read && is_correct && is_smpte_type);
	return static_cast<short>(smpte_byte[0]);
}

short mp::File::getFramesPerSecond() const
{
	assert(is_read && is_correct && is_smpte_type);
	return static_cast<short>(smpte_byte[1]);
}

bool mp::File::isSmpteType() const
{
	assert(is_read && is_correct);
	return is_smpte_type;
}

const mp::Track mp::File::connectTracks() const
{
	assert(is_read && is_correct);
	return Track();
}

const mp::Track& mp::File::operator[](const unsigned int index) const
{
	assert(is_read && is_correct);
	return tracks[index];
}

template<typename T>
T mp::File::changeEndian(const T value)
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