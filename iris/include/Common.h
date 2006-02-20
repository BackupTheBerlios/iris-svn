#ifndef _COMMON_H_
#define _COMMON_H_

// Values
#ifndef NULL
#define NULL 0
#endif

// Macros
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

#endif
