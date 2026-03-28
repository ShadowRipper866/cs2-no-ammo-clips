#include "no_ammo_clips.h"
NoAmmoClips nac;

PLUGIN_EXPOSE(NoAmmoClips, nac);
IUtilsApi *utils;
CEntityListener entityListener;
IVEngineServer2 *engine = nullptr;
CGameEntitySystem *g_pGameEntitySystem = nullptr;
CEntitySystem *g_pEntitySystem = nullptr;
CGlobalVars *gpGlobals = nullptr;

CGameEntitySystem *GameEntitySystem()
{
    return utils->GetCGameEntitySystem();
}

std::unordered_map<std::string, int> weaponClassnames = { // <- мапа ИИшная, но вроде верная (класснейм оружия / всего патронов)
    {"weapon_glock", 120},
    {"weapon_usp_silencer", 24},
    {"weapon_hkp2000", 52},
    {"weapon_p250", 26},
    {"weapon_tec9", 90},
    {"weapon_fiveseven", 100},
    {"weapon_cz75a", 12},
    {"weapon_elite", 120},
    {"weapon_deagle", 35},
    {"weapon_revolver", 8},
    {"weapon_mac10", 100},
    {"weapon_mp9", 120},
    {"weapon_mp7", 120},
    {"weapon_mp5sd", 120},
    {"weapon_ump45", 100},
    {"weapon_p90", 100},
    {"weapon_bizon", 120},
    {"weapon_ak47", 90},
    {"weapon_m4a1", 90},
    {"weapon_m4a1_silencer", 80},
    {"weapon_galilar", 105},
    {"weapon_famas", 75},
    {"weapon_aug", 90},
    {"weapon_sg556", 90},
    {"weapon_ssg08", 90},
    {"weapon_awp", 30},
    {"weapon_g3sg1", 90},
    {"weapon_scar20", 90},
    {"weapon_m249", 200},
    {"weapon_negev", 300},
    {"weapon_nova", 32},
    {"weapon_xm1014", 32},
    {"weapon_mag7", 32},
    {"weapon_sawedoff", 32}
};

std::set<std::string> modified;

bool IsPlayerValid(CEntityInstance *entityInstance)
{
    if (!strcmp(entityInstance->GetClassname(), "player"))
    {
        CBasePlayerPawn *pawn = (CBasePlayerPawn *)entityInstance;
        CCSPlayerController *player = (CCSPlayerController *)pawn->m_hController().Get();
        if (!player)
            return false;
        if (player->IsAlive() && !player->IsBot() && engine->IsClientFullyAuthenticated(player->GetPlayerSlot()))
            return true;
        return false;
    }
    return false;
}

void CEntityListener::OnEntityParentChanged(CEntityInstance *entityInstance, CEntityInstance *pNewParent)
{
    std::unordered_map<std::string, int>::iterator it = weaponClassnames.find(entityInstance->GetClassname());
    if (it == weaponClassnames.end() || !pNewParent)
        return;
    const char *weaponName = it->first.c_str();
    if (!strcmp(entityInstance->GetClassname(), weaponName))
    {
        CBasePlayerWeapon *playerWeapon = (CBasePlayerWeapon *)entityInstance;
        CCSWeaponBase *weaponBase = (CCSWeaponBase *)playerWeapon;
        CBasePlayerWeaponVData *vdata = weaponBase->GetWeaponVData();
        CCSWeaponBaseVData *vdata2 = (CCSWeaponBaseVData *)playerWeapon->GetWeaponVData();
        if (modified.count(it->first) == 0)
        {
            vdata->m_bReserveAmmoAsClips = false;
            if (!strcmp(weaponName, "weapon_hkp2000")) // ебанные инвалиды на Valve сделали так, что класснейм юспа и п2000 == weapon_hkp2000(т.е п2000), поэтому проверка...
            {
                if (playerWeapon->m_iClip1 == 12)
                {
                    int uspClip = weaponClassnames["weapon_usp_silencer"];
                    playerWeapon->m_pReserveAmmo()[0] = uspClip;
                    vdata2->m_nPrimaryReserveAmmoMax = uspClip;
                    utils->SetStateChanged(playerWeapon, "CBasePlayerWeapon", "m_iReserveAmmo");
                    if (IsPlayerValid(pNewParent))
                        modified.insert("weapon_usp_silencer");
                    return;
                }
            }
            if (!strcmp(weaponName, "weapon_deagle")) // я хуею...
            {
                if (playerWeapon->m_iClip1 == 8)
                {
                    int revolverClip = weaponClassnames["weapon_revolver"];
                    playerWeapon->m_pReserveAmmo()[0] = revolverClip;
                    vdata2->m_nPrimaryReserveAmmoMax = revolverClip;
                    utils->SetStateChanged(playerWeapon, "CBasePlayerWeapon", "m_iReserveAmmo");
                    if (IsPlayerValid(pNewParent))
                        modified.insert("weapon_revolver");
                    return;
                }
            }
            playerWeapon->m_pReserveAmmo()[0] = it->second;
            vdata2->m_nPrimaryReserveAmmoMax = it->second;
            utils->SetStateChanged(playerWeapon, "CBasePlayerWeapon", "m_iReserveAmmo");
            if (IsPlayerValid(pNewParent))
                modified.insert(it->first);
        }
    } 
}

void Startup()
{
    g_pEntitySystem = utils->GetCEntitySystem();
    g_pGameEntitySystem = GameEntitySystem();
    gpGlobals = utils->GetCGlobalVars();
    g_pGameEntitySystem->AddListenerEntity(&entityListener);
}

bool NoAmmoClips::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    GET_V_IFACE_ANY(GetEngineFactory, g_pSchemaSystem, ISchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);

    ConVar_Register(FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);
    g_SMAPI->AddListener(this, this);
    return true;
}

void NoAmmoClips::AllPluginsLoaded()
{
    int ret;
    char error[64] = {0};
    utils = (IUtilsApi *)g_SMAPI->MetaFactory(Utils_INTERFACE, &ret, NULL);
    if (ret == META_IFACE_FAILED)
    {
        V_strncpy(error, "Missing Utils system plugin", 64);
        ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", g_PLAPI->GetLogTag(), error);
        std::string sBuffer = "meta unload " + std::to_string(g_PLID);
        engine->ServerCommand(sBuffer.c_str());
        return;
    }
    utils->StartupServer(g_PLID, Startup);
    utils->MapEndHook(g_PLID, []() { modified.clear(); });
}

bool NoAmmoClips::Unload(char *error, size_t maxlen)
{
    ConVar_Unregister();
    utils->ClearAllHooks(g_PLID);
    g_pGameEntitySystem->RemoveListenerEntity(&entityListener);
    return true;
}
const char *NoAmmoClips::GetLicense(){ return "Public"; }
const char *NoAmmoClips::GetVersion(){ return "1.0.2"; }
const char *NoAmmoClips::GetDate(){ return __DATE__; }
const char *NoAmmoClips::GetLogTag(){return "[NAC]"; }
const char *NoAmmoClips::GetAuthor(){ return "ShadowRipper"; }
const char *NoAmmoClips::GetDescription(){ return ""; }
const char *NoAmmoClips::GetName(){ return "No Ammo Clips"; }
const char *NoAmmoClips::GetURL(){ return ""; }
