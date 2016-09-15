#ifndef EVENT_HPP
#define EVENT_HPP

#include <vector>

namespace mp
{

class Event
{
public:
	friend class File;
	
	enum class Name
	{
		ChannelVoice,
		ChannelMode,
		Sysex,
		Metadata
	};

	enum class Type
	{
		NoteOff,
		NoteOn,
		PolyphonicKeyPressure,
		ControllerChange,
		ProgramChange,
		ChannelKeyPressure,
		PitchBend,
		AllSoundOff,
		ResetAllControllers,
		LocalControl,
		AllNotesOff,
		OmniOff,
		OmniOn,
		MonoOn,
		PolyOn,
		F0SysexEvent,
		F7SysexEvent,
		SequenceNumber,
		TextEvent,
		CopyrightNotice,
		SequenceOrTrackName,
		InstrumentName,
		Lyric,
		Marker,
		CuePoint,
		ProgramName,
		DeviceName,
		MidiChannelPrefix,
		MidiPort,
		EndOfTrack,
		SetTempo,
		SmpteOffset,
		TimeSignature,
		KeySignature,
		SequencerSpecificMetaEvent
	};
	
	Event();

	Name          getEventName()                       const;
	Type          getEventType()                       const;

	double        getDeltaTimeInMicroseconds()         const;
	int           getDeltaTimeInTicks()                const;

	double        getAbsoluteTimeInMicroseconds()      const;
	long long int getAbsoluteTimeInTicks()             const;

	unsigned int  getDataQuantity()                    const;
	int           getChannel()                         const;

	char          operator[](const unsigned int index) const;
private:
	Name              name;
	Type              type;

	double            delta_time_microseconds;
	int               delta_time_ticks;

	double            absolute_time_microseconds;
	long long int     absolute_time_ticks;

	std::vector<char> data;

	short			  channel;
};

}

#endif