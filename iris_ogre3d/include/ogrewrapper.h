#ifndef _OGREWRAPPER_H_
#define _OGREWRAPPER_H_
#include <OgrePrerequisites.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreUserDefinedObject.h>
/// cOgreWrapper by ghoulsblade

class cTimer;
	
class cMapblock3D;
class cDynamicObject;
class sStaticObject;
class cCharacter;
class cOgreUserObjectWrapper : public Ogre::UserDefinedObject { public :
	cMapblock3D*		mpMapBlock3D;
	cDynamicObject*		mpDynamicObject;
	sStaticObject*		mpStaticObject;
	cCharacter*			mpCharacter;
	Ogre::Entity*		mpEntity;
	
	cOgreUserObjectWrapper();
	virtual ~cOgreUserObjectWrapper();
	virtual long getTypeID(void) const;
	virtual const Ogre::String& getTypeName(void) const;
};

class cOgreWrapper { public :
	bool					mbAlive;
    Ogre::Root*				mRoot;
    Ogre::Camera*			mCamera;
    Ogre::Viewport*			mViewport;
    Ogre::SceneManager*		mSceneMgr;
    Ogre::RenderWindow*		mWindow;
    Ogre::InputReader*		mInputReader;
    Ogre::EventProcessor*	mEventProcessor;
    Ogre::EventQueue*		mEventQueue;
	Ogre::Overlay*			mpOverlayGUI;
	Ogre::Overlay*			mpOverlayCursor;
	Ogre::RaySceneQuery*	mpRaySceneQuery;
	cTimer*					mpTimer;
	size_t					miObjectCounter;
	static size_t			miFrameNum;
	
	cOgreWrapper();
	
	// cOgreWrapper::GetSingleton().
	inline static cOgreWrapper& GetSingleton () { 
		static cOgreWrapper* mSingleton = 0;
		if (!mSingleton) mSingleton = new cOgreWrapper();
		return *mSingleton;
	}
	
	/// returns true on success
	bool	Init			(); 
	void	DrawOneFrame	();
	void	DeInit			();
	void	TakeScreenshot	();
	
	void	AttachCamera	(Ogre::SceneNode* pSceneNode=0);
	void	SetCameraPos	(const Ogre::Vector3 vPos=Ogre::Vector3::ZERO);
	void	SetCameraRot	(const Ogre::Quaternion qRot=Ogre::Quaternion::IDENTITY);
	void	CameraLookAt	(const Ogre::Vector3 vPos=Ogre::Vector3::ZERO);
	
	// utils
	static std::string 		GetUniqueName	();
	
	// GUI
	void	GUI_SetOrigin	(const int posx,const int posy);
	
	// Ray/Intersection Queries
	bool	RayEntityQuery	(const Ogre::Ray* pRay,Ogre::Entity* pEntity,float* pfHitDist=0);
};

class MeshShape	{ public :
	std::vector<Ogre::Vector3>	mlVertices;
	std::vector<int>			mlIndices;
	MeshShape();
	MeshShape(Ogre::Entity *entity);
	~MeshShape();
	void		Update			(Ogre::Entity *entity);
	static bool	IsAnimated		(Ogre::Entity *entity);
	bool		RayIntersect	(const Ogre::Vector3& ray_origin,const Ogre::Vector3& ray_dir,float* pfHitDist=0);
};

#endif
