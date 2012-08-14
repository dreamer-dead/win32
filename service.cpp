#include "service.hpp"
#include "handles.hpp"
//#include "win32/event_log.hpp"

typedef w32::auto_handle< SC_HANDLE, &::CloseServiceHandle, NULL > service_handle_t;

//w32::registered_eventlog_holder g_service_eventlog( L"System" );

service_base::service_base( const TCHAR * service_name )
{
	SERVICE_STATUS default_status = {
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_START,
		SERVICE_ACCEPT_PARAMCHANGE | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP,
		NO_ERROR, 0, 0, 0
	};

	status_ = default_status;
	_tcsncpy( name_, service_name, sizeof(name_) / sizeof(name_[0]) );
}

bool service_base::install( const TCHAR * command, const TCHAR * name, bool silent ) 
{
	bool result = false;
	{
		service_handle_t manager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		service_handle_t srv = ::CreateService(manager.value(), name, name, 0,
			SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
			command, NULL, NULL, NULL, NULL, NULL);

		result = (!!srv);
	}

	if ( !silent )
	{
		if (result) 
		{
			::MessageBoxW(NULL, L"Служба успешно установлена", L"Внимание", MB_OK | MB_ICONINFORMATION);
		} 
		else 
		{
			switch ( ::GetLastError() ) 
			{
				case ERROR_SERVICE_EXISTS:
					::MessageBoxW(NULL, L"Служба уже установлена", L"Ошибка!", MB_OK | MB_ICONERROR);
					break;
				case ERROR_DUPLICATE_SERVICE_NAME:
					::MessageBoxW(NULL, L"Служба с таким именем уже установлена", L"Ошибка!", MB_OK | MB_ICONERROR);
					break;
				default:
					::MessageBoxW(NULL, L"У вас нет прав для выполнения операции", L"Ошибка!", MB_OK | MB_ICONERROR);
					break;
			}
		}
	}

	return result;
}

bool service_base::uninstall( const TCHAR * name, bool silent ) 
{
	bool result = false;
	{
		service_handle_t manager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		service_handle_t srv = ::OpenService(manager.value(), name, DELETE);

		if ( !!srv )
			result = !!::DeleteService( srv.value() );
	}		

	if ( !silent )
	{
		if (result) 
		{
			::MessageBoxW(NULL, L"Служба успешно удалена", L"Внимание", MB_OK | MB_ICONINFORMATION);
		} else 
		{
			switch ( ::GetLastError() ) 
			{
					case ERROR_SERVICE_MARKED_FOR_DELETE:
						::MessageBoxW(NULL, 
							L"Служба уже помечена для удаления. Перезагрузите компьютер", 
							L"Внимание", MB_OK | MB_ICONINFORMATION);
						break;
					default:
						::MessageBoxW(NULL, 
							L"У вас нет прав для выполнения операции", 
							L"Ошибка!", MB_OK | MB_ICONERROR);
			}
		}
	}

	return result;
}

DWORD service_base::start( LPSERVICE_MAIN_FUNCTION service_main_proc ) 
{
	SERVICE_TABLE_ENTRY services[] = {
		{ name_, service_main_proc },
		{NULL, NULL}
	};

	return ::StartServiceCtrlDispatcher( services );
}

bool service_base::register_ctrl_handler( LPHANDLER_FUNCTION_EX f )
{
	this->status_handle_ = ::RegisterServiceCtrlHandlerEx( this->name_, f, this );

	return ( NULL != this->status_handle_ );
}

DWORD service_base::set_status( DWORD status, bool flush )
{
	DWORD old = this->status_.dwCurrentState;

	this->status_.dwCurrentState = status;

	/*
	switch( status )
	{
	case SERVICE_RUNNING: g_service_eventlog.report( L"service status set to SERVICE_RUNNING" ); break;
	case SERVICE_START_PENDING: g_service_eventlog.report( L"service status set to SERVICE_START_PENDING" ); break;
	case SERVICE_PAUSED: g_service_eventlog.report( L"service status set to SERVICE_PAUSED" ); break;
	case SERVICE_STOPPED: g_service_eventlog.report( L"service status set to SERVICE_STOPPED" ); break;
	case SERVICE_INTERROGATE: g_service_eventlog.report( L"service status set to SERVICE_INTERROGATE" ); break;
	}
	//*/

	if ( flush )
	{
		if ( status == SERVICE_START_PENDING )
			this->status_.dwControlsAccepted = 0;
		else 
			this->status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;

		if ( (status == SERVICE_RUNNING) || (status == SERVICE_STOPPED) )
			this->status_.dwCheckPoint = 0;
		else 
			this->status_.dwCheckPoint++;

		this->status_.dwWin32ExitCode = NO_ERROR;
		this->status_.dwWaitHint = 0;
		::SetServiceStatus(this->status_handle_, &this->status_ );
	}

	return old;
}

DWORD service_base::status() const 
{
	return this->status_.dwCurrentState;
}

