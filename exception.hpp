#if defined _MSC_VER && _MSC_VER >= 1300
#pragma once
#endif

#if !defined __HDR_WIN32_EXCEPTION_HPP__
#define __HDR_WIN32_EXCEPTION_HPP__

#include <exception>
#include <string>

namespace w32
{
	typedef unsigned long error_t;

	extern error_t get_error_code();

	struct exception : std::exception
	{
		exception( error_t err = get_error_code() );

		exception( const char * msg, error_t err = get_error_code() );

		virtual const char * what() const;
		virtual ~exception();

		error_t error;

	protected :	
		//char * message;
		std::string message;
	};	
}

#endif /* __HDR_WIN32_EXCEPTION_HPP__ */