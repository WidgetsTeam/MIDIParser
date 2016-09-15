#include <iostream>
#include <MidiParser/File.hpp>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Incorrect use of program." << std::endl;
		std::cout << "Use: " << argv[0] << " file.mid" << std::endl;
		return 1;
	}

	try
	{
		mp::File file(argv[1]);

		std::cout << "MIDI File Format: " << file.getFormat() << std::endl;
		std::cout << "Tracks quantity: " << file.getTracksQuantity() << std::endl;
		if (file.isSmpteType())
		{
			std::cout << "Frames per second: " << file.getFramesPerSecond() << std::endl;
			std::cout << "Ticks per frame: " << file.getTicksPerFrame() << std::endl;
		}
		else 
			std::cout << "Ticks per quater note: " << file.getTicksPerQuaterNote() << std::endl;

		for (int i = 0; i < file.getTracksQuantity(); i++)
		{
			std::cout << std::endl << "__TRACK " << i + 1 << "__" << std::endl;
			for (int j = 0; j < file[i].getEventsQuantity(); j++)
			{
				switch (file[i][j].getEventName())
				{
					case 0: std::cout << "ChannelVoice  "; break;
					case 1: std::cout << "ChannelMode  "; break;
					case 2: std::cout << "Sysex  "; break;
					case 3: std::cout << "Metadata  "; break;
				}

				std::cout << "Delta time: " << std::dec << file[i][j].getDeltaTimeInTicks() << "  ";
				std::cout << "Event data: ";
				for (int k = 0; k < file[i][j].getDataQuantity(); k++)
					std::cout << std::hex << static_cast<unsigned short>(file[i][j][k] & 0xFF) << ' ';

				std::cout << std::endl;
			}
		}

	}
	catch (const mp::Error& error)
	{
		switch (error.what())
		{
			case mp::Error::Unread:
				std::cerr << "File is unread";
				break;
			case mp::Error::Incorrect:
				std::cerr << "File is incorrect";
				break;
		}
	}

	std::cin.get();
	return 0;
}
