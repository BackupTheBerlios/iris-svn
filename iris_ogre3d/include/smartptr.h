#ifndef SMARTPTR_H
#define SMARTPTR_H

// #include "listener.h"
#include <list>
#include <set>
#include <assert.h>

class cListener;
template<class _T> class cSmartPtr;
	
class cListenable { public:
			 cListenable();
	virtual ~cListenable();
	void	NotifyAllListeners	(const size_t eventcode = 0,const size_t param = 0);
	void	RegisterListener	(cListener* pListener,const size_t userdata = 0);
	void	UnRegisterListener	(cListener* pListener,const size_t userdata = 0);
	
	private:
	std::list< std::pair< cSmartPtr< cListener > *,size_t > >	mlListener;
};

/// common interface to avoid template problems in cSmartPointable
class cISmartPtr { public:
	virtual	void	SmartPtr_TargetDestroyed () = 0;
};

// cSmartPtr is cListenable for target destroyed event
template<class _T> class cSmartPtr : public cISmartPtr, public cListenable { public:
	//friend class cSmartPointable;
	
	enum { kDefaultTargetDestroyedEventCode = 255 };
	size_t	miTargetDestroyedEventCode;
		
			 cSmartPtr(_T* target=0,const size_t iTargetDestroyedEventCode=kDefaultTargetDestroyedEventCode)
							: target(target), miTargetDestroyedEventCode(iTargetDestroyedEventCode)
							{ if (target) target->RegisterSmartPtr(this); }
							
	virtual ~cSmartPtr()	{ if (target) target->UnRegisterSmartPtr(this); target = 0; }
	void	SmartPtr_SetTarget (_T* newtarget) {
		if (target) target->UnRegisterSmartPtr(this);
		target = newtarget;
		if (target) target->RegisterSmartPtr(this);
	}
	inline const cSmartPtr<_T>& operator = (cSmartPtr<_T> othersmartptr)	{ SmartPtr_SetTarget(*othersmartptr);	return *this; }
	inline const cSmartPtr<_T>& operator = (_T* newtarget) 					{ SmartPtr_SetTarget(newtarget);		return *this; }
	inline _T* operator * ()			{ return target; }
	
	/// only called from cSmartPointable's destructor
	/// no need to unregister here, would even break iterator in cSmartPointable's destructor
	void	SmartPtr_TargetDestroyed ()	{ NotifyAllListeners(miTargetDestroyedEventCode); target = 0; }
	
	private:
	_T*	target; 
};

// cSmartPointable is cListenable for death event
class cSmartPointable : public cListenable { public:
	
	enum { kDefaultDeathEventCode = 255 };
	size_t	miDeathEventCode;
	
			 cSmartPointable(const size_t iDeathEventCode=kDefaultDeathEventCode) : miDeathEventCode(iDeathEventCode) {}
	virtual ~cSmartPointable() { ReleaseAllSmartPtr(); NotifyAllListeners(miDeathEventCode); }
	void	ReleaseAllSmartPtr	() { 
		// foreach mlPtr
		for (std::set<cISmartPtr*>::iterator itor = mlPtr.begin();itor != mlPtr.end();) {
			 std::set<cISmartPtr*>::iterator itor_temp = itor;++itor; // now it doesn't matter if the iterator itself is destroyed
			(*itor_temp)->SmartPtr_TargetDestroyed();
		}
		mlPtr.clear();
	}
	void	RegisterSmartPtr	(cISmartPtr* ptr) { assert(ptr); mlPtr.insert(ptr); }
	void	UnRegisterSmartPtr	(cISmartPtr* ptr) { assert(ptr); mlPtr.erase(ptr); }
	
	private:
	std::set<cISmartPtr*>	mlPtr;
};
	
class cListener : public cSmartPointable { public: // cSmartPointable might also become listenable
	virtual void Listener_Notify (cListenable* pTarget,const size_t eventcode = 0,const size_t param = 0,const size_t userdata = 0) = 0;
};

#endif
