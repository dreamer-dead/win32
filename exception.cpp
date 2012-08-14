#include "exception.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace w32
{
	error_t get_error_code()
	{
		return ::GetLastError();
	}

	std::string get_message( error_t err )
	{
		struct local_deleter
		{
			HGLOBAL memory;

			local_deleter( HGLOBAL mem ) : memory( mem ) {}
			~local_deleter() { if ( this->memory ) ::LocalFree( this->memory); }
		};

		char * p = NULL;
		/* DWORD len = */::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&p), 0, NULL);
		local_deleter deleter( p );

		return p;
	}

	exception::exception( error_t err )
		: error( err ), message( get_message( err ) )
	{}

	exception::exception( const char * msg, error_t err )
		: error( err ), message( msg + std::string(" (") + get_message( err ) + std::string(" )") )
	{}

	const char * exception::what() const 
	{
		return (this->message.c_str());
	}	

	exception::~exception()
	{
	}
}
