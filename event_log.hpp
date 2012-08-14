#if !defined HDR_EVENTLOG_HPP
#define HDR_EVENTLOG_HPP

#define _WIN32_WINNT 0x500
#include "handles.hpp"

namespace w32 
{
	namespace detail 
	{
		struct eventlog_create_params 
		{
			LPCWSTR unc_server;
		};

		inline HANDLE detail_create( LPCWSTR name, const eventlog_create_params& params )
		{
			return ::OpenEventLogW( params.unc_server, name );
		}
	}
	
	struct eventlog_holder : public w32::auto_handle< HANDLE, &::CloseEventLog, NULL >
	{
		typedef w32::auto_handle< HANDLE, &::CloseEventLog, NULL > base_type;

		eventlog_holder()
		{}

		explicit eventlog_holder( value_type hndl ) : base_type( hndl ) {}

		eventlog_holder( const eventlog_holder& evt ) : base_type( evt.value() ) {}		

		bool open( LPCWSTR name, LPCWSTR unc_server = NULL )
		{
			this->attach( 
				::OpenEventLogW( unc_server, name ) 
				);

			return this->valid();
		}

		/*
		bool register( LPCWSTR name, LPCWSTR )
		{
			this->attach( ::OpenSemaphoreA( acc, (inherit ? TRUE : FALSE), name ) );
			return this->valid();
		}
		//*/
	};

	struct registered_eventlog_holder : public w32::auto_handle< HANDLE, &::DeregisterEventSource, NULL >
	{
		typedef w32::auto_handle< HANDLE, &::DeregisterEventSource, NULL > base_type;

		registered_eventlog_holder()
		{}

		explicit registered_eventlog_holder( value_type hndl ) : base_type( hndl ) {}

		registered_eventlog_holder( LPCWSTR name ) : base_type( ::RegisterEventSourceW( NULL, name ) ) {}

		registered_eventlog_holder( const registered_eventlog_holder& evt ) : base_type( evt.value() ) {}		

			bool register_log( LPCWSTR name, LPCWSTR unc_server = NULL )
            {
                this->attach( 
                    ::RegisterEventSourceW( unc_server, name ) 
                    );

                return this->valid();
            }        

		bool report( LPCWSTR message, WORD type = EVENTLOG_INFORMATION_TYPE )
		{
			return !!::ReportEventW( this->value(), type, 0, 1, NULL, 1, 0, &message, NULL );
		}
	};
}

#endif /* HDR_EVENTLOG_HPP */ 
