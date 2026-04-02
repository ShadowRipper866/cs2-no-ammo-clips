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

#include <unordered_map>

// {Индекс -> патроны}
std::unordered_map<int, int> weaponIndexes = {
    {1, 35},   // deagle
    {2, 120},  // беретты
    {3, 100},  // fiveseven
    {4, 120},  // glock
    {7, 90},   // ak47
    {8, 90},   // aug
    {9, 30},   // awp
    {10, 75},  // famas
    {11, 90},  // g3sg1
    {13, 105}, // galilar
    {14, 200}, // m249
    {16, 90},  // m4a1 (мка четверка)
    {17, 100}, // mac10
    {19, 100}, // p90
    {23, 120}, // mp5sd
    {24, 100}, // ump45
    {25, 32},  // xm1014
    {26, 120}, // bizon
    {27, 32},  // mag7
    {28, 300}, // negev
    {29, 32},  // sawedoff
    {30, 90},  // tec9
    {32, 52},  // hkp2000
    {33, 120}, // mp7
    {34, 120}, // mp9
    {35, 32},  // nova
    {36, 26},  // p250
    {38, 90},  // scar20
    {39, 90},  // sg553)
    {40, 90},  // ssg08
    {60, 80},  // m4a1_silencer
    {61, 24},  // usp_silencer
    {63, 12},  // cz75a
    {64, 8}    // revolver
};

std::set<int> modified;

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
    if (!pNewParent)
    {
        return;
    }
    if (!strncmp(entityInstance->GetClassname(), "weapon", 6))
    {
        CBasePlayerWeapon *playerWeapon = (CBasePlayerWeapon *)entityInstance;
        CCSWeaponBase *weaponBase = (CCSWeaponBase *)playerWeapon;
        CBasePlayerWeaponVData *vdata = weaponBase->GetWeaponVData();
        CCSWeaponBaseVData *vdata2 = (CCSWeaponBaseVData *)playerWeapon->GetWeaponVData();
        int weaponIndex = playerWeapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex(); // бескостыльный фикс!!!
        std::unordered_map<int, int>::iterator it = weaponIndexes.find(weaponIndex);
        if (it == weaponIndexes.end())
        {
            return;
        }
        if (modified.count(weaponIndex) == 0)
        {
            vdata->m_bReserveAmmoAsClips = false;
            int weaponAmmo = it->second;
            playerWeapon->m_pReserveAmmo()[0] = weaponAmmo;
            vdata2->m_nPrimaryReserveAmmoMax = weaponAmmo;
            utils->SetStateChanged(playerWeapon, "CBasePlayerWeapon", "m_iReserveAmmo");
            if (IsPlayerValid(pNewParent))
            {
                modified.insert(weaponIndex);
            }
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
const char *NoAmmoClips::GetLicense() { return "Public"; }
const char *NoAmmoClips::GetVersion() { return "1.0.3"; }
const char *NoAmmoClips::GetDate() { return __DATE__; }
const char *NoAmmoClips::GetLogTag() { return "[NAC]"; }
const char *NoAmmoClips::GetAuthor() { return "ShadowRipper"; }
const char *NoAmmoClips::GetDescription() { return ""; }
const char *NoAmmoClips::GetName() { return "No Ammo Clips"; }
const char *NoAmmoClips::GetURL() { return ""; }
