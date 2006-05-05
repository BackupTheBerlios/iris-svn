#ifndef _OGREWRAPPER_H_
#define _OGREWRAPPER_H_
#include <OgrePrerequisites.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreUserDefinedObject.h>
#include <vector>
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

class cOgreGrannyWrapper { public:
	enum eMatrixType {
		GL_MODELVIEW_MATRIX
	};
	cOgreGrannyWrapper();
	virtual ~cOgreGrannyWrapper();
	virtual void	glPushMatrix	() = 0;
	virtual void	glPopMatrix		() = 0;
	virtual void	glLoadIdentity	() = 0;
	virtual void	glLoadMatrixf	(const float* m) = 0;
	virtual void	glMultMatrixf	(const float* m) = 0;
	virtual void	glGetFloatv		(const eMatrixType type,float* buf) = 0;
	virtual void	glScalef		(const float x,const float y,const float z) = 0;
	virtual void	glTranslatef	(const float x,const float y,const float z) = 0;
	virtual void	glRotatef		(const float ang,const float x,const float y,const float z) = 0;
	virtual void	glColor3f		(const float r,const float g,const float b) = 0;
	virtual void	glColor4f		(const float r,const float g,const float b,const float a) = 0;
	virtual void	glTexCoord2fv	(const float* buf) = 0;
	virtual void	glVertex3fv		(const float* buf) = 0;
	virtual void	glBegin			(const char* sMat,const size_t iNumPolys) = 0;
	virtual void	glEnd			() = 0;
	virtual void	DrawStep		(const bool bVisible,const float x,const float y,const float z) = 0;
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

	cOgreGrannyWrapper* CreateOgreGrannyWrapper();
	
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
