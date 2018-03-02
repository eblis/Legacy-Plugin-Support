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

#include "commonheaders.h"

HINSTANCE hInstance;

PLUGINLINK *pluginLink;

HMODULE hLegacyPlugin = NULL;
PLUGININFOEX *pluginInfo = NULL;

char ModuleName[] = "LPS Bridge";

MUUID interfaces[MAX_INTERFACES + 1 + 1] = {0};

int PlugPluginUUIDAndInterfaces(PLUGININFOEX *pluginInfo)
{
	MUUID uuid = {0};
	
	int cInterfaces = GetPluginUUIDAndInterfaces(pluginInfo->shortName, &uuid, interfaces, MAX_INTERFACES);
	
	if ((!IsUUIDNull(uuid)) && (cInterfaces > 0))
	{
		pluginInfo->cbSize = sizeof(PLUGININFOEX);
		pluginInfo->uuid = uuid;
		
		for (int i = 0; i < cDefaultInterfaces; i++)
		{
			if (pluginInfo->replacesDefaultModule == defaultInterfaces[i].defMod)
			{
				interfaces[cInterfaces++] = defaultInterfaces[i].defModInterface;
			}
		}
		
		interfaces[cInterfaces++] = UUID_NULL;
		
		return 1;
	}
	else{
		MessageBox(0, "The plugin does not have a valid UUID set or the interfaces are not valid.\nPlease update the legacy configuration file.\nBridge plugin cannot be used.", pluginInfo->shortName, MB_OK | MB_ICONERROR);
	}
	
	return 0;
}

int IsValidLegacyPlugin()
{
	FARPROC proc = NULL;
	if (GetProcAddress(hLegacyPlugin, "MirandaPluginInfoEx") || (GetProcAddress(hLegacyPlugin, "MirandaPluginInterfaces")))
	{
		return 0;
	}
	
	return ((GetProcAddress(hLegacyPlugin, "MirandaPluginInfo")) && (GetProcAddress(hLegacyPlugin, "Load")) && (GetProcAddress(hLegacyPlugin, "Unload")));
}

int LoadBridgedPlugin()
{
	char path[1024];
	GetModuleFileName(hInstance, path, sizeof(path));
	
	DWORD mirandaVersion = PLUGIN_MAKE_VERSION(0, 7, 3, 0);

	PLUGININFOEX *(*MirandaPluginInfo)(DWORD);
	
	strncat(path, ".", sizeof(path));
	strncat(path, LEGACY_EXTENSION, sizeof(path));
	
	hLegacyPlugin = LoadLibrary(path);
	
	if (hLegacyPlugin != INVALID_HANDLE_VALUE)
	{
		if (IsValidLegacyPlugin()) //we're interested only in legacy plugins
		{
			MirandaPluginInfo = (PLUGININFOEX *(*)(DWORD))GetProcAddress(hLegacyPlugin, "MirandaPluginInfo");
			if (MirandaPluginInfo) //doesn't have the Ex version but has the old one, a legacy plugin
			{
				pluginInfo = CopyPluginInfo(MirandaPluginInfo(mirandaVersion));
				if (pluginInfo)
				{
					if (PlugPluginUUIDAndInterfaces(pluginInfo))
					{
						char *displayName = (char *) malloc(strlen(pluginInfo->shortName) + 50); //should be enough
						strcpy(displayName, pluginInfo->shortName);
						strcat(displayName, BRIDGED_SUFFIX);
						
						free(pluginInfo->shortName);
						pluginInfo->shortName = displayName;
					}
					
					return 0;
				}
			}
		}
	}
	
	return 1;
}

extern "C" __declspec(dllexport) PLUGININFOEX *MirandaPluginInfoEx(DWORD mirandaVersion) 
{
	if (mirandaVersion < PLUGIN_MAKE_VERSION(0, 8, 0, 9))
	{
		MessageBox(0, "The Bridge plugin is not required for Miranda versions prior to 0.8.0.9", "Error", MB_OK | MB_ICONEXCLAMATION);
		
		return NULL;
	}
	
	return pluginInfo;
}

extern "C" __declspec(dllexport) const MUUID *MirandaPluginInterfaces()
{
	return interfaces;
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	
	InitReplacements();
	
	int (*LegacyLoad)(PLUGINLINK *);
	
	LegacyLoad = (int (*)(PLUGINLINK *)) GetProcAddress(hLegacyPlugin, "Load");
	
	if (LegacyLoad)
	{
		LegacyLoad(pluginLink);
	}

	return 0;
}

extern "C" int __declspec(dllexport) Unload()
{
	int (*LegacyUnload)();
	LegacyUnload = (int (*)()) GetProcAddress(hLegacyPlugin, "Unload");
	
	if (LegacyUnload)
	{
		LegacyUnload();
	}
	
	return 0;
}

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hInstance = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH)
		{
			DisableThreadLibraryCalls(hinstDLL);
		}
	
	LoadBridgedPlugin();
	
	return TRUE;
}
