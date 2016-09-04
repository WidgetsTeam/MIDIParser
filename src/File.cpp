#include "File.hpp"

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

	std::string header_chunk_signature(4, '\0');
	readFromFile(midi_file, header_chunk_signature[0], 4);

	if (header_chunk_signature != "MThd")
		throw Error(Error::Incorrect);

	int header_chunk_data_length;
	readFromFile(midi_file, header_chunk_data_length, 4);
	changeEndian(header_chunk_data_length);

	readFromFile(midi_file, format, 2);
	changeEndian(format);

	if (format < 0 || format > 2)
		throw Error(Error::Incorrect);

	readFromFile(midi_file, tracks_quantity, 2);
	changeEndian(tracks_quantity);

	short division;
	readFromFile(midi_file, division, 2);
	changeEndian(division);

	if (division >> 15 == 0)
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
	
	for (int track_number = 0; track_number < tracks_quantity; track_number++)
	{
		std::string track_chunk_signature(4, '\0');
		readFromFile(midi_file, track_chunk_signature[0], 4);

		if (track_chunk_signature != "MTrk")
			throw Error(Error::Incorrect);

		unsigned int track_chunk_data_length;
		readFromFile(midi_file, track_chunk_data_length, 4);
		changeEndian(track_chunk_data_length);

		Track track;

		for (unsigned int bytes_to_read = track_chunk_data_length; bytes_to_read > 0;) // One course of loop = reading one event
		{
			Event event;

			readVariableLengthQuantity(midi_file, event.delta_time_ticks, bytes_to_read);

			unsigned char status_byte;
			readFromFile(midi_file, status_byte, bytes_to_read);
			event.data.push_back(status_byte);

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

				int bytes_to_read_in_midi_event = 2;

				if (event.type == Event::Type::ProgramChange ||
					event.type == Event::Type::ChannelKeyPressure)
				{
						bytes_to_read_in_midi_event = 1;
				}

				for (int i = 0; i < bytes_to_read_in_midi_event; i++)
				{
					char data_byte;
					readFromFile(midi_file, data_byte, bytes_to_read);
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
				readVariableLengthQuantity(midi_file, sysex_data_length, bytes_to_read);

				for (int j = 0; j < sysex_data_length; j++)
				{
					char data_byte;
					readFromFile(midi_file, data_byte, bytes_to_read);
					event.data.push_back(data_byte);
				}
			}
			else if (status_byte == 0xFF) // Meta Event
			{
				event.name = Event::Name::Metadata;

				char type;
				readFromFile(midi_file, type, bytes_to_read);

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
				readVariableLengthQuantity(midi_file, meta_data_length, bytes_to_read);

				for (int i = 0; i < meta_data_length; i++)
				{
					char data_byte;
					readFromFile(midi_file, data_byte, bytes_to_read);
					event.data.push_back(data_byte);
				}
			}
			else
				throw Error(Error::Incorrect);

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
	return Track();
}

const mp::Track& mp::File::operator[](const unsigned int index) const
{
	return tracks[index];
}

void mp::File::readVariableLengthQuantity(std::ifstream& file, int& value, unsigned int& bytes_to_read)
{
	std::vector<char> bytes;
	char byte;
	int bytes_counter = 0;

	do
	{
		readFromFile(file, byte, bytes_to_read);
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
}

template<typename T>
void mp::File::readFromFile(std::ifstream& file, T& value, const int quantity)
{
	file.read(reinterpret_cast<char*>(&value), quantity);
	/*
	for(int i = 0; i < quantity; i++)
	{
		std::cout << *(reinterpret_cast<char*>(&value) + i);
	}
	*/
	if (!file.good())
		throw Error(Error::Incorrect);
}

template<typename T>
void mp::File::readFromFile(std::ifstream& file, T& value, unsigned int& bytes_to_read, const int quantity)
{
	readFromFile(file, value, quantity);
	bytes_to_read -= quantity; 
}

template<typename T>
void mp::File::changeEndian(T& value)
{
	char* pointer = reinterpret_cast<char*>(&value);
	std::reverse(pointer, pointer + sizeof(T));
}