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


#include "utils.h"

InterfaceTable defaultInterfaces[] = {{DEFMOD_UIFINDADD, MIID_UIFINDADD, "MIID_UIFINDADD"},       {DEFMOD_UIUSERINFO, MIID_UIUSERINFO, "MIID_UIUSERINFO"},
																			{DEFMOD_SRMESSAGE, MIID_SRMM, "MIID_SRMM"},                 {DEFMOD_SRURL, MIID_SRURL, "MIID_SRURL"},
																			{DEFMOD_SREMAIL, MIID_SREMAIL, "MIID_SREMAIL"},             {DEFMOD_SRAUTH, MIID_SRAUTH, "MIID_SRAUTH"},
																			{DEFMOD_SRFILE, MIID_SRFILE, "MIID_SRFILE"},                {DEFMOD_UIHELP, MIID_UIHELP, "MIID_UIHELP"},
																			{DEFMOD_UIHISTORY, MIID_UIHISTORY, "MIID_UIHISTORY"},       {DEFMOD_SRAWAY, MIID_SRAWAY, "MIID_SRAWAY"},
																			{DEFMOD_CLISTALL, MIID_CLIST, "MIID_CLIST"},                {DEFMOD_DB, MIID_DATABASE, "MIID_DATABASE"},
																			{DEFMOD_FONTSERVICE, MIID_FONTSERVICE, "MIID_FONTSERVICE"}, {DEFMOD_UPDATENOTIFY, MIID_UPDATENOTIFY, "MIID_UPDATENOTIFY"},
																		 };

int cDefaultInterfaces = sizeof(defaultInterfaces) / sizeof(defaultInterfaces[0]);


int LogInit()
{
#ifdef USE_LOG
	//FILE *fout = fopen(LOG_FILE, "wt");
	//fclose(fout);
	Log("********************** Miranda started **********************");
#endif
	return 0;
}

int Log(char *format, ...)
{
#ifdef USE_LOG
	char		str[4096];
	va_list	vararg;
	int tBytes;
	FILE *fout = fopen(LOG_FILE, "at");
	if (!fout)
		{
//			MessageBox(0, "can't open file", NULL, MB_OK);
			return -1;
		}
	time_t tNow = time(NULL);
	struct tm *now = localtime(&tNow);
	strftime(str, sizeof(str), "%d %b %Y @ %H:%M:%S: ", now);
	fputs(str, fout);
	va_start(vararg, format);
	
	tBytes = _vsnprintf(str, sizeof(str), format, vararg);
	if (tBytes > 0)
		{
			str[tBytes] = 0;
		}

	va_end(vararg);
	if (str[strlen(str) - 1] != '\n')
		{
			strcat(str, "\n");
		}
	fputs(str, fout);
	fclose(fout);
#endif
	return 0;
}

int Info(char *title, char *format, ...)
{
	char str[4096];
	va_list vararg;
	int tBytes;
	va_start(vararg, format);
	tBytes = _vsnprintf(str, sizeof(str), format, vararg);
	if (tBytes > 0)
		{
			str[tBytes] = 0;
		}
	va_end(vararg);
	return MessageBoxA(0, str, title, MB_OK | MB_ICONINFORMATION);		
}

int MyPUShowMessage(char *lpzText, BYTE kind)
{
	if (ServiceExists(MS_POPUP_SHOWMESSAGE))
	{
		return PUShowMessage(lpzText, kind);
	}
	else{
		char *title = (kind == SM_NOTIFY) ? Translate("Notify") : Translate("Warning");
		
		return MessageBox(NULL, lpzText, title, MB_OK | (kind == SM_NOTIFY) ? MB_ICONINFORMATION : MB_ICONEXCLAMATION);
	}
}

#define HEX_SIZE 8

char *BinToHex(int size, PBYTE data)
{
	char *szresult = NULL;
	char buffer[32] = {0}; //should be more than enough
	int maxSize = size * 2 + HEX_SIZE + 1;
	szresult = (char *) new char[ maxSize ];
	memset(szresult, 0, maxSize);
	sprintf(buffer, "%0*X", HEX_SIZE, size);
	strcpy(szresult, buffer);
	int i;
	for (i = 0; i < size; i++)
		{
			sprintf(buffer, "%02X", data[i]);
			strcpy(szresult + (HEX_SIZE + i * 2), buffer);
		}
	return szresult; 
}

void HexToBin(char *inData, ULONG &size, LPBYTE &outData)
{
	char buffer[32] = {0};
	strcpy(buffer, "0x");
	STRNCPY(buffer + 2, inData, HEX_SIZE);
	sscanf(buffer, "%x", &size);
	outData = (unsigned char*)new char[size*2];
	UINT i;
	//size = i;
	char *tmp = inData + HEX_SIZE;
	buffer[4] = '\0'; //mark the end of the string
	for (i = 0; i < size; i++)
		{
			STRNCPY(buffer + 2, &tmp[i * 2], 2);
			sscanf(buffer, "%x", &outData[i]);
		}
	i = size;
}

int GetStringFromDatabase(HANDLE hContact, char *szModule, char *szSettingName, char *szError, char *szResult, int size)
{
	DBVARIANT dbv = {0};
	int res = 1;
	int len;
	dbv.type = DBVT_ASCIIZ;
	if (DBGetContactSetting(hContact, szModule, szSettingName, &dbv) == 0)
		{
			res = 0;
			int tmp = strlen(dbv.pszVal);
			len = (tmp < size - 1) ? tmp : size - 1;
			strncpy(szResult, dbv.pszVal, len);
			szResult[len] = '\0';
			MirandaFree(dbv.pszVal);
		}
		else{
			res = 1;
			if (szError)
				{
					int tmp = strlen(szError);
					len = (tmp < size - 1) ? tmp : size - 1;
					strncpy(szResult, szError, len);
					szResult[len] = '\0';
				}
				else{
					szResult[0] = '\0';
				}
		}
	return res;
}

int GetStringFromDatabase(HANDLE hContact, char *szModule, char *szSettingName, WCHAR *szError, WCHAR *szResult, int count)
{
	DBVARIANT dbv = {0};
	int res = 1;
	int len;
	dbv.type = DBVT_WCHAR;
	if (DBGetContactSettingWString(hContact, szModule, szSettingName, &dbv) == 0)
		{
			res = 0;
			if (dbv.type != DBVT_WCHAR)
			{
				MultiByteToWideChar(CP_ACP, 0, dbv.pszVal, -1, szResult, count);
			}
			else{
				int tmp = wcslen(dbv.pwszVal);
				len = (tmp < count - 1) ? tmp : count - 1;
				wcsncpy(szResult, dbv.pwszVal, len);
				szResult[len] = L'\0';
			}
			MirandaFree(dbv.pwszVal);
		}
		else{
			res = 1;
			if (szError)
				{
					int tmp = wcslen(szError);
					len = (tmp < count - 1) ? tmp : count - 1;
					wcsncpy(szResult, szError, len);
					szResult[len] = L'\0';
				}
				else{
					szResult[0] = L'\0';
				}
		}
	return res;
}

int GetStringFromDatabase(char *szSettingName, char *szError, char *szResult, int size)
{
	return GetStringFromDatabase(NULL, ModuleName, szSettingName, szError, szResult, size);
}

int GetStringFromDatabase(char *szSettingName, WCHAR *szError, WCHAR *szResult, int count)
{
	return GetStringFromDatabase(NULL, ModuleName, szSettingName, szError, szResult, count);
}

#pragma warning (disable: 4312) 
TCHAR *GetContactName(HANDLE hContact, char *szProto)
{
	CONTACTINFO ctInfo;
	int ret;
	char proto[200];
	
	ZeroMemory((void *) &ctInfo, sizeof(ctInfo));	
	ctInfo.cbSize = sizeof(ctInfo);
	if (szProto)
		{
			ctInfo.szProto = szProto;
		}
		else{
			GetContactProtocol(hContact, proto, sizeof(proto));
			ctInfo.szProto = proto;
		}
	ctInfo.dwFlag = CNF_DISPLAY;
#ifdef _UNICODE
	ctInfo.dwFlag += CNF_UNICODE;
#endif	
	ctInfo.hContact = hContact;
	//_debug_message("retrieving contact name for %d", hContact);
	ret = CallService(MS_CONTACT_GETCONTACTINFO, 0, (LPARAM) &ctInfo);
	//_debug_message("	contact name %s", ctInfo.pszVal);
	TCHAR *buffer;
	if (!ret)
		{
			buffer = _tcsdup(ctInfo.pszVal);
		}
	MirandaFree(ctInfo.pszVal);
	if (!ret)
		{
			return buffer;
		}
		else{
			return NULL;
		}
	return buffer;
}
#pragma warning (default: 4312)

#pragma warning (disable: 4312) 
void GetContactProtocol(HANDLE hContact, char *szProto, int size)
{
	GetStringFromDatabase(hContact, "Protocol", "p", NULL, szProto, size);
}
#pragma warning (default: 4312)

#pragma warning (disable: 4312)
TCHAR *GetContactID(HANDLE hContact)
{
	char protocol[256];
	GetContactProtocol(hContact, protocol, sizeof(protocol));

	return GetContactID(hContact, protocol);
}

TCHAR *GetContactID(HANDLE hContact, char *szProto)
{
	CONTACTINFO ctInfo;
	int ret;

	ZeroMemory((void *) &ctInfo, sizeof(ctInfo));	
	ctInfo.cbSize = sizeof(ctInfo);
	ctInfo.szProto = szProto;
	ctInfo.dwFlag = CNF_UNIQUEID;
#ifdef _UNICODE
	ctInfo.dwFlag |= CNF_UNICODE;
#endif
	ctInfo.hContact = hContact;
	ret = CallService(MS_CONTACT_GETCONTACTINFO, 0, (LPARAM) &ctInfo);
	TCHAR *buffer;
	if (!ret)
		{
			TCHAR tmp[16];
			switch (ctInfo.type)
				{
					case CNFT_BYTE:
						{
							_stprintf(tmp, _T("%d"), ctInfo.bVal);
							buffer = _tcsdup(tmp);
						
							break;
						}
						
					case CNFT_WORD:
						{
							_stprintf(tmp, _T("%d"), ctInfo.wVal);
							buffer = _tcsdup(tmp);
						
							break;
						}
						
					case CNFT_DWORD:
						{
							_stprintf(tmp, _T("%ld"), ctInfo.dVal);
							buffer = _tcsdup(tmp);
							
							break;
						}
						
					case CNFT_ASCIIZ:
					default:
						{
							buffer = _tcsdup(ctInfo.pszVal);
							
							break;
						}
				}
				

		}
	MirandaFree(ctInfo.pszVal);
	if (!ret)
		{
			return buffer;
		}
		else{
			return NULL;
		}
}
#pragma warning (default: 4312)

#pragma warning (disable: 4312)
HANDLE GetContactFromID(TCHAR *szID, char *szProto)
{
	HANDLE hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	TCHAR *szHandle;
	TCHAR dispName[1024];
	char cProtocol[256];
	char *tmp;

	int found = 0;
	while (hContact)
	{
		GetContactProtocol(hContact, cProtocol, sizeof(cProtocol));
		szHandle = GetContactID(hContact, cProtocol);
		
		tmp = (char *) CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) hContact, 0);
		STRNCPY(dispName, tmp, sizeof(dispName));
		
		if ((szHandle) && ((_tcsicmp(szHandle, szID) == 0) || (_tcsicmp(dispName, szID) == 0)) && ((szProto == NULL) || (_stricmp(szProto, cProtocol) == 0)))
		{
			found = 1;
		}
		if (szHandle) { free(szHandle); }
	
		if (found) { break; }
		hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM) hContact, 0);
	}
	
	return hContact;
}
#pragma warning (default: 4312)

#pragma warning (disable: 4312)
HANDLE GetContactFromID(TCHAR *szID, wchar_t *szProto)
{
	char protocol[1024];
	WideCharToMultiByte(CP_ACP, 0, szProto, -1, protocol, sizeof(protocol), NULL, NULL);
	return GetContactFromID(szID, protocol);
}
#pragma warning (default: 4312)

void ScreenToClient(HWND hWnd, LPRECT rect)
{
	POINT pt;
	int cx = rect->right - rect->left;
	int cy = rect->bottom - rect->top;
	pt.x = rect->left;
	pt.y = rect->top;
	ScreenToClient(hWnd, &pt);
	rect->left = pt.x;
	rect->top = pt.y;
	rect->right = pt.x + cx;
	rect->bottom = pt.y + cy;
}

void AnchorMoveWindow(HWND window, const WINDOWPOS *parentPos, int anchors)
{
	RECT rParent;
	RECT rChild;
	
	if (parentPos->flags & SWP_NOSIZE)
		{
			return;
		}
	GetWindowRect(parentPos->hwnd, &rParent);
	rChild = AnchorCalcPos(window, &rParent, parentPos, anchors);
	MoveWindow(window, rChild.left, rChild.top, rChild.right - rChild.left, rChild.bottom - rChild.top, FALSE);
}

RECT AnchorCalcPos(HWND window, const RECT *rParent, const WINDOWPOS *parentPos, int anchors)
{
	RECT rChild;
	RECT rTmp;

	GetWindowRect(window, &rChild);
	ScreenToClient(parentPos->hwnd, &rChild);

	int cx = rParent->right - rParent->left;
	int cy = rParent->bottom - rParent->top;
	if ((cx == parentPos->cx) && (cy == parentPos->cy))
		{
			return rChild;
		}
	if (parentPos->flags & SWP_NOSIZE)
		{
			return rChild;
		}

	rTmp.left = parentPos->x - rParent->left;
	rTmp.right = (parentPos->x + parentPos->cx) - rParent->right;
	rTmp.bottom = (parentPos->y + parentPos->cy) - rParent->bottom;
	rTmp.top = parentPos->y - rParent->top;
	
	cx = (rTmp.left) ? -rTmp.left : rTmp.right;
	cy = (rTmp.top) ? -rTmp.top : rTmp.bottom;	
	
	rChild.right += cx;
	rChild.bottom += cy;
	//expanded the window accordingly, now we need to enforce the anchors
	if ((anchors & ANCHOR_LEFT) && (!(anchors & ANCHOR_RIGHT)))
		{
			rChild.right -= cx;
		}
	if ((anchors & ANCHOR_TOP) && (!(anchors & ANCHOR_BOTTOM)))
		{
			rChild.bottom -= cy;
		}
	if ((anchors & ANCHOR_RIGHT) && (!(anchors & ANCHOR_LEFT)))
		{
			rChild.left += cx;
		}
	if ((anchors & ANCHOR_BOTTOM) && (!(anchors & ANCHOR_TOP)))
		{
			rChild.top += cy;
		}
	return rChild;
}

inline char *STRNCPY(char *output, const char *input, size_t size)
{
	char *res = strncpy(output, input, size);
	output[size - 1] = 0;
	
	return res;
}

BOOL IsUUIDNull(MUUID uuid)
{
	int i;
	for (i = 0; i < sizeof(uuid.d); i++)
	{
		if (uuid.d[i])
		{
			return 0;
		}
	}
	
	return ((uuid.a == 0) && (uuid.b == 0) && (uuid.c == 0));
}

BOOL UUIDsEqual(MUUID uuid1, MUUID uuid2)
{
	int i;
	
	if ((uuid1.a != uuid2.a) || (uuid1.b != uuid2.b) || (uuid1.c != uuid2.c))
	{
		return 0;
	}
	
	for (i = 0; i < sizeof(uuid1.d); i++)
	{
		if (uuid1.d[i] != uuid2.d[i])
		{
			return 0;
		}
	}
	
	return 1;
}


char *StrCopy(char *source, int index, const char *what, int count)
{
	int i;
	for (i = 0; i < count; i++)
		{
			source[index + i] = what[i];
		}
	return source;
}

char *StrDelete(char *source, int index, int count)
{
	int len = strlen(source);
	int i;
	count = (count + index > len) ? len - index : count;
	for (i = index; i + count <= len; i++)
		{
			source[i] = source[i + count];
		}
	return source;
}

char *StrInsert(char *source, int index, const char *what)
{
	int whatLen = strlen(what);
	int sourceLen = strlen(source);
	int i;
	for (i = sourceLen; i >= index; i--)
		{
			source[i + whatLen] = source[i];
		}
	for (i = 0; i < whatLen; i++)
		{
			source[index + i] = what[i];
		}
	return source;
}

char *StrReplace(char *source, const char *what, const char *withWhat)
{
	char *pos;
	int whatLen = strlen(what);
	int withWhatLen = strlen(withWhat);
	int minLen;
	int index;
	
	while ((pos = strstr(source, what)))
		{
			minLen = min(whatLen, withWhatLen);
			StrCopy(source, pos - source, withWhat, minLen); 
			index = pos - source + minLen;
			if (whatLen > withWhatLen)
				{
					StrDelete(source, index, whatLen - withWhatLen);
				}
				else{
					if (whatLen < withWhatLen)
						{
							StrInsert(source, index, withWhat + minLen);
						}
				}
		}
	return source;
}

char *StrTrim(char *szText, const char *szTrimChars)
{
	int i = strlen(szText) - 1;
	while ((i >= 0) && (strchr(szTrimChars, szText[i])))
		{
			szText[i--] = '\0';
		}
	i = 0;
	while (((unsigned int )i < strlen(szText)) && (strchr(szTrimChars, szText[i])))
		{
			i++;
		}
	if (i)
		{
			StrDelete(szText, 0, i);
		}
	return szText;
}

int GetMirandaPluginsFolder(char *pluginsFolder, int size)
{
	if (!pluginsFolder)
	{
		return 1;
	}
	
	GetModuleFileName(NULL, pluginsFolder, size); //get miranda's folder
	int i = strlen(pluginsFolder);
	while ((pluginsFolder[i - 1] != '\\') && (i > 0)) { i--; }
	if (i <= 0)
	{
		*pluginsFolder = 0;
		return 1;
	}
	
	pluginsFolder[i] = 0;
	strncat(pluginsFolder, "Plugins", size); //miranda path\plugins
	
	return 0;
}

//From Egodust or Cyreve... I don't really know.
PLUGININFOEX *CopyPluginInfo(PLUGININFOEX *piSrc)
{
	PLUGININFOEX *pi;
	if(piSrc==NULL) return NULL;
	pi=(PLUGININFOEX *)malloc(sizeof(PLUGININFOEX));
	
	*pi=*piSrc;	
	pi->uuid = UUID_NULL;
	
	if (piSrc->cbSize >= sizeof(PLUGININFOEX))
	{
		pi->uuid = piSrc->uuid;
	}
	
	if (piSrc->cbSize >= sizeof(PLUGININFO))
	{
		if(pi->author) pi->author=_strdup(pi->author);
		if(pi->authorEmail) pi->authorEmail=_strdup(pi->authorEmail);
		if(pi->copyright) pi->copyright=_strdup(pi->copyright);
		if(pi->description) pi->description=_strdup(pi->description);
		if(pi->homepage) pi->homepage=_strdup(pi->homepage);
		if(pi->shortName) pi->shortName=_strdup(pi->shortName);
	}
	
	return pi;
}

void FreePluginInfo(PLUGININFOEX *pi)
{
	if(pi->author) free(pi->author);
	if(pi->authorEmail) free(pi->authorEmail);
	if(pi->copyright) free(pi->copyright);
	if(pi->description) free(pi->description);
	if(pi->homepage) free(pi->homepage);
	if(pi->shortName) free(pi->shortName);
	free(pi);
}

int PluginNameMatches(char *line, char *displayName)
{
	if (strstr(line, displayName) == line) //the display name is at the start of the line
	{
		return 1;
	}
	
	return 0;
}

int GetUUID(char *line, char uuid[], int size)
{
	char *start = strchr(line, '{');
	if (start)
	{
		char *finish = strchr(start, '}');
		if (finish)
		{
			*finish = 0;
			mir_snprintf(uuid, size, "%s", start + 1);
			*finish = '}';
			
			return (start - line);
		}
	}
	
	return 0;
}

MUUID CharToUUID(char *uuid)
{
	unsigned long data1 = 0;
	unsigned short data2 = 0;
	unsigned short data3 = 0;
	unsigned short data4[8] = {0};
	
	int count = sscanf(uuid, "%lx-%hx-%hx-%2hx%2hx-%2hx%2hx%2hx%2hx%2hx%2hx", &data1, &data2, &data3, &data4[0], &data4[1], &data4[2], &data4[3], &data4[4], &data4[5], &data4[6], &data4[7]);
	
	if (count >= 11)
	{
		MUUID tmp = {data1, data2, data3, (char) data4[0], (char) data4[1], (char) data4[2], (char) data4[3], (char) data4[4], (char) data4[5], (char) data4[6], (char) data4[7]};
		return tmp;
	}
	
	return UUID_NULL;
}

BOOL UUIDToString(MUUID uuid, char *str, int len)
{
	if ((len < sizeof(MUUID)) || (!str))
	{
		return 0;
	}
	
	mir_snprintf(str, len, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", uuid.a, uuid.b, uuid.c, uuid.d[0], uuid.d[1], uuid.d[2], uuid.d[3], uuid.d[4], uuid.d[5], uuid.d[6], uuid.d[7]);
	
	return 1;
}

int GetPluginUUIDAndInterfaces(char *displayName, MUUID *uuid, MUUID interfaces[], int maxInterfaces)
{
	char path[1024];
	GetMirandaPluginsFolder(path, sizeof(path));
	strncat(path, "\\", sizeof(path));
	strncat(path, CONFIGURATION_FILE, sizeof(path));
	*uuid = UUID_NULL;
	interfaces[0] = UUID_NULL;
	char *search;
	
	int countInterfaces = 0;
	
	FILE *fin = fopen(path, "rt");
	if (fin)
	{
		char line[4096] = {0};
		char strUUID[256] = {0};
		MUUID tmp = {0};
		int index = 0;
		int found = 0;
		while ((!found) && (!feof(fin)))
		{
			fgets(line, sizeof(line), fin);
			search = line;
			StrTrim(search, " \t\r\n");
			if ((strlen(search) > 0) && (search[0] != COMMENT_CHAR))
			{
				if (PluginNameMatches(search, displayName)) //we found the plugin we were looking for
				{
					found = 1;
					index = GetUUID(search, strUUID, sizeof(strUUID));
					if (index) //there's an uuid in the file
					{
						tmp = CharToUUID(strUUID);
						if (!IsUUIDNull(tmp))
						{
							*uuid = tmp;
							
							while (index > 0)
							{
								search += index + 1; //move past the uuid, get the interfaces
								StrTrim(search, " \t\r\n");
								index = GetUUID(search, strUUID, sizeof(strUUID));
								if (index)
								{
									tmp = CharToUUID(strUUID);
									if (!IsUUIDNull(tmp))
									{
										if (countInterfaces < maxInterfaces)
										{
											interfaces[countInterfaces++] = tmp;
										}
									}
									else{
										break; //stop searching for interfaces
									}
								}
							} //while index > 0
						}
					}
				} //if plugin name matches
			} //if !comment
		} //while !found
	
		fclose(fin);
	}
	
	return countInterfaces;
}

void ManagePlugin(char *fileName, int bManage)
{
	char oldPath[1024];
	char newPath[1024];
	char templatePath[1024];
	GetMirandaPluginsFolder(oldPath, sizeof(oldPath));
	strncat(oldPath, "\\", sizeof(oldPath));
	strcpy(newPath, oldPath);
	strcpy(templatePath, oldPath);
	
	strncat(templatePath, BRIDGE_TEMPLATE, sizeof(templatePath));
	
	int ok = 0;
	
	strncat(oldPath, fileName, sizeof(oldPath));
	
	char newFile[512];
	strncpy(newFile, fileName, sizeof(newFile));
	
	char *dot = strrchr(newFile, '.');
	if (dot)
	{
		char extension[128];
		strncpy(extension, dot + 1, sizeof(extension));
		_strlwr(extension);
		int legacy = (strcmp(extension, LEGACY_EXTENSION) == 0);
		
		if (bManage)
		{
			if (!legacy)
			{
				strncat(newFile, ".", sizeof(newFile));
				strncat(newFile, LEGACY_EXTENSION, sizeof(newFile));
				
				ok = 1;
			}
		}
		else{
			if (legacy)
			{
				*dot = 0;
				
				char *p = strrchr(newFile, '.');
				if (!p)
				{
					strncat(newFile, ".dll", sizeof(newFile));
				}
				
				ok = 1;
			}
		}
	}
	
	if (ok)
	{
		strncat(newPath, newFile, sizeof(newPath));
		
		if (!bManage)
		{
			DeleteFile(newPath);
		}
		
		MoveFile(oldPath, newPath);
		
		if (bManage)
		{
			CopyFile(templatePath, oldPath, TRUE);
		}
	}
}