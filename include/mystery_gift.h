#ifndef GUARD_MYSTERY_GIFT_H
#define GUARD_MYSTERY_GIFT_H

#include "global.h"


struct MysteryGiftMon
{
    u16 species;
    u16 item;
    const u8* password;
    u16 ivs;
    u16 moves[4];
    u8 level;
    u8 nature;
    u8 shiny;
    const u8* otname;

};

extern const struct MysteryGiftMon gMysteryGiftMons[];

void SetArgsForGiveMysteryGiftMon(u16 id);
u16 GetMysteryGift();
void ShowEnterPasswordScreen(void);
u8 GiveMysteryGiftMon(u16 species, u16 item, u16 ivs, u16 move1, u16 move2, u16 move3, u16 move4, u8 level, u8 nature, u8 shiny, const u8* otName);

#endif //GUARD_MYSTERY_GIFT_H
