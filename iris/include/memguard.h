#ifndef __MEM_GUARD_H__

#define __MEM_GUARD_H__

#ifndef __CONFIG_H__
#error  wrong include sequence (config.h needed first)!
#endif


#if MEM_GUARD_LEVEL == 0

#  define  malloc(s,name)      malloc(s)
#  define  new(x)              new 


#elif MEM_GUARD_LEVEL==1

   extern long alloc_count;

#  define  malloc(s,name)      malloc(s);alloc_count++
#  define  free(x)             free(x);alloc_count--
#  define  realloc(p,s)        realloc(p,s)
#  define  new(x)              new 


#elif MEM_GUARD_LEVEL==2

   void *operator new(size_t size);
   void operator delete(void *mem);
   void *operator new(size_t size, byte *text);
   void operator delete(void *mem, byte *text);
   #define  malloc(s,name)      gmalloc(s,name)
   #define  free(x)             gfree(x)
   #define  realloc(p,s)        grealloc(p,s)

   // keep track on memory
   
   extern void *gmalloc(size_t size, byte *name);
   extern void gfree(void *memblock);
   extern void showMemory();

   extern void *memmark(void *point, byte *name);
   extern void  memunmark(void *point);
   extern void *grealloc(void *memblock, size_t size);

#else 
# error invalid guard level specified.
#endif

#endif
