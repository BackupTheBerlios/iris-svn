#include "smartptr.h"

#include "profile.h"

cListenable::cListenable() {}
cListenable::~cListenable() {}
	
	
void	cListenable::NotifyAllListeners	(const size_t eventcode,const size_t param) {PROFILE
	cListener*	pListener;
	std::list< std::pair<cSmartPtr<cListener>*,size_t> >::iterator		itor,itor_temp;
	for (itor = mlListener.begin();itor != mlListener.end();) {
		itor_temp = itor; ++itor; // now it doesn't matter if the iterator itself is destroyed
		pListener = **(*itor_temp).first;
		if (pListener) {
			pListener->Listener_Notify(this,eventcode,param,(*itor_temp).second);
		} else {
			// remove dead cListener
			delete	(*itor_temp).first; 
					(*itor_temp).first = 0;
			mlListener.erase(itor_temp);
		}
	}
}

/// if you register twice, you also have to unregister twice
void	cListenable::RegisterListener	(cListener* pListener,const size_t userdata) {PROFILE 
	assert(pListener); 
	mlListener.push_front( std::pair< cSmartPtr<cListener>*,size_t>( new cSmartPtr<cListener>(pListener) , userdata) ); 
}

/// if you registered twice, you also have to unregister twice (or just die)
/// listeners don't have to unregister when they are destroyed, cSmartPtr takes care of that (if you call your destructors correctly, MAKE THEM VIRTUAL!!!)...
/// userdata HAS to be exactly the same as when registering
void	cListenable::UnRegisterListener	(cListener* pListener,const size_t userdata) {PROFILE
	assert(pListener); 
	std::list< std::pair<cSmartPtr<cListener>*,size_t> >::iterator itor;
	for (itor = mlListener.begin();itor != mlListener.end();itor++) {
		if (pListener == **(*itor).first && (*itor).second == userdata) {
			// remove dead cListener in a safe way
			delete	(*itor).first; 
					(*itor).first = 0;
			mlListener.erase(itor);
			return;
		}
	}
}
