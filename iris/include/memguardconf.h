#ifndef __CONFIG_H__

#define __CONFIG_H__

//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
// Level
// 0 - disables memory guarding
// 1 - counts allocs/frees
// 2 - memory leak statistics
//
//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

#define MEM_GUARD_LEVEL 1



#if MEM_GUARD_LEVEL == 0
#include <malloc.h>
#endif


#ifdef __MEM_GUARD_H__
#error wrong include sequence (config.h comes before memguard.h)
#endif

#endif
