#include "ogrewrapper.h"
#include "robinput.h"
#include "robstring1.2.h"
#include "timer.h"
#include <Ogre.h>
#include <OgreInput.h>
#include <OgreInputEvent.h>
#include <OgreEventListeners.h>
#include <OgreKeyEvent.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <SDL/SDL.h>
#include <map>
#include "profile.h"

using namespace Ogre;

bool	IntersectRayTriangle	(const Vector3& ray_origin,const Vector3& ray_dir,const Vector3& a,const Vector3& b,const Vector3& c,float* pfHitDist=0,float* pfABC=0);

	
#define NUM_SHADOW_TECH 5
String mShadowTechDescriptions[NUM_SHADOW_TECH] = 
{
    "Stencil Shadows (Additive)",
    "Stencil Shadows (Modulative)",
	"Texture Shadows (Additive)",
    "Texture Shadows (Modulative)",
    "None"
};
ShadowTechnique mShadowTech[NUM_SHADOW_TECH] = 
{
    SHADOWTYPE_STENCIL_ADDITIVE,
    SHADOWTYPE_STENCIL_MODULATIVE,
	SHADOWTYPE_TEXTURE_ADDITIVE,
    SHADOWTYPE_TEXTURE_MODULATIVE,
    SHADOWTYPE_NONE
};
int mCurrentShadowTechnique = NUM_SHADOW_TECH-1;

Light*  gMainLight;
Ogre::PanelOverlayElement* 		mpPanel;
Ogre::TextAreaOverlayElement* 	mpText;
size_t cOgreWrapper::miFrameNum = 0;

cOgreUserObjectWrapper::cOgreUserObjectWrapper() : mpMapBlock3D(0), mpDynamicObject(0), mpStaticObject(0), mpCharacter(0), mpEntity(0) {}
cOgreUserObjectWrapper::~cOgreUserObjectWrapper() {}
long cOgreUserObjectWrapper::getTypeID(void) const { return 23; }
const Ogre::String& cOgreUserObjectWrapper::getTypeName(void) const { static Ogre::String eris("shiva"); return eris; }


cOgreWrapper::cOgreWrapper() : mbAlive(true), mRoot(0), mpTimer(0) {PROFILE
    mCamera = 0;
    mViewport = 0;
    mSceneMgr = 0;
    mWindow = 0;
    mInputReader = 0;
    mEventProcessor = 0;
    mEventQueue = 0;
	mpOverlayGUI = 0;
	mpOverlayCursor = 0;
	miObjectCounter = 0;
}

bool	cOgreWrapper::Init			() {PROFILE
	mpTimer = new cTimer(SDL_GetTicks());
	
	mRoot = new Root();

	//setupResources();
	{
        // Load resource paths from config file
        ConfigFile cf;
        cf.load("resources.cfg");

        // Go through all sections & settings in the file
        ConfigFile::SectionIterator seci = cf.getSectionIterator();

        String secName, typeName, archName;
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            ConfigFile::SettingsMultiMap *settings = seci.getNext();
            ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
                ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
            }
        }
	}
	
	//bool carryOn = configure();
	//if (!carryOn) return false;
	if (!mRoot->restoreConfig() && !mRoot->showConfigDialog()) return false;
	mWindow = mRoot->initialise(true,"Iris with Ogre");
	
	printf("\n\n Ogre Root-Init Successful\n\n");
	
	class cMyInputListener : public Ogre::KeyListener { public:
		cInput& input;
		cMyInputListener() : input(cInput::GetSingleton()) {}
		virtual void 	keyClicked (KeyEvent *e) 			{ }
		virtual void 	keyPressed (KeyEvent *e)			{
			input.KeyDown(input.KeyConvertOgre(e->getKey()),e->getKeyChar()); 
			//CEGUI::System::getSingleton().injectKeyDown(e->getKey());
			//CEGUI::System::getSingleton().injectChar(e->getKeyChar());
			if (e->getKey() == Ogre::KC_ESCAPE) cOgreWrapper::GetSingleton().mbAlive = false;
		}
		virtual void 	keyReleased		(KeyEvent *e)	{ 
			input.KeyUp( input.KeyConvertOgre(e->getKey()));
			//CEGUI::System::getSingleton().injectKeyUp(e->getKey());
		}
		virtual void 	keyFocusIn		(KeyEvent *e)	{ printf("keyFocusIn\n"); input.FocusChange(true); }
		virtual void 	keyFocusOut		(KeyEvent *e)	{ printf("keyFocusOut\n"); input.FocusChange(false); }
	};
	
	mEventProcessor = new EventProcessor();
	mEventProcessor->initialise(mWindow); // hungry ogre eats the mouse when using sdl...
	mEventProcessor->startProcessingEvents();
	
	cMyInputListener* myInputListener = new cMyInputListener();
	mEventProcessor->addKeyListener(myInputListener);
	
	mInputReader = mEventProcessor->getInputReader(); // buffered, not really needed, better use listeners
	mInputReader->setBufferedInput(true,false); // need absolute mouse pos, not some weirdly scaled relative move events
	// MOUSEWHEEL is only supported in unbuffered mode for SDL anyway
	
	printf("\n\n Ogre Event-Init Successful\n\n");
	
	//chooseSceneManager();
	//for ogre 1.0
	//mSceneMgr = mRoot->getSceneManager(ST_GENERIC);
	//for ogre 1.2
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC,"main");
	
	printf("\n\n Ogre SceneManager-Init Successful\n\n");
	
	//createCamera();
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		// Position it at 500 in Z direction
		mCamera->setPosition(Vector3(-8,-8,40));
		// Look back along -Z
		mCamera->lookAt(Vector3(-8,-8,0));
		mCamera->setNearClipDistance(1);
		//mCamera->setPolygonMode(PM_WIREFRAME);
	}
	
	printf("\n\n Ogre Camera-Init Successful\n\n");
		
	// Create one viewport, entire window
	mViewport = mWindow->addViewport(mCamera);
	mViewport->setBackgroundColour(ColourValue(0,0,0));
	
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));
	
	
	printf("\n\n Ogre Viewport-Init Successful\n\n");
	
	
	if (0) { // mbRttHack
		// render to texture hack, required for hagish's weird gfx-setup only =)
		RenderTexture* rttTex = mRoot->getRenderSystem()->createRenderTexture( "RttTex", 512, 512, TEX_TYPE_2D, PF_R8G8B8 );
		Viewport* vp = rttTex->addViewport( mCamera );
		vp->setOverlaysEnabled( false );
		vp->setClearEveryFrame( true );
		vp->setBackgroundColour( ColourValue::Black );
		
		printf("\n\n Ogre RTT-HACK-Init Successful\n\n");
	}
	
	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	/*
	mFiltering = TFO_TRILINEAR; mAniso = 1;
	mFiltering = TFO_ANISOTROPIC; mAniso = 8;
	mFiltering = TFO_BILINEAR; mAniso = 1;
	MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
	MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);
	*/
		
	// TODO : Create any resource listeners (for loading screens)
	
	// Load resources
	// loadResources();
	// Initialise, parse scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// printf("\n\n Ogre Ressource-Init Successful\n\n");
	
	mpOverlayGUI = OverlayManager::getSingleton().create("mpOverlayGUI");
	mpOverlayGUI->show();
	mpOverlayCursor = OverlayManager::getSingleton().create("mpOverlayCursor");
	mpOverlayCursor->show();
	
	mpPanel = static_cast<PanelOverlayElement*>(OverlayManager::getSingleton().createOverlayElement("Panel",GetUniqueName()));
	mpText = static_cast<TextAreaOverlayElement*>(OverlayManager::getSingleton().createOverlayElement("TextArea", GetUniqueName()));
	
	mpPanel->setMetricsMode(Ogre::GMM_PIXELS);
	mpPanel->setDimensions(100,50);
	mpPanel->setPosition(-100,0);
	mpPanel->setHorizontalAlignment(Ogre::GHA_RIGHT);
	mpPanel->setVerticalAlignment(Ogre::GVA_TOP);
	
	mpText->setMetricsMode(Ogre::GMM_PIXELS);
	mpText->setDimensions(100,50);
	mpText->setHorizontalAlignment(Ogre::GHA_LEFT);
	mpText->setVerticalAlignment(Ogre::GVA_TOP);
	mpText->setCaption("iris with ogre");
	mpText->setCharHeight(16); // TODO : unhardcode
	#ifdef WIN32
	mpText->setFontName("BlueHighway"); // TODO : unhardcode
	#else
	mpText->setFontName("TrebuchetMSBold"); // TODO : unhardcode
	#endif
	
	mpOverlayGUI->add2D(mpPanel);
	mpPanel->addChild(mpText);
	
	if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_HWRENDER_TO_TEXTURE))
	{
		// In D3D, use a 1024x1024 shadow texture
		mSceneMgr->setShadowTextureSettings(1024, 2);
	}
	else
	{
		// Use 512x512 texture in GL since we can't go higher than the window res
		mSceneMgr->setShadowTextureSettings(512, 2);
	}
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));

		
		
	// Draw a sample scene featuring an ogrehead while loading the rest =)
	if (1) {
		// Set ambient light
		mSceneMgr->setAmbientLight(ColourValue(0.4, 0.4, 0.4));
		
		SceneNode* mynode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
		mynode->attachObject(ogreHead);
		mynode->setPosition(0,0,-500);
		
		// sky 
		//mSceneMgr->setSkyBox( true, "GhoulSkyBox1" );
		
        // Create a light
        gMainLight = mSceneMgr->createLight("MainLight");
        gMainLight->setType(Ogre::Light::LT_DIRECTIONAL);
        //gMainLight->setPosition(0,0,40);
        gMainLight->setDirection(1,1,-1);
		
		DrawOneFrame();
		
		// crashes withoug ogrehead.... don't detach, probably the opengl of iris does interfere with ogre..
		ogreHead->setVisible(false);
		//mynode->detachObject(ogreHead);
		//mynode->detachAllObjects(); // hide the ogre head now, so it doesn't bother us anymore =)
	}
	
	mpRaySceneQuery = mSceneMgr->createRayQuery(Ray());
	return true;
}

void	cOgreWrapper::DrawOneFrame	() {PROFILE
	// draw one frame
	mRoot->renderOneFrame();
	PlatformManager::getSingleton().messagePump(mWindow); // mouse and keyboard events, unused for SDL-Platform...
	
	// prepare next frame
	
	if (1) {
		//mInputReader->capture();
		cInput::GetSingleton().MouseStateUpdate(
			mInputReader->getMouseAbsX(),
			mInputReader->getMouseAbsY(),
			mInputReader->getMouseRelZ(),
			mInputReader->getMouseButton(0),
			mInputReader->getMouseButton(1),
			mInputReader->getMouseButton(2));
		SDL_ShowCursor(SDL_ENABLE); // make sure the mouse hasn't been eaten by any hungry ogres..
	}
	mpTimer->StartFrame(SDL_GetTicks());
	
	if (1) {
		// cam move hack
		Vector3 cammove(0,0,0);
		float speed = 1.0;
		float rotspeed = 5.0;
		/*
		if (cInput::bKeys[cInput::kkey_a]) cammove.x += -speed;
		if (cInput::bKeys[cInput::kkey_d]) cammove.x -= -speed;
		if (cInput::bKeys[cInput::kkey_r]) cammove.y += speed;
		if (cInput::bKeys[cInput::kkey_f]) cammove.y -= speed;
		if (cInput::bKeys[cInput::kkey_w]) cammove.z += -speed;
		if (cInput::bKeys[cInput::kkey_s]) cammove.z -= -speed;
		mCamera->moveRelative(cammove);
		if (cInput::bKeys[cInput::kkey_up])		mCamera->rotate(mCamera->getDerivedRight(),Degree(rotspeed));
		if (cInput::bKeys[cInput::kkey_down])	mCamera->rotate(mCamera->getDerivedRight(),Degree(-rotspeed));
		if (cInput::bKeys[cInput::kkey_left])	mCamera->rotate(mCamera->getDerivedUp(),Degree(rotspeed));
		if (cInput::bKeys[cInput::kkey_right])	mCamera->rotate(mCamera->getDerivedUp(),Degree(-rotspeed));
		if (cInput::bKeys[cInput::kkey_prior])	mCamera->rotate(mCamera->getDerivedDirection(),Degree(rotspeed));
		if (cInput::bKeys[cInput::kkey_next])	mCamera->rotate(mCamera->getDerivedDirection(),Degree(-rotspeed));
		*/
		
		if (cInput::bKeys[cInput::kkey_r])	mCamera->rotate(mCamera->getDerivedRight(),Degree(rotspeed));
		if (cInput::bKeys[cInput::kkey_f])	mCamera->rotate(mCamera->getDerivedRight(),Degree(-rotspeed));
		if (cInput::bKeys[cInput::kkey_a])	mCamera->rotate(Vector3::UNIT_Z,Degree(rotspeed));
		if (cInput::bKeys[cInput::kkey_d])	mCamera->rotate(Vector3::UNIT_Z,Degree(-rotspeed));
		if (cInput::bKeys[cInput::kkey_q])	mCamera->moveRelative(Vector3::UNIT_Y * speed);
		if (cInput::bKeys[cInput::kkey_e])	mCamera->moveRelative(Vector3::UNIT_Y * -speed);
		if (cInput::bKeys[cInput::kkey_s]) mCamera->moveRelative(Vector3::UNIT_Z * speed);
		if (cInput::bKeys[cInput::kkey_w]) mCamera->moveRelative(Vector3::UNIT_Z * -speed);
			
		
		/*
		static bool holdg = false;
		if (cInput::bKeys[cInput::kkey_g]) {
			if (!holdg) {
				mCurrentShadowTechnique = (mCurrentShadowTechnique+1) % NUM_SHADOW_TECH;
				gMainLight->setCastShadows( mCurrentShadowTechnique != NUM_SHADOW_TECH-1);
				mSceneMgr->setShadowTechnique(mShadowTech[mCurrentShadowTechnique]);
				holdg = true;
			}
		} else { holdg = false; }
		*/
		
	
	
		
		/*
		Vector3 dx = mCamera->getDerivedRight();
		Vector3 dy = mCamera->getDerivedUp();
		Vector3 dz = mCamera->getDerivedDirection();
		float zdot = dz.dotProduct(Vector3::UNIT_Z);
		
		Vector3 r1 = dx;
		Vector3 r2;
		
		if (zdot < -0.9 || zdot > 0.9) 
				r2 = dy.crossProduct(Vector3::UNIT_Z);
		else	r2 = dz.crossProduct(Vector3::UNIT_Z);
		
		mCamera->rotate(r1.getRotationTo(r2));
		printf("r1(%0.2f,%0.2f,%0.2f) r2(%0.2f,%0.2f,%0.2f)\n",r1.x,r1.y,r1.z,r2.x,r2.y,r2.z);
		*/
	}
	
	
	/*
	if (miMaxFPS > 0) {
		// break down framerate a little, for developing locally with 2 clients running on one computer
		// TODO : substract frametime
		usleep(1000*(1000/miMaxFPS));
	}
	*/
	// usleep(1000*(1000/60)); // maxfps : 60
	
	
	// copyed from void SDLScreen::DisplayFps()
	static float framesPerSecond = 0.0f;    // This will store our fps
	static float lastTime = 0.0f; // This will hold the time from the last frame
	static char strFrameRate[80] = { "Fps: " };

	float currentTime = SDL_GetTicks () * 0.001f;

	++framesPerSecond;
	if ( currentTime - lastTime > 1 )
	{
		lastTime = currentTime;
		// Copy the frames per second into a string to display in the window title bar
		printf( "Iris using Ogre, fps: %0.2f\n", framesPerSecond);
		
		sprintf(strFrameRate,"fps:%0.2f",framesPerSecond);
		mpText->setCaption(strFrameRate);
		
		// todo : can't set window title by ogre ?
		//sprintf( strFrameRate, "Iris using Ogre, fps: %d", int( framesPerSecond ) );
		

		// Set the window title bar to our string
		// SDL_WM_SetCaption( strFrameRate, NULL ); // The First argument is the window title
		// Reset the frames per second
		framesPerSecond = 0;
	}
	++miFrameNum;
}

void	cOgreWrapper::DeInit		() {PROFILE
	if (mpRaySceneQuery) mSceneMgr->destroyQuery(mpRaySceneQuery);
	if (mRoot) delete mRoot; mRoot = 0;
}

void	cOgreWrapper::AttachCamera	(SceneNode* pSceneNode) {PROFILE
	if (pSceneNode) 
			pSceneNode->attachObject(mCamera);
	else	mSceneMgr->getRootSceneNode()->attachObject(mCamera);
}

void	cOgreWrapper::SetCameraPos		(const Vector3 vPos) {PROFILE
	mCamera->setPosition(vPos);
}

void	cOgreWrapper::SetCameraRot		(const Quaternion qRot) {PROFILE
	mCamera->setOrientation(qRot);
}

void	cOgreWrapper::CameraLookAt	(const Vector3 vPos) { PROFILE
	mCamera->lookAt(vPos);  
}

void	cOgreWrapper::TakeScreenshot	() {PROFILE
	mWindow->writeContentsToTimestampedFile("screenshots/iris",".jpg");
}

std::string		cOgreWrapper::GetUniqueName () {PROFILE
	static int iLastName = 0;
	return strprintf("UniqueName%d",++iLastName);
}

void	cOgreWrapper::GUI_SetOrigin	(const int posx,const int posy) { PROFILE
	// TODO : implement me
	// mpOverlayGUI
}


/*
// NOTE THAT THIS FUNCTION IS BASED ON MATERIAL FROM:

// Magic Software, Inc.
// http://www.geometrictools.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.
// http://www.geometrictools.com/License/WildMagic3License.pdf
// see http://www.geometrictools.com/Foundation/Intersection/Wm3IntrRay3Triangle3.cpp

// Find-intersection query.  The point of intersection is
//   P = origin + t*direction = b0*V0 + b1*V1 + b2*V2
// a,b,c are the 3 edges of the triangle
*/
/// pfHitDist is a pointer to ONE float, that will receive the distance of the hit
/// pfABC is a pointer to THREE floats, that will receive the "edge-factors" or whatever you call it, can be used to find the texcoords of the hit
///  The point of intersection is  P = origin + (*pfHitDist)*direction = pfABC[0]*a + pfABC[1]*b + pfABC[2]*c
bool	IntersectRayTriangle	(const Vector3& ray_origin,const Vector3& ray_dir,const Vector3& a,const Vector3& b,const Vector3& c,float* pfHitDist,float* pfABC) {
    // compute the offset origin, edges, and normal
    Vector3 kDiff = ray_origin - a;
    Vector3 kEdge1 = b - a;
    Vector3 kEdge2 = c - a;
    Vector3 kNormal = kEdge1.crossProduct(kEdge2);
	Real	ZERO_TOLERANCE = 0.1E-6;

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = ray_dir.dotProduct(kNormal);
    Real fSign;
    if (fDdN > ZERO_TOLERANCE)
    {
        fSign = (Real)1.0;
    }
    else if (fDdN < -ZERO_TOLERANCE)
    {
        fSign = (Real)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Ray and triangle are parallel, call it a "no intersection"
        // even if the ray does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*ray_dir.dotProduct(kDiff.crossProduct(kEdge2));
    if (fDdQxE2 >= (Real)0.0)
    {
        Real fDdE1xQ = fSign*ray_dir.dotProduct(kEdge1.crossProduct(kDiff));
        if (fDdE1xQ >= (Real)0.0)
        {
            if (fDdQxE2 + fDdE1xQ <= fDdN)
            {
                // line intersects triangle, check if ray does
                Real fQdN = -fSign*kDiff.dotProduct(kNormal);
                if (fQdN >= (Real)0.0)
                {
					// ray intersects triangle
					if (pfABC || pfHitDist) {
						Real fInv = ((Real)1.0)/fDdN;
						if (pfHitDist) *pfHitDist = fQdN*fInv;
						if (pfABC) {
							pfABC[1] = fDdQxE2*fInv;
							pfABC[2] = fDdE1xQ*fInv;
							pfABC[0] = (Real)1.0 - pfABC[1] - pfABC[2];
						}
					}
                    return true;
                }
                // else: t < 0, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}

std::map<Ogre::Entity*,MeshShape*>	gEntityShapeMap;

/// TODO : consider using OgreOpCode here ?
/// returns true if the ray intersects the entities triangles
/// the resulting distance in the case of a hit is stored into pfHitDist
bool	cOgreWrapper::RayEntityQuery		(const Ogre::Ray* pRay,Ogre::Entity* pEntity,float* pfHitDist) {
	if (!pRay) return false;
	if (!pEntity) return false;
	if (!pEntity->isVisible()) return false;
	SceneNode* scenenode = pEntity->getParentSceneNode();
	if (!scenenode) return false; // TODO : tagpoint (like knife in hand) attachment currently not supported...
		
	// caching for static meshes, no need to update them every frame...
	MeshShape*& shape = gEntityShapeMap[pEntity];
	if (!shape) shape = new MeshShape(pEntity);
	else if (MeshShape::IsAnimated(pEntity)) shape->Update(pEntity);
	
	// get origin & dir in coordinates local to the entity
	Vector3 ray_origin	= pRay->getOrigin() - scenenode->_getDerivedPosition();
	Vector3 ray_dir 	= scenenode->_getDerivedOrientation().Inverse() * pRay->getDirection();
	return shape->RayIntersect(ray_origin,ray_dir,pfHitDist);
}


MeshShape::MeshShape() {}
MeshShape::MeshShape(Ogre::Entity *entity) { Update(entity); }
MeshShape::~MeshShape() {}
	
/// returns true if the entity is possibly animated (=has skeleton) , this means Update should be called every frame
bool	MeshShape::IsAnimated		(Ogre::Entity *entity) {
	if (!entity) return false;
	return entity->hasSkeleton();
}

/// reads out mesh vertices and indices
/// must be called every frame for animated meshes
/// calling it once for static meshes is enough
/// code is based on OgreOpCode MeshCollisionShape::convertMeshData
void	MeshShape::Update			(Ogre::Entity *entity) {
	mlVertices.clear();
	mlIndices.clear();
	if (!entity) return;
		
	MeshPtr mesh = entity->getMesh();
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;
	int numOfSubs = 0;

	bool useSoftwareBlendingVertices = entity->hasSkeleton();

	if (useSoftwareBlendingVertices)
	{
		entity->_updateAnimation();
	}

	// Run through the submeshes again, adding the data into the arrays
	for ( size_t i = 0; i < mesh->getNumSubMeshes(); ++i) {
		SubMesh* submesh = mesh->getSubMesh(i);
		bool useSharedVertices = submesh->useSharedVertices;

		//----------------------------------------------------------------
		// GET VERTEXDATA
		//----------------------------------------------------------------
		const VertexData * vertex_data;
		if(useSoftwareBlendingVertices)
				vertex_data = useSharedVertices ? entity->_getSkelAnimVertexData() : entity->getSubEntity(i)->_getSkelAnimVertexData();
		else	vertex_data = useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if((!useSharedVertices)||(useSharedVertices && !added_shared))
		{
			if(useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
			
			HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex =
				static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real is
			//  comiled/typedefed as double:
			float* pReal;

			mlVertices.reserve(mlVertices.size()+vertex_data->vertexCount);
			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);
				mlVertices.push_back(Vector3(pReal[0],pReal[1],pReal[2]));
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}
		
		
		// TODO : GET TEXCOORD DATA
		// TODO : GET FACE-MATERIAL MAP, or at least material cound....
		// TODO : no need to update index, texcoord and material buffers for animation !
		
		// TODO : const VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES);
		// for texture alpha checking, VertexElementType should be VET_FLOAT2 

		//----------------------------------------------------------------
		// GET INDEXDATA
		//----------------------------------------------------------------
		IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == HardwareIndexBuffer::IT_32BIT);

		uint32 *pLong = static_cast<uint32*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
		uint16* pShort = reinterpret_cast<uint16*>(pLong);

		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

		mlIndices.reserve(mlIndices.size()+3*numTris);
		if ( use32bitindexes )
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				mlIndices.push_back(pLong[k] + static_cast<int>(offset));
			}
		}
		else
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				mlIndices.push_back(static_cast<int>(pShort[k]) + static_cast<int>(offset));
			}
		}

		ibuf->unlock();

		current_offset = next_offset;
	}
}

/// checks if a ray intersects the mesh, ray_origin and ray_dir must be in local coordinates
bool	MeshShape::RayIntersect	(const Vector3& ray_origin,const Vector3& ray_dir,float* pfHitDist) {
	int i;
	for (i=0;i<mlIndices.size();i+=3) {
		if (IntersectRayTriangle(ray_origin,ray_dir,
			mlVertices[mlIndices[i+0]],
			mlVertices[mlIndices[i+1]],
			mlVertices[mlIndices[i+2]],pfHitDist)) return true;
	}
	return false;
}

cOgreGrannyWrapper::cOgreGrannyWrapper() {}
cOgreGrannyWrapper::~cOgreGrannyWrapper() {}
class cOgreGrannyWrapperImpl : public cOgreGrannyWrapper { public:
	/*
	, public MovableObject
	class OgreGrannyWrapperSection : public Renderable {
		cOgreGrannyWrapperImpl* mpParent;
	};
	std::vector<OgreGrannyWrapperSection*>	mlSections;
	*/
	
	bool minmaxinit;
	Vector3 mvMin,mvMax;
		
	Ogre::ManualObject* mpManualObject;
	std::vector<Ogre::Matrix4>				mlMatrixStack;
	Ogre::Matrix4 		mCurMatrix;
	SceneNode* 			mpSceneNode;
	bool				mbVisible;
	int					miCurSection;
	size_t				miIndexCounter;
	
	cOgreGrannyWrapperImpl() : mpSceneNode(0), mpManualObject(0), mbVisible(false), minmaxinit(false) {}
	virtual ~cOgreGrannyWrapperImpl () {}
	
	static void FillMatrixFromBuffer (Ogre::Matrix4& matrix,const float* buf) { 
		Real* row;
		//*
		row = matrix[0]; row[0] = buf[ 0]; row[1] = buf[ 4]; row[2] = buf[ 8]; row[3] = buf[12];
		row = matrix[1]; row[0] = buf[ 1]; row[1] = buf[ 5]; row[2] = buf[ 9]; row[3] = buf[13];
		row = matrix[2]; row[0] = buf[ 2]; row[1] = buf[ 6]; row[2] = buf[10]; row[3] = buf[14];
		row = matrix[3]; row[0] = buf[ 3]; row[1] = buf[ 7]; row[2] = buf[11]; row[3] = buf[15];
		/*/
		row = matrix[0]; row[0] = *buf; ++buf; row[1] = *buf; ++buf; row[2] = *buf; ++buf; row[3] = *buf; ++buf;
		row = matrix[1]; row[0] = *buf; ++buf; row[1] = *buf; ++buf; row[2] = *buf; ++buf; row[3] = *buf; ++buf;
		row = matrix[2]; row[0] = *buf; ++buf; row[1] = *buf; ++buf; row[2] = *buf; ++buf; row[3] = *buf; ++buf;
		row = matrix[3]; row[0] = *buf; ++buf; row[1] = *buf; ++buf; row[2] = *buf; ++buf; row[3] = *buf; ++buf;
		//*/
	}
	static void FillBufferFromMatrix (float* buf,const Ogre::Matrix4& matrix) { 
		const Real* row;
		//*
		row = matrix[0]; buf[ 0] = row[0]; buf[ 4] = row[1]; buf[ 8] = row[2]; buf[12] = row[3];
		row = matrix[1]; buf[ 1] = row[0]; buf[ 5] = row[1]; buf[ 9] = row[2]; buf[13] = row[3];
		row = matrix[2]; buf[ 2] = row[0]; buf[ 6] = row[1]; buf[10] = row[2]; buf[14] = row[3];
		row = matrix[3]; buf[ 3] = row[0]; buf[ 7] = row[1]; buf[11] = row[2]; buf[15] = row[3];
		/*/
		row = matrix[0]; *buf = row[0]; ++buf; *buf = row[1]; ++buf; *buf = row[2]; ++buf; *buf = row[3]; ++buf; 
		row = matrix[1]; *buf = row[0]; ++buf; *buf = row[1]; ++buf; *buf = row[2]; ++buf; *buf = row[3]; ++buf; 
		row = matrix[2]; *buf = row[0]; ++buf; *buf = row[1]; ++buf; *buf = row[2]; ++buf; *buf = row[3]; ++buf; 
		row = matrix[3]; *buf = row[0]; ++buf; *buf = row[1]; ++buf; *buf = row[2]; ++buf; *buf = row[3]; ++buf; 
		//*/
	}
	
	virtual void	glPushMatrix	() { mlMatrixStack.push_back(mCurMatrix); }
	virtual void	glPopMatrix		() { mCurMatrix = mlMatrixStack.back(); mlMatrixStack.pop_back(); }
	virtual void	glLoadIdentity	() { mCurMatrix.makeTrans(Vector3::ZERO); }
	virtual void	glLoadMatrixf	(const float* m) { FillMatrixFromBuffer(mCurMatrix,m); }
	virtual void	glGetFloatv		(const eMatrixType type,float* buf) { FillBufferFromMatrix(buf,mCurMatrix); }
	
	/*
	virtual void	glMultMatrixf	(const float* m) {
		static Ogre::Matrix4 mulmat; 
		FillMatrixFromBuffer(mulmat,m);
		mCurMatrix = mulmat * mCurMatrix;
	}
	virtual void	glScalef		(const float x,const float y,const float z) { mCurMatrix = Matrix4::getScale(x,y,z) * mCurMatrix; }
	virtual void	glTranslatef	(const float x,const float y,const float z) { mCurMatrix = Matrix4::getTrans(x,y,z) * mCurMatrix; }
	virtual void	glRotatef		(const float ang,const float x,const float y,const float z) { mCurMatrix = Matrix4(Quaternion(Degree(ang),Vector3(x,y,z))) * mCurMatrix; }
	/*/
	virtual void	glMultMatrixf	(const float* m) {
		static Ogre::Matrix4 mulmat; 
		FillMatrixFromBuffer(mulmat,m);
		mCurMatrix = mCurMatrix * mulmat;
	}
	virtual void	glScalef		(const float x,const float y,const float z) { mCurMatrix = mCurMatrix * Matrix4::getScale(x,y,z); }
	virtual void	glTranslatef	(const float x,const float y,const float z) { mCurMatrix = mCurMatrix * Matrix4::getTrans(x,y,z); }
	virtual void	glRotatef		(const float ang,const float x,const float y,const float z) { mCurMatrix = mCurMatrix * Matrix4(Quaternion(Degree(ang),Vector3(x,y,z))); }
	//*/
	
	virtual void	glColor3f		(const float r,const float g,const float b) {}
	virtual void	glColor4f		(const float r,const float g,const float b,const float a) {}
	virtual void	glTexCoord2fv	(const float* buf) {
		mpManualObject->textureCoord(buf[0],buf[1]);
	}
	virtual void	glVertex3fv		(const float* buf) {
		Vector3 myvec = mCurMatrix *  Vector3(buf[0],buf[1],buf[2]);
		
		if (!minmaxinit || myvec.x < mvMin.x) mvMin.x = myvec.x;
		if (!minmaxinit || myvec.y < mvMin.y) mvMin.y = myvec.y;
		if (!minmaxinit || myvec.z < mvMin.z) mvMin.z = myvec.z;
		if (!minmaxinit || myvec.x > mvMax.x) mvMax.x = myvec.x;
		if (!minmaxinit || myvec.y > mvMax.y) mvMax.y = myvec.y;
		if (!minmaxinit || myvec.z > mvMax.z) mvMax.z = myvec.z;
		minmaxinit = true;
		
		mpManualObject->position(myvec);
		mpManualObject->index(miIndexCounter++);
	}
		
	virtual void	glBegin			(const char* sMat,const size_t iNumPolys) {
		mpManualObject->begin(sMat?sMat:"BaseWhiteNoLighting");
		//mpManualObject->begin("BaseWhiteNoLighting");
		miIndexCounter = 0;
	}
	virtual void	glEnd			() {
		mpManualObject->end();
	}
		
	virtual void	DrawStep		(const bool bVisible,const float x,const float y,const float z) {
		if (minmaxinit) {
			//printf("min(%0.1f,%0.1f,%0.1f),max(%0.1f,%0.1f,%0.1f)\n",mvMin.x,mvMin.y,mvMin.z,mvMax.x,mvMax.y,mvMax.z);
			minmaxinit = false;
		}
		if (!bVisible && !mpSceneNode) return;
		if (bVisible && !mpSceneNode) {
			mpSceneNode = cOgreWrapper::GetSingleton().mSceneMgr->getRootSceneNode()->createChildSceneNode();
			mpManualObject = cOgreWrapper::GetSingleton().mSceneMgr->createManualObject(cOgreWrapper::GetUniqueName());
			mpSceneNode->attachObject(mpManualObject);
		}
		if (mbVisible != bVisible) {
			mbVisible = bVisible;
			mpManualObject->setVisible(mbVisible);
		}
		
		mpSceneNode->setPosition(x,y,z);
		glLoadIdentity();
		miCurSection = 0;
		mpManualObject->clear();
	}
};

cOgreGrannyWrapper* cOgreWrapper::CreateOgreGrannyWrapper() {
	return new cOgreGrannyWrapperImpl();
}
