#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <fstream>
#include <vector>

#include <MidiParser/Track.hpp>
#include <MidiParser/Error.hpp>

namespace mp
{

class File
{
public:
	File();
	File(const std::string& filename);
	
	void           open(const std::string& filename);

	short          getFormat()                          const;
	unsigned short getTracksQuantity()                  const;

	short          getTicksPerQuaterNote()              const;
	short          getFramesPerSecond()                 const;
	short          getTicksPerFrame()                   const;

	bool           isSmpteType()                        const;

	const Track    connectTracks()                      const;

	const Track&   operator[](const unsigned int index)          const;
private:
	std::size_t readVariableLengthQuantity(std::ifstream& file, int& value);

	std::size_t readFromFile(std::ifstream& file, std::string& string, const int quantity = 1);
	template<typename T>
	std::size_t readFromFile(std::ifstream& file, T& value, const int quantity = 1);

	template<typename T>
	void changeEndian(T& value);

	short          format;
	unsigned short tracks_quantity;

	union
	{
		short ticks_per_quater_note;
		unsigned char  smpte_byte[2];
	};

	bool               is_smpte_type;

	std::vector<Track> tracks;
};

}

#endif