#if !defined HDR_W32_HANDLES_HPP
#define HDR_W32_HANDLES_HPP

#define WIN32_LEAN_AND_MEAN

#if !defined _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif
#include <windows.h>

#define THROW_HANDLE_WIN32_ERROR() throw w32::exception()

namespace w32 
{
	template < typename val_type, typename ret_type = BOOL >
	struct func_helper 
	{
		typedef ret_type (WINAPI * type)(val_type);
	};

	template < typename val_type >
	struct base_close_policy 
	{
		typedef val_type value_type;

		static bool close( value_type hndl )
		{
			return true;
		}
	};

	template < typename val_type, val_type invalid >
	struct invalid_holder 
	{
		typedef val_type value_type;

		static const value_type invalid_value;		
	};

	template< typename val_type, val_type invalid >
	const val_type invalid_holder< val_type, invalid>::invalid_value = invalid;

	template < typename val_type, val_type invalid >
	struct base_clone_policy : invalid_holder<val_type, invalid>
	{
		static value_type clone( value_type hndl )
		{
			return hndl;
		}
	};

	template<typename val_type, val_type invalid>
	struct win32_clone_policy : invalid_holder<val_type, invalid>
	{
		static value_type clone( value_type hndl )
		{
			value_type ret = invalid;
			HANDLE process = ::GetCurrentProcess();

			if ( FALSE != ::DuplicateHandle( 
				process, hndl,
				process, &ret, 
				0, FALSE, DUPLICATE_SAME_ACCESS ) )
				return ret;
			else 
				return invalid;
		}
	};

	template<
		typename val_type, 
		typename ret_type, 
		ret_type (WINAPI * close_fn)(val_type)
	>
	struct win32_close_policy : base_close_policy<val_type>
	{
	private :
		using base_close_policy<val_type>::close;

	public :
		static bool close( value_type hndl )
		{
			return !!close_fn( hndl );
		}
	};

	//*
	template<
		typename val_type,
		typename close_pl,
		typename clone_pl
	>
	struct full_handle_traits 
		//: invalid_holder<val_type, clone_pl::invalid_value>
	{
		typedef val_type value_type;
		typedef close_pl close_policy_type;
		typedef clone_pl clone_policy_type;

		static const value_type invalid_value;

		static bool close( value_type hndl )
		{
			return close_pl::close( hndl );
		}

		static value_type clone( value_type hndl )
		{
			return clone_pl::clone( hndl );
		}

		static value_type dupl( value_type hndl )
		{
			return clone( hndl );
		}

		template <typename T = value_type>
		struct rebind : full_handle_traits<val_type, close_pl, clone_pl> {};
	};	

	template<
		typename val_type,
		typename close_pl,
		typename clone_pl
	>
	const val_type 
	full_handle_traits< val_type, close_pl, clone_pl>::invalid_value = clone_pl::invalid_value;

	template < typename val_type >
	struct base_handle_traits : full_handle_traits<
		val_type,
		win32_close_policy< val_type, BOOL, ::CloseHandle >,
		win32_clone_policy< val_type, NULL >
	>
	{
	};

	template < typename val_type, BOOL ( WINAPI * close_fn )( val_type ) >
	struct detailed_handle_traits : full_handle_traits<
		val_type,
		win32_close_policy< val_type, BOOL, close_fn >,
		win32_clone_policy< val_type, NULL >
	>
	{
	};

	template < typename val_type, BOOL (WINAPI * close_fn)(val_type), val_type invalid >
	struct auto_handle_traits : full_handle_traits<
		val_type,
		win32_close_policy< val_type, BOOL, close_fn >,
		base_clone_policy< val_type, invalid >
	>
	{
	};
	//*/	

	//template < typename val_type >
	struct find_detailed_handle_traits : public detailed_handle_traits< HANDLE, &::FindClose >
	{};

	template < typename val_type >
	struct light_handle_traits : full_handle_traits<
		val_type,
		base_close_policy< val_type >,
		base_clone_policy< val_type, base_handle_traits<val_type>::invalid_value >
	>
	{
	};
	
	template < typename val_type = HANDLE, class traits = base_handle_traits< val_type > >
	struct handle_holder
	{
		typedef traits traits_type;
		typedef typename traits_type::value_type value_type;

		value_type handle_;

		handle_holder() : handle_( traits_type::invalid_value ) {}

		handle_holder( value_type h ) : handle_(h) {}

		handle_holder( const handle_holder& other ) : handle_(traits_type::invalid_value) 
		{
			if ( other.valid() )
				this->handle_ = traits_type::clone( other.value() );
		}

		template < class traits_other >
		handle_holder( const handle_holder< value_type, traits_other >& other ) : handle_(traits_type::invalid_value)
		{
			//attach( other.value() );
			if ( other.valid() )
				this->handle_ = traits_type::clone( other.value() );
		}

		const handle_holder& operator = ( const handle_holder& rhs )
		{
			this->attach( 
				rhs.valid() ? traits_type::clone( rhs.value() ) : traits_type::invalid_value
				);
			
			return *this;
		}

		const handle_holder& operator = ( value_type rhs )
		{
			this->attach( rhs );

			return *this;
		}

		bool attach( value_type other )
		{
			this->close();
			this->handle_ = other;

			return true;
		}

		template < class traits_other >
		void duplicate( const handle_holder< value_type, traits_other >& from )
		{
			this->attach( 
				traits_type::dupl( from.value() ) 
				);
		}

		handle_holder< value_type, light_handle_traits<value_type> > duplicate() const
		{
			return handle_holder< light_handle_traits<value_type> >( traits_type::dupl( this->value() ) )
		}

		bool operator ! () const 
		{
			return ( !this->valid() );
		}

		value_type value() const 
		{
			return this->handle_;
		}

		bool valid() const 
		{
			return ( this->handle_ != traits_type::invalid_value );
		}

		bool close() //const
		{
			if ( this->valid() )
			{
				if ( traits_type::close( this->handle_ ) )
					this->handle_ = traits_type::invalid_value;
			}

			return !this->valid();
		}

		DWORD wait( DWORD ms = INFINITE ) const 
		{
			return ::WaitForSingleObject( this->value(), ms );
		}

		template < class traits_other >
		DWORD signal_and_wait( 
			const handle_holder< value_type, traits_other >& signal,
			DWORD ms,
			bool alert 
			) const 
		{
			return ::SignalObjectAndWait( signal.value(), this->value(), ms, static_cast<BOOL>(alert) );
		}

		bool register_wait
			( PHANDLE pnNewObject
			, WAITORTIMERCALLBACK pfnCallback
			, PVOID pContext = NULL
			, ULONG nMilliseconds = INFINITE
			, ULONG nFlags = WT_EXECUTEDEFAULT
			) const
		{
			return !!::RegisterWaitForSingleObject( 
				pnNewObject, this->value(),
				pfnCallback, pContext, nMilliseconds, nFlags
				);
		}

		bool unregister_wait( HANDLE wait_object ) const
		{
			return !!::UnregisterWait( wait_object );
		}

		bool unregister_wait( HANDLE wait_object, HANDLE completion_event ) const
		{
			return !!::UnregisterWaitEx( wait_object, completion_event );
		}

		bool get_information( LPDWORD flags ) const
		{
			return !!::GetHandleInformation( this->value(), flags );
		}

		DWORD get_information() const
		{
			DWORD flags = 0;

			if ( TRUE != ::GetHandleInformation( this->value(), &flags ) )
				THROW_HANDLE_WIN32_ERROR();

			return flags;
		}

		bool set_information( DWORD mask, DWORD flags ) const
		{
			return !!::SetHandleInformation( this->value(), mask, flags );
		}

		~handle_holder()
		{
			this->close();
		}
	};	

	template <
		typename val_type = HANDLE, 
		typename func_helper<val_type>::type close_fn = &::CloseHandle, 
		val_type invalid = INVALID_HANDLE_VALUE >
	struct auto_handle : 
		handle_holder< 
			val_type, 
			auto_handle_traits<val_type, close_fn, invalid >
		> 
	{
		typedef handle_holder< 
			val_type, 
			auto_handle_traits<val_type, close_fn, invalid >
		> base_t;

		using base_t::operator =;
		using base_t::operator !;

		auto_handle() {}

		auto_handle( value_type h ) : base_t(h) {}		

		template < class traits_other >
		auto_handle( const handle_holder< value_type, traits_other >& other ) : base_t(other) {}

	private :
		auto_handle( const auto_handle& other ) : base_t(other) {}
	};
}

#endif /* HDR_W32_HANDLES_HPP */
