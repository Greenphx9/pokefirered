#include "global.h"
#include "gflib.h"
#include "random.h"
#include "text.h"
#include "data.h"
#include "battle.h"
#include "battle_anim.h"
#include "item.h"
#include "event_data.h"
#include "util.h"
#include "pokemon_storage_system.h"
#include "battle_gfx_sfx_util.h"
#include "battle_controllers.h"
#include "evolution_scene.h"
#include "battle_message.h"
#include "battle_util.h"
#include "link.h"
#include "m4a.h"
#include "pokedex.h"
#include "strings.h"
#include "overworld.h"
#include "party_menu.h"
#include "field_specials.h"
#include "constants/items.h"
#include "constants/item_effects.h"
#include "constants/hoenn_cries.h"
#include "constants/pokemon.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/item_effects.h"
#include "constants/trainers.h"
#include "constants/hold_effects.h"
#include "constants/battle_move_effects.h"
#include "naming_screen.h"
#include "mystery_gift.h"

void SetArgsForGiveMysteryGiftMon(u16 id);
u16 GetMysteryGift();
void ShowEnterPasswordScreen(void);
u8 GiveMysteryGiftMon(u16 species, u16 item, u16 ivs, u16 move1, u16 move2, u16 move3, u16 move4, u8 level, u8 nature, u8 shiny, const u8* otName);


const u8 gMewPassword[] = _("MEWPASSWORD");
const u8 gMewOtName[] = _("MEW");

const struct MysteryGiftMon gMysteryGiftMons[] =
{
    {
        .species = SPECIES_MEW,
        .item = ITEM_NONE,
        .password = gMewPassword,
        .ivs = 31,
        .moves = 
        {
            MOVE_PSYCHIC,
            MOVE_THUNDERBOLT,
            MOVE_ICE_BEAM,
            MOVE_FLAMETHROWER,
        },
        .level = 100,
        .nature = NATURE_HARDY,
        .shiny = 1,
        .otname = gMewOtName,
    },
};

void SetArgsForGiveMysteryGiftMon(u16 id)
{
    u16 species, item, ivs;
    u16 move1, move2, move3, move4;
    u8 level, nature, shiny;
    const struct MysteryGiftMon mGiftMon = gMysteryGiftMons[id];
    const u8* otName = mGiftMon.otname;
    species = mGiftMon.species;
    item = mGiftMon.item;
    ivs = mGiftMon.ivs;
    move1 = mGiftMon.moves[0];
    move2 = mGiftMon.moves[1];
    move3 = mGiftMon.moves[2];
    move4 = mGiftMon.moves[3];
    level = mGiftMon.level;
    nature = mGiftMon.nature;
    shiny = mGiftMon.shiny;
    GiveMysteryGiftMon(species, item, ivs, move1, move2, move3, move4, level, nature, shiny, otName);
}

u16 GetMysteryGift() 
{
    u8 i;
    const u8* password = gStringVar1;
    for(i = 0; i < ARRAY_COUNT(gMysteryGiftMons); i++)
    {
        if(StringCompare(gMysteryGiftMons[i].password, password) == 0)
        {
            SetArgsForGiveMysteryGiftMon(i);
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
            return i;
        }
    }
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
    return 0xFFFF;
}


//CREDITS TO SKELI (password naming screen)
void ShowEnterPasswordScreen(void)
{
    u8 type;
    type = NAMING_SCREEN_ENTER_PASSWORD;
    gStringVar1[0] = EOS; //Empty input
	DoNamingScreen(type, gStringVar1, 0, 0, 0, (void*) GetMysteryGift);
}


//CREDITS TO GHOULSLASH (customgivemon)
u8 GiveMysteryGiftMon(u16 species, u16 item, u16 ivs, u16 move1, u16 move2, u16 move3, u16 move4, u8 level, u8 nature, u8 shiny, const u8* otName)
{
    u8 heldItem[2];
    struct Pokemon mon;
    u8 i;

    if (nature == 25 || nature == 0xFF)
        nature = Random() % 25;
    if(shiny == 1)
        CreateShinyMonWithNature(&mon, species, level, nature);
    else
        CreateMonWithNature(&mon, species, level, 32, nature);
    CalculateMonStats(&mon);
    SetMonMoveSlot(&mon, move1, 0);
    SetMonMoveSlot(&mon, move2, 1);
    SetMonMoveSlot(&mon, move3, 2);
    SetMonMoveSlot(&mon, move4, 3);
    for (i = 0; i < NUM_STATS; i++)
    {
        SetMonData(&mon, MON_DATA_HP_IV + i, &ivs);
    }
    heldItem[0] = item;
    heldItem[1] = item >> 8;
    SetMonData(&mon, MON_DATA_HELD_ITEM, heldItem);
    SetMonData(&mon, MON_DATA_OT_NAME, otName);
    SetMonData(&mon, MON_DATA_OT_GENDER, &gSaveBlock2Ptr->playerGender);
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL) == SPECIES_NONE)
            break;
    }
    CopyMon(&gPlayerParty[i], &mon, sizeof(mon));
    gPlayerPartyCount = i + 1;
}