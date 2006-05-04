#ifndef TIMER_H
#define TIMER_H

#include "listener.h"
#include <set>
#include <list>
#include <vector>

/// manages timing in applications using frames.
/// similar to cListenable
/// all times are in milliseconds (1000=1second)
/// iFrameInterval is for calling every n frames, must be a power of two (1,2,4,8,..)  
/// RegisterFrameIntervalListener takes an EXPONENT, i.e. passing 3 as iFrameIntervalExp will result in getting called every 2^3 = 1<<3 = 8 frames
/// frame intervals are distributed a bit to avoid bumps in the framerate, e.g. 2 and 4 will never occur on the same frame
class cTimer { public :
	size_t		miTimeSinceLastFrame;
	size_t		miLastFrameTime;
	size_t		miCurFrameNum;
	
	/// eventcodes for cListener
	enum {
		kListenerEvent_Timeout,
		kListenerEvent_Interval,
		kListenerEvent_FrameInterval
	};
	enum { kMaxFrameIntervalExp = 24 }; // a callback every 1<<24 frames (more than 3 days at 60fps) should be enough, use timeout if you really need that...
	
	
	static cTimer*	GetSingletonPtr	();
	
			cTimer	(const size_t iTime);
	virtual	~cTimer	();
	
	void	StartFrame	(const size_t iTime); 
	
	/// commonly used by Timeout, Interval and FrameInterval
	class cTimerRegistration { public:
		size_t					miTime;
		size_t					miInterval; /// =0 for timeouts and frameintervals
		size_t					miFrameInterval; /// =0 for timeouts and intervals
		size_t					miIntervalCount; /// incremented by one on each hit
		cSmartPtr< cListener >	mpListener;
		size_t					miUserData;
		bool					mbIsAlive;
		
		cTimerRegistration	(cListener* pListener,const size_t iUserData,const size_t iTime,const size_t iInterval=0,const size_t iFrameInterval=0) :
			mpListener(pListener), miUserData(iUserData), miTime(iTime), miInterval(iInterval), miFrameInterval(iFrameInterval), miIntervalCount(0), mbIsAlive(true) {
			//printf("\n\ncTimerRegistration(pListener=%#08x,iUserData=%d,iTime=%d,iInterval=%d,iFrameInterval=%d)\n\n",pListener,iUserData,iTime,iInterval,iFrameInterval);
		}
		
		/// returns false if this registration should be removed
		bool			Trigger		(const size_t iCurTime,const size_t iEvent);
		inline	void	Cancel		() { mbIsAlive = false; }
	};
	
	// registration
	cTimerRegistration*	  RegisterTimeoutListener		(cListener* pListener,const size_t iTimeOut,			const size_t userdata = 0);
	cTimerRegistration*	  RegisterIntervalListener		(cListener* pListener,const size_t iInterval,			const size_t userdata = 0);
	cTimerRegistration*	  RegisterFrameIntervalListener	(cListener* pListener,const size_t iFrameIntervalExp,	const size_t userdata = 0);
	
	// FrameInterval timing
	inline	bool	IsCurFrameInInterval		(const size_t iInterval) { return IsFrameInInterval(iInterval,miCurFrameNum); }
	static inline	size_t	GetIntervalStart	(const size_t iInterval) { return (iInterval-1)/2; }
	static inline	bool	IsFrameInInterval	(const size_t iInterval,const size_t iFrame) {
		return ((iFrame+iInterval-GetIntervalStart(iInterval)) % iInterval) == 0;
	}
	
	// removal of registrations is done only during iteration, so no search is required, the timeouts are sorted so iteration is short
	struct cTimerRegistrationCompare { bool operator()(cTimerRegistration* a, cTimerRegistration* b) { return a->miTime < b->miTime; } };
	std::multiset< 			cTimerRegistration*,cTimerRegistrationCompare>	mlTimeouts;
	std::list<				cTimerRegistration*>							mlIntervals;
	std::vector< std::list<	cTimerRegistration*>* >							mlFrameIntervals;
};

#endif
