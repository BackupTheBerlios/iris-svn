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
