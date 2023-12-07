
//Jan-25-2023 Flicker Fix (be on fullscreen)
//Jan-26-2023 Full Flicker Fix n added GR

//Command
//i686-w64-mingw32-gcc-win32 parCour.c -o parCour.exe -lopengl32 -lglu32 -lgdi32 -municode

#include <windows.h>
#include <stdio.h>	
#include <stdlib.h>
#include <unistd.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>

#define GR_WIDTH    640
#define GR_HEIGHT   480

#define PLATFORM_NUM    100
#define PLAYER_WIDTH    8
#define PLAYER_HEIGHT   8

bool rst_left,rst_right,rst_up;
int grav,jump_height;
double g_x,g_y,g_x1,g_x2,g_y1,g_y2;
double platform_x[PLATFORM_NUM],platform_y[PLATFORM_NUM];
int platform_length[PLATFORM_NUM];
//
//
int RandNum(int min, int max) {return rand()%(max-min + 1) + min;}

void GrRect(HWND hwnd, HDC hdc, PAINTSTRUCT ps,double x,double y,int l, int h,int COLOR) {
  HBRUSH hBrush,holdBrush;
  HPEN hPen,holdPen;

  hBrush = CreateSolidBrush(COLOR);
  hPen=CreatePen(PS_NULL,1,COLOR);
  holdPen=SelectObject(hdc,hPen);
  holdBrush=(HBRUSH) SelectObject(hdc,hBrush);

  Rectangle(hdc,x,y,x+l,y+h);

  SelectObject(hdc, holdBrush);
  SelectObject(hdc, holdPen);
  DeleteObject(hPen);
  DeleteObject(hBrush);
}

void GrLine(HWND hwnd, HDC hdc, PAINTSTRUCT ps,double x1,double y1,double x2,double y2,int COLOR) {
  HPEN hPen = CreatePen(PS_SOLID, 1, COLOR);
  HPEN hOldPen = SelectObject(hdc, hPen);
  MoveToEx(hdc,x1,y1,NULL);
  LineTo(hdc,x2,y2);
  SelectObject(hdc, hOldPen);
  DeleteObject(hPen);
}
//
//
void InitPlayer() {
  grav=1;
  rst_left=rst_right=rst_up=0;
  jump_height=0;
  g_x=GR_WIDTH/2;
  g_y=GR_HEIGHT/2;
  g_x1=g_x-PLAYER_WIDTH/2;
  g_y1=g_y-PLAYER_HEIGHT/2;
  g_x2=g_x+PLAYER_WIDTH/2;
  g_y2=g_y+PLAYER_HEIGHT/2;
}

void SetPlatform(int i,int y) {
  platform_x[i]=8*RandNum(0,GR_WIDTH/8);
  platform_y[i]=y;
  platform_length[i]=8*RandNum(1,8);
}

void InitPlatforms() {
  platform_x[0]=g_x-20;
  platform_y[0]=g_y+PLAYER_HEIGHT;
  platform_length[0]=40;
  for (int i=1;i<PLATFORM_NUM;i++) {
    SetPlatform(i,8*RandNum(0,GR_HEIGHT/8));
  }
}

void Init() {
  InitPlayer();
  InitPlatforms();
}
//
//
void PlayerAct() {
  //gravity
  if (jump_height<-10 && grav<2) {
    grav++;
  }
  g_y+=1;
  jump_height--;
  //movement
  if (jump_height==62) {
    jump_height=0;
  }
  if (rst_up && jump_height==63) {
    jump_height=50;
  } 
  if (0<jump_height && jump_height<51) {
    g_y-=2;
  }
  if (rst_left && g_x1>0) {
    g_x--;
  } else if (rst_right && g_x2<GR_WIDTH) {
    g_x++;
  }
  //set axis
  g_x1=g_x-PLAYER_WIDTH/2;
  g_y1=g_y-PLAYER_HEIGHT/2;
  g_x2=g_x+PLAYER_WIDTH/2;
  g_y2=g_y+PLAYER_HEIGHT/2;
}

void PlatformAct(int pid,int g) {
  //gravity
  if (g==0) {
    platform_y[pid]+=0.5;
  }
  if (platform_y[pid]>GR_HEIGHT) {
    SetPlatform(pid,8*RandNum(0,4));
  }
  //collision
  if ((platform_y[pid]-2<g_y2 && g_y2<=platform_y[pid]) && 
       ((platform_x[pid]<=g_x1 && g_x1<=platform_x[pid]+platform_length[pid]) || 
        (platform_x[pid]<=g_x2 && g_x2<=platform_x[pid]+platform_length[pid])
      )) {
    grav=1;
    jump_height=64;
    g_y-=2;
  }
}

DWORD WINAPI AnimateTask01(LPVOID lpArg) {
  bool b=1;
  char x;
  while (b) {
    if (g_y>GR_HEIGHT+8) {
      Init();
    }
    for (int j=0;j<grav;j++) {
      for (int i=0;i<PLATFORM_NUM;i++) {
        PlatformAct(i,j);
      }
      PlayerAct();
    }
    usleep(6000);
  }
}
//
//
void DrawBackground(HWND hwnd, HDC hdc, PAINTSTRUCT ps) {
  GrRect(hwnd,hdc,ps,0,0,GR_WIDTH,GR_HEIGHT,RGB(253, 2, 139));
}

void DrawPlayer(HWND hwnd, HDC hdc, PAINTSTRUCT ps) {
  GrRect(hwnd,hdc,ps,g_x1,g_y1,PLAYER_WIDTH,PLAYER_HEIGHT,RGB(0, 76, 255));
}

void DrawPlatforms(HWND hwnd, HDC hdc, PAINTSTRUCT ps) {
  for (int i=0;i<PLATFORM_NUM;i++) {
    GrLine(hwnd,hdc,ps,platform_x[i],platform_y[i],platform_x[i]+platform_length[i],platform_y[i],RGB(255,255,255));
  }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  HDC hdc, hdcBackbuff;
  HBITMAP bitmap;
  PAINTSTRUCT ps;
  switch(msg) {
    case WM_KEYDOWN:
      switch (wParam) {
        case 'D':case VK_RIGHT:rst_right=1;break;
        case 'A':case VK_LEFT:rst_left=1;break;
        case 'W':case VK_UP:rst_up=1;break;
        case VK_RETURN:Init();break;
      }
      break;
    case WM_KEYUP:            
      switch (wParam) {
        case 'D':case VK_RIGHT:if(rst_right)rst_right=0;break;
        case 'A':case VK_LEFT:if(rst_left)rst_left=0;break;
        case 'W':case VK_UP:if(rst_up)rst_up=0;break;
      }
      break;
    case WM_ERASEBKGND:
      InvalidateRect(hwnd,NULL,1);
      return 1;
      break;
    case WM_PAINT: //https://cplusplus.com/forum/beginner/269434/
      hdc=BeginPaint(hwnd, &ps);
      hdcBackbuff=CreateCompatibleDC(hdc);
      bitmap=CreateCompatibleBitmap(hdc,1000,1000);
      SelectObject(hdcBackbuff,bitmap);

      DrawBackground(hwnd,hdcBackbuff,ps);
      DrawPlayer(hwnd,hdcBackbuff,ps);
      DrawPlatforms(hwnd,hdcBackbuff,ps);

      BitBlt(hdc, 0, 0, 1000, 1000, hdcBackbuff, 0, 0, SRCCOPY);
      DeleteDC(hdcBackbuff);
      DeleteObject(bitmap);
      EndPaint(hwnd, &ps);
      return 1;
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
  //Window Class
  WNDCLASSW wc = {0};
  wc.style = 0;//CS_HREDRAW | CS_VREDRAW;
  wc.lpszClassName = L"DrawIt";
  wc.hInstance     = hInstance;
  wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
  wc.lpfnWndProc   = WndProc;
  wc.hCursor       = LoadCursor(0, IDC_ARROW);
  RegisterClassW(&wc);

  //create window
  CreateWindowW(wc.lpszClassName,
                L"ParCour (Press [Enter] to Restart)",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                0,
                0,
                GR_WIDTH+7,
                GR_HEIGHT+27,
                NULL,
                NULL,
                hInstance, 
                NULL);

  //Init
  srand(time(NULL));
  Init();

  //threads
  int i=0;
  int *lpArgPtr;
  HANDLE hHandles[1];
  DWORD ThreadId;
  lpArgPtr=(int *)malloc(sizeof(int));
  *lpArgPtr=i;
  hHandles[i]=CreateThread(NULL,0,AnimateTask01,lpArgPtr,0,&ThreadId);

  MSG  msg;
  while (GetMessage(&msg,NULL,0,0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (int) msg.wParam;
}




