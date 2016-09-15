#include <MidiParser/File.hpp>

#include <algorithm>
#include <cmath>

mp::File::File()
{

}

mp::File::File(const std::string& filename)
{
	open(filename);
}

void mp::File::open(const std::string& filename)
{
	tracks.clear();

	std::ifstream midi_file(filename, std::ios_base::binary);
	
	if (!midi_file.is_open())
		throw Error(Error::Unread);


	std::string header_chunk_signature;
	readFromFile(midi_file, header_chunk_signature, 4);

	if (header_chunk_signature != "MThd")
		throw Error(Error::Incorrect);

	int header_chunk_data_length;
	readFromFile(midi_file, header_chunk_data_length, 4);

	readFromFile(midi_file, format, 2);
	if (format < 0 || format > 2)
		throw Error(Error::Incorrect);

	readFromFile(midi_file, tracks_quantity, 2);

	short division;
	readFromFile(midi_file, division, 2);

	if ((division >> 15) == 0) // Checking is 15. bit of division set as 0 (SMPTE type or not)
	{
		ticks_per_quater_note = division;
		is_smpte_type = false;
	}
	else
	{
		smpte_byte[1] = division & 0xFF;
		division >>= 8;
		smpte_byte[0] = abs(division);
		is_smpte_type = true;
	}
	
	unsigned int tempo = 500000; //Default tempo [microseconds / quater note]

	for (int track_number = 0; track_number < tracks_quantity; track_number++)
	{

		std::string track_chunk_signature;
		readFromFile(midi_file, track_chunk_signature, 4);

		if (track_chunk_signature != "MTrk")
			throw Error(Error::Incorrect);

		unsigned int track_chunk_data_length;
		readFromFile(midi_file, track_chunk_data_length, 4);

		Track track;
		unsigned char status_byte = 0;
		unsigned long long delta_time_counter = 0;

		for (unsigned int bytes_to_read = track_chunk_data_length; bytes_to_read > 0;) // One course of loop = reading one event
		{
			Event event;

			bytes_to_read -= readVariableLengthQuantity(midi_file, event.delta_time_ticks);

			unsigned char byte;
			bytes_to_read -= readFromFile(midi_file, byte);

			int difference = 0;

			if (byte >= 0x80)
				status_byte = byte;
			else
				difference = 1;

			event.data.push_back(status_byte);

			if (difference == 1)
				event.data.push_back(byte);


			if (status_byte >= 0x80 && status_byte <= 0xEF) // MIDI Event
			{
				event.channel = status_byte & 0xF;

				switch ((status_byte >> 4) & 0xF)
				{
					case 0x8: event.type = Event::Type::NoteOff;               break;
					case 0x9: event.type = Event::Type::NoteOn;                break;
					case 0xA: event.type = Event::Type::PolyphonicKeyPressure; break;
					case 0xB: event.type = Event::Type::ControllerChange;      break;
					case 0xC: event.type = Event::Type::ProgramChange;         break;
					case 0xD: event.type = Event::Type::ChannelKeyPressure;    break;
					case 0xE: event.type = Event::Type::PitchBend;             break;
				}

				if (event.type == Event::Type::ProgramChange ||
					event.type == Event::Type::ChannelKeyPressure)
						difference++;

				for (int i = 0; i < 2 - difference; i++)
				{
					char data_byte;
					bytes_to_read -= readFromFile(midi_file, data_byte);
					event.data.push_back(data_byte);
				}

				if (event.type == Event::Type::ControllerChange &&
				   (event.data[1] >= 0x78 ||
					event.data[1] <= 0x7F))
				{
					event.name = Event::Name::ChannelMode;
				}
				else
					event.name = Event::Name::ChannelVoice;
			}
			else if (status_byte == 0xF0 || status_byte == 0xF7) // Sysex Event
			{
				event.name = Event::Name::Sysex;

				switch (status_byte)
				{
					case 0xF0: event.type = Event::Type::F0SysexEvent; break;
					case 0xF7: event.type = Event::Type::F7SysexEvent; break;
				}

				int sysex_data_length;
				bytes_to_read -= readVariableLengthQuantity(midi_file, sysex_data_length);

				for (int j = 0; j < sysex_data_length; j++)
				{
					char data_byte;
					bytes_to_read -= readFromFile(midi_file, data_byte);
					event.data.push_back(data_byte);
				}
			}
			else if (status_byte == 0xFF) // Meta Event
			{
				event.name = Event::Name::Metadata;

				char type;
				bytes_to_read -= readFromFile(midi_file, type);

				event.data.push_back(type);

				switch (type)
				{
					case 0x00: event.type = Event::Type::SequenceNumber;             break;
					case 0x01: event.type = Event::Type::TextEvent;                  break;
					case 0x02: event.type = Event::Type::CopyrightNotice;            break;
					case 0x03: event.type = Event::Type::SequenceOrTrackName;        break;
					case 0x04: event.type = Event::Type::InstrumentName;             break;
					case 0x05: event.type = Event::Type::Lyric;                      break;
					case 0x06: event.type = Event::Type::Marker;                     break;
					case 0x07: event.type = Event::Type::CuePoint;                   break;
					case 0x08: event.type = Event::Type::ProgramName;                break;
					case 0x09: event.type = Event::Type::DeviceName;                 break;
					case 0x20: event.type = Event::Type::MidiChannelPrefix;          break;
					case 0x21: event.type = Event::Type::MidiPort;                   break;
					case 0x2F: event.type = Event::Type::EndOfTrack;                 break;
					case 0x51: event.type = Event::Type::SetTempo;                   break;
					case 0x54: event.type = Event::Type::SmpteOffset;                break;
					case 0x58: event.type = Event::Type::TimeSignature;              break;
					case 0x59: event.type = Event::Type::KeySignature;               break;
					case 0x7F: event.type = Event::Type::SequencerSpecificMetaEvent; break;
				}

				int meta_data_length;
				bytes_to_read -= readVariableLengthQuantity(midi_file, meta_data_length);

				for (int i = 0; i < meta_data_length; i++)
				{
					char data_byte;
					bytes_to_read -= readFromFile(midi_file, data_byte);
					event.data.push_back(data_byte);
				}

				if (event.type == Event::Type::SetTempo)
				{
					std::string tempo_as_string(event.data.begin() + 2, event.data.end());
					tempo_as_string = '\0' + tempo_as_string;
					tempo = *reinterpret_cast<int*>(&tempo_as_string[0]);
					changeEndian(tempo);
				}
			}
			else
				throw Error(Error::Incorrect);

			event.absolute_time_ticks = delta_time_counter;
			delta_time_counter += event.delta_time_ticks;

			track.events.push_back(event);

		}

		tracks.push_back(track);
	}

	return;
}

short mp::File::getFormat() const
{
	return format;
}

unsigned short mp::File::getTracksQuantity() const
{
	return tracks_quantity;
}

short mp::File::getTicksPerQuaterNote() const
{
	return ticks_per_quater_note;
}

short mp::File::getFramesPerSecond() const
{
	return static_cast<short>(smpte_byte[0]);
}

short mp::File::getTicksPerFrame() const
{
	return static_cast<short>(smpte_byte[1]);
}

bool mp::File::isSmpteType() const
{
	return is_smpte_type;
}

const mp::Track mp::File::connectTracks() const
{
	// TODO: Implementation
	Track connected_tracks;

	for (int i = 0; i < tracks.size(); i++)
	{
		for (int j = 0; j < tracks[i].events.size(); j++)
		{
			connected_tracks.events.push_back(tracks[i].events[j]);
		}
	}

	std::sort(connected_tracks.events.begin(), connected_tracks.events.end(), [](Event a, Event b)
	{
		return a.absolute_time_ticks < b.absolute_time_ticks;
	});

	return connected_tracks;
}

const mp::Track& mp::File::operator[](const unsigned int index) const
{
	return tracks[index];
}

std::size_t mp::File::readVariableLengthQuantity(std::ifstream& file, int& value)
{
	std::vector<char> bytes;
	char byte;
	int bytes_counter = 0;

	do
	{
		readFromFile(file, byte);
		bytes.push_back(byte);

		bytes_counter++;

	} while ((byte >> 7 & 1) == 1 && bytes_counter < 4);

	if (bytes_counter == 4)
		throw Error(Error::Incorrect);

	int multipler = 1;
	int end_value = 0;

	for (std::vector<char>::reverse_iterator it = bytes.rbegin(); it != bytes.rend(); it++)
	{
		end_value += (*it & 127) * multipler;
		multipler *= 128;
	}

	value = end_value;

	return bytes_counter;
}

std::size_t mp::File::readFromFile(std::ifstream& file, std::string& string, const int quantity)
{
	string.append(quantity, '\0');
	file.read(&string[0], quantity);

	if (!file.good())
		throw Error(Error::Incorrect);

	return quantity;
}

template<typename T>
std::size_t mp::File::readFromFile(std::ifstream& file, T& value, const int quantity)
{
	file.read(reinterpret_cast<char*>(&value), quantity);
	changeEndian(value);
	/*for (int i = 0; i < quantity; i++)
	{
		std::cout << std::hex << (int)*(reinterpret_cast<unsigned char*>(&value) + i) << " " << std::dec;
	}*/
	
	if (!file.good())
		throw Error(Error::Incorrect);

	return quantity;
}

template<typename T>
void mp::File::changeEndian(T& value)
{
	char* pointer = reinterpret_cast<char*>(&value);
	std::reverse(pointer, pointer + sizeof(T));
}