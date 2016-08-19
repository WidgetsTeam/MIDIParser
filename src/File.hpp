#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <vector>
#include "Track.hpp"

namespace mp
{

class File
{
public:
	File();
	File(const std::string& filename);
	
	void           open(const std::string& filename);

	bool           isGood()                             const;
	bool           isRead()                             const;
	bool           isCorrect()                          const;

	short          getFormat()                          const;
	unsigned short getTracksQuantity()                  const;

	short          getTicksPerQuaterNote()              const;
	short          getTicksPerFrame()                   const;
	short          getFramesPerSecond()                 const;

	bool           isSmpteType()                        const;

	const Track    connectTracks()                      const;

	const Track&   operator[](const unsigned int index) const;
private:
	template<typename T>
	T changeEndian(const T value);

	bool           is_read;
	bool           is_correct;

	short          format;
	unsigned short tracks_quantity;

	union
	{
		short ticks_per_quater_note;
		char  smpte_byte[2];
	};

	bool               is_smpte_type;

	std::vector<Track> tracks;
};

}

#endif