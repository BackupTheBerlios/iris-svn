#include "profile.h"
#include <SDL/SDL.h>
#include <vector>

std::vector<unsigned int> gCallStack;
std::vector<unsigned int> gHistory;
bool gDoInit = true;

#define GET_TIMESTAMP ((unsigned int)SDL_GetTicks())

// #define PROFILE_CALLTIME // keeps a history of function calls, to do call-count and call-time profiling
// #define KEEP_HISTORY // keeps a history of function calls, to do call-count and call-time profiling

#ifdef PROFILE_CALLTIME
	#define CALLSTACK_ELEM_SIZE 4
#else
	#define CALLSTACK_ELEM_SIZE 3
#endif

cProfiler::cProfiler(const char* sFile,const int iLine,const char* sFunc) {
	if (gDoInit) {
		gCallStack.reserve(1024*32);
		#ifdef KEEP_HISTORY 
			gHistory.reserve(1024*1024*32);
		#endif
		gDoInit = false;
	}
	gCallStack.push_back((unsigned int)sFile);
	gCallStack.push_back((unsigned int)iLine);
	gCallStack.push_back((unsigned int)sFunc);
	#ifdef PROFILE_CALLTIME
		gCallStack.push_back(GET_TIMESTAMP);
	#endif
}

cProfiler::~cProfiler() {
	#ifdef KEEP_HISTORY 
		gHistory.push_back(gCallStack[0+gCallStack.size()-CALLSTACK_ELEM_SIZE]);
		gHistory.push_back(gCallStack[1+gCallStack.size()-CALLSTACK_ELEM_SIZE]);
		gHistory.push_back(gCallStack[2+gCallStack.size()-CALLSTACK_ELEM_SIZE]);
		#ifdef PROFILE_CALLTIME
			gHistory.push_back(GET_TIMESTAMP - gCallStack.back());
		#endif
	#endif
	gCallStack.pop_back();
	gCallStack.pop_back();
	gCallStack.pop_back();
	#ifdef PROFILE_CALLTIME
		gCallStack.pop_back();
	#endif
}

void		cProfiler::PrintStackTrace	() {
	for (int i=0;i<gCallStack.size();i+=CALLSTACK_ELEM_SIZE) {
		for (int j=0;j<i/CALLSTACK_ELEM_SIZE;++j) printf(" ");
		printf("%s:%d:%s\n",(const char*) gCallStack[i],
							(unsigned int)gCallStack[i+1],
							(const char*) gCallStack[i+2]);
	}
}





/*
old, obsolete
#include "profile.h"
#include <SDL/SDL.h> // for timing
#include <map>
#include <vector>
#include <string>


#ifndef ENABLE_PROFILING
	class cProfiledCodeBlock;
	class cProfiler { public:
		unsigned int			miTime;
		cProfiledCodeBlock*		mpBlock;
		
		// registers himself in a 
		cProfiler(const char* sFile,const unsigned int iLine,const char* sFunction); // save current time, GenProfileID
		~cProfiler(); // calc time since constructor, and register as one call somewhere in profiler.cpp
		static void		PrintCollectedInfo			(); // print info that has been collected somewhere in profiler.cpp , call at program exit ?
		static void		DebugPrintWithStackTrace	(const char* sMsg);
	};
#endif
	
	
/// it is better to komment out the "#define ENABLE_PROFILING" in profiler.h, but that requires a complete recompile
#define SKIP_PROFILER_SOFT 


class cProfiledCodeBlock { public:
	unsigned int	miTotalCalls; 
	unsigned int	miTotalTime; 
	unsigned int	miLine; 
	std::string		msFile;
	std::string		msFunction;
	
	cProfiledCodeBlock	(const char* sFile,const unsigned int iLine,const char* sFunction) :
		msFile(sFile), miLine(iLine), msFunction(sFunction), miTotalCalls(0), miTotalTime(0) {}
	
	void	PrintStackTracePart	(const int iLevel) {
		int i;
		for (i=0;i<iLevel;++i) printf(" "); // indent one space for each level
		//printf(":%d:%s\n",miLine,msFunction.c_str()); // print only line and function name (line to differentiate overloaded functions)
		printf("%s:%d:%s\n",msFile.c_str(),miLine,msFunction.c_str()); // print function name with file and line number
	}
			
	void	EndCall	(const unsigned int iTimeDiff) {
		miTotalTime += iTimeDiff;
		++miTotalCalls;
	}
};

typedef std::map<const char*,cProfiledCodeBlock* > 	tProfiledLine;
std::map<unsigned int, tProfiledLine* > 			glProfiledCodeBlock; // first index is line number, second index is code file, it's faster this way
std::vector<cProfiledCodeBlock*>					glCallStack;

cProfiledCodeBlock* GetOrCreateCodeBlock	(const char* sFile,const unsigned int iLine,const char* sFunction) {
	tProfiledLine*& line = glProfiledCodeBlock[iLine];
	if (!line) line = new tProfiledLine();
	cProfiledCodeBlock*& codeblock = (*line)[sFile];
	if (!codeblock) codeblock = new cProfiledCodeBlock(sFile,iLine,sFunction);
	return codeblock;
}


#ifdef SKIP_PROFILER_SOFT
	cProfiler::cProfiler(const char* sFile,const unsigned int iLine,const char* sFunction) {}
	cProfiler::~cProfiler() {}
#else
	/// the cProfiler is a temporary object, that lives in the scope of one codeblock
	/// as it is constructed when entering the codeblock and destructed when exiting,
	/// it is possible to measure the time spent within the codeblock and to maintain a callstack in runtime
	/// a macro helps getting the filename,linenumber and function name right
	cProfiler::cProfiler(const char* sFile,const unsigned int iLine,const char* sFunction) {
		mpBlock = GetOrCreateCodeBlock(sFile,iLine,sFunction);
		glCallStack.push_back(mpBlock);
		miTime = SDL_GetTicks();
	}
	cProfiler::~cProfiler() {
		mpBlock->EndCall(SDL_GetTicks() - miTime);
		glCallStack.pop_back();
	}
#endif
	
	
void	Profiler_PrintStackTrace		() {
	int iLevel=0;
	for (std::vector<cProfiledCodeBlock*>::iterator itor=glCallStack.begin();itor!=glCallStack.end();++itor) (*itor)->PrintStackTracePart(++iLevel);
}

void			cProfiler::PrintCollectedInfo			() {
	
}
void			cProfiler::DebugPrintWithStackTrace	(const char* sMsg) {
	Profiler_PrintStackTrace();
	printf("%s\n",sMsg);
}
*/
