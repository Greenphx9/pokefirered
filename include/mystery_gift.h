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
};

extern const struct MysteryGiftMon gMysteryGiftMons[];

u16 GetMysteryGift(const u8* password);
void ShowEnterPasswordScreen(void);
u8 GiveMysteryGiftMon(u16 species, u16 item, u16 ivs, u16 move1, u16 move2, u16 move3, u16 move4, u8 level, u8 nature);

#endif //GUARD_MYSTERY_GIFT_H
