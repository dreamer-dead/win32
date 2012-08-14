#if !defined HDR_W32_COMM_HPP
#define HDR_W32_COMM_HPP

// must be on the same directory level with
#include "file.hpp"

namespace w32 
{
	namespace detail 
	{		
		struct dcb : DCB 
		{			
			dcb( const TCHAR * str )			
			{
				//*
				::ZeroMemory( static_cast< LPDCB >(this), sizeof( DCB ) );
				this->DCBlength = sizeof( DCB );
				if ( !::BuildCommDCB( str, static_cast< LPDCB >(this) ) )
				{
					DWORD err = ::GetLastError();
					err;
				}
				//*/							
			}			
		};
	}

	template < class traits = base_handle_traits<HANDLE> >
	struct comm_holder : public file_holder< traits >
	{
		typedef file_holder< traits > base_type;
		typedef detail::file_create_params create_params;

		enum funcs 
		{
			func_clrdtr   = CLRDTR,		/* Clears the DTR (data-terminal-ready) signal. */
			func_clrrts   = CLRRTS,		/* Clears the RTS (request-to-send) signal. */
			func_setdtr   = SETDTR,		/* Sends the DTR (data-terminal-ready) signal. */
			func_setrts   = SETRTS,		/* Sends the RTS (request-to-send) signal. */
			func_setxoff  = SETXOFF,	/* Causes transmission to act as if an XOFF character has been received. */
			func_setxon   = SETXON,		/* Causes transmission to act as if an XON character has been received. */
			func_setbreak = SETBREAK,	/* Suspends character transmission and places the transmission line in a break state until the ClearCommBreak function is called (or EscapeCommFunction is called with the CLRBREAK extended function code). The SETBREAK extended function code is identical to the SetCommBreak function. Note that this extended function does not flush data that has not been transmitted. */
			func_clrbreak = CLRBREAK	/* Restores character transmission and places the transmission line in a nonbreak state. The CLRBREAK extended function code is identical to the ClearCommBreak function. */
		};

		enum masks
		{
			mask_break		= EV_BREAK,		/* A break was detected on input. */
			mask_cts		= EV_CTS,		/* The CTS (clear-to-send) signal changed state. */
			mask_dsr		= EV_DSR,		/* The DSR (data-set-ready) signal changed state. */
			mask_err		= EV_ERR,		/* A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. */
			mask_event1		= EV_EVENT1,	/* An event of the first provider-specific type occurred. */
			mask_event2		= EV_EVENT2,	/* An event of the second provider-specific type occurred. */
			mask_perr		= EV_PERR,		/* A printer error occurred. */
			mask_ring		= EV_RING,		/* A ring indicator was detected. */
			mask_rlsd		= EV_RLSD,		/* The RLSD (receive-line-signal-detect) signal changed state. */
			mask_rx80full	= EV_RX80FULL,	/* The receive buffer is 80 percent full. */
			mask_rxchar		= EV_RXCHAR,	/* A character was received and placed in the input buffer. */
			mask_rxflag		= EV_RXFLAG,	/* The event character was received and placed in the input buffer. The event character is specified in the device's DCB structure, which is applied to a serial port by using the SetCommState function. */
			mask_txempty	= EV_TXEMPTY	/* The last character in the output buffer was sent. */
		};

		enum modem_statuses
		{
			modem_status_cts_on  = MS_CTS_ON,	/* The CTS (clear-to-send) signal is on. */
			modem_status_dsr_on  = MS_DSR_ON,	/* The DSR (data-set-ready) signal is on. */
			modem_status_ring_on = MS_RING_ON,	/* The ring indicator signal is on. */
			modem_status_rlds_on = MS_RLSD_ON	/* The RLSD (receive-line-signal-detect) signal is on. */
		};

		enum purge_flags
		{
			purge_txabort = PURGE_TXABORT,	/* Terminates all outstanding overlapped write operations and returns immediately, even if the write operations have not been completed. */
			purge_rxabort = PURGE_RXABORT,	/* Terminates all outstanding overlapped read operations and returns immediately, even if the read operations have not been completed. */
			purge_txclear = PURGE_TXCLEAR,	/* Clears the output buffer (if the device driver has one). */
			purge_rxclear = PURGE_RXCLEAR	/* Clears the input buffer (if the device driver has one). */
		};

		using base_type::operator =;

		comm_holder()
		{}

		explicit comm_holder( const TCHAR * name ) : base_type( name )
		{			
		}

		comm_holder( const TCHAR * name, const create_params& params ) 
			: base_type( name, params )
		{			
		}

		explicit comm_holder( HANDLE hndl ) 
			: base_type( hndl )
		{			
		}

		comm_holder( const file_holder& other ) : base_type( other ) 
		{		
		}

		template < class traits_other >
		comm_holder( const file_holder< traits_other >& other ) : base_type( other ) 
		{		
		}		

		bool set_timeouts( LPCOMMTIMEOUTS times ) const
		{
			return FALSE != ::SetCommTimeouts( this->value(), times );
		}

		bool get_timeouts( LPCOMMTIMEOUTS times ) const
		{
			return FALSE != ::GetCommTimeouts( this->value(), times );
		}

		bool clear_break() const 
		{
			return FALSE != ::ClearCommBreak( this->value() );
		}

		bool clear_error( LPDWORD lpErrors, LPCOMSTAT lpState ) const 
		{
			return FALSE != ::ClearCommError( this->value(), lpErrors, lpState );
		}

		bool escape_function( DWORD comm_func )
		{
			return !!::EscapeCommFunction( this->value(), comm_func );
		}

		bool get_config( LPCOMMCONFIG lpConf, LPDWORD lpSize ) const 
		{
			return !!::GetCommConfig( this->value(), lpConf, lpSize );
		}

		bool get_mask( LPDWORD lpMask ) const
		{
			return !!::GetCommMask( this->value(), lpMask );
		}

		bool set_mask( DWORD dwMask ) const
		{
			return !!::SetCommMask( this->value(), dwMask );
		}

		bool get_modem_status( LPDWORD lpStatus ) const
		{
			return !!::GetCommModemStatus( this->value(), lpStatus );
		}				

		bool get_properties( LPCOMMPROP props ) const 
		{
			return !!::GetCommProperties( this->value(), props );
		}

		bool get_state( LPDCB dcb ) const
		{
			return !!::GetCommState( this->value(), dcb );
		}

		bool set_state( LPDCB dcb ) const
		{
			return !!::SetCommState( this->value(), dcb );
		}

		bool purge( DWORD dwFlags ) const
		{
			return !!::PurgeComm( this->value(), dwFlags );
		}

		bool transmit_char( CHAR ch ) const
		{
			return !!::TransmitCommChar( this->value(), ch );
		}

		bool wait_event( LPDWORD lpMask, LPOVERLAPPED lpover ) const
		{
			return !!::WaitCommEvent( this->value(), lpMask, lpover );
		}

	public :
		
		inline masks mask() const 
		{
			DWORD dwMask = 0;
			this->get_mask( &dwMask );

			return (masks)dwMask;
		}

		inline comm_holder& mask( masks mask_ ) 
		{
			this->set_mask( static_cast<DWORD>(mask_) );
			return *this;
		}

		inline void mask( masks mask_ ) const
		{
			this->set_mask( static_cast<DWORD>(mask_) );
		}

		inline comm_holder& escape_function( funcs func_ ) 
		{
			this->escape_function( static_cast<DWORD>(func_) );
			return *this;
		}

		inline void escape_function( funcs func_ ) const
		{
			this->escape_function( static_cast<DWORD>(func_) );
		}

		inline modem_statuses modem_status() const 
		{
			DWORD dwStatus = 0;
			this->get_modem_status( &dwStatus );

			return (modem_statuses)dwStatus;
		}

		inline comm_holder& purge( purge_flags purge_ ) 
		{
			this->purge( static_cast<DWORD>(purge_) );
			return *this;
		}

		inline void purge( purge_flags purge_ ) const
		{
			this->purge( static_cast<DWORD>(purge_) );
		}
	};
}

#endif /* HDR_W32_COMM_HPP */
