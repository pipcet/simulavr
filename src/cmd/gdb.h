/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002  Theodore A. Roth
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 *
 *  $Id$
 */


#ifndef SIM_GDB_H
#define SIM_GDB_H

#include "config.h"

#ifdef HAVE_SYS_MINGW
#   include <winsock2.h>
#   include <sys/types.h>
#else
#   include <sys/socket.h>
#   include <sys/types.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <arpa/inet.h>
#endif

#include <vector>
#include "avrdevice.h"
#include "types.h"
#include "simulationmember.h"

#define MAX_BUF 400 /* Maximum size of read/write buffers. */

// this are similar to unix signal numbers, but here used only as number, not
// as signal! See signum.h on unix systems for the values.
#define GDB_SIGHUP  1      // Hangup (POSIX).
#define GDB_SIGINT  2      // Interrupt (ANSI).
#define GDB_SIGILL  4      // Illegal instruction (ANSI).
#define GDB_SIGTRAP 5      // Trace trap (POSIX).

//! Interface for server socket wrapper
class GdbServerSocket {
    public:
        //GdbServerSocket(int port);
        virtual void Close(void)=0;
        virtual int ReadByte(void)=0;
        virtual void Write(const void* buf, size_t count)=0;
        virtual void SetBlockingMode(int mode)=0;
        virtual bool Connect(void)=0;
        virtual void CloseConnection(void)=0;
};

#ifdef HAVE_SYS_MINGW

//! Interface implementation for server socket wrapper on MingW systems (windows)
class GdbServerSocketMingW: public GdbServerSocket {
    
    private:
        static void Start();
        static void End();
        static int socketCount;
        SOCKET _socket;
        SOCKET _conn;
        
    public:
        GdbServerSocketMingW(int port);
        ~GdbServerSocketMingW();
        virtual void Close(void);
        virtual int ReadByte(void);
        virtual void Write(const void* buf, size_t count);
        virtual void SetBlockingMode(int mode);
        virtual bool Connect(void);
        virtual void CloseConnection(void);
};

#else

//! Interface implementation for server socket wrapper on unix systems
class GdbServerSocketUnix: public GdbServerSocket {
    private:
        int sock;       //!< the opened os-net-socket
        int conn;       //!< the real established connection
        struct sockaddr_in address[1];
        socklen_t          addrLength[1]; 

    public:
        GdbServerSocketUnix(int port);
        ~GdbServerSocketUnix();
        virtual void Close(void);
        virtual int ReadByte(void);
        virtual void Write(const void* buf, size_t count);
        virtual void SetBlockingMode(int mode);
        virtual bool Connect(void);
        virtual void CloseConnection(void);
};

#endif

//! GDB server instance to give the possibility to debug target by debugger
class GdbServer: public SimulationMember {
    
    protected: 
        static std::vector<GdbServer*> allGdbServers;
        AvrDevice *core;
        GdbServerSocket *server; //!< the server socket wrapper
        bool connState; //!< result of server->Connect()

        time_t oldTime;     //remember when we have tried to open a tcp connection
                            //last time. Only one try per second
 
        int global_debug_on;    //debugging the debugger
        int waitForGdbConnection;
        int runMode;
        //bool untilCoreStepFinished; //we must ever run until a complete step, taht is requred by
                                    //gdb, but we should never ask gdb for enything until the step really done
        bool lastCoreStepFinished;

        //old function local static vars, must move to class, no way to handle
        //method local static vars.
        char *last_reply;  //used in last_reply();
        char buf[MAX_BUF]; //used in send_reply();
        int block_on;      //used in pre_parse_packet();


        word avr_core_flash_read(int addr) ;
        void avr_core_flash_write(int addr, word val) ;
        void avr_core_flash_write_hi8( int addr, byte val) ;
        void avr_core_flash_write_lo8( int addr, byte val) ;
        void avr_core_remove_breakpoint(dword pc) ;
        void avr_core_insert_breakpoint(dword pc) ;
        int signal_has_occurred(int signo); 
        void signal_watch_start(int signo);
        void signal_watch_stop(int signo);
        int avr_core_step() ;
        int hex2nib( char hex );
        const char* gdb_last_reply( const char *reply );
        void gdb_send_ack( );
        void gdb_send_reply( const char *reply );
        void gdb_read_registers( );
        void gdb_write_registers(  char *pkt );
        int gdb_extract_hex_num( char **pkt, char stop );
        void gdb_read_register( char *pkt );
        void gdb_write_register( char *pkt );
        int gdb_get_addr_len( char *pkt, char a_end, char l_end, unsigned int *addr, int *len);
        void gdb_read_memory( char *pkt );
        void gdb_write_memory( char *pkt );
        void gdb_break_point( char *pkt );
        void gdb_continue( char *pkt );
        int gdb_get_signal(char *pkt);
        int gdb_parse_packet( char *pkt );
        int gdb_pre_parse_packet( int blocking );
        void gdb_main_loop(); 
        void gdb_interact( int port, int debug_on );
        void IdleStep();

    public:
        int Step(bool &trueHwStep, SystemClockOffset *timeToNextStepIn_ns=0) ;
        int InternalStep(bool &trueHwStep, SystemClockOffset *timeToNextStepIn_ns=0) ;
        void TryConnectGdb();
        void SendPosition(int signal); //send gdb the actual position where the simulation is stopped
        int SleepStep();
        GdbServer( AvrDevice*, int port, int debugOn, int WaitForGdbConnection=true);
        virtual ~GdbServer();
        void Run();      //helper, would be removed in the future
};

#endif /* SIM_GDB_H */
