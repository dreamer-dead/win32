#if !defined HDR_W32_PIPE_HPP
#define HDR_W32_PIPE_HPP

#include "file_base.hpp"
#include "blob.hpp"
#include <string>
#include <utility>

namespace w32 
{
	namespace detail 
	{
		struct pipe_create_params 
		{
			DWORD mode;
		};

		struct state 
		{
			DWORD mode;
			DWORD instances;
			DWORD max_collection_count;
			DWORD timeout;
		};

		HANDLE named_pipe( const char *name );	
		HANDLE detail_create( const char *name, const pipe_create_params& params );
		std::string get_full_name( const char * name );
	}

	template < class traits = base_handle_traits<HANDLE> >
	struct named_pipe_holder : public file_base< handle_holder< HANDLE, traits > >
	{
		typedef file_base< handle_holder< HANDLE, traits > > base_type;
		typedef detail::pipe_create_params create_params;

		std::string name_;

		using base_type::operator =;

		named_pipe_holder() : name_("")
		{}

		named_pipe_holder( const char * name ) : 						
			base_type( detail::named_pipe( name ) ),
			name_( detail::get_full_name(name) )
		{
		}

		named_pipe_holder( const named_pipe_holder& other ) : 
			base_type( other ),
			name_( other.name_ )
		{
		}

		template < class traits_other >
		named_pipe_holder( const named_pipe_holder< traits_other >& other ) : 
			base_type( other ),
			name_( other.name() )
		{		
		}

		const std::string& name() const 
		{
			return name_;
		}
	
		bool connect( LPOVERLAPPED pover = NULL ) const
		{
			return ( FALSE != ::ConnectNamedPipe( this->value(), pover ) );
		}
		
		bool disconnect() const
		{
			return ( FALSE != ::DisconnectNamedPipe( this->value() ) );
		}

		bool wait( DWORD timeout = NMPWAIT_USE_DEFAULT_WAIT ) const
		{
			return ( FALSE != ::WaitNamedPipeA( name_.c_str(), timeout ) );
		}

		bool get_state( detail::state& st ) const
		{
			return ( FALSE != GetNamedPipeHandleState( 
				this->value(), 
				&st.mode, &st.instances, &st.max_collection_count, &st.timeout 
				) );
		}

		bool set_state( const detail::state& st ) const
		{
			DWORD 
				mode = st.mode,
				count = st.max_collection_count,
				tout = st.timeout;

			return ( FALSE != SetNamedPipeHandleState( 
				this->value(),
				&mode, &count, &tout ) );
		}

		/*
		bool transact( blob& rd, const blob& wr ) const 
		{			
			return ( FALSE != TransactNamedPipe( 
				this->value(),
				wr.data, wr.size,
				rd.data, rd.size,
				&rd.size, NULL
				) );			
		}
		//*/

		std::pair<bool, size_type> transact( const blob& rd, const blob& wr ) const 
		{
			size_type readed = 0;
			bool res = ( FALSE != TransactNamedPipe( 
				this->value(),
				wr.data, wr.size,
				rd.data, rd.size,
				&readed, NULL
				) );

			return std::make_pair( res, readed );			
		}
	};
}

#endif /* HDR_W32_PIPE_HPP */
