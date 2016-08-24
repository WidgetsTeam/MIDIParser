#ifndef EVENT_HPP
#define EVENT_HPP

#include <vector>

namespace mp
{

class Event
{
public:
	friend class File;
	
	enum Name
	{
		ChannelVoice,
		ChannelMode,
		Sysex,
		Metadata
	};

	enum Type
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
		MidiChannelPrefix,
		EndOfTrack,
		SetTempo,
		SmpteOffset,
		TimeSignature,
		KeySignature,
		SequencerSpecificMetaEvent
	};
	
	Event();

	Name                     getEventName()                  const;
	Type                     getEventType()                  const;

	double                   getDeltaTimeInMicroseconds()    const;
	int                      getDeltaTimeInTicks()           const;

	double                   getAbsoluteTimeInMicroseconds() const;
	long long int            getAbsoluteTimeInTicks()        const;

	const std::vector<char>& getData()                       const;
	int                      getChannel()                    const;
private:
	Name              event_name;
	Type              event_type;

	double            delta_time_microseconds;
	int               delta_time_ticks;

	double            absolute_time_microseconds;
	long long int     absolute_time_ticks;

	std::vector<char> event_data;

	short			  channel;
};

}

#endif