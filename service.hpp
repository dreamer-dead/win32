/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#if !defined HDR_AUTOGRAPH_SERVICE_HPP
#define HDR_AUTOGRAPH_SERVICE_HPP

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

class service_base
{	
	SERVICE_STATUS_HANDLE status_handle_;
	SERVICE_STATUS status_;
	TCHAR name_[256];	

protected :
	virtual ~service_base() {}

public:
	static bool install( const TCHAR * command, const TCHAR * name, bool silent = false );
	static bool uninstall( const TCHAR * name, bool silent = false );

	service_base( const TCHAR * service_name );

	DWORD start( LPSERVICE_MAIN_FUNCTION service_main_proc );
	bool register_ctrl_handler( LPHANDLER_FUNCTION_EX f );
	DWORD set_status( DWORD status, bool flush = true );
	DWORD status() const;	
};

template < typename T >
class service : public service_base 
{
	static DWORD __stdcall service_handler_proc(DWORD control, DWORD eventType, void * eventData, void * ctx)
	{
		service_base * ptr_this = (service_base *)ctx;
		if ( !ptr_this )
			return ERROR_INVALID_PARAMETER;

		return static_cast< service<T> * >(ptr_this)->control_handler( control, eventType, eventData );
	}
	
	DWORD control_handler(DWORD control, DWORD eventType, void * eventData) 
	{ 
		return static_cast<T*>(this)->handler( control, eventType, eventData ); 
	}

public :

	service( const TCHAR * service_name ) : service_base(service_name) {}

	bool register_ctrl_handler()
	{
		return service_base::register_ctrl_handler( &service_handler_proc );
	}	
};

#endif /* HDR_AUTOGRAPH_SERVICE_HPP */
