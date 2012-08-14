#include "file.hpp"

namespace w32
{
	namespace detail
	{	
		HANDLE detail_create( const TCHAR *name, const file_create_params& params )
		{
			return ::CreateFile( 
				name, 
				params.desired_access, 
				params.share_mode, 
				params.sec_attributes, 
				params.creation_disposition, 
				params.flags, 
				params.temp_file );
		}
	}
}