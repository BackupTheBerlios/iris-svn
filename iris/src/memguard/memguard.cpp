#include <stddef.h>
#include <stdio.h>
#include <malloc.h>

#include "config.h"

long alloc_count = 0;

#if MEM_GUARD_LEVEL == 2

#define MAX_MEMORY 65000

long tablePos    = 0;

typedef struct {

  void *point;

  byte *name;

} MEM_ENTRY;

MEM_ENTRY memtable[MAX_MEMORY] = {0,};



void *gmalloc(size_t size, byte *name) 

{

  void *point = malloc(size);

  if (tablePos + 1 >= MAX_MEMORY) {

    System::panic("MEMGUARD: Memory guard table exceded!\n");

    return point;

  }    

  memtable[tablePos].name = name;

  memtable[tablePos].point = point;

  tablePos++;



  return point; 

}



void gfree(void *memblock)

{

  if (memblock == NULL) {

    System::panic("MEMGUARD: Freeing null pointer!\n");

    return;

  }

  for (long p = 0; p < tablePos; p++) {

    if (memtable[p].point == memblock) {

      memtable[p].point = NULL;

      memtable[p].name  = NULL;

      break;

    }

  }

  if (p >= tablePos)

    System::panic("MEMGUARD: Freeing unallocated memory!");

  else 

    free(memblock);

}



void *grealloc(void *memblock, size_t size) 

{

  if (memblock == NULL)

    System::panic("MEMGUARD: Reallocating null pointer!");

  void *point = realloc(memblock, size);

  for (long p = 0; p < tablePos; p++) {

    if (memtable[p].point == memblock) {

      memtable[p].point = point;

      memtable[p].name;

      break;

    }

  }

  if (p >= tablePos)

    System::panic("MEMGUARD: Reallocating unallocated memory!");

  return point;

}





void *memmark(void *point, byte *name)

{

 if (tablePos + 1 >= MAX_MEMORY) {

    System::panic("MEMGUARD: Memory guard table exceded!");

    return point;

  }    

  memtable[tablePos].name = name;

  memtable[tablePos].point = point;

  tablePos++;

  return point;

}



void memunmark(void *point)

{

  if (point == NULL)

    System::panic("MEMGUARD: Unmarking null pointer!");

  for (long p = 0; p < tablePos; p++) {

    if (memtable[p].point == point) {

      memtable[p].point = NULL;

      memtable[p].name  = NULL;

      break;

    }

  }

  if (p >= tablePos)

    System::panic("MEMGUARD: Unmarking unmarked memory.");

}



void showMemory()

{

  for (long p = 0; p < tablePos; p++) {

    if (memtable[p].point != NULL) {

      Console::printf("MEMGUARD: unfreed pointer: %p,  name: %s", memtable[p].point, memtable[p].name);

    }

  }

}



void *operator new(size_t size, byte *text)

{

#if MEM_GUARD_LEVEL == 2

  void *mem = malloc(size);

  memmark(mem, text);

  return mem;

#else

  return malloc(size);

#endif

}



void *operator new(size_t size)

{

  void *mem = malloc(size);

  memmark(mem, "(null)");

  return mem;

}



void operator delete(void *mem)

{

  memunmark(mem);

  free(mem);

}



void operator delete(void *mem, byte *text)

{

  memunmark(mem);

  free(mem);

}



#endif

