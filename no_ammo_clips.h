#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <ISmmPlugin.h>
#include <sh_vector.h>
#include <stdio.h>
#include <iserver.h>
#include "vector.h"
#include <functional>
#include <utlstring.h>
#include <complex>
#include <iomanip>
#include "CBaseEntity.h"
#include "CBasePlayerPawn.h"
#include "CBasePlayerController.h"
#include "CCSPlayerPawn.h"
#include "CCSPlayerController.h"
#include "metamod_oslink.h"
#include "include/menus.h"
#include "schemasystem/schemasystem.h"
#include "algorithm"
#include "map"
#include "CCSWeaponBase.h"
#include "igameevents.h"

class CEntityListener : public IEntityListener
{
    void OnEntityParentChanged(CEntityInstance *pEntity, CEntityInstance *pNewParent) override;
};

class NoAmmoClips final : public ISmmPlugin, public IMetamodListener
{
public:
    bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) override;
    bool Unload(char *error, size_t maxlen) override;
    void AllPluginsLoaded() override;

private:
    const char *GetAuthor() override;
    const char *GetName() override;
    const char *GetDescription() override;
    const char *GetURL() override;
    const char *GetLicense() override;
    const char *GetVersion() override;
    const char *GetDate() override;
    const char *GetLogTag() override;
};

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_