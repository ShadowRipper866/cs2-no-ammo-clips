#include "entity_listener.hpp"
#include <CCSPlayerController.h>
#include <CCSWeaponBase.h>
#include <entityinstance.h>
#include "globals.hpp"
#include "no_ammo_clips.hpp"
#include <cstring>

void EntityListener::OnEntitySpawned(CEntityInstance* entity_instance)
{
	if ( strncmp(entity_instance->GetClassname(), "weapon", 6) == 0 )
	{
		CCSWeaponBase* weapon_base = static_cast<CCSWeaponBase*>(entity_instance);
		CCSWeaponBaseVData* vdata = weapon_base->GetWeaponVData();
		int weapon_index = weapon_base->m_AttributeManager().m_Item().m_iItemDefinitionIndex(); // бескостыльный фикс!!!
		auto it = WEAPON_INDEXES.find(weapon_index);
		if ( it == WEAPON_INDEXES.end() )
			return;

		if ( !modified.contains(weapon_index) )
		{
			vdata->m_bReserveAmmoAsClips(false);
			int weapon_ammo = it->second;
			weapon_base->m_pReserveAmmo()[0] = weapon_ammo;
			vdata->m_nPrimaryReserveAmmoMax(weapon_ammo);
			utils->SetStateChanged(weapon_base, "CBasePlayerWeapon", "m_pReserveAmmo");
			modified.insert(weapon_index);
		}
	}
}