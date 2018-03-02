/*
Legacy Plugin Support plugin for Miranda IM

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

#include "dlg_handlers.h"
#include "commctrl.h"

#define CMP(a, b) ( ((a == b) ? 0 : ((a < b) ? -1 : 1)) )

HWND hOptDlg = NULL;

const char *szColumns[] = {"Plugin", "Name", "Version"};
const int cxColumns[] = {150, 240, 80};
const int cColumns = sizeof(szColumns) / sizeof(szColumns[0]);

WNDPROC oldPluginsListProc = NULL;

static int ValidExtension(char *fileName, char *extension)
{
	char *dot = strrchr(fileName, '.');
	if ((dot != NULL) && (lstrcmpiA(dot + 1, extension) == 0))
	{
		if (dot[strlen(extension) + 1] == 0)
		{
			return 1;
		}
	}

	return 0;
}

int LoadColumns(HWND hList)
{
	int i;
	int start = 0;
	LVCOLUMN col;
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	
	while (ListView_DeleteColumn(hList, 1)) //delete all columns
	{
		start = 1;
	}
	
	for (i = start; i < cColumns; i++)
	{
		col.pszText = TranslateTS(szColumns[i]);
	
		col.cx = cxColumns[i];
	
		ListView_InsertColumn(hList, i, &col);
	}
	
	return 0;
}

int AddPlugin(HWND hList, char *fileName, char *displayName, DWORD version, int enabled)
{
	LVITEM item = {0};
	item.mask = LVIF_TEXT | LVIF_PARAM;
	
	int index = ListView_GetItemCount(hList);
	
	item.iItem = index;
	item.lParam = enabled;
	item.pszText = fileName;

	ListView_InsertItem(hList, &item);
	ListView_SetCheckState(hList, index, enabled);
	ListView_SetItemText(hList, index, 1, displayName);
	
	char aux[128];
	wsprintf(aux,"%d.%d.%d.%d",	(version>>24)&0xFF, (version>>16)&0xFF, (version>>8)&0xFF, (version)&0xFF);
	ListView_SetItemText(hList, index, 2, aux);
	
	return 0;
}

int LoadPlugins(HWND hList, char *pluginsFolder, char *extension, int enabled)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	HINSTANCE hInstPlugin = NULL;
	
	DWORD mirandaVersion = 0;
	
	mirandaVersion=(DWORD)CallService(MS_SYSTEM_GETVERSION,0,0);
	
	PLUGININFOEX *(*MirandaPluginInfo)(DWORD);
	PLUGININFOEX *pluginInfo = NULL;
	
	char search[1024];
	mir_snprintf(search, sizeof(search), "%s\\*.%s", pluginsFolder, extension);
	
	hFind = FindFirstFile(search, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!ValidExtension(fd.cFileName, extension))
			{
				continue;
			}
		
			char pp[1024];
			mir_snprintf(pp, sizeof(pp), "%s\\%s", pluginsFolder, fd.cFileName);
			
			hInstPlugin = LoadLibrary(pp);
			if (hInstPlugin) //did the dll load ?
			{
				MirandaPluginInfo = (PLUGININFOEX *(*)(DWORD))GetProcAddress(hInstPlugin, "MirandaPluginInfoEx");
				
				if (!MirandaPluginInfo) //we're interested only in legacy plugins
				{
					MirandaPluginInfo = (PLUGININFOEX *(*)(DWORD))GetProcAddress(hInstPlugin, "MirandaPluginInfo");
					if (MirandaPluginInfo) //doesn't have the Ex version but has the old one, a legacy plugin
					{
						pluginInfo = CopyPluginInfo(MirandaPluginInfo(mirandaVersion));
						AddPlugin(hList, fd.cFileName, pluginInfo->shortName, pluginInfo->version, enabled);
						FreePluginInfo(pluginInfo);
					}
				}
				
				FreeLibrary(hInstPlugin); //unload the library
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	
	return 0;
}

int CALLBACK PluginsSort(LPARAM lParam1, LPARAM lParam2, LPARAM param)
{
	LVITEM item = {0};
	char first[512];
	char second[512];
	HWND hList = (HWND) param;
	
	item.mask = LVIF_TEXT;
	item.cchTextMax = sizeof(first);
	item.pszText = first;
	item.iItem = lParam1;
	ListView_GetItem(hList, &item);
	
	item.cchTextMax = sizeof(second);
	item.pszText = second;
	item.iItem = lParam2;
	ListView_GetItem(hList, &item);
	
	return _stricmp(first, second);
}

int LoadLegacyPlugins(HWND hList)
{
	char path[1024];
	GetMirandaPluginsFolder(path, sizeof(path));
	
	ListView_DeleteAllItems(hList);
	
	LoadPlugins(hList, path, "dll", 0);
	
	LoadPlugins(hList, path, LEGACY_EXTENSION, 1);
	
	ListView_SortItemsEx(hList, PluginsSort, hList);

	return 0;
}

void AddInterface(char *buffer, int size, MUUID uuid)
{
	int found = 0;
	for (int i = 0; i < cDefaultInterfaces; i++)
	{
		if (UUIDsEqual(uuid, defaultInterfaces[i].defModInterface))
		{
			strncat(buffer, defaultInterfaces[i].name, size);
			found = 1;
		}
	}
	
	if (!found)
	{
		char str[256];
		UUIDToString(uuid, str, sizeof(str));
		strncat(buffer, str, size);
	}
}

void LoadPluginUUIDs(HWND hWnd, int index)
{
	HWND hList = GetDlgItem(hWnd, IDC_LIST_PLUGINS);
	char displayName[1024] = {0};
	
	ListView_GetItemText(hList, index, 1, displayName, sizeof(displayName));
	
	MUUID interfaces[MAX_INTERFACES] = {0};
	MUUID uuid = {0};
	int cInterfaces = GetPluginUUIDAndInterfaces(displayName, &uuid, interfaces, MAX_INTERFACES);
	
	char buffer[1024] = {0};
	char str[256];
	
	UUIDToString(uuid, str, sizeof(str));
	SetWindowText(GetDlgItem(hWnd, IDC_UUID), str);
	
	for (int i = 0; i < cInterfaces - 1; i++)
	{
		AddInterface(buffer, sizeof(buffer), interfaces[i]);
		
		strncat(buffer, "\r\n", sizeof(buffer));
	}
	if (cInterfaces > 0)
	{
		AddInterface(buffer, sizeof(buffer), interfaces[cInterfaces - 1]);
	}
	
	SetWindowText(GetDlgItem(hWnd, IDC_INTERFACES), buffer);
}

int GetSelectedPluginIndex(HWND hWnd)
{
	int i;
	HWND hList = GetDlgItem(hWnd, IDC_LIST_PLUGINS);
	int count = ListView_GetItemCount(hList);
	for (i = 0; i < count; i++)
	{
		if (ListView_GetItemState(hList, i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			return i;
		}
	}
	
	return 0;
}

int ProcessLegacyPlugins(HWND hWnd)
{
	HWND hList = GetDlgItem(hWnd, IDC_LIST_PLUGINS);
	int i;
	int count = ListView_GetItemCount(hList);
	LVITEM item = {0};
	char fileName[512];
	char displayName[512];
	
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.cchTextMax = sizeof(fileName);
	item.pszText = fileName;
	
	int checked;
	int processed = 0;
	
	MUUID uuid;
	MUUID interfaces[MAX_INTERFACES];
	int cInterfaces;
	
	for (i = 0; i < count; i++)
	{
		checked = ListView_GetCheckState(hList, i);
		item.iItem = i;
		ListView_GetItem(hList, &item);
		
		if (checked != item.lParam) //item changed state
		{
			if (checked) //manage new plugin
			{
				ListView_GetItemText(hList, i, 1, displayName, sizeof(displayName));
				cInterfaces = GetPluginUUIDAndInterfaces(displayName, &uuid, interfaces, MAX_INTERFACES);
				if ((!IsUUIDNull(uuid)) && (cInterfaces > 0))
				{
					ManagePlugin(fileName, 1);
					processed = 1;
				}
				else{
					MessageBox(0, "Plugin does not have a valid UUID or valid interfaces", fileName, MB_OK | MB_ICONERROR);
					ListView_SetCheckState(hList, i, item.lParam); //reset state
				}
			}
			else{ //unmanage plugin
				ManagePlugin(fileName, 0);
				processed = 1;
			}
		}
	}
	
	return processed;
}

int CALLBACK DlgProcOptions(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int bInitializing = 0;
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			bInitializing = 1;
			hOptDlg = hWnd;
			HWND hList = GetDlgItem(hWnd, IDC_LIST_PLUGINS);
			
			TranslateDialogDefault(hWnd);

			ListView_SetExtendedListViewStyleEx(hList, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
			
			LoadColumns(hList);
			
			LoadLegacyPlugins(hList);
			
			bInitializing = 0;
			
			return TRUE;
			break;
		}
		
		case WM_DESTROY:
		{
			hOptDlg = NULL;
			
			break;
		}
		
		case WM_COMMAND:
		{
		
			break;
		}
		
		case WM_NOTIFY:
		{
			switch(((LPNMHDR)lParam)->idFrom)
			{
				case 0:
				{
					switch (((LPNMHDR)lParam)->code)
					{
						case PSN_APPLY:
						{
							if (ProcessLegacyPlugins(hWnd))
							{
								bInitializing = 1;
								LoadLegacyPlugins(GetDlgItem(hWnd, IDC_LIST_PLUGINS));
								bInitializing = 0;
							}
						
							break;
						}
					}
					
					break;
				}
				
				case IDC_LIST_PLUGINS:
				{
					switch (((LPNMHDR)lParam)->code)
					{
						case LVN_ITEMCHANGED:
						{
							NMLISTVIEW *data = (NMLISTVIEW *) lParam;
							if (data->uNewState & LVIS_SELECTED) //new item
							{
								LoadPluginUUIDs(hWnd, GetSelectedPluginIndex(hWnd));
							}
							
							if (!bInitializing)
							{
								int checked = ListView_GetCheckState(GetDlgItem(hWnd, IDC_LIST_PLUGINS), data->iItem) ? 1 : 0;
								if (data->lParam != checked)
								{
									SendMessage(GetParent(hWnd), PSM_CHANGED, 0, 0);
								}
							}
						
							break;
						}
					}
				
					break;
				}
			}
			
			break;
		}
	}
	
	return 0;
}