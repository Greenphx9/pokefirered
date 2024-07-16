#define TMHM_LEARNSET(moves) {(u32)(moves), ((u64)(moves) >> 32)}
#define TMHM(tmhm) ((u64)1 << (ITEM_##tmhm - ITEM_TM01))

// This table determines which TMs and HMs a species is capable of learning.
// Each entry is a 64-bit bit array spread across two 32-bit values, with
// each bit corresponding to a machine.
static const u32 sTMHMLearnsets[][2] =
{
    

};
