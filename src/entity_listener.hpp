#pragma once

#include "ientitylistener.h"
#include <unordered_map>

class EntityListener : public IEntityListener
{
  public:
	void OnEntitySpawned(CEntityInstance* entity_instance) override;

  private:
	static inline const std::unordered_map<int, int> WEAPON_INDEXES = {
	    {1, 35},   // deagle
	    {2, 120},  // berettas
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
	    {64, 8},   // revolver
	};
};
