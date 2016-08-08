#ifndef FILE_HPP
#define FILE_HPP

#include <string>

namespace mp
{

class File
{
public:
	File();
	File(const std::string& filename);
	
	void 		open(const std::string& filename);
	void 		close();

	bool 		isGood()		const;
	bool 		isExisting() 		const;
	bool 		isCorrect() 		const;

	short  		getMidiFileFormat() 	const;
	unsigned short	getTracksQuantity() 	const;


	bool  		getDivisionType()	const;
	short 		getTicksPerQuaterNote() const;
	short 		getTicksPerFrame() 	const;
	short 		getFramesPerSecond() 	const;

private:
	bool file_existing;
	bool file_correct;

	short 	midi_file_format;
	unsigned short	tracks_quantity;
	union
	{
		short ticks_per_quater_note;
		char smtpe_byte[2];
	};
	bool division_type;
	
	template<typename T>
	T changeEndian(T value);
};

}

#endif
