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

#include "replacements.h"

CREATEHOOKABLEEVENT realCreateHookableEvent = NULL;
DESTROYHOOKABLEEVENT realDestroyHookableEvent = NULL;
NOTIFYEVENTHOOKS realNotifyEventHooks = NULL;
HOOKEVENT realHookEvent = NULL;
HOOKEVENTMESSAGE realHookEventMessage = NULL;
UNHOOKEVENT realUnhookEvent = NULL;

CREATESERVICEFUNCTION realCreateServiceFunction = NULL;
CREATETRANSIENTSERVICEFUNCTION realCreateTransientServiceFunction = NULL;
CREATESERVICEFUNCTIONPARAM realCreateServiceFunctionParam = NULL;
DESTROYSERVICEFUNCTION realDestroyServiceFunction = NULL;
CALLSERVICE realCallService = NULL;
CALLSERVICESYNC realCallServiceSync = NULL;

void InitReplacements()
{
	//events
	realCreateHookableEvent = pluginLink->CreateHookableEvent;
	pluginLink->CreateHookableEvent = BridgeCreateHookableEvent;
	
	realDestroyHookableEvent = pluginLink->DestroyHookableEvent;
	pluginLink->DestroyHookableEvent = BridgeDestroyHookableEvent;
	
	realNotifyEventHooks = pluginLink->NotifyEventHooks;
	pluginLink->NotifyEventHooks = BridgeNotifyEventHooks;
	
	realHookEvent = pluginLink->HookEvent;
	pluginLink->HookEvent = BridgeHookEvent;
	
	realHookEventMessage = pluginLink->HookEventMessage;
	pluginLink->HookEventMessage = BridgeHookEventMessage;
	
	realUnhookEvent = pluginLink->UnhookEvent;
	pluginLink->UnhookEvent = BridgeUnhookEvent;
	
	//services
	realCreateServiceFunction = pluginLink->CreateServiceFunction;
	pluginLink->CreateServiceFunction = BridgeCreateServiceFunction;
	
	realCreateTransientServiceFunction = pluginLink->CreateTransientServiceFunction;
	pluginLink->CreateTransientServiceFunction = BridgeCreateTransientServiceFunction;
	
	realCreateServiceFunctionParam = pluginLink->CreateServiceFunctionParam;
	pluginLink->CreateServiceFunctionParam = BridgeCreateServiceFunctionParam;
	
	realDestroyServiceFunction = pluginLink->DestroyServiceFunction;
	pluginLink->DestroyServiceFunction = BridgeDestroyServiceFunction;
	
	realCallService = pluginLink->CallService;
	pluginLink->CallService = BridgeCallService;
	
	realCallServiceSync = pluginLink->CallServiceSync;
	pluginLink->CallServiceSync = BridgeCallServiceSync;
}

void DestroyReplacements()
{
		//events
	pluginLink->CreateHookableEvent = realCreateHookableEvent;
	
	pluginLink->DestroyHookableEvent = realDestroyHookableEvent ;
	
	pluginLink->NotifyEventHooks = realNotifyEventHooks ;
	
	pluginLink->HookEvent = realHookEvent;
	
	pluginLink->HookEventMessage = realHookEventMessage;
	
	pluginLink->UnhookEvent = realUnhookEvent;
	
	//services
	pluginLink->CreateServiceFunction = realCreateServiceFunction;
	
	pluginLink->CreateTransientServiceFunction = realCreateTransientServiceFunction;
	
	pluginLink->CreateServiceFunctionParam = realCreateServiceFunctionParam;
	
	pluginLink->DestroyServiceFunction = realDestroyServiceFunction;
	
	pluginLink->CallService = realCallService;
	
	pluginLink->CallServiceSync = realCallServiceSync;
}

//events
HANDLE BridgeCreateHookableEvent(const char *hookableEvent)
{
	return realCreateHookableEvent(hookableEvent);
}

int BridgeDestroyHookableEvent(HANDLE hEvent)
{
	return realDestroyHookableEvent(hEvent);
}

int BridgeNotifyEventHooks(HANDLE hEvent, WPARAM wParam, LPARAM lParam)
{
	return realNotifyEventHooks(hEvent, wParam, lParam);
}

HANDLE BridgeHookEvent(const char *hookableEvent, MIRANDAHOOK hook)
{
	return realHookEvent(hookableEvent, hook);
}

HANDLE BridgeHookEventMessage(const char *hookableEvent, HWND hWnd, UINT msg)
{
	return realHookEventMessage(hookableEvent, hWnd, msg);
}

int BridgeUnhookEvent(HANDLE hEvent)
{
	return realUnhookEvent(hEvent);
}

//services
HANDLE BridgeCreateServiceFunction(const char *service, MIRANDASERVICE function)
{
	return realCreateServiceFunction(service, function);
}

HANDLE BridgeCreateTransientServiceFunction(const char *service, MIRANDASERVICE function)
{
	return realCreateTransientServiceFunction(service, function);
}

HANDLE BridgeCreateServiceFunctionParam(const char *service, MIRANDASERVICEPARAM functionParam, LPARAM lParam)
{
	return realCreateServiceFunctionParam(service, functionParam, lParam);
}

int BridgeDestroyServiceFunction(HANDLE hService)
{
	return realDestroyServiceFunction(hService);
}

int BridgeCallService(const char *service, WPARAM wParam, LPARAM lParam)
{
	return realCallService(service, wParam, lParam);
}

int BridgeCallServiceSync(const char *service, WPARAM wParam, LPARAM lParam)
{
	return realCallServiceSync(service, wParam, lParam);
}