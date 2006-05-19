#include "robinput.h"
#include <string.h>
#include <SDL/SDL.h>
#include <OgreInput.h>
#include <OgreInputEvent.h>
#include <OgreEventListeners.h>
#include <OgreKeyEvent.h>
#include "profile.h"

#ifdef WIN32   
 
#ifndef snprintf  
     #define snprintf _snprintf  
#endif  
 
#ifndef vsnprintf   
     #define vsnprintf _vsnprintf  
#endif  
 
#else  
     #include <unistd.h> 
     #include <sys/stat.h>  
 
#ifndef stricmp   
     #define stricmp strcasecmp  
#endif  
  
#ifndef _strnicmp   
     #define _strnicmp strncasecmp  
#endif  
 
#ifndef strnicmp   
     #define strnicmp strncasecmp  
#endif  

#endif 

#ifndef stricmp
	#define stricmp strcasecmp
#endif
	
// key definition in accordance with win32 virtual keycodes,
// partially overlapps with SDL keycodes

bool	cInput::bKeys[256];
bool	cInput::bButton[3];		// mousebuttons, 3 is middle
int		cInput::iMouseWheel;
int		cInput::iMouseWheel_pressed;
int		cInput::iMouseWheel_all_since_last_step;
int		cInput::iMouseWheel_pressed_since_last_step;
int		cInput::iMouse[2];
	
const char * cInput::szKeyNames[256] = {
	"",// 0x00
	"mouse1",
	"mouse2",
	"mouse3",
	"mouse4",
	"mouse5",
	"wheelup",
	"wheeldown",
	"backspace",
	"tab",
	"",// 0x0A
	"",
	"stopclear",
	"return",
	"np_enter",
	"",


	"lshift",// 0x10
	"lcontrol",
	"lalt",
	"pause",
	"capslock",
	"","","","","",
	"",// 0x1a
	"escape",
	"","","","",


	"space",// 0x20
	"pgup",
	"pgdn",
	"end",
	"home",
	"left",
	"up",
	"right",
	"down",
	"",
	"",// 0x2a
	"",
	"screen",
	"ins",
	"del",
	"",


	"0",// 0x30
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",// 0x3a
	"","","","","",


	"",// 0x40
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",// 0x4a
	"l",
	"m",
	"n",
	"o",


	"p",// 0x50
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",// 0x5a
	"lwin",
	"rwin",
	"menu",
	"",
	"",



	"np0",// 0x60
	"np1",
	"np2",
	"np3",
	"np4",
	"np5",
	"np6",
	"np7",
	"np8",
	"np9",
	"npmult",// 0x6a
	"npadd",
	"",
	"npsub",
	"npkomma",
	"npdiv",


	"f1",// 0x70
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"f11",// 0x7a
	"f12",
	"f13",
	"f14",
	"f15",
	"",


	// 0x80
	"","","","","","","","","","","","","","","","",

	// 0x90
	"numlock","scroll",
	"","","","","","","","","","","","","","",

	// 0xA0
	"","rshift",
	"","rcontrol",
	"","ralt",
	"","","","","","","","","","",

	// 0xB0
	"","","","","","","","","","",
	"ue",
	"plus",
	"komma",
	"minus",
	"point",
	"grid",

	// 0xC0
	"oe","","","","","","","","","","","","","","","",

	// 0xD0
	"","","","","","","","","","","",
	"bslash",
	"console",
	"accent",
	"ae",
	"",

	// 0xE0
	"","","greater","","","","","","","","","","","","","",

	// 0xF0
	"","","","","","","","","","","","","","","",""
};


// ****** ****** ****** Keyboard and Mouse


// constructor
cInput::cInput	() { PROFILE Reset(); }

/// Reset
/// desc :		resets all keys, mousebuttons, and the wheel
/// params :		none
void			cInput::Reset	() { PROFILE
	memset(bKeys,0,sizeof(bool)*256);
	memset(bButton,0,sizeof(bool)*3);
	iMouse[0] = 0;
	iMouse[1] = 0;
	iMouseWheel = 0;
	iMouseWheel_pressed = 0;
	iMouseWheel_all_since_last_step = 0;
	iMouseWheel_pressed_since_last_step = 0;
}

/// called every event loop
void cInput::Step() { PROFILE
	// get mousepos
	iMouseWheel_all_since_last_step = 0;
	iMouseWheel_pressed_since_last_step = 0;
	// SDL_GetMouseState((int*)&iMouse[0],(int*)&iMouse[1]);
}

/// checks for change and generates events (process unbuffered mouse input from ogre)
void cInput::MouseStateUpdate (const int iAbsX,const int iAbsY,const int iRelZ,const bool b1,const bool b2,const bool b3) { PROFILE
	#ifdef WIN32
		static int firstabsx,firstabsy;
		static bool init_lastabs = true;
		if (init_lastabs) {
			firstabsx = iAbsX;
			firstabsy = iAbsY;
			init_lastabs = false;
		}
		if (iAbsX - firstabsx < 0) firstabsx += iAbsX - firstabsx;
		if (iAbsY - firstabsy < 0) firstabsy += iAbsY - firstabsy;
		iMouse[0] = iAbsX - firstabsx;
		iMouse[1] = iAbsY - firstabsy;
	#else
		iMouse[0] = iAbsX;
		iMouse[1] = iAbsY;
	#endif
	
	/*
	// todo iRelZ = inputreader->mMouseRelativeZ for mousewheel
	cInput::kkey_wheelup	= 0x06,
	cInput::kkey_wheeldown	= 0x07,
	*/
	
	//printf("MouseStateUpdate:%d,%d,%d\n",b1?1:0,b2?1:0,b3?1:0);
	
	if (bKeys[kkey_mouse1] && !b1) KeyUp(kkey_mouse1);
	if (bKeys[kkey_mouse2] && !b2) KeyUp(kkey_mouse2);
	if (bKeys[kkey_mouse3] && !b3) KeyUp(kkey_mouse3);
		
	if (!bKeys[kkey_mouse1] && b1) KeyDown(kkey_mouse1);
	if (!bKeys[kkey_mouse2] && b2) KeyDown(kkey_mouse2);
	if (!bKeys[kkey_mouse3] && b3) KeyDown(kkey_mouse3);
}

/// called when the application loses or gains focus (minimization, activation)
void cInput::FocusChange (const bool bGain) { PROFILE
	if (bGain) Reset();
}


/// KeyConvertWin
/// desc :		convert a win32 virtual keycode to platform independant representation
/// params :
///	- iVKey		virtual keycode
///	- bRight	bRight extended key -> right control differentiation
unsigned char		cInput::KeyConvertWin	(const int iVKey,const bool bRight) { PROFILE
	if (bRight && iVKey == kkey_lshift)		return kkey_rshift;
	if (bRight && iVKey == kkey_lcontrol)	return kkey_rcontrol;
	if (bRight && iVKey == kkey_lalt)		return kkey_ralt;
	if (bRight && iVKey == kkey_return)		return kkey_np_enter;
	return iVKey;
}





/// KeyConvertSDL
/// desc :		convert a sdl virtual keycode to platform independant representation
/// params :
///	- iVKey		sdlkeysym or something
unsigned char		cInput::KeyConvertSDL	(const int iVKey) { PROFILE
	if (iVKey == SDLK_BACKQUOTE)				return kkey_console;
	if (iVKey >= SDLK_a && iVKey <= SDLK_z)		return kkey_a + iVKey - 'a';
	if (iVKey >= SDLK_0 && iVKey <= SDLK_9)		return kkey_0 + iVKey - SDLK_0;
	if (iVKey >= SDLK_KP0 && iVKey <= SDLK_KP9)	return kkey_numpad0 + iVKey - SDLK_KP0;
	if (iVKey >= SDLK_F1 && iVKey <= SDLK_F15)	return kkey_f1 + iVKey - SDLK_F1;

	switch (iVKey) {
			  case SDLK_ESCAPE:		return	kkey_escape;
		break;case SDLK_SYSREQ:		return	kkey_screen;
		break;case SDLK_SCROLLOCK:	return	kkey_scroll;
		break;case SDLK_PAUSE:		return	kkey_pause;
		break;case SDLK_BACKQUOTE:	return	kkey_console;
		
		//break;case SDLK_BACKQUOTE:	return	kkey_bslash;
		//break;case SDLK_BACKQUOTE:	return	kkey_accent;
		break;case SDLK_BACKSPACE:	return	kkey_back;

		break;case SDLK_PAGEUP:		return	kkey_prior;
		break;case SDLK_PAGEDOWN:	return	kkey_next;
		break;case SDLK_END:		return	kkey_end;
		break;case SDLK_HOME:		return	kkey_home;
		break;case SDLK_INSERT:		return	kkey_ins;
		break;case SDLK_DELETE:		return	kkey_del;

		break;case SDLK_LEFT:		return	kkey_left;
		break;case SDLK_UP:			return	kkey_up;
		break;case SDLK_RIGHT:		return	kkey_right;
		break;case SDLK_DOWN:		return	kkey_down;

		break;case SDLK_KP_MULTIPLY:	return	kkey_np_mult;
		break;case SDLK_KP_PLUS:		return	kkey_np_add;
		break;case SDLK_KP_MINUS:		return	kkey_np_sub;
		break;case SDLK_KP_PERIOD:		return	kkey_np_komma;
		break;case SDLK_KP_DIVIDE:		return	kkey_np_div;
		break;case SDLK_NUMLOCK:		return	kkey_numlock;
		
		break;case SDLK_LSHIFT:		return	kkey_lshift;
		break;case SDLK_RSHIFT:		return	kkey_rshift;
		break;case SDLK_LCTRL:		return	kkey_lcontrol;
		break;case SDLK_RCTRL:		return	kkey_rcontrol;
		break;case SDLK_LALT:		return	kkey_lalt;
		break;case SDLK_RALT:		return	kkey_ralt;
		break;case SDLK_LMETA:		return	kkey_lwin;
		break;case SDLK_RMETA:		return	kkey_rwin;

		break;case SDLK_CAPSLOCK:	return	kkey_capslock;
		break;case SDLK_MENU:		return	kkey_menu;
		
		break;case SDLK_TAB:		return	kkey_tab;
		break;case SDLK_RETURN:		return	kkey_return;
		break;case SDLK_KP_ENTER:	return	kkey_np_enter;
		break;case SDLK_SPACE:		return	kkey_space;
		
		break;case SDLK_RIGHTBRACKET:	return	kkey_plus;
		break;case SDLK_BACKSLASH:		return	kkey_grid;
		break;case SDLK_SLASH:			return	kkey_minus;
		break;case SDLK_PERIOD:			return	kkey_point;

		break;case SDLK_COMMA:			return	kkey_komma;
		//break;case SDLK_BACKSLASH:		return	kkey_greater; // DOUBLE

		break;case SDLK_LEFTBRACKET:	return	kkey_ue;
		break;case SDLK_QUOTE:			return	kkey_ae;
		break;case SDLK_SEMICOLON:		return	kkey_oe;
	}

	printf("cInput::KeyConvertSDL found an unknown key : %i",iVKey);

	return iVKey;
}


/// SDL-mousebutton number to keycode
unsigned char	cInput::KeyConvertSDLMouse	(const int iMButton) { PROFILE
	switch (iMButton) {
		case SDL_BUTTON_LEFT:		return kkey_mouse1;
		case SDL_BUTTON_RIGHT:		return kkey_mouse2;
		case SDL_BUTTON_MIDDLE:		return kkey_mouse3;
		case SDL_BUTTON_WHEELUP:	return kkey_wheelup;
		case SDL_BUTTON_WHEELDOWN:	return kkey_wheeldown;
	}
	printf("cInput::KeyConvertSDLMouse : unknown SDL-Mouse-Button %d\n",iMButton);
	return 0;
}


unsigned char	cInput::KeyConvertOgre		(const int iKeyCode) { PROFILE
	// see  virtual bool Ogre::InputReader::isKeyDown  	(   	KeyCode   	 kc  	 )   	 const
	switch (iKeyCode) {
		case Ogre::KC_ESCAPE			: return cInput::kkey_escape;
		case Ogre::KC_1 				: return cInput::kkey_1;
		case Ogre::KC_2 				: return cInput::kkey_2;
		case Ogre::KC_3 				: return cInput::kkey_3;
		case Ogre::KC_4 				: return cInput::kkey_4;
		case Ogre::KC_5 				: return cInput::kkey_5;
		case Ogre::KC_6 				: return cInput::kkey_6;
		case Ogre::KC_7 				: return cInput::kkey_7;
		case Ogre::KC_8 				: return cInput::kkey_8;
		case Ogre::KC_9 				: return cInput::kkey_9;
		case Ogre::KC_0 				: return cInput::kkey_0;
		case Ogre::KC_MINUS 			: return cInput::kkey_minus;
		case Ogre::KC_BACK 				: return cInput::kkey_back;
		//case Ogre::KC_EQUALS 			: return cInput::kkey_;
		case Ogre::KC_TAB 				: return cInput::kkey_tab;
		case Ogre::KC_Q 				: return cInput::kkey_q;
		case Ogre::KC_W 				: return cInput::kkey_w;
		case Ogre::KC_E 				: return cInput::kkey_e;
		case Ogre::KC_R 				: return cInput::kkey_r;
		case Ogre::KC_T 				: return cInput::kkey_t;
		case Ogre::KC_Y 				: return cInput::kkey_y;
		case Ogre::KC_U 				: return cInput::kkey_u;
		case Ogre::KC_I 				: return cInput::kkey_i;
		case Ogre::KC_O 				: return cInput::kkey_o;
		case Ogre::KC_P 				: return cInput::kkey_p;
		//case Ogre::KC_LBRACKET 			: return cInput::kkey_;
		//case Ogre::KC_RBRACKET 			: return cInput::kkey_;
		case Ogre::KC_RETURN 			: return cInput::kkey_return;
		case Ogre::KC_LCONTROL 			: return cInput::kkey_lcontrol;
		case Ogre::KC_A 				: return cInput::kkey_a;
		case Ogre::KC_S 				: return cInput::kkey_s;
		case Ogre::KC_D 				: return cInput::kkey_d;
		case Ogre::KC_F 				: return cInput::kkey_f;
		case Ogre::KC_G 				: return cInput::kkey_g;
		case Ogre::KC_H 				: return cInput::kkey_h;
		case Ogre::KC_J 				: return cInput::kkey_j;
		case Ogre::KC_K 				: return cInput::kkey_k;
		case Ogre::KC_L 				: return cInput::kkey_l;
		//case Ogre::KC_SEMICOLON 		: return cInput::kkey_;
		//case Ogre::KC_APOSTROPHE 		: return cInput::kkey_console;
		//case Ogre::KC_GRAVE 			: return cInput::kkey_accent;
		case Ogre::KC_LSHIFT 			: return cInput::kkey_lshift;
		//case Ogre::KC_BACKSLASH 		: return cInput::kkey_bslash;
		case Ogre::KC_Z 				: return cInput::kkey_z;
		case Ogre::KC_X 				: return cInput::kkey_x;
		case Ogre::KC_C 				: return cInput::kkey_c;
		case Ogre::KC_V 				: return cInput::kkey_v;
		case Ogre::KC_B 				: return cInput::kkey_b;
		case Ogre::KC_N 				: return cInput::kkey_n;
		case Ogre::KC_M 				: return cInput::kkey_m;
		case Ogre::KC_COMMA 			: return cInput::kkey_komma;
		case Ogre::KC_PERIOD 			: return cInput::kkey_point;
		case Ogre::KC_SLASH 			: return 0; // sth like kkey_ue  kkey_ae kkey_oe ?
		case Ogre::KC_RSHIFT 			: return cInput::kkey_rshift;
		case Ogre::KC_MULTIPLY 			: return cInput::kkey_np_mult;
		case Ogre::KC_LMENU 			: return cInput::kkey_lalt;
		case Ogre::KC_SPACE 			: return cInput::kkey_space;
		case Ogre::KC_CAPITAL 			: return cInput::kkey_capslock;
		case Ogre::KC_F1 				: return cInput::kkey_f1;
		case Ogre::KC_F2 				: return cInput::kkey_f2;
		case Ogre::KC_F3 				: return cInput::kkey_f3;
		case Ogre::KC_F4 				: return cInput::kkey_f4;
		case Ogre::KC_F5 				: return cInput::kkey_f5;
		case Ogre::KC_F6 				: return cInput::kkey_f6;
		case Ogre::KC_F7 				: return cInput::kkey_f7;
		case Ogre::KC_F8 				: return cInput::kkey_f8;
		case Ogre::KC_F9 				: return cInput::kkey_f9;
		case Ogre::KC_F10 				: return cInput::kkey_f10;
		case Ogre::KC_NUMLOCK 			: return cInput::kkey_numlock;
		case Ogre::KC_SCROLL 			: return cInput::kkey_scroll;
		case Ogre::KC_NUMPAD7 			: return cInput::kkey_numpad7;
		case Ogre::KC_NUMPAD8 			: return cInput::kkey_numpad8;
		case Ogre::KC_NUMPAD9 			: return cInput::kkey_numpad9;
		case Ogre::KC_SUBTRACT 			: return cInput::kkey_np_sub;
		case Ogre::KC_NUMPAD4 			: return cInput::kkey_numpad4;
		case Ogre::KC_NUMPAD5 			: return cInput::kkey_numpad5;
		case Ogre::KC_NUMPAD6 			: return cInput::kkey_numpad6;
		case Ogre::KC_ADD 				: return cInput::kkey_np_add;
		case Ogre::KC_NUMPAD1 			: return cInput::kkey_numpad1;
		case Ogre::KC_NUMPAD2 			: return cInput::kkey_numpad2;
		case Ogre::KC_NUMPAD3 			: return cInput::kkey_numpad3;
		case Ogre::KC_NUMPAD0 			: return cInput::kkey_numpad0;
		case Ogre::KC_DECIMAL 			: return cInput::kkey_np_komma;  // double ? KC_NUMPADCOMMA
		case Ogre::KC_OEM_102 			: return cInput::kkey_greater;
		case Ogre::KC_F11 				: return cInput::kkey_f11;
		case Ogre::KC_F12 				: return cInput::kkey_f12;
		case Ogre::KC_F13 				: return cInput::kkey_f13;
		case Ogre::KC_F14 				: return cInput::kkey_f14;
		case Ogre::KC_F15 				: return cInput::kkey_f15;
		/*
		case Ogre::KC_KANA 				: return cInput::kkey_;
		case Ogre::KC_ABNT_C1 			: return cInput::kkey_;
		case Ogre::KC_CONVERT 			: return cInput::kkey_;
		case Ogre::KC_NOCONVERT 		: return cInput::kkey_;
		case Ogre::KC_YEN 				: return cInput::kkey_;
		case Ogre::KC_ABNT_C2 			: return cInput::kkey_;
		case Ogre::KC_NUMPADEQUALS 		: return cInput::kkey_;
		case Ogre::KC_PREVTRACK 		: return cInput::kkey_;
		case Ogre::KC_AT 				: return cInput::kkey_;
		case Ogre::KC_COLON 			: return cInput::kkey_;
		case Ogre::KC_UNDERLINE 		: return cInput::kkey_;
		case Ogre::KC_KANJI 			: return cInput::kkey_;
		case Ogre::KC_STOP 				: return cInput::kkey_;
		case Ogre::KC_AX 				: return cInput::kkey_;
		case Ogre::KC_UNLABELED 		: return cInput::kkey_;
		case Ogre::KC_NEXTTRACK 		: return cInput::kkey_;
		*/
		case Ogre::KC_NUMPADENTER 		: return cInput::kkey_np_enter;
		case Ogre::KC_RCONTROL 			: return cInput::kkey_rcontrol;
		/*
		case Ogre::KC_MUTE 				: return cInput::kkey_;
		case Ogre::KC_CALCULATOR 		: return cInput::kkey_;
		case Ogre::KC_PLAYPAUSE 		: return cInput::kkey_;
		case Ogre::KC_MEDIASTOP 		: return cInput::kkey_;
		case Ogre::KC_VOLUMEDOWN 		: return cInput::kkey_;
		case Ogre::KC_VOLUMEUP 			: return cInput::kkey_;
		case Ogre::KC_WEBHOME 			: return cInput::kkey_;
			case Ogre::KC_NUMPADCOMMA 		: return cInput::kkey_np_komma; // double ?  KC_DECIMAL
			case Ogre::KC_DIVIDE 			: return cInput::kkey_;
		*/
		case Ogre::KC_SYSRQ 			: printf("key:KC_SYSRQ=kkey_screen?\n"); return cInput::kkey_screen;
		case Ogre::KC_RMENU 			: return cInput::kkey_ralt;
		case Ogre::KC_PAUSE 			: return cInput::kkey_pause;
		case Ogre::KC_HOME 				: return cInput::kkey_home;
		case Ogre::KC_UP 				: return cInput::kkey_up;
		case Ogre::KC_PGUP 				: return cInput::kkey_prior;
		case Ogre::KC_LEFT 				: return cInput::kkey_left;
		case Ogre::KC_RIGHT 			: return cInput::kkey_right;
		case Ogre::KC_END 				: return cInput::kkey_end;
		case Ogre::KC_DOWN 				: return cInput::kkey_down;
		case Ogre::KC_PGDOWN 			: return cInput::kkey_next;
		case Ogre::KC_INSERT 			: return cInput::kkey_ins;
		case Ogre::KC_DELETE 			: return cInput::kkey_del;
		case Ogre::KC_LWIN 				: return cInput::kkey_lwin;
		case Ogre::KC_RWIN 				: return cInput::kkey_rwin;
		case Ogre::KC_APPS 				: return cInput::kkey_menu;
		/*
		case Ogre::KC_POWER 			: return cInput::kkey_;
		case Ogre::KC_SLEEP 			: return cInput::kkey_;
		case Ogre::KC_WAKE 				: return cInput::kkey_;
		case Ogre::KC_WEBSEARCH 		: return cInput::kkey_;
		case Ogre::KC_WEBFAVORITES 		: return cInput::kkey_;
		case Ogre::KC_WEBREFRESH 		: return cInput::kkey_;
		case Ogre::KC_WEBSTOP 			: return cInput::kkey_;
		case Ogre::KC_WEBFORWARD 		: return cInput::kkey_;
		case Ogre::KC_WEBBACK 			: return cInput::kkey_;
		case Ogre::KC_MYCOMPUTER 		: return cInput::kkey_;
		case Ogre::KC_MAIL 				: return cInput::kkey_;
		case Ogre::KC_MEDIASELECT 		: return cInput::kkey_;
		*/
		/*
		// see OgreInput.h
		// number pad
		kkey_np_div		= 0x6F,
		kkey_np_stopclear	= 0x0C, // STRANGE THING, numlock + np_5

		kkey_plus		= 0xBB,
		kkey_grid		= 0xBF,
		*/
	}
	printf("cInput::KeyConvertOgre : unknown key %#02x\n",iKeyCode);
	return 0;
}

unsigned char	cInput::KeyConvertOgreMouse	(const int iButtonID) { PROFILE
	switch (iButtonID) {
		case Ogre::InputEvent::BUTTON0_MASK : return cInput::kkey_mouse1;
		case Ogre::InputEvent::BUTTON1_MASK : return cInput::kkey_mouse2;
		case Ogre::InputEvent::BUTTON2_MASK : return cInput::kkey_mouse3;
	}
	printf("cInput::KeyConvertOgreMouse : unknown button %d\n",iButtonID);
	return 0;
}


/// GetNamedKey
/// desc :		get key number for a given key name
/// params :
///	- szName	key name
unsigned char		cInput::GetNamedKey		(const char* szName) { PROFILE
	int i;for (i=0;i<256;i++)
	if (*szKeyNames[i] != 0)
	if (stricmp(szKeyNames[i],szName) == 0)
		return i;
	return 0;
}

/// returns the human readable (english) name for a key
const char*		cInput::GetKeyName			(const unsigned char iKey) { PROFILE 
	return szKeyNames[iKey];
}


/// KeyDown
/// desc :		register keypush (and autokeyrepeat)
/// params :
///	- iKey		key number
///	- bFirst	really first push, or only autorepeat
void	cInput::KeyDown			(const unsigned char iKey,const char sChar) { PROFILE
	bool bIsRepetition = bKeys[iKey];

	// update records
	bKeys[iKey] = true;
	if (iKey == kkey_mouse1) 	bButton[0] = true;
	if (iKey == kkey_mouse2) 	bButton[1] = true;
	if (iKey == kkey_mouse3) 	bButton[2] = true;
	if (iKey == kkey_wheelup) 	{
		iMouseWheel++;
		iMouseWheel_all_since_last_step++;
		if (!bIsRepetition) {
			iMouseWheel_pressed++;
			iMouseWheel_pressed_since_last_step++;
		}
	}
	if (iKey == kkey_wheeldown) {
		iMouseWheel--;
		iMouseWheel_all_since_last_step--;
		if (!bIsRepetition) {
			iMouseWheel_pressed--;
			iMouseWheel_pressed_since_last_step--;
		}
	}

	NotifyAllListeners(bIsRepetition?kListenerEvent_KeyRepeat:kListenerEvent_KeyPress,iKey);
	if (sChar) NotifyAllListeners(kListenerEvent_Char,sChar);
}



/// KeyUp
/// desc :		register keyrelease
/// params :
///	- iKey		key number
void	cInput::KeyUp			(const unsigned char iKey) { PROFILE
	// update records
	bKeys[iKey] = false;
	if (iKey == kkey_mouse1) bButton[0] = false;
	if (iKey == kkey_mouse2) bButton[1] = false;
	if (iKey == kkey_mouse3) bButton[2] = false;
		
	NotifyAllListeners(kListenerEvent_KeyRelease,iKey);
}
