#include "no_ammo_clips.hpp"
#include <ISmmPlugin.h>
#include "globals.hpp"
#include "include/utils.h"
#include <CCSPlayerPawn.h>
#include <set>

NoAmmoClips nac;
PLUGIN_EXPOSE(NoAmmoClips, nac);

std::set<int> modified;

void Startup()
{
	g_pEntitySystem = utils->GetCEntitySystem();
	g_pGameEntitySystem = GameEntitySystem();
	gpGlobals = utils->GetCGlobalVars();
	g_pGameEntitySystem->AddListenerEntity(&entity_listener);
}

bool NoAmmoClips::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_ANY(GetEngineFactory, g_pSchemaSystem, ISchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetEngineFactory, g_pGameResourceServiceServer, IGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
	ConVar_Register(FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);
	g_SMAPI->AddListener(this, this);
	return true;
}

void NoAmmoClips::AllPluginsLoaded()
{
	int ret;
	utils = static_cast<IUtilsApi*>(g_SMAPI->MetaFactory(UTILS_INTERFACE, &ret, NULL));
	if ( ret == META_IFACE_FAILED )
	{
		META_CONPRINTF("[FATAL] %s Missing Utils system plugin", this->GetLogTag());
		std::string unload_cmd = "meta unload " + std::to_string(g_PLID);
		engine->ServerCommand(unload_cmd.c_str());
		return;
	}
	utils->StartupServer(g_PLID, Startup);
	utils->MapEndHook(g_PLID, []()
	{
		modified.clear();
	});
}

bool NoAmmoClips::Unload(char* error, size_t maxlen)
{
	ConVar_Unregister();
	utils->ClearAllHooks(g_PLID);
	g_pGameEntitySystem->RemoveListenerEntity(&entity_listener);
	return true;
}
const char* NoAmmoClips::GetLicense() { return "Public"; }
const char* NoAmmoClips::GetVersion() { return "1.0.3"; }
const char* NoAmmoClips::GetDate() { return __DATE__; }
const char* NoAmmoClips::GetLogTag() { return "[NAC]"; }
const char* NoAmmoClips::GetAuthor() { return "ShadowRipper"; }
const char* NoAmmoClips::GetDescription() { return ""; }
const char* NoAmmoClips::GetName() { return "No Ammo Clips"; }
const char* NoAmmoClips::GetURL() { return ""; }