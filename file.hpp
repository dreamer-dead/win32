#if !defined HDR_W32_FILE_HPP
#define HDR_W32_FILE_HPP

// must be on the same directory level with
#include "file_base.hpp"

namespace w32 
{
	namespace detail 
	{
		struct file_create_params 
		{
			DWORD desired_access;
			DWORD share_mode;
			LPSECURITY_ATTRIBUTES sec_attributes;
			DWORD creation_disposition;
			DWORD flags;
			HANDLE temp_file;

			file_create_params() :
				desired_access(STANDARD_RIGHTS_READ),
				share_mode(FILE_SHARE_READ),
				sec_attributes( NULL ),
				creation_disposition(OPEN_EXISTING),
				flags(FILE_ATTRIBUTE_NORMAL),
				temp_file( NULL )
			{
			}
		};

		HANDLE detail_create( const TCHAR *name, const file_create_params& /* params */ );		
	}

	template < class traits = base_handle_traits<HANDLE> >
	struct file_holder : public file_base< handle_holder< HANDLE, traits > >
	{
		typedef file_base< handle_holder< HANDLE, traits > > base_type;
		typedef detail::file_create_params create_params;

		enum seek_direction
		{
			seek_cur = FILE_CURRENT,
			seek_set = FILE_BEGIN,
			seek_end = FILE_END
		};

		using base_type::operator =;

		file_holder()
		{}

		explicit file_holder( const TCHAR * name ) 
			: base_type( detail::detail_create( name, detail::file_create_params() ) )
		{			
		}

		file_holder( const TCHAR * name, const detail::file_create_params& params ) 
			: base_type( detail::detail_create( name, params ) )
		{			
		}

		explicit file_holder( HANDLE hndl ) 
			: base_type( hndl )
		{			
		}

		file_holder( const file_holder& other ) : base_type( other ) 
		{		
		}

		template < class traits_other >
		file_holder( const file_holder< traits_other >& other ) : base_type( other ) 
		{		
		}		

		bool create( const TCHAR * name )
		{
			return this->create( name, detail::file_create_params() );
		}

		bool create( const TCHAR * name, const detail::file_create_params& params )
		{
			this->attach( detail::detail_create( name, params ) );
			this->check();

			return this->valid();
		}

#if defined _WIN32_WINNT && _WIN32_WINNT >= 0x502

		bool reopen( DWORD acc, DWORD mode, DWORD attr )
		{
			this->handle_ = ::ReOpenFile( this->value(), acc, mode, attr );
		}

#else
//#	error your Windows version not match
#endif // defined _WIN32_WINNT && _WIN32_WINNT >= 0x502


		size_type seek( size_type pos, seek_direction dir ) const
		{
			return ::SetFilePointer( this->value(), static_cast<LONG>(pos), NULL, dir );
		}

		/*
		size_type position() const
		{
			::SetFilePointer()
		}
		//*/

		size_type size() const 
		{
			return GetFileSize( value(), NULL );
		}
	};
}

#endif /* HDR_W32_FILE_HPP */
