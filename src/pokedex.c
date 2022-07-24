#include "global.h"
#include "pokedex.h"
#include "pokedex_screen.h"

extern const u16 gPokedexOrder_Regional[151];

const u8 *sub_8088E20(u16 dexNum)
{
    return gPokedexEntries[dexNum].categoryName;
}

u16 GetPokedexHeightWeight(u16 dexNum, u8 data)
{
    switch (data)
    {
    case 0:  // height
        return gPokedexEntries[dexNum].height;
    case 1:  // weight
        return gPokedexEntries[dexNum].weight;
    default:
        return 1;
    }
}

s8 GetSetPokedexFlag(u16 nationalDexNo, u8 caseID)
{
    return DexScreen_GetSetPokedexFlag(nationalDexNo, caseID, 0);
}

u16 GetNationalPokedexCount(u8 caseID)
{
    u16 count = 0;
    u16 i;

    for (i = 0; i < NATIONAL_DEX_COUNT; i++)
    {
        switch (caseID)
        {
        case FLAG_GET_SEEN:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_SEEN))
                count++;
            break;
        case FLAG_GET_CAUGHT:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_CAUGHT))
                count++;
            break;
        }
    }
    return count;
}

u16 GetKantoPokedexCount(u8 caseId)
{
	u16 i, count;

	for (i = 0, count = 0; i < ARRAY_COUNT(gPokedexOrder_Regional); ++i)
	{
		if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(gPokedexOrder_Regional[i]), caseId))
			count++;
	}

	return count;
}

bool16 HasAllHoennMons(void)
{
    u16 i;

    for (i = 0; i < HOENN_DEX_COUNT - 2; i++)
    {
        if (!GetSetPokedexFlag(HoennToNationalOrder(i + 1), FLAG_GET_CAUGHT))
            return FALSE;
    }
    return TRUE;
}

bool16 HasAllKantoMons(void)
{
	u16 i;

	for (i = 0; i < ARRAY_COUNT(gPokedexOrder_Regional); ++i)
	{
		if (!GetSetPokedexFlag(SpeciesToNationalPokedexNum(gPokedexOrder_Regional[i]), FLAG_GET_CAUGHT))
			return FALSE;
	}

	return TRUE;
}

bool16 HasAllMons(void)
{
	u16 i;

	for (i = 1; i <= 898; ++i)
	{
		if (!GetSetPokedexFlag(i, FLAG_GET_CAUGHT))
			return FALSE;
	}

	return TRUE;
}
