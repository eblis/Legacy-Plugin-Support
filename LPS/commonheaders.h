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

#ifndef M_LPS_COMMONHEADERS_H
#define M_LPS_COMMONHEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#include "version.h"
#include "mirandaMem.h"
#include "services.h"
#include "dlg_handlers.h"

#include "resource.h"

#include "../../../include/newpluginapi.h"
#include "../../../include/m_database.h"
#include "../../../include/m_system.h"
#include "../../../include/m_clist.h"
#include "../../../include/m_clui.h"
#include "../../../include/m_contacts.h"
#include "../../../include/m_langpack.h"
#include "../../../include/m_options.h"
#include "../../../include/m_protosvc.h"
#include "../../../include/m_protocols.h"
#include "../../../include/m_popup.h"
#include "../../../include/m_utils.h"
#include "../../../include/m_updater.h"
#include "../../../include/m_message.h"
#include "../../../include/m_statusplugins.h"
#include "../../../include/m_versioninfo.h"

#include "hooked_events.h"
#include "utils.h"

extern char ModuleName[];
extern HINSTANCE hInstance;
extern PLUGINLINK *pluginLink;

const MUUID UUID_NULL = {0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};

#define MAX_INTERFACES 10

#define CONFIGURATION_FILE "LPS.ini"
#define BRIDGE_TEMPLATE "LPS.template"

#define COMMENT_CHAR '#'
#define LEGACY_EXTENSION "legacy"

#define OLD_MIRANDAPLUGININFO_SUPPORT PLUGININFO oldPluginInfo = { \
	sizeof(PLUGININFO), \
	pluginInfo.shortName, \
	pluginInfo.version, \
	pluginInfo.description, \
	pluginInfo.author, \
	pluginInfo.authorEmail, \
	pluginInfo.copyright, \
	pluginInfo.homepage, \
	pluginInfo.flags, \
	pluginInfo.replacesDefaultModule \
}; \
\
extern "C" __declspec(dllexport) PLUGININFO *MirandaPluginInfo(DWORD mirandaVersion) \
{ \
	return &oldPluginInfo; \
}

#endif //M_PONTIFEX_COMMONHEADERS_H