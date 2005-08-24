#include <windows.h>
#include "Debug.h"

void Error(LPTSTR FuncName,LPTSTR Flags)
{
        TCHAR szBuf[80]; 
        LPVOID lpMsgBuf;
        DWORD dw = GetLastError(); 

        FormatMessage(
             FORMAT_MESSAGE_ALLOCATE_BUFFER | 
             FORMAT_MESSAGE_FROM_SYSTEM,
             NULL,
             dw,
             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
             (LPTSTR) &lpMsgBuf,
             0, NULL );

        wsprintf(szBuf, 
             "%s failed with error %d:\r\n%s\r\nFlags:%s", FuncName,
             dw, lpMsgBuf, Flags); 
              
        pDebug.Log(szBuf,__FILE__, __LINE__,
	       LEVEL_WARNING);
	       
        LocalFree(lpMsgBuf);
        ExitProcess(dw);     
}

bool FindLocation(int *x, int *y, int *z, int *facet)
{
     HINSTANCE hinst;
     hinst = LoadLibrary("UltimaWrapper.dll");
     if(!hinst)
     {
         pDebug.Log("Could not create a instance for loading DLL!",__FILE__, __LINE__,
	       LEVEL_WARNING);
         return false;
     }
         
     bool (*FindLoc)(int*,int*,int*,int*);
     
     FindLoc = (bool(*)(int*,int*,int*,int*))GetProcAddress(hinst,"FindLocation");
     if(!FindLoc)
     {
        Error("GetProcAddress()","FindLocation");
        return false;
     } 
     else  
         return FindLoc(x,y,z,facet);
}

bool ClientIsRunning()
{
     HINSTANCE hinst;
     hinst = LoadLibrary("UltimaWrapper.dll");
     if(!hinst)
     {
         pDebug.Log("Could not create a instance for loading DLL!",__FILE__, __LINE__,
	       LEVEL_WARNING);
         return false;
     }
         
     bool (*IsRunning)();
     
     IsRunning = (bool(*)())GetProcAddress(hinst,"ClientIsRunning");
     if(!IsRunning)
     {
        Error("GetProcAddress()","ClientIsRunning");
        return false;
     } 
     else  
         return IsRunning();

}
