#include <iostream>

#include "File.hpp"

int main()
{
	try
	{
		mp::File file("file.mid");
		for(int i = 0; i < file.getTracksQuantity(); i++)
		{
			for(int j = 0; j < file[i].getEventsQuantity(); j++)
			{
				for(int k = 0; k < file[i][j].getDataQuantity(); k++)
					std::cout << std::hex << static_cast<unsigned short>(file[i][j][k] & 0xFF) << ' ';
				std::cout << std::endl;
			}
		}
	}
	catch (const mp::Error& error)
	{
		switch(error.what())
		{
			case mp::Error::Unread:
				std::cout << "File is unread";
				break;
			case mp::Error::Incorrect:
				std::cout << "File is incorrect";
				break;
		}
	}

	std::cin.get();
	return 0;
}