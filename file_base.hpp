#if !defined HDR_FILE_BASE_HPP
#define HDR_FILE_BASE_HPP

#include "handles.hpp"
#include "blob.hpp"

#include <tchar.h>

namespace w32 
{
	template < class handle >
	struct file_base : handle
	{
		typedef DWORD size_type;
		typedef handle handle_type;
		typedef typename handle::traits_type traits_type;	

		file_base() : handle_type()
		{
			this->check();
		}

		file_base( typename handle_type::value_type h ) : handle_type( h )
		{
			this->check();
		}

		file_base( const handle_type& h ) : handle_type( h )
		{
			this->check();
		}

		template < class traits_other >
		file_base( const handle_holder< typename handle::value_type, traits_other >& other )
			: handle_type( other )
		{
			this->check();
		}

		const file_base& operator = ( const file_base& rhs )
		{
			this->handle_type::operator =( rhs );
			return *this;
		}

		const file_base& operator = ( typename traits_type::value_type rhs )
		{
			this->handle_type::operator =( (rhs == INVALID_HANDLE_VALUE ? 0 : rhs) );
			return *this;
		}

		size_type write( const void * data, size_type size, LPOVERLAPPED lpover = NULL ) const
		{
			size_type written = 0;
			if ( this->valid() && size != 0 && data != 0 )
			{
				::WriteFile( this->value(), data, size, &written, lpover );				
			}
			return written;
		}

		size_type read( void * buffer, size_type size, LPOVERLAPPED lpover = NULL ) const
		{
			size_type readed = 0;
			BOOL ret_code = FALSE;
			if ( this->valid() && buffer != NULL && size != 0 )
				ret_code = ::ReadFile( this->value(), buffer, size, &readed, lpover );
			return readed;
		}

		size_type write( const blob& b ) const
		{
			return this->write( b.data, b.size );
		}

		size_type read( const blob& b ) const
		{
			return this->read( b.data, b.size );
		}

		bool flush() const 
		{
			return ( FALSE != ::FlushFileBuffers( this->value() ) );
		}

		void check()
		{
			if ( this->value() == INVALID_HANDLE_VALUE )
				this->handle_ = traits_type::invalid_value;
		}

		//*
		template < typename ptype >
		bool create( const TCHAR * name, const ptype& params = ptype() )
		{
			this->attach( detail_create( name, params ) );
			this->check();

			return this->valid();
		}
		//*/
	};
}

#endif /* HDR_FILE_BASE_HPP */
