#ifndef DEFINE_OS_H
#define DEFINE_OS_H

#if defined(__APPLE__) && defined(__GNUC__)  
#  define OS_MACX  
#elif defined(__MACOSX__)  
#  define OS_MACX  
#elif defined(macintosh)  
#  define OS_MAC9  
#elif defined(__CYGWIN__)  
#  define OS_CYGWIN  
#elif defined(MSDOS) || defined(_MSDOS)  
#  define OS_MSDOS  
#elif defined(__OS2__)  
#  if defined(__EMX__)  
#    define OS_OS2EMX  
#  else  
#    define OS_OS2  
#  endif  
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))  
#  define OS_WIN32  
#  define OS_WIN64  

#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))  
#  define OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)  
#  define OS_WIN32  
#elif defined(__sun) || defined(sun)	
#  define OS_SOLARIS  
#elif defined(hpux) || defined(__hpux)  
#  define OS_HPUX  
#elif defined(__ultrix) || defined(ultrix)  
#  define OS_ULTRIX  
#elif defined(sinix)  
#  define OS_RELIANT  
#elif defined(__linux__) || defined(__linux)  
#  define OS_LINUX  
#elif defined(__FreeBSD__)  
#  define OS_FREEBSD  
#  define OS_BSD4  
#elif defined(__NetBSD__)  
#  define OS_NETBSD  
#  define OS_BSD4  
#elif defined(__OpenBSD__)  
#  define OS_OPENBSD  
#  define OS_BSD4  
#elif defined(__bsdi__)  
#  define OS_BSDI  
#  define OS_BSD4  
#elif defined(__sgi)  
#  define OS_IRIX  
#elif defined(__osf__)  
#  define OS_OSF  
#elif defined(_AIX)  
#  define OS_AIX  
#elif defined(__Lynx__)  
#  define OS_LYNX  
#elif defined(__GNU_HURD__)  
#  define OS_HURD  
#elif defined(__DGUX__)  
#  define OS_DGUX  
#elif defined(__QNXNTO__)  
#  define OS_QNX6  
#elif defined(__QNX__)  
#  define OS_QNX  
#elif defined(_SEQUENT_)  
#  define OS_DYNIX  
#elif defined(_SCO_DS)                   /* SCO OpenServer 5 + GCC */  
#  define OS_SCO  
#elif defined(__USLC__)                  /* all SCO platforms + UDK or OUDK */  
#  define OS_UNIXWARE  
#  define OS_UNIXWARE7  
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */  
#  define OS_UNIXWARE  
#  define OS_UNIXWARE7  
#else  
#  error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"  
#endif  

#if defined(OS_MAC9) || defined(OS_MACX)  
#  define OS_MAC  
#endif  

#if defined(OS_MAC9) || defined(OS_MSDOS) || defined(OS_OS2) || defined(OS_WIN32) || defined(OS_WIN64)  
#  undef OS_UNIX  
#elif !defined(OS_UNIX)  
#  define OS_UNIX  
#endif

#endif