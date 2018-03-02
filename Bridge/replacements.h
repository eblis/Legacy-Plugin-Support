/*
Legacy Plugin Support - Bridge plugin for Miranda IM

Copyright © 2008 Cristian Libotean

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef M_LPSBRIDGE_REPLACEMENTS_H
#define M_LPSBRIDGE_REPLACEMENTS_H

#include "commonheaders.h"

#ifndef MIRANDA_FUNCTIONS
#define MIRANDA_FUNCTIONS

typedef HANDLE (*CREATEHOOKABLEEVENT)(const char *);
typedef int (*DESTROYHOOKABLEEVENT)(HANDLE);
typedef int (*NOTIFYEVENTHOOKS)(HANDLE,WPARAM,LPARAM);
typedef HANDLE (*HOOKEVENT)(const char *,MIRANDAHOOK);
typedef HANDLE (*HOOKEVENTMESSAGE)(const char *,HWND,UINT);
typedef int (*UNHOOKEVENT)(HANDLE);

typedef HANDLE (*CREATESERVICEFUNCTION)(const char *,MIRANDASERVICE);
typedef HANDLE (*CREATETRANSIENTSERVICEFUNCTION)(const char *,MIRANDASERVICE);
typedef int (*DESTROYSERVICEFUNCTION)(HANDLE);
typedef int (*CALLSERVICE)(const char *,WPARAM,LPARAM);
typedef int (*SERVICEEXISTS)(const char *);		  //v0.1.0.1+
typedef int (*CALLSERVICESYNC)(const char *,WPARAM,LPARAM);		//v0.3.3+
typedef int (*CALLFUNCTIONASYNC) (void (__stdcall *)(void *), void *);	//v0.3.4+
typedef int (*SETHOOKDEFAULTFORHOOKABLEEVENT) (HANDLE, MIRANDAHOOK); // v0.3.4 (2004/09/15)
typedef HANDLE (*CREATESERVICEFUNCTIONPARAM)(const char *,MIRANDASERVICEPARAM,LPARAM); // v0.7+ (2007/04/24)
typedef int (*NOTIFYEVENTHOOKSDIRECT)(HANDLE,WPARAM,LPARAM); // v0.7+

#endif

void InitReplacements();
void DestroyReplacements();

extern CREATEHOOKABLEEVENT realCreateHookableEvent;
extern DESTROYHOOKABLEEVENT realDestroyHookableEvent;
extern NOTIFYEVENTHOOKS realNotifyEventHooks;
extern HOOKEVENT realHookEvent;
extern HOOKEVENTMESSAGE realHookEventMessage;
extern UNHOOKEVENT realUnhookEvent;

extern CREATESERVICEFUNCTION realCreateServiceFunction;
extern CREATETRANSIENTSERVICEFUNCTION realCreateTransientServiceFunction;
extern CREATESERVICEFUNCTIONPARAM realCreateServiceFunctionParam;
extern DESTROYSERVICEFUNCTION realDestroyServiceFunction;
extern CALLSERVICE realCallService;
extern CALLSERVICESYNC realCallServiceSync;

HANDLE BridgeCreateHookableEvent(const char *hookableEvent);
int BridgeDestroyHookableEvent(HANDLE hEvent);
int BridgeNotifyEventHooks(HANDLE hEvent, WPARAM wParam, LPARAM lParam);
HANDLE BridgeHookEvent(const char *hookableEvent, MIRANDAHOOK hook);
HANDLE BridgeHookEventMessage(const char *hookableEvent, HWND hWnd, UINT msg);
int BridgeUnhookEvent(HANDLE hEvent);

HANDLE BridgeCreateServiceFunction(const char *service, MIRANDASERVICE function);
HANDLE BridgeCreateTransientServiceFunction(const char *service, MIRANDASERVICE function);
HANDLE BridgeCreateServiceFunctionParam(const char *service, MIRANDASERVICEPARAM functionParam, LPARAM lParam);
int BridgeDestroyServiceFunction(HANDLE hService);

int BridgeCallService(const char *service, WPARAM wParam, LPARAM lParam);
int BridgeCallServiceSync(const char *service, WPARAM wParam, LPARAM lParam);

#endif