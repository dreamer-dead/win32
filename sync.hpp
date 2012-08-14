#if !defined HDR_SYNC_HPP
#define HDR_SYNC_HPP

#include "handles.hpp"

//#define STORE_EVENT_INFO

#if defined STORE_EVENT_INFO
#include <string>
#endif

namespace w32 
{
	namespace detail 
	{
		struct event_create_params 
		{
			LPSECURITY_ATTRIBUTES sec;
			LONG initial;
			LONG max_count;
			//LPCTSTR name;
		};

		inline HANDLE detail_create( const char * name, const event_create_params& params )
		{
			return ::CreateSemaphoreA( params.sec, params.initial, params.max_count, name );
		}
	}

	template < template <typename> class traits = base_handle_traits >
	struct event_holder : public handle_holder< HANDLE, traits >
	{
		typedef handle_holder< HANDLE, traits > base_type;

#if defined STORE_EVENT_INFO
		std::string name_;
		LONG max_count_;
#endif

		event_holder() 
#if defined STORE_EVENT_INFO
		: max_count_(0)
#endif 
		{}
		explicit event_holder( value_type hndl ) : base_type( hndl ) {}

		event_holder( const event_holder& evt ) : base_type( evt ) {}

		template < template <typename> class other_traits >
		event_holder( const event_holder< other_traits >& evt ) : base_type( evt ) {}

		bool create( const char * name, LONG max_count )
		{
#if defined STORE_EVENT_INFO
			this->max_count_ = max_count;
			this->name_ = name;
#endif

			this->attach( 
				::CreateSemaphoreA( NULL, max_count, max_count, name ) 
				);

			return this->valid();
		}

		bool open( const char * name, DWORD acc, bool inherit )
		{
#if defined STORE_EVENT_INFO
			this->name_ = name;
#endif

			this->attach( ::OpenSemaphoreA( acc, (inherit ? TRUE : FALSE), name ) );
			return this->valid();
		}

		bool set() const 
		{
			return ( FALSE != ::SetEvent( this->value() ) );
		}

		bool pulse() const 
		{
			return ( FALSE != ::PulseEvent( this->value() ) );
		}

		bool reset() const 
		{
			return ( FALSE != ResetEvent( this->value() ) );
		}

#if defined STORE_EVENT_INFO
		const std::string& name() const 
		{
			return this->name_;
		}

		LONG max_count() const 
		{
			return this->max_count_;
		}
#endif
	};
}

#endif /* HDR_SYNC_HPP */ 
