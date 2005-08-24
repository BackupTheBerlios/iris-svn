//
// File: MapRenderer.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
//

#include <windows.h>
#include <iostream>
#include <process.h>
#include <olectl.h>
#include <ole2.h>
#include "SDLEvent.h"
#include "renderer/SDLScreen.h"
#include "Debug.h"
#include "Game.h"
#include "Config.h"
#include "renderer/TextureBuffer.h"
#include "loaders/Map.h"
#include "Image.h"
#include "Support.h"
#include "Rendering.h"
#include "GenFiles.h"

using namespace std;

#define IDC_STARTBTN 101
#define IDC_PREVIEWBTN 102
#define IDC_STATIC_BMP 104
#define IDC_FROMBTN 108
#define IDC_ENDBTN 116


SDLEvent *SDLevent;
SDLScreen *SDLscreen;


LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

char szClassName[ ] = "MapRendererApp";

HWND StartButton,ReviewButton, Memo, hMainWnd;
HWND FromX, FromY, ToX, ToY, Detail, FileName, GetFromBegin, GetFromEnd;
HFONT font;
HINSTANCE MainInst;
HWND Preview;
HBITMAP       hBitmap, hOldBitmap;
HPALETTE      hPalette, hOldPalette;
HANDLE hThread = NULL;

unsigned int htimer = 0;
#define ThreadTimer 1



bool ReadyToPaint = false, IsRendering = false, CancelThread = false;
bool SaveOnExit = true;
char *str;
int detail, divisor;
int ClientX, ClientY, ClientZ, ClientFacet;


int WinMain(      
    HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nCmdShow
)
{
  MainInst = hThisInstance;
  str = (char*)malloc(10);
  memset(str,0,10);
  if(!nConfig::Init()) {
    pDebug.Log("Unable to load configuration file - Using defaults ", __FILE__, __LINE__,
	       LEVEL_WARNING);
  }


  SDLscreen = new SDLScreen();
  SDLevent = new SDLEvent;


// CREATE THE WINDOW HERE!

    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    font = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
         VARIABLE_PITCH | FF_SWISS, "Modern");

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (hThisInstance, "A");
    wincl.hIconSm = LoadIcon (hThisInstance, "A");
    wincl.hCursor = LoadCursor (hThisInstance, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;


    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hMainWnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Maprenderer",       /* Title Text */
           WS_POPUP |WS_SYSMENU | WS_CAPTION, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    char MapFile[1024];
    memset(MapFile,0,sizeof(MapFile));
    sprintf(MapFile,"%s\\map0.mul",nConfig::mulpath.c_str());
    FILE *fp = fopen(MapFile,"r");
    if(!fp)
    {
         memset(MapFile,0,sizeof(MapFile));
         if(!GenLib::GenFiles::folderDialog(hMainWnd, "Please select you Ultima Online Installation", MapFile, false, 0))
         {
             MessageBox(hMainWnd,"I'm not able to render anything without knowing your Ultima Online directory! Quitting now!","ERROR",MB_ICONERROR);
             SaveOnExit=false;
             DestroyWindow(hMainWnd);
         }
         else
         nConfig::mulpath = MapFile;
    }
    else
         fclose(fp);


    HWND label1 = CreateWindowEx(0, "STATIC", "From X:", WS_CHILD|WS_VISIBLE|SS_LEFT, 10, 13, 50, 15, hMainWnd, 0, hThisInstance, NULL);
    SendMessage(label1, WM_SETFONT, (UINT)font, TRUE);
    itoa( nConfig::minx, str, 10 );
    FromX = CreateWindowEx (
          0,
          "EDIT",
          str,
          WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
          60, 10, 35, 20,
          hMainWnd,
          0,
          hThisInstance, NULL);
    SendMessage(FromX, WM_SETFONT, (UINT)font, TRUE);

    HWND label2 = CreateWindowEx(0, "STATIC", "From Y:", WS_CHILD|WS_VISIBLE|SS_LEFT, 115, 13, 50, 15, hMainWnd, 0, hThisInstance, NULL);
    SendMessage(label2, WM_SETFONT, (UINT)font, TRUE);
    itoa( nConfig::miny, str, 10 );
    FromY = CreateWindowEx (
          0,
          "EDIT",
          str,
          WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
          155, 10, 35, 20,
          hMainWnd,
          0,
          hThisInstance, NULL);
    SendMessage(FromY, WM_SETFONT, (UINT)font, TRUE);


    HWND label3 = CreateWindowEx(0, "STATIC", "To X:", WS_CHILD|WS_VISIBLE|SS_LEFT, 10, 38, 50, 15, hMainWnd, 0, hThisInstance, NULL);
    SendMessage(label3, WM_SETFONT, (UINT)font, TRUE);
    itoa( nConfig::maxx, str, 10 );
    ToX = CreateWindowEx (
          0,
          "EDIT",
          str,
          WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
          60, 35, 35, 20,
          hMainWnd,
          0,
          hThisInstance, NULL);
    SendMessage(ToX, WM_SETFONT, (UINT)font, TRUE);

    HWND label4 = CreateWindowEx(0, "STATIC", "To Y:", WS_CHILD|WS_VISIBLE|SS_LEFT, 115, 38, 50, 15, hMainWnd, 0, hThisInstance, NULL);
    SendMessage(label4, WM_SETFONT, (UINT)font, TRUE);
    itoa( nConfig::maxy, str, 10 );
    ToY = CreateWindowEx (
          0,
          "EDIT",
          str,
          WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
          155, 35, 35, 20,
          hMainWnd,
          0,
          hThisInstance, NULL);
    SendMessage(ToY, WM_SETFONT, (UINT)font, TRUE);

    HWND label8 = CreateWindowEx(0, "STATIC", "Filename:", WS_CHILD|WS_VISIBLE|SS_LEFT, 10, 60, 50, 15, hMainWnd, 0, hThisInstance, NULL);
    SendMessage(label8, WM_SETFONT, (UINT)font, TRUE);
    str = (char*)nConfig::output.c_str();
    FileName = CreateWindowEx (
          0,
          "EDIT",
          str,
          WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
          10, 75, 100, 20,
          hMainWnd,
          0,
          hThisInstance, NULL);
    SendMessage(FileName, WM_SETFONT, (UINT)font, TRUE);
    
    
    StartButton = CreateWindowEx(0, "BUTTON", "Start Rendering",
          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
          140, 70, 90, 25,
          hMainWnd,
          (HMENU)IDC_STARTBTN,
          hThisInstance, NULL);
    SendMessage(StartButton, WM_SETFONT, (UINT)font, TRUE);

    ReviewButton = CreateWindowEx(0, "BUTTON", "Preview",
          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
          240, 70, 70, 25,
          hMainWnd,
          (HMENU)IDC_PREVIEWBTN,
          hThisInstance, NULL);
    SendMessage(ReviewButton, WM_SETFONT, (UINT)font, TRUE);
    
    Memo = CreateWindowEx(0,
          "EDIT", "Ready...\r\n",
          WS_BORDER |WS_CHILD | WS_VISIBLE | ES_LEFT |ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
          10, 100, 300, 230,
          hMainWnd,
          0,
          hThisInstance, NULL);
    SendMessage(Memo, WM_SETFONT, (UINT)font, TRUE);

    GetFromBegin = CreateWindowEx(0,
          "BUTTON", "Get from client (start)",
          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
          320, 10, 130, 20,
          hMainWnd,
          (HMENU)IDC_FROMBTN,
          hThisInstance, NULL);          
    SendMessage(GetFromBegin, WM_SETFONT, (UINT)font, TRUE);

    GetFromEnd = CreateWindowEx(0,
          "BUTTON", "Get from client (end)",
          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
          320, 35, 130, 20,
          hMainWnd,
          (HMENU)IDC_ENDBTN,
          hThisInstance, NULL);
    SendMessage(GetFromEnd, WM_SETFONT, (UINT)font, TRUE);

    HWND label6 = CreateWindowEx(0,
     "STATIC", "Preview:",
     WS_CHILD | WS_VISIBLE |SS_LEFT,
     320, 60, 100, 20,
     hMainWnd,
     0,
     hThisInstance, NULL);
    SendMessage(label6, WM_SETFONT, (UINT)font, TRUE); 

    Preview = CreateWindowEx(WS_EX_CLIENTEDGE, 
      "STATIC", "", 
      WS_CHILD | WS_BORDER | WS_VISIBLE, 
     320, 80, 200, 200, 
     hMainWnd,
     0, 
     hThisInstance, NULL);

    HWND label7 = CreateWindowEx(0,
     "STATIC", "Details:",
     WS_CHILD | WS_VISIBLE |SS_LEFT,
     200, 10, 100, 20,
     hMainWnd,
     0,
     hThisInstance, NULL);
    SendMessage(label7, WM_SETFONT, (UINT)font, TRUE); 

    itoa( nConfig::detail, str, 10 );    
    Detail = CreateWindowEx(0,
     "COMBOBOX", str,
     WS_CHILD | WS_VISIBLE| CBS_DROPDOWN,
     200, 30, 100, 25,
     hMainWnd,
     0,
     hThisInstance, NULL);
    SetWindowText(Detail, str);
    SendMessage(Detail, WM_SETFONT, (UINT)font, TRUE); 
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"0");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"1");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"2");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"3");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"4");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"5");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"6");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"7");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"8");
    SendMessage(Detail, CB_ADDSTRING, 0, (LPARAM)"9");

    /* Make the window visible on the screen */
    ShowWindow (hMainWnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }  

  return messages.wParam;
}

/*
void ShowError(){
 LPVOID lpMsgBuf;
 FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL
 );

 MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

 LocalFree( lpMsgBuf );
}*/


LRESULT CALLBACK WindowProcedure (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    unsigned int ThreadID;
    char out[2000];
    char gettext[1000];
    DWORD ThreadExitCode;

    switch (message)                  /* handle the messages */
    {
        case WM_TIMER:
            GetExitCodeThread(hThread,&ThreadExitCode);
            if((ThreadExitCode != STILL_ACTIVE) && (IsRendering != CancelThread)){
             CancelThread = false;
             IsRendering = false;
             KillTimer(hMainWnd,ThreadTimer);
             switch (ThreadExitCode){
              case 1:
               MessageBox(hWnd,"These coordinates don't make sense. Please choose a minimum difference of 50 between From & To.","Warning",MB_ICONEXCLAMATION);
               sprintf(out,"Rendering canceled!");
               break;
              case 2:
              case 3:
               sprintf(out,"Rendering canceled at user request!");
               break;
              case 0:
               SendMessage(Memo, WM_GETTEXT, 2000, (LPARAM)gettext);
               sprintf(out,"%s\r\n\r\nDone!",gettext);
               break;
              default:
               sprintf(out,"Thread terminated with an unknown exitcode. Most likely this happens due to programming failures.");
               break;
              }
              SetWindowText(Memo,out);
              SetWindowText(StartButton, "Start Rendering");
              EnableWindow(StartButton, true);
             }

        break;
        case WM_COMMAND:
             switch(LOWORD(wParam))
             {
                  case IDC_STARTBTN:
                       if(IsRendering){
                        CancelThread=true;
                        IsRendering=false;
                        EnableWindow(StartButton, false);
                       }
                       else{
                        SetWindowText(StartButton, "Stop Rendering");
                        IsRendering = true;
                        hThread = (HANDLE)_beginthreadex(NULL, 0, RunRenderer, NULL, 0, &ThreadID);
                        htimer = SetTimer(hMainWnd,ThreadTimer,100,NULL); //specify the timer for (this window, timer named 1, timeout, without callback, send wm_timer msg)
                       }
                       break;
                  case IDC_PREVIEWBTN:
                       hThread = (HANDLE)_beginthreadex(NULL, 0, RunPreview, NULL, 0, &ThreadID);
                       break;
                  case IDC_FROMBTN:
                     if(ClientIsRunning())
                     {
                             FindLocation(&ClientX,&ClientY,&ClientZ,&ClientFacet);
                             if(ClientX < 0 || ClientY < 0)
                             {
                                        MessageBox(hWnd,"Please login to your account and go into the shards world.","Warning",MB_ICONEXCLAMATION);
                             }
                             else
                             {
                                        sprintf(out, "%d", ClientX);
                                        SetWindowText(FromX,out);
                                        sprintf(out, "%d", ClientY);
                                        SetWindowText(FromY,out);
                             }
                     }
                     else
                             MessageBox(hWnd, "Please start your Ultima Online Client to get coordinates from it.","Warning",MB_ICONEXCLAMATION);
                       break;
                  case IDC_ENDBTN:
                       if(ClientIsRunning())
                       {
                             FindLocation(&ClientX,&ClientY,&ClientZ,&ClientFacet);
                             if(ClientX < 0 || ClientY < 0)
                             {
                                        MessageBox(hWnd,"Please login to your account and go into the shards world.","Warning", MB_ICONEXCLAMATION);
                             }
                             else
                             {
                                        sprintf(out, "%d", ClientX);
                                        SetWindowText(ToX,out);
                                        sprintf(out, "%d", ClientY);
                                        SetWindowText(ToY,out);
                             }
                       }
                       else
                             MessageBox(hWnd, "Please start your Ultima Online Client to get coordinates from it.","Warning",MB_ICONEXCLAMATION);
                       break;
             }
             break;
        case WM_PAINT:
            if(ReadyToPaint)
            {
              PAINTSTRUCT   ps;
              HDC           hDC, hMemDC;
              BITMAP        bm;
              
              hDC = BeginPaint( Preview, &ps );

                  GetObject( hBitmap, sizeof(BITMAP), &bm );
                  hMemDC = CreateCompatibleDC( hDC );
                  hOldBitmap = (HBITMAP)SelectObject( hMemDC, hBitmap );
                  hOldPalette = SelectPalette( hDC, hPalette, FALSE );
                  RealizePalette( hDC );
                  
                  StretchBlt( hDC, 0, 0, 194, 194,
                              hMemDC, 0, 0, bm.bmWidth, bm.bmHeight,
                              SRCCOPY);          
//                  BitBlt( hDC, 0, 0, bm.bmWidth, bm.bmHeight,
//                          hMemDC, 0, 0, SRCCOPY );
            
                  SelectObject( hMemDC, hOldBitmap );
                  SelectPalette( hDC, hOldPalette, FALSE );

              EndPaint( Preview, &ps );
            }
          return DefWindowProc (hWnd, message, wParam, lParam);
          break;
        case WM_DESTROY:
            SendMessage(FromX,WM_GETTEXT, 5, (LPARAM)out);
            nConfig::minx = atoi(out);
            SendMessage(FromY,WM_GETTEXT, 5, (LPARAM)out);
            nConfig::miny = atoi(out);
            SendMessage(ToX,WM_GETTEXT, 5, (LPARAM)out);
            nConfig::maxx = atoi(out);
            SendMessage(ToY,WM_GETTEXT, 5, (LPARAM)out);
            nConfig::maxy = atoi(out);
            SendMessage(Detail,WM_GETTEXT, 5, (LPARAM)out);
            nConfig::detail = atoi(out);
            SendMessage(FileName,WM_GETTEXT, 1024, (LPARAM)out);
            nConfig::output = out;
            if(SaveOnExit) nConfig::writeDocument(); 
            delete SDLevent;
            delete SDLscreen;
             
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hWnd, message, wParam, lParam);
    }

    return 0;
}
