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


u16 GetMysteryGift(const u8* password);
void ShowEnterPasswordScreen(void);
u8 GiveMysteryGiftMon(u16 species, u16 item, u16 ivs, u16 move1, u16 move2, u16 move3, u16 move4, u8 level, u8 nature);


const u8 gMewPassword[] = _("A");

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
    },
};

u16 GetMysteryGift(const u8* password) 
{
    u8 i;
    for(i = 0; i < ARRAY_COUNT(gMysteryGiftMons); i++)
    {
        if(StringCompare(gMysteryGiftMons[i].password, password))
        {
            return i;
        }
    }
    return 0xFFFF;
}


//CREDITS TO SKELI (password naming screen)
void ShowEnterPasswordScreen(void)
{
    u8 type;
    type = NAMING_SCREEN_ENTER_PASSWORD;
    gStringVar1[0] = EOS; //Empty input
	DoNamingScreen(type, gStringVar1, 0, 0, 0, (void*) CB2_ReturnToFieldContinueScriptPlayMapMusic);
}


//CREDITS TO GHOULSLASH (customgivemon)
u8 GiveMysteryGiftMon(u16 species, u16 item, u16 ivs, u16 move1, u16 move2, u16 move3, u16 move4, u8 level, u8 nature)
{
    u8 heldItem[2];
    struct Pokemon mon;
    u8 i;

    if (nature == 25 || nature == 0xFF)
        nature = Random() % 25;
    CreateMonWithNature(&mon, species, level, 32, nature);
    CalculateMonStats(&mon);
    SetMonMoveSlot(&mon, move1, 0);
    SetMonMoveSlot(&mon, move2, 1);
    SetMonMoveSlot(&mon, move3, 2);
    SetMonMoveSlot(&mon, move4, 3);
    heldItem[0] = item;
    heldItem[1] = item >> 8;
    SetMonData(&mon, MON_DATA_HELD_ITEM, heldItem);
    SetMonData(&mon, MON_DATA_OT_NAME, gSaveBlock2Ptr->playerName);
    SetMonData(&mon, MON_DATA_OT_GENDER, &gSaveBlock2Ptr->playerGender);
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL) == SPECIES_NONE)
            break;
    }
    CopyMon(&gPlayerParty[i], &mon, sizeof(mon));
    gPlayerPartyCount = i + 1;
}