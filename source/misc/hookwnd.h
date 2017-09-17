/*
Module : HOOKWND.H
Purpose: Defines the interface for an MFC class to implement message hooking before CWnd gets there
Created: PJN / 24-02-1999
History: None

Copyright (c) 1999 - 2003 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


#ifndef __HOOKWND_H__
#define __HOOKWND_H__

#ifndef __AFXMT_H__
#pragma message("To avoid this message, please put afxmt.h in your PCH (normally stdafx.h)")
#include <afxmt.h>
#endif

#if defined(_M_IX86)
#pragma pack(push, 1)
struct _HookWndProcThunk
{
  DWORD m_mov;     //mov dword ptr [esp+0x4], pThis (esp is hWnd)
  DWORD m_this;
  BYTE  m_jmp;     //jmp WndPro
  DWORD m_relproc; //relative jmp
};
#pragma pack(pop)
#else
#error CHookWnd currently only supports X86 processors
#endif

class CHookWndProcThunk
{
public:
//member variables
  _HookWndProcThunk thunk;

//Methods
  void Init(WNDPROC proc, void* pThis)
  {
    thunk.m_mov = 0x042444C7; //C7 44 24 0C
    thunk.m_this = (DWORD) pThis;
    thunk.m_jmp = 0xe9;
    thunk.m_relproc = (int)proc - ((int)this + sizeof(_HookWndProcThunk));

    //Write block from data cache and flush from instruction cache
    FlushInstructionCache(GetCurrentProcess(), &thunk, sizeof(thunk));
  }
};

//Class which implements message hooking before CWnd gets there
class CHookWnd : public CObject
{
public:
//Constructors / Destructors
  CHookWnd(BOOL bAutoDelete = FALSE);
  virtual ~CHookWnd();

//Methods
  virtual BOOL     Hook(HWND hWnd); 
  virtual BOOL     Hook(CWnd* pWnd); 
  virtual HWND     Unhook();
  BOOL             IsHooked() const;
  BOOL             FirstInChain() const;
  BOOL             LastInChain() const;
  int              MiddleOfChain() const;
  int              SizeOfHookChain() const;

//Static methods
  static CHookWnd* FirstHook(HWND hWnd);
  static CHookWnd* FirstHook(CWnd* pWnd);
  static CHookWnd* FindHook(HWND hWnd, const CRuntimeClass* pClass);
  static CHookWnd* FindHook(CWnd* pWnd, const CRuntimeClass* pClass);
  static CHookWnd* CreateHook(HWND hWnd, CRuntimeClass* pClass);
  static CHookWnd* CreateHook(CWnd* pWnd, CRuntimeClass* pClass);
  static BOOL      IsHooked(HWND hWnd);
  static BOOL      IsHooked(CWnd* pWnd);

protected:
  DECLARE_DYNAMIC(CHookWnd)

  //Virtual functions which get called when this CHookWnd is added or removed
  virtual void OnSubclass(HWND hWnd); 
  virtual void OnUnsubclass(HWND hWnd);

  //This is the function you need to override in your derived
  //version of CHookWnd so that you can handle the window messages
  virtual BOOL ProcessWindowMessage(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) = 0;

  //The callback function through which all window messages will be handled when they are hooked by CHookWnd 
  static LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

  //Calls the default window proc
  LRESULT DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
  
  static CHookWnd* _FirstHook(HWND hWnd);
  static void RemoveAll(HWND hWnd);
  static void RemoveAll(CHookWnd* pFirstHook);

//Data variables
  HWND                    m_hWnd;               //HWND which we are hooking
  WNDPROC                 m_pfnSuperWindowProc; //The orignal window procedure
  BOOL                    m_bAutoDelete;        //Variable to determine if this instance should be auto destroyed
  CHookWndProcThunk       m_thunk;              //The assembly language thunk we use
  CHookWnd*               m_pNextHook;          //The next CHookWnd in the chain
  static CCriticalSection sm_cs;                //Critical section used to thread protect various methods
  static LPCTSTR          sm_pszFirstHookProp;  //The property name to store the first hook in
};


#endif //__HOOKWND_H__