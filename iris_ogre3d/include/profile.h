#ifndef _PROFILE_H_
#define _PROFILE_H_


#define ENABLE_PROFILING

#ifdef ENABLE_PROFILING
	// for calltime and callcount profiling and runtime callstacks/backtraces
		
	class cProfiler { public:
		cProfiler(const char* sFile,const int iLine,const char* sFunc); // save current time
		~cProfiler(); // calc time since constructor
		
		static void		PrintStackTrace	();
	};
	
	/// put this macro at the beginning of a function
	#define	PROFILE		cProfiler local_profiler(__FILE__,__LINE__,__FUNCTION__);
	#define	PROFILE_PRINT_STACKTRACE cProfiler::PrintStackTrace();
	#define	DEBUG_PRINT_WITH_STACKTRACE(msg)	{ PROFILE_PRINT_STACKTRACE printf("%s\n",msg); }
#else 
	#define	PROFILE 
	#define	PROFILE_PRINT_STACKTRACE
	#define	DEBUG_PRINT_WITH_STACKTRACE(msg) 
#endif
	
	
/* ... old ... obsolete ... 
//#define ENABLE_PROFILING

#ifdef ENABLE_PROFILING
	// TODO : ghoulsblade : this is just a note how a profiler could be done, 
	// i don't have the time to implement it right now, the thought just crossed my mind, 
	// and i wanted to write it down
	// should also be possible to write callstacks with this

	class cProfiledCodeBlock; // details defined only locally in profiler.cpp
		
	class cProfiler { public:
		unsigned int			miTime;
		cProfiledCodeBlock*		mpBlock;
		
		// registers himself in a 
		cProfiler(const char* sFile,const unsigned int iLine,const char* sFunction); // save current time, GenProfileID
		~cProfiler(); // calc time since constructor, and register as one call somewhere in profiler.cpp
		static void		PrintCollectedInfo			(); // print info that has been collected somewhere in profiler.cpp , call at program exit ?
		static void		DebugPrintWithStackTrace	(const char* sMsg);
	};
	
	/// put this macro at the beginning of a function
	#define	PROFILE		cProfiler local_profiler(__FILE__,__LINE__,__FUNCTION__);
	
	/// call at program exit or so
	#define	PRINT_COLLECTED_PROFILE_INFO		cProfiler::PrintCollectedInfo();
	#define	DEBUG_PRINT_WITH_STACKTRACE(msg)	cProfiler::DebugPrintWithStackTrace(msg);
#else 
	#define	PROFILE	
	#define	PRINT_COLLECTED_PROFILE_INFO
	#define	DEBUG_PRINT_WITH_STACKTRACE(msg) 
#endif
*/
	
#endif
