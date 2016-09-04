#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <fstream>
#include <vector>

#include "Track.hpp"
#include "Error.hpp"

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
	void readVariableLengthQuantity(std::ifstream& file, int& value, unsigned int& bytes_to_read);

	template<typename T>
	void readFromFile(std::ifstream& file, T& value, const int quantity = 1);

	template<typename T>
	void readFromFile(std::ifstream& file, T& value, unsigned int& bytes_to_read, const int quantity = 1);

	template<typename T>
	void changeEndian(T& value);

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