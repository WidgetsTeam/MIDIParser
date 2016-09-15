#ifndef ERROR_HPP
#define ERROR_HPP

namespace mp
{

	class Error
	{
	public:
		enum ErrorType
		{
			Unread,
			Incorrect
		};

		Error(const mp::Error::ErrorType error_type);

		ErrorType what() const;
	private:
		ErrorType error_type;
	};

}

#endif
