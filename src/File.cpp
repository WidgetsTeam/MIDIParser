#include "File.hpp"

#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <iostream>

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
	tracks.clear();

	std::ifstream midi_file(filename, std::ios_base::binary);
	
	if (!midi_file.is_open())
		return;

	is_read = true;


	std::string header_chunk_signature(4, '\0');
	midi_file.read(&header_chunk_signature[0], 4);

	if (header_chunk_signature != "MThd")
	{
		midi_file.close();
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
		is_smpte_type = false;
	}
	else
	{
		smpte_byte[0] = division & 0xFF;
		division >>= 8;
		smpte_byte[1] = abs(division);
		is_smpte_type = true;
	}
	

	for (int track_number = 0; track_number < tracks_quantity; track_number++)
	{
		std::string track_chunk_signature(4, '\0');
		midi_file.read(&track_chunk_signature[0], 4);

		if (track_chunk_signature != "MTrk")
		{
			midi_file.close();
			return;
		}

		unsigned int track_chunk_data_length;
		midi_file.read(reinterpret_cast<char*>(&track_chunk_data_length), 4);
		track_chunk_data_length = changeEndian<unsigned int>(track_chunk_data_length);

		tracks.push_back(Track());

		int event_number = 0;

		for (unsigned int bytes_to_read = track_chunk_data_length; bytes_to_read > 0;) //Jeden przebieg pętli = wczytanie 1 eventu
		{
			tracks[track_number].events.push_back(Event());
			tracks[track_number].events_quantity++;

			unsigned int delta_time = readVariableLengthQuantity(midi_file, bytes_to_read);

			char status_byte;
			midi_file.get(status_byte);
			bytes_to_read--;
			if (status_byte >= 0x80 && status_byte <= 0xEF) //MIDI Event
			{
				tracks[track_number].events[event_number].channel = status_byte & 0xF;

				switch ((status_byte >> 4) & 0xF)
				{
					case 0x8: tracks[track_number].events[event_number].event_type = Event::Type::NoteOff; break;
					case 0x9: tracks[track_number].events[event_number].event_type = Event::Type::NoteOn; break;
					case 0xA: tracks[track_number].events[event_number].event_type = Event::Type::PolyphonicKeyPressure; break;
					case 0xB: tracks[track_number].events[event_number].event_type = Event::Type::ControllerChange; break;
					case 0xC: tracks[track_number].events[event_number].event_type = Event::Type::ProgramChange; break;
					case 0xD: tracks[track_number].events[event_number].event_type = Event::Type::ChannelKeyPressure; break;
					case 0xE: tracks[track_number].events[event_number].event_type = Event::Type::PitchBend; break;
				}

				tracks[track_number].events[event_number].event_data.push_back(status_byte);

				int bytes_to_read_in_midi_event = 2;

				if (tracks[track_number].events[event_number].event_type == Event::Type::ProgramChange ||
					tracks[track_number].events[event_number].event_type == Event::Type::ChannelKeyPressure)
						bytes_to_read_in_midi_event = 1;

				char data_byte;

				for (int j = 0; j < bytes_to_read_in_midi_event; j++)
				{
					midi_file.get(data_byte);
					tracks[track_number].events[event_number].event_data.push_back(data_byte);
					bytes_to_read--;
				}

				tracks[track_number].events[event_number].event_name = Event::Name::ChannelVoice;

				if (tracks[track_number].events[event_number].event_type == Event::Type::ControllerChange &&
					(tracks[track_number].events[event_number].event_data[1] >= 0x78 
					 || tracks[track_number].events[event_number].event_data[1] <= 0x7F ))
						tracks[track_number].events[event_number].event_name = Event::Name::ChannelMode;	

			}
			else if (status_byte == 0xF0 || status_byte == 0xF7) //SysEx
			{
				tracks[track_number].events[event_number].event_name = Event::Name::Sysex;

				if (status_byte == 0xF0)
					tracks[track_number].events[event_number].event_data.push_back(status_byte);

				int sysex_event_data_length = readVariableLengthQuantity(midi_file, bytes_to_read);

				char data_byte;

				for (int j = 0; j < sysex_event_data_length; j++)
				{
					midi_file.get(data_byte);
					tracks[track_number].events[event_number].event_data.push_back(data_byte);
					bytes_to_read--;
				}
			}
			else if (status_byte == 0xFF) //Meta Event
			{
				char event_type;
				midi_file.get(event_type);
				bytes_to_read--;
				tracks[track_number].events[event_number].event_data.push_back(event_type);
				switch (event_type)
				{
					case 0x00: tracks[track_number].events[event_number].event_type = Event::Type::SequenceNumber; break;
					case 0x01: tracks[track_number].events[event_number].event_type = Event::Type::TextEvent; break;
					case 0x02: tracks[track_number].events[event_number].event_type = Event::Type::CopyrightNotice; break;
					case 0x03: tracks[track_number].events[event_number].event_type = Event::Type::SequenceOrTrackName; break;
					case 0x04: tracks[track_number].events[event_number].event_type = Event::Type::InstrumentName; break;
					case 0x05: tracks[track_number].events[event_number].event_type = Event::Type::Lyric; break;
					case 0x06: tracks[track_number].events[event_number].event_type = Event::Type::Marker; break;
					case 0x07: tracks[track_number].events[event_number].event_type = Event::Type::CuePoint; break;
					case 0x20: tracks[track_number].events[event_number].event_type = Event::Type::MidiChannelPrefix; break;
					case 0x2F: tracks[track_number].events[event_number].event_type = Event::Type::EndOfTrack; break;
					case 0x51: tracks[track_number].events[event_number].event_type = Event::Type::SetTempo; break;
					case 0x54: tracks[track_number].events[event_number].event_type = Event::Type::SmpteOffset; break;
					case 0x58: tracks[track_number].events[event_number].event_type = Event::Type::TimeSignature; break;
					case 0x59: tracks[track_number].events[event_number].event_type = Event::Type::KeySignature; break;
					case 0x7F: tracks[track_number].events[event_number].event_type = Event::Type::SequencerSpecificMetaEvent; break;
				}

				unsigned int meta_event_data_length = readVariableLengthQuantity(midi_file, bytes_to_read);

				for (int j = 0; j < meta_event_data_length; j++)
				{
					char data_byte;
					midi_file.get(data_byte);

					tracks[track_number].events[event_number].event_data.push_back(data_byte);
					bytes_to_read--;
				}
			}
			else
			{
				midi_file.close();
				return;
			}

			event_number++;
		}
	}

	is_correct = true;
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


int mp::File::readVariableLengthQuantity(std::ifstream& file, unsigned int& bytes_to_read)
{
	int end_value = 0;
	char byte;
	int multipler = 1;

	do
	{
		file.get(byte);
		bytes_to_read--;

		end_value += (byte & 127) * multipler;
		multipler *= 128;
	} while (byte >> 7 == 1);

	return end_value;
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