#include "pipe.hpp"
#include <tchar.h>

namespace w32 
{
	namespace detail 
	{
		static const DWORD 
			DEF_PIPE_BUFSIZE = 1024 * 4,         // output buffer size 			
			DEF_PIPE_TIMEOUT = 1000 * 2;         // client time-out 

		const TCHAR prefix[] = _TEXT( "\\\\.\\pipe\\" );
		const DWORD prefix_len = sizeof( prefix ) / sizeof( prefix[0]);

		std::basic_string< TCHAR > get_full_name( const TCHAR * name )
		{
			int len = lstrlen( name );

			if ( len >= prefix_len )
			{
				int cmp = ::CompareString( 
					LOCALE_SYSTEM_DEFAULT, 
					NORM_IGNORECASE, 
					name, prefix_len - 1,	// without TERM ZERO !
					prefix, prefix_len - 1	// without TERM ZERO !
					);

				if ( !cmp  )
					return std::basic_string< TCHAR >( name );
			}			

			return std::basic_string< TCHAR >( prefix ) + name;
		}

		HANDLE named_pipe( const TCHAR * name )
		{		
			std::basic_string< TCHAR > full_name = get_full_name( name );
			HANDLE pipe = INVALID_HANDLE_VALUE;
			
			pipe = ::CreateNamedPipe( 
				full_name.c_str(), 
				PIPE_ACCESS_DUPLEX,       // read/write access 
				PIPE_TYPE_MESSAGE |       // message type pipe 
				PIPE_READMODE_MESSAGE |   // message-read mode 
				PIPE_WAIT,                // blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				DEF_PIPE_BUFSIZE,         // output buffer size 
				DEF_PIPE_BUFSIZE,         // input buffer size 
				DEF_PIPE_TIMEOUT,         // client time-out 
				NULL
				);						

			return pipe;
		}

		HANDLE detail_create( const TCHAR *name, const pipe_create_params& /* params */ )
		{
			return named_pipe( name );
		}
	}
}