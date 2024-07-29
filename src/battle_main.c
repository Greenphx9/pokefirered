#include "global.h"
#include "gflib.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_main.h"
#include "battle_message.h"
#include "battle_scripts.h"
#include "battle_setup.h"
#include "berry.h"
#include "data.h"
#include "debug.h"
#include "decompress.h"
#include "event_data.h"
#include "evolution_scene.h"
#include "graphics.h"
#include "help_system.h"
#include "item.h"
#include "link.h"
#include "link_rfu.h"
#include "load_save.h"
#include "m4a.h"
#include "party_menu.h"
#include "pokeball.h"
#include "pokedex.h"
#include "quest_log.h"
#include "random.h"
#include "roamer.h"
#include "safari_zone.h"
#include "scanline_effect.h"
#include "string.h"
#include "task.h"
#include "trig.h"
#include "wild_encounter.h"
#include "vs_seeker.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_setup.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/songs.h"
#include "constants/trainers.h"

static void SpriteCB_UnusedDebugSprite(struct Sprite *sprite);
static void HandleAction_UseMove(void);
static void HandleAction_Switch(void);
static void HandleAction_UseItem(void);
static void HandleAction_Run(void);
static void HandleAction_WatchesCarefully(void);
static void HandleAction_SafariZoneBallThrow(void);
static void HandleAction_ThrowBait(void);
static void HandleAction_ThrowRock(void);
static void HandleAction_SafariZoneRun(void);
static void HandleAction_OldManBallThrow(void);
static void HandleAction_TryFinish(void);
static void HandleAction_NothingIsFainted(void);
static void HandleAction_ActionFinished(void);
static void HandleEndTurn_ContinueBattle(void);
static void HandleEndTurn_BattleWon(void);
static void HandleEndTurn_BattleLost(void);
static void HandleEndTurn_RanFromBattle(void);
static void HandleEndTurn_MonFled(void);
static void HandleEndTurn_FinishBattle(void);
static void CB2_InitBattleInternal(void);
static void CB2_PreInitMultiBattle(void);
static void CB2_HandleStartMultiBattle(void);
static u8 CreateNPCTrainerParty(struct Pokemon *party, u16 trainerNum, bool8 firstTrainer);
static void CB2_HandleStartBattle(void);
static void TryCorrectShedinjaLanguage(struct Pokemon *mon);
static void BattleMainCB1(void);
static void CB2_QuitPokedudeBattle(void);
static void SpriteCB_UnusedDebugSprite_Step(struct Sprite *sprite);
static void CB2_EndLinkBattle(void);
static void EndLinkBattleInSteps(void);
static void SpriteCB_MoveWildMonToRight(struct Sprite *sprite);
static void SpriteCB_WildMonShowHealthbox(struct Sprite *sprite);
static void SpriteCB_AnimFaintOpponent(struct Sprite *sprite);
static void SpriteCB_BlinkVisible(struct Sprite *sprite);
static void oac_poke_ally_(struct Sprite *sprite);
static void SpriteCB_Idle(struct Sprite *sprite);
static void SpriteCB_BounceEffect(struct Sprite *sprite);
static void SpriteCB_PlayerThrowUpdate(struct Sprite *sprite);
static void BattleStartClearSetData(void);
static void DoBattleIntro(void);
static void BattleIntroGetMonsData(void);
static void TurnValuesCleanUp(bool8 var0);
static void SpecialStatusesClear(void);
static void TryDoEventsBeforeFirstTurn(void);
static void HandleTurnActionSelectionState(void);
static void RunTurnActionsFunctions(void);
static void SetActionsAndBattlersTurnOrder(void);
static void CheckFocusPunch_ClearVarsBeforeTurnStarts(void);
static void HandleEndTurn_FinishBattle(void);
static void FreeResetData_ReturnToOvOrDoEvolutions(void);
static void ReturnFromBattleToOverworld(void);
static void TryEvolvePokemon(void);
static void WaitForEvoSceneToFinish(void);
static bool8 AllAtActionConfirmed(void);
static void UpdateBattlerPartyOrdersOnSwitch(u32 battler);
static void TryChangeTurnOrder(void);
static void TryChangingTurnOrderEffects(u32 battler1, u32 battler2);
static void CheckChangingTurnOrderEffects(void);
static u32 Crc32B (const u8 *data, u32 size);
static u32 GeneratePartyHash(const struct Trainer *trainer, u32 i);

EWRAM_DATA u16 gBattle_BG0_X = 0;
EWRAM_DATA u16 gBattle_BG0_Y = 0;
EWRAM_DATA u16 gBattle_BG1_X = 0;
EWRAM_DATA u16 gBattle_BG1_Y = 0;
EWRAM_DATA u16 gBattle_BG2_X = 0;
EWRAM_DATA u16 gBattle_BG2_Y = 0;
EWRAM_DATA u16 gBattle_BG3_X = 0;
EWRAM_DATA u16 gBattle_BG3_Y = 0;
EWRAM_DATA u16 gBattle_WIN0H = 0;
EWRAM_DATA u16 gBattle_WIN0V = 0;
EWRAM_DATA u16 gBattle_WIN1H = 0;
EWRAM_DATA u16 gBattle_WIN1V = 0;
EWRAM_DATA u8 gDisplayedStringBattle[425] = {0}; // Increased in size to fit Juan's defeat text (SootopolisCity_Gym_1F_Text_JuanDefeat)
EWRAM_DATA u8 gBattleTextBuff1[TEXT_BUFF_ARRAY_COUNT] = {0};
EWRAM_DATA u8 gBattleTextBuff2[TEXT_BUFF_ARRAY_COUNT] = {0};
EWRAM_DATA u8 gBattleTextBuff3[TEXT_BUFF_ARRAY_COUNT + 13] = {0};   // expanded for stupidly long z move names
EWRAM_DATA u32 gBattleTypeFlags = 0;
EWRAM_DATA u8 gBattleTerrain = 0;
EWRAM_DATA struct MultiBattlePokemonTx gMultiPartnerParty[3] = {0};
EWRAM_DATA u8 *gBattleAnimBgTileBuffer = NULL;
EWRAM_DATA u8 *gBattleAnimBgTilemapBuffer = NULL;
static EWRAM_DATA u16 *sUnknownDebugSpriteDataBuffer = NULL;
EWRAM_DATA u32 gBattleControllerExecFlags = 0;
EWRAM_DATA u8 gBattlersCount = 0;
EWRAM_DATA u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerPositions[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gActionsByTurnOrder[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerByTurnOrder[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gCurrentTurnActionNumber = 0;
EWRAM_DATA u8 gCurrentActionFuncId = 0;
EWRAM_DATA struct BattlePokemon gBattleMons[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerSpriteIds[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gCurrMovePos = 0;
EWRAM_DATA u8 gChosenMovePos = 0;
EWRAM_DATA u16 gCurrentMove = 0;
EWRAM_DATA u16 gChosenMove = 0;
EWRAM_DATA u16 gCalledMove = 0;
EWRAM_DATA s32 gBattleMoveDamage = 0;
EWRAM_DATA s32 gHpDealt = 0;
EWRAM_DATA s32 gTakenDmg[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastUsedItem = 0;
EWRAM_DATA u16 gLastUsedAbility = 0;
EWRAM_DATA u8 gBattlerAttacker = 0;
EWRAM_DATA u8 gBattlerTarget = 0;
EWRAM_DATA u8 gBattlerFainted = 0;
EWRAM_DATA u8 gEffectBattler = 0;
EWRAM_DATA u8 gPotentialItemEffectBattler = 0;
EWRAM_DATA u8 gAbsentBattlerFlags = 0;
EWRAM_DATA u8 gIsCriticalHit = FALSE;
EWRAM_DATA u8 gMultiHitCounter = 0;
EWRAM_DATA const u8 *gBattlescriptCurrInstr = NULL;
EWRAM_DATA u32 gUnusedBattleMainVar = 0;
EWRAM_DATA u8 gChosenActionByBattler[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA const u8 *gSelectionBattleScripts[MAX_BATTLERS_COUNT] = {NULL};
EWRAM_DATA u16 gLastPrintedMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastLandedMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastHitByType[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastResultingMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLockedMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastUsedMove = 0;
EWRAM_DATA u8 gLastHitBy[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gChosenMoveByBattler[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gMoveResultFlags = 0;
EWRAM_DATA u32 gHitMarker = 0;
EWRAM_DATA u8 gTakenDmgByBattler[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBideTarget[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u32 gSideStatuses[NUM_BATTLE_SIDES] = {0};
EWRAM_DATA struct SideTimer gSideTimers[2] = {0};
EWRAM_DATA u32 gStatuses3[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u32 gStatuses4[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA struct DisableStruct gDisableStructs[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gPauseCounterBattle = 0;
EWRAM_DATA u16 gPaydayMoney = 0;
EWRAM_DATA u8 gBattleCommunication[BATTLE_COMMUNICATION_ENTRIES_COUNT] = {0};
EWRAM_DATA u8 gBattleOutcome = 0;
EWRAM_DATA struct ProtectStruct gProtectStructs[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA struct SpecialStatus gSpecialStatuses[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gBattleWeather = 0;
EWRAM_DATA struct WishFutureKnock gWishFutureKnock = {0};
EWRAM_DATA u16 gIntroSlideFlags = 0;
EWRAM_DATA u8 gSentPokesToOpponent[2] = {0};
EWRAM_DATA struct BattleEnigmaBerry gEnigmaBerries[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA struct BattleScripting gBattleScripting = {0};
EWRAM_DATA struct BattleStruct *gBattleStruct = NULL;
EWRAM_DATA u8 *gLinkBattleSendBuffer = NULL;
EWRAM_DATA u8 *gLinkBattleRecvBuffer = NULL;
EWRAM_DATA struct BattleResources *gBattleResources = NULL;
EWRAM_DATA u8 gActionSelectionCursor[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gMoveSelectionCursor[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerStatusSummaryTaskId[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerInMenuId = 0;
EWRAM_DATA bool8 gDoingBattleAnim = FALSE;
EWRAM_DATA u32 gTransformedPersonalities[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA bool8 gTransformedShininess[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gPlayerDpadHoldFrames = 0;
EWRAM_DATA struct BattleSpriteData *gBattleSpritesDataPtr = NULL;
EWRAM_DATA struct MonSpritesGfx *gMonSpritesGfxPtr = NULL;
EWRAM_DATA u16 gBattleMovePower = 0;
EWRAM_DATA u16 gMoveToLearn = 0;
EWRAM_DATA u8 gBattlerAbility = 0;
EWRAM_DATA u32 gFieldStatuses = 0;
EWRAM_DATA struct FieldTimer gFieldTimers = {0};
EWRAM_DATA struct QueuedStatBoost gQueuedStatBoosts[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA bool8 gHasFetchedBall = FALSE;
EWRAM_DATA u8 gLastUsedBall = 0;
EWRAM_DATA u16 gLastThrownBall = 0;
EWRAM_DATA u16 gBallToDisplay = 0;
EWRAM_DATA bool8 gLastUsedBallMenuPresent = FALSE;
EWRAM_DATA u8 gPartyCriticalHits[PARTY_SIZE] = {0};
EWRAM_DATA static u8 sTriedEvolving = 0;


void (*gPreBattleCallback1)(void);
void (*gBattleMainFunc)(void);
struct BattleResults gBattleResults;
u8 gLeveledUpInBattle;
u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
u8 gMultiUsePlayerCursor;
u8 gNumberOfMovesToChoose;

static const struct ScanlineEffectParams sIntroScanlineParams16Bit =
{
    &REG_BG3HOFS, SCANLINE_EFFECT_DMACNT_16BIT, 1
};

const struct SpriteTemplate gUnknownDebugSprite =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_UnusedDebugSprite,
};

static const u8 sText_ShedinjaJpnName[] = _("ヌケニン"); // Nukenin

const struct OamData gOamData_BattlerOpponent =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

const struct OamData gOamData_BattlerPlayer =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 2,
    .affineParam = 0,
};

static const s8 sPlayerThrowXTranslation[] = { -32, -16, -16, -32, -32 };

// .generic is large enough that the text for TYPE_ELECTRIC will exceed TEXT_BUFF_ARRAY_COUNT.
const struct TypeInfo gTypesInfo[NUMBER_OF_MON_TYPES] =
{
    [TYPE_NORMAL] =
    {
        .name = _("Normal"),
        .generic = _("a NORMAL move"),
        .palette = 13,
        .zMove = MOVE_BREAKNECK_BLITZ,
        .maxMove = MOVE_MAX_STRIKE,
        //.enhanceItem = ITEM_SILK_SCARF,
        //.berry = ITEM_CHILAN_BERRY,
        //.gem = ITEM_NORMAL_GEM,
        //.zCrystal = ITEM_NORMALIUM_Z,
        //.teraShard = ITEM_NORMAL_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_NORMAL,
    },
    [TYPE_FIGHTING] =
    {
        .name = _("Fight"),
        .generic = _("a FIGHTING move"),
        .palette = 13,
        .zMove = MOVE_ALL_OUT_PUMMELING,
        .maxMove = MOVE_MAX_KNUCKLE,
        //.enhanceItem = ITEM_BLACK_BELT,
        //.berry = ITEM_CHOPLE_BERRY,
        //.gem = ITEM_FIGHTING_GEM,
        //.zCrystal = ITEM_FIGHTINIUM_Z,
        //.plate = ITEM_FIST_PLATE,
        //.memory = ITEM_FIGHTING_MEMORY,
        //.teraShard = ITEM_FIGHTING_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_FIGHTING,
    },
    [TYPE_FLYING] =
    {
        .name = _("Flying"),
        .generic = _("a FLYING move"),
        .palette = 14,
        .zMove = MOVE_SUPERSONIC_SKYSTRIKE,
        .maxMove = MOVE_MAX_AIRSTREAM,
        //.enhanceItem = ITEM_SHARP_BEAK,
        //.berry = ITEM_COBA_BERRY,
        //.gem = ITEM_FLYING_GEM,
        //.zCrystal = ITEM_FLYINIUM_Z,
        //.plate = ITEM_SKY_PLATE,
        //.memory = ITEM_FLYING_MEMORY,
        //.teraShard = ITEM_FLYING_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_FLYING,
    },
    [TYPE_POISON] =
    {
        .name = _("Poison"),
        .generic = _("a POISON move"),
        .palette = 14,
        .zMove = MOVE_ACID_DOWNPOUR,
        .maxMove = MOVE_MAX_OOZE,
        //.enhanceItem = ITEM_POISON_BARB,
        //.berry = ITEM_KEBIA_BERRY,
        //.gem = ITEM_POISON_GEM,
        //.zCrystal = ITEM_POISONIUM_Z,
        //.plate = ITEM_TOXIC_PLATE,
        //.memory = ITEM_POISON_MEMORY,
        //.teraShard = ITEM_POISON_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_POISON,
    },
    [TYPE_GROUND] =
    {
        .name = _("Ground"),
        .generic = _("a GROUND move"),
        .palette = 13,
        .zMove = MOVE_TECTONIC_RAGE,
        .maxMove = MOVE_MAX_QUAKE,
        //.enhanceItem = ITEM_SOFT_SAND,
        //.berry = ITEM_SHUCA_BERRY,
        //.gem = ITEM_GROUND_GEM,
        //.zCrystal = ITEM_GROUNDIUM_Z,
        //.plate = ITEM_EARTH_PLATE,
        //.memory = ITEM_GROUND_MEMORY,
        //.teraShard = ITEM_GROUND_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_GROUND,
    },
    [TYPE_ROCK] =
    {
        .name = _("Rock"),
        .generic = _("a ROCK move"),
        .palette = 13,
        .zMove = MOVE_CONTINENTAL_CRUSH,
        .maxMove = MOVE_MAX_ROCKFALL,
        //.enhanceItem = ITEM_HARD_STONE,
        //.berry = ITEM_CHARTI_BERRY,
        //.gem = ITEM_ROCK_GEM,
        //.zCrystal = ITEM_ROCKIUM_Z,
        //.plate = ITEM_STONE_PLATE,
        //.memory = ITEM_ROCK_MEMORY,
        //.teraShard = ITEM_ROCK_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_ROCK,
    },
    [TYPE_BUG] =
    {
        .name = _("Bug"),
        .generic = _("a BUG move"),
        .palette = 15,
        .zMove = MOVE_SAVAGE_SPIN_OUT,
        .maxMove = MOVE_MAX_FLUTTERBY,
        //.enhanceItem = ITEM_SILVER_POWDER,
        //.berry = ITEM_TANGA_BERRY,
        //.gem = ITEM_BUG_GEM,
        //.zCrystal = ITEM_BUGINIUM_Z,
        //.plate = ITEM_INSECT_PLATE,
        //.memory = ITEM_BUG_MEMORY,
        //.teraShard = ITEM_BUG_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_BUG,
    },
    [TYPE_GHOST] =
    {
        .name = _("Ghost"),
        .generic = _("a GHOST move"),
        .palette = 14,
        .zMove = MOVE_NEVER_ENDING_NIGHTMARE,
        .maxMove = MOVE_MAX_PHANTASM,
        //.enhanceItem = ITEM_SPELL_TAG,
        //.berry = ITEM_KASIB_BERRY,
        //.gem = ITEM_GHOST_GEM,
        //.zCrystal = ITEM_GHOSTIUM_Z,
        //.plate = ITEM_SPOOKY_PLATE,
        //.memory = ITEM_GHOST_MEMORY,
        //.teraShard = ITEM_GHOST_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_GHOST,
    },
    [TYPE_STEEL] =
    {
        .name = _("Steel"),
        .generic = _("a STEEL move"),
        .palette = 13,
        .zMove = MOVE_CORKSCREW_CRASH,
        .maxMove = MOVE_MAX_STEELSPIKE,
        //.enhanceItem = ITEM_METAL_COAT,
        //.berry = ITEM_BABIRI_BERRY,
        //.gem = ITEM_STEEL_GEM,
        //.zCrystal = ITEM_STEELIUM_Z,
        //.plate = ITEM_IRON_PLATE,
        //.memory = ITEM_STEEL_MEMORY,
        //.teraShard = ITEM_STEEL_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_STEEL,
    },
    [TYPE_MYSTERY] =
    {
        .name = _("???"),
        .generic = _("a ??? move"),
        .palette = 15,
    },
    [TYPE_FIRE] =
    {
        .name = _("Fire"),
        .generic = _("a FIRE move"),
        .palette = 13,
        .zMove = MOVE_INFERNO_OVERDRIVE,
        .maxMove = MOVE_MAX_FLARE,
        //.enhanceItem = ITEM_CHARCOAL,
        //.berry = ITEM_OCCA_BERRY,
        //.gem = ITEM_FIRE_GEM,
        //.zCrystal = ITEM_FIRIUM_Z,
        //.plate = ITEM_FLAME_PLATE,
        //.memory = ITEM_FIRE_MEMORY,
        //.teraShard = ITEM_FIRE_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_FIRE,
    },
    [TYPE_WATER] =
    {
        .name = _("Water"),
        .generic = _("a WATER move"),
        .palette = 14,
        .zMove = MOVE_HYDRO_VORTEX,
        .maxMove = MOVE_MAX_GEYSER,
        //.enhanceItem = ITEM_MYSTIC_WATER,
        //.berry = ITEM_PASSHO_BERRY,
        //.gem = ITEM_WATER_GEM,
        //.zCrystal = ITEM_WATERIUM_Z,
        //.plate = ITEM_SPLASH_PLATE,
        //.memory = ITEM_WATER_MEMORY,
        //.teraShard = ITEM_WATER_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_WATER,
    },
    [TYPE_GRASS] =
    {
        .name = _("Grass"),
        .generic = _("a GRASS move"),
        .palette = 15,
        .zMove = MOVE_BLOOM_DOOM,
        .maxMove = MOVE_MAX_OVERGROWTH,
        //.enhanceItem = ITEM_MIRACLE_SEED,
        //.berry = ITEM_RINDO_BERRY,
        //.gem = ITEM_GRASS_GEM,
        //.zCrystal = ITEM_GRASSIUM_Z,
        //.plate = ITEM_MEADOW_PLATE,
        //.memory = ITEM_GRASS_MEMORY,
        //.teraShard = ITEM_GRASS_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_GRASS,
    },
    [TYPE_ELECTRIC] =
    {
        .name = _("Electr"),
        .generic = _("an ELECTRIC move"),
        .palette = 13,
        .zMove = MOVE_GIGAVOLT_HAVOC,
        .maxMove = MOVE_MAX_LIGHTNING,
        //.enhanceItem = ITEM_MAGNET,
        //.berry = ITEM_WACAN_BERRY,
        //.gem = ITEM_ELECTRIC_GEM,
        //.zCrystal = ITEM_ELECTRIUM_Z,
        //.plate = ITEM_ZAP_PLATE,
        //.memory = ITEM_ELECTRIC_MEMORY,
        //.teraShard = ITEM_ELECTRIC_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_ELECTRIC,
    },
    [TYPE_PSYCHIC] =
    {
        .name = _("Psychc"),
        .generic = _("a PSYCHIC move"),
        .palette = 14,
        .zMove = MOVE_SHATTERED_PSYCHE,
        .maxMove = MOVE_MAX_MINDSTORM,
        //.enhanceItem = ITEM_TWISTED_SPOON,
        //.berry = ITEM_PAYAPA_BERRY,
        //.gem = ITEM_PSYCHIC_GEM,
        //.zCrystal = ITEM_PSYCHIUM_Z,
        //.plate = ITEM_MIND_PLATE,
        //.memory = ITEM_PSYCHIC_MEMORY,
        //.teraShard = ITEM_PSYCHIC_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_PSYCHIC,
    },
    [TYPE_ICE] =
    {
        .name = _("Ice"),
        .generic = _("an ICE move"),
        .palette = 14,
        .zMove = MOVE_SUBZERO_SLAMMER,
        .maxMove = MOVE_MAX_HAILSTORM,
        //.enhanceItem = ITEM_NEVER_MELT_ICE,
        //.berry = ITEM_YACHE_BERRY,
        //.gem = ITEM_ICE_GEM,
        //.zCrystal = ITEM_ICIUM_Z,
        //.plate = ITEM_ICICLE_PLATE,
        //.memory = ITEM_ICE_MEMORY,
        //.teraShard = ITEM_ICE_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_ICE,
    },
    [TYPE_DRAGON] =
    {
        .name = _("Dragon"),
        .generic = _("a DRAGON move"),
        .palette = 15,
        .zMove = MOVE_DEVASTATING_DRAKE,
        .maxMove = MOVE_MAX_WYRMWIND,
        //.enhanceItem = ITEM_DRAGON_FANG,
        //.berry = ITEM_HABAN_BERRY,
        //.gem = ITEM_DRAGON_GEM,
        //.zCrystal = ITEM_DRAGONIUM_Z,
        //.plate = ITEM_DRACO_PLATE,
        //.memory = ITEM_DRAGON_MEMORY,
        //.teraShard = ITEM_DRAGON_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_DRAGON,
    },
    [TYPE_DARK] =
    {
        .name = _("Dark"),
        .generic = _("a DARK move"),
        .palette = 13,
        .zMove = MOVE_BLACK_HOLE_ECLIPSE,
        .maxMove = MOVE_MAX_DARKNESS,
        //.enhanceItem = ITEM_BLACK_GLASSES,
        //.berry = ITEM_COLBUR_BERRY,
        //.gem = ITEM_DARK_GEM,
        //.zCrystal = ITEM_DARKINIUM_Z,
        //.plate = ITEM_DREAD_PLATE,
        //.memory = ITEM_DARK_MEMORY,
        //.teraShard = ITEM_DARK_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_DARK,
    },
    [TYPE_FAIRY] =
    {
        .name = _("Fairy"),
        .generic = _("a FAIRY move"),
        .palette = 14,
        .zMove = MOVE_TWINKLE_TACKLE,
        .maxMove = MOVE_MAX_STARFALL,
        //.enhanceItem = ITEM_FAIRY_FEATHER,
        //.berry = ITEM_ROSELI_BERRY,
        //.gem = ITEM_FAIRY_GEM,
        //.zCrystal = ITEM_FAIRIUM_Z,
        //.plate = ITEM_PIXIE_PLATE,
        //.memory = ITEM_FAIRY_MEMORY,
        //.teraShard = ITEM_FAIRY_TERA_SHARD,
        //.arceusForm = SPECIES_ARCEUS_FAIRY,
    },
    /*
    [TYPE_STELLAR] =
    {
        .name = _("Stellar"),
        .teraShard = ITEM_STELLAR_TERA_SHARD,
    },
    */
};

// extra args are money and ball
#define TRAINER_CLASS(trainerClass, trainerName, ...)   \
    [TRAINER_CLASS_##trainerClass] =                    \
    {                                                   \
        .name = _(trainerName),                         \
        .money = DEFAULT(5, __VA_ARGS__),               \
        .ball = DEFAULT_2(ITEM_POKE_BALL, __VA_ARGS__), \
    }

const struct TrainerClass gTrainerClasses[TRAINER_CLASS_COUNT] =
{
    TRAINER_CLASS(NONE, "{PKMN} TRAINER"),
    TRAINER_CLASS(PKMN_TRAINER_UNUSED, "{PKMN} TRAINER"),
    TRAINER_CLASS(LEADER, "LEADER", 25),
    TRAINER_CLASS(ELITE_FOUR, "ELITE FOUR", 25, ITEM_ULTRA_BALL),
    TRAINER_CLASS(PKMN_PROF, "{PKMN} PROF.", 25),
    TRAINER_CLASS(RIVAL_EARLY, "RIVAL", 4),
    TRAINER_CLASS(RIVAL_LATE, "RIVAL", 9),
    TRAINER_CLASS(CHAMPION, "CHAMPION", 25),
    TRAINER_CLASS(YOUNGSTER, "YOUNGSTER", 4),
    TRAINER_CLASS(BUG_CATCHER, "BUG CATCHER", 3),
    TRAINER_CLASS(HIKER, "HIKER", 9),
    TRAINER_CLASS(BIRD_KEEPER, "BIRD KEEPER", 6),
    TRAINER_CLASS(PICNICKER, "PICNICKER", 5),
    TRAINER_CLASS(SUPER_NERD, "SUPER NERD", 6),
    TRAINER_CLASS(FISHERMAN, "FISHERMAN", 9),
    TRAINER_CLASS(TEAM_ROCKET, "TEAM ROCKET", 8),
    TRAINER_CLASS(LASS, "LASS", 4),
    TRAINER_CLASS(BEAUTY, "BEAUTY", 18),
    TRAINER_CLASS(BLACK_BELT, "BLACK BELT", 6, ITEM_ULTRA_BALL),
    TRAINER_CLASS(CUE_BALL, "CUE BALL", 6),
    TRAINER_CLASS(CHANNELER, "CHANNELER", 8),
    TRAINER_CLASS(ROCKER, "ROCKER", 6),
    TRAINER_CLASS(GENTLEMAN, "GENTLEMAN", 18, ITEM_LUXURY_BALL),
    TRAINER_CLASS(BURGLAR, "BURGLAR", 22),
    TRAINER_CLASS(SWIMMER_M, "SWIMMER♂", 1, ITEM_DIVE_BALL),
    TRAINER_CLASS(ENGINEER, "ENGINEER", 12),
    TRAINER_CLASS(JUGGLER, "JUGGLER", 10),
    TRAINER_CLASS(SAILOR, "SAILOR", 8),
    TRAINER_CLASS(COOLTRAINER, "COOLTRAINER", 9, ITEM_ULTRA_BALL),
    TRAINER_CLASS(POKEMANIAC, "POKéMANIAC", 12),
    TRAINER_CLASS(TAMER, "TAMER", 10),
    TRAINER_CLASS(CAMPER, "CAMPER", 5),
    TRAINER_CLASS(PSYCHIC, "PSYCHIC", 5),
    TRAINER_CLASS(BIKER, "BIKER", 5),
    TRAINER_CLASS(GAMER, "GAMER", 18),
    TRAINER_CLASS(SCIENTIST, "SCIENTIST", 12),
    TRAINER_CLASS(CRUSH_GIRL, "CRUSH GIRL", 6),
    TRAINER_CLASS(TUBER, "TUBER", 1),
    TRAINER_CLASS(PKMN_BREEDER, "{PKMN} BREEDER", 7, B_TRAINER_CLASS_POKE_BALLS >= GEN_8 ? ITEM_HEAL_BALL : ITEM_FRIEND_BALL),
    TRAINER_CLASS(PKMN_RANGER, "{PKMN} RANGER", 9),
    TRAINER_CLASS(AROMA_LADY, "AROMA LADY", 7),
    TRAINER_CLASS(RUIN_MANIAC, "RUIN MANIAC", 12),
    TRAINER_CLASS(LADY, "LADY", 50),
    TRAINER_CLASS(PAINTER, "PAINTER", 4),
    TRAINER_CLASS(TWINS, "TWINS", 3),
    TRAINER_CLASS(YOUNG_COUPLE, "YOUNG COUPLE", 7),
    TRAINER_CLASS(SIS_AND_BRO, "SIS AND BRO", 1),
    TRAINER_CLASS(COOL_COUPLE, "COOL COUPLE", 6),
    TRAINER_CLASS(CRUSH_KIN, "CRUSH KIN", 6),
    TRAINER_CLASS(SWIMMER_F, "SWIMMER♀", 1, ITEM_DIVE_BALL),
    TRAINER_CLASS(PLAYER, "PLAYER", 1),
    TRAINER_CLASS(RS_LEADER, "LEADER", 25),
    TRAINER_CLASS(RS_ELITE_FOUR, "ELITE FOUR", 25, ITEM_ULTRA_BALL),
    TRAINER_CLASS(RS_LASS, "LASS", 4),
    TRAINER_CLASS(RS_YOUNGSTER, "YOUNGSTER", 4),
    TRAINER_CLASS(PKMN_TRAINER, "{PKMN} TRAINER", 15),
    TRAINER_CLASS(RS_HIKER, "HIKER", 10),
    TRAINER_CLASS(RS_BEAUTY, "BEAUTY", 20),
    TRAINER_CLASS(RS_FISHERMAN, "FISHERMAN", 10, B_TRAINER_CLASS_POKE_BALLS >= GEN_8 ? ITEM_DIVE_BALL : ITEM_LURE_BALL),
    TRAINER_CLASS(RS_LADY, "LADY", 50),
    TRAINER_CLASS(TRIATHLETE, "TRIATHLETE", 10),
    TRAINER_CLASS(TEAM_AQUA, "TEAM AQUA", 5),
    TRAINER_CLASS(RS_TWINS, "TWINS", 3),
    TRAINER_CLASS(RS_SWIMMER_F, "SWIMMER♀", 2, ITEM_DIVE_BALL),
    TRAINER_CLASS(RS_BUG_CATCHER, "BUG CATCHER", 4),
    TRAINER_CLASS(SCHOOL_KID, "SCHOOL KID", 5),
    TRAINER_CLASS(RICH_BOY, "RICH BOY", 50),
    TRAINER_CLASS(SR_AND_JR, "SR. AND JR.", 4),
    TRAINER_CLASS(RS_BLACK_BELT, "BLACK BELT", 8, ITEM_ULTRA_BALL),
    TRAINER_CLASS(RS_TUBER_F, "TUBER", 1),
    TRAINER_CLASS(HEX_MANIAC, "HEX MANIAC", 6),
    TRAINER_CLASS(RS_PKMN_BREEDER, "{PKMN} BREEDER", 10, B_TRAINER_CLASS_POKE_BALLS >= GEN_8 ? ITEM_HEAL_BALL : ITEM_FRIEND_BALL),
    TRAINER_CLASS(TEAM_MAGMA, "TEAM MAGMA", 5),
    TRAINER_CLASS(INTERVIEWER, "INTERVIEWER", 12),
    TRAINER_CLASS(RS_TUBER_M, "TUBER", 1),
    TRAINER_CLASS(RS_YOUNG_COUPLE, "YOUNG COUPLE", 8),
    TRAINER_CLASS(GUITARIST, "GUITARIST", 8),
    TRAINER_CLASS(RS_GENTLEMAN, "GENTLEMAN", 20, ITEM_LUXURY_BALL),
    TRAINER_CLASS(RS_CHAMPION, "CHAMPION", 50),
    TRAINER_CLASS(MAGMA_LEADER, "MAGMA LEADER", 20, ITEM_MASTER_BALL),
    TRAINER_CLASS(BATTLE_GIRL, "BATTLE GIRL", 6),
    TRAINER_CLASS(RS_SWIMMER_M, "SWIMMER♂", 2, ITEM_DIVE_BALL),
    TRAINER_CLASS(POKEFAN, "POKéFAN", 20),
    TRAINER_CLASS(EXPERT, "EXPERT", 10),
    TRAINER_CLASS(DRAGON_TAMER, "DRAGON TAMER", 12),
    TRAINER_CLASS(RS_BIRD_KEEPER, "BIRD KEEPER", 8),
    TRAINER_CLASS(NINJA_BOY, "NINJA BOY", 3),
    TRAINER_CLASS(PARASOL_LADY, "PARASOL LADY", 10),
    TRAINER_CLASS(BUG_MANIAC, "BUG MANIAC", 15),
    TRAINER_CLASS(RS_SAILOR, "SAILOR", 8),
    TRAINER_CLASS(COLLECTOR, "COLLECTOR", 15, ITEM_PREMIER_BALL),
    TRAINER_CLASS(RS_PKMN_RANGER, "{PKMN} RANGER", 12),
    TRAINER_CLASS(MAGMA_ADMIN, "MAGMA ADMIN", 10),
    TRAINER_CLASS(RS_AROMA_LADY, "AROMA LADY", 10),
    TRAINER_CLASS(RS_RUIN_MANIAC, "RUNE MANIAC", 15),
    TRAINER_CLASS(RS_COOLTRAINER, "COOLTRAINER", 12, ITEM_ULTRA_BALL),
    TRAINER_CLASS(RS_POKEMANIAC, "POKéMANIAC", 15),
    TRAINER_CLASS(KINDLER, "KINDLER", 8),
    TRAINER_CLASS(RS_CAMPER, "CAMPER", 4),
    TRAINER_CLASS(RS_PICNICKER, "PICNICKER", 4),
    TRAINER_CLASS(RS_PSYCHIC, "PSYCHIC", 6),
    TRAINER_CLASS(RS_SIS_AND_BRO, "SIS AND BRO", 3),
    TRAINER_CLASS(OLD_COUPLE, "OLD COUPLE", 10),
    TRAINER_CLASS(AQUA_ADMIN, "AQUA ADMIN", 10),
    TRAINER_CLASS(AQUA_LEADER, "AQUA LEADER", 20, ITEM_MASTER_BALL),
    TRAINER_CLASS(BOSS, "BOSS", 25),
};

static void (*const sTurnActionsFuncsTable[])(void) =
{
    [B_ACTION_USE_MOVE]               = HandleAction_UseMove,
    [B_ACTION_USE_ITEM]               = HandleAction_UseItem,
    [B_ACTION_SWITCH]                 = HandleAction_Switch,
    [B_ACTION_RUN]                    = HandleAction_Run,
    [B_ACTION_SAFARI_WATCH_CAREFULLY] = HandleAction_WatchesCarefully,
    [B_ACTION_SAFARI_BALL]            = HandleAction_SafariZoneBallThrow,
    [B_ACTION_SAFARI_BAIT]            = HandleAction_ThrowBait,
    [B_ACTION_SAFARI_GO_NEAR]         = HandleAction_ThrowRock,
    [B_ACTION_SAFARI_RUN]             = HandleAction_SafariZoneRun,
    [B_ACTION_OLDMAN_THROW]           = HandleAction_OldManBallThrow,
    [B_ACTION_EXEC_SCRIPT]            = HandleAction_RunBattleScript,
    [B_ACTION_TRY_FINISH]             = HandleAction_TryFinish,
    [B_ACTION_FINISHED]               = HandleAction_ActionFinished,
    [B_ACTION_NOTHING_FAINTED]        = HandleAction_NothingIsFainted,
    [B_ACTION_THROW_BALL]             = HandleAction_ThrowBall,
};

static void (*const sEndTurnFuncsTable[])(void) =
{
    [0]                           = HandleEndTurn_ContinueBattle,
    [B_OUTCOME_WON]               = HandleEndTurn_BattleWon,
    [B_OUTCOME_LOST]              = HandleEndTurn_BattleLost,
    [B_OUTCOME_DREW]              = HandleEndTurn_BattleLost,
    [B_OUTCOME_RAN]               = HandleEndTurn_RanFromBattle,
    [B_OUTCOME_PLAYER_TELEPORTED] = HandleEndTurn_FinishBattle,
    [B_OUTCOME_MON_FLED]          = HandleEndTurn_MonFled,
    [B_OUTCOME_CAUGHT]            = HandleEndTurn_FinishBattle,
    [B_OUTCOME_NO_SAFARI_BALLS]   = HandleEndTurn_FinishBattle,
};

const u8 gStatusConditionString_PoisonJpn[] = _("どく$$$$$");
const u8 gStatusConditionString_SleepJpn[] = _("ねむり$$$$");
const u8 gStatusConditionString_ParalysisJpn[] = _("まひ$$$$$");
const u8 gStatusConditionString_BurnJpn[] = _("やけど$$$$");
const u8 gStatusConditionString_IceJpn[] = _("こおり$$$$");
const u8 gStatusConditionString_ConfusionJpn[] = _("こんらん$$$");
const u8 gStatusConditionString_LoveJpn[] = _("メロメロ$$$");

const u8 *const gStatusConditionStringsTable[][2] =
{
    {gStatusConditionString_PoisonJpn, gText_Poison},
    {gStatusConditionString_SleepJpn, gText_Sleep},
    {gStatusConditionString_ParalysisJpn, gText_Paralysis},
    {gStatusConditionString_BurnJpn, gText_Burn},
    {gStatusConditionString_IceJpn, gText_Ice},
    {gStatusConditionString_ConfusionJpn, gText_Confusion},
    {gStatusConditionString_LoveJpn, gText_Love}
};

void CB2_InitBattle(void)
{
    MoveSaveBlocks_ResetHeap();
    AllocateBattleResources();
    AllocateBattleSpritesData();
    AllocateMonSpritesGfx();
    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        HandleLinkBattleSetup();
        SetMainCallback2(CB2_PreInitMultiBattle);
        gBattleCommunication[MULTIUSE_STATE] = 0;
    }
    else
    {
        CB2_InitBattleInternal();
        if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                    SetHelpContext(HELPCONTEXT_TRAINER_BATTLE_DOUBLE);
                else
                    SetHelpContext(HELPCONTEXT_TRAINER_BATTLE_SINGLE);
            }
            else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
            {
                SetHelpContext(HELPCONTEXT_SAFARI_BATTLE);
            }
            else
            {
                SetHelpContext(HELPCONTEXT_WILD_BATTLE);
            }
        }
    }
}

static void CB2_InitBattleInternal(void)
{
    s32 i;

    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);

    CpuFill32(0, (void *)(VRAM), VRAM_SIZE);

    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    SetGpuReg(REG_OFFSET_WIN0H, DISPLAY_WIDTH);
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(DISPLAY_HEIGHT / 2, DISPLAY_HEIGHT / 2 + 1));
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);

    gBattle_WIN0H = DISPLAY_WIDTH;

    // TODO: Implement partner battles
    /*if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && gPartnerTrainerId < TRAINER_PARTNER(PARTNER_NONE))
    {
        gBattle_WIN0V = DISPLAY_HEIGHT - 1;
        gBattle_WIN1H = DISPLAY_WIDTH;
        gBattle_WIN1V = 32;
    }
    else
    {*/
        gBattle_WIN0V = WIN_RANGE(DISPLAY_HEIGHT / 2, DISPLAY_HEIGHT / 2 + 1);
        ScanlineEffect_Clear();

        for (i = 0; i < 80; ++i)
        {
            gScanlineEffectRegBuffers[0][i] = 0xF0;
            gScanlineEffectRegBuffers[1][i] = 0xF0;
        }
        for (; i < 160; ++i)
        {
            gScanlineEffectRegBuffers[0][i] = 0xFF10;
            gScanlineEffectRegBuffers[1][i] = 0xFF10;
        }
        ScanlineEffect_SetParams(sIntroScanlineParams16Bit);
    //}

    ResetPaletteFade();
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;

    #if DEBUG_OVERWORLD_MENU == TRUE
    if (!gIsDebugBattle)
#endif
    {
        gBattleTerrain = BattleSetup_GetTerrainId();
    }

    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && !(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER
                                                                        | BATTLE_TYPE_TRAINER_TOWER)))
    {
        gBattleTypeFlags |= (IsTrainerDoubleBattle(gTrainerBattleOpponent_A) ? BATTLE_TYPE_DOUBLE : 0);
    }

    InitBattleBgsVideo();
    LoadBattleTextboxAndBackground();
    ResetSpriteData();
    ResetTasks();
    DrawBattleEntryBackground();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 4;
    SetVBlankCallback(VBlankCB_Battle);
    SetUpBattleVars();

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        SetMainCallback2(CB2_HandleStartMultiBattle);
    else
        SetMainCallback2(CB2_HandleStartBattle);
#if DEBUG_OVERWORLD_MENU == TRUE
    if (!gIsDebugBattle)
#endif
    {
        if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK)))
        {
            CreateNPCTrainerParty(&gEnemyParty[0], gTrainerBattleOpponent_A, TRUE);
            // TODO: Implement 2v1 battles
            //if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !BATTLE_TWO_VS_ONE_OPPONENT)
            //    CreateNPCTrainerParty(&gEnemyParty[PARTY_SIZE / 2], gTrainerBattleOpponent_B, FALSE);
            SetWildMonHeldItem();
            CalculateEnemyPartyCount();
        }
    }

    gMain.inBattle = TRUE;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        AdjustFriendship(&gPlayerParty[i], FRIENDSHIP_EVENT_LEAGUE_BATTLE);
        // Apply party-wide start-of-battle form changes for both sides.
        TryFormChange(i, B_SIDE_PLAYER, FORM_CHANGE_BEGIN_BATTLE);
        TryFormChange(i, B_SIDE_OPPONENT, FORM_CHANGE_BEGIN_BATTLE);
    }
    
    gBattleCommunication[MULTIUSE_STATE] = 0;
}

#define BUFFER_PARTY_VS_SCREEN_STATUS(party, flags, i)                    \
    for ((i) = 0; (i) < PARTY_SIZE; (i)++)                                \
    {                                                                     \
        u16 species = GetMonData(&(party)[(i)], MON_DATA_SPECIES_OR_EGG); \
        u16 hp = GetMonData(&(party)[(i)], MON_DATA_HP);                  \
        u32 status = GetMonData(&(party)[(i)], MON_DATA_STATUS);          \
                                                                          \
        if (species == SPECIES_NONE)                                      \
            continue;                                                     \
                                                                          \
        /* Is healthy mon? */                                             \
        if (species != SPECIES_EGG && hp != 0 && status == 0)             \
            (flags) |= 1 << (i) * 2;                                      \
                                                                          \
        if (species == SPECIES_NONE) /* Redundant */                      \
            continue;                                                     \
                                                                          \
        /* Is Egg or statused? */                                         \
        if (hp != 0 && (species == SPECIES_EGG || status != 0))           \
            (flags) |= 2 << (i) * 2;                                      \
                                                                          \
        if (species == SPECIES_NONE) /* Redundant */                      \
            continue;                                                     \
                                                                          \
        /* Is fainted? */                                                 \
        if (species != SPECIES_EGG && hp == 0)                            \
            (flags) |= 3 << (i) * 2;                                      \
    }

// For Vs Screen at link battle start
static void BufferPartyVsScreenHealth_AtStart(void)
{
    u16 flags = 0;
    s32 i;

    BUFFER_PARTY_VS_SCREEN_STATUS(gPlayerParty, flags, i);
    gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsLo = flags;
    *(&gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsHi) = flags >> 8;
}

static void SetPlayerBerryDataInBattleStruct(void)
{
    s32 i;
    struct BattleStruct *battleStruct = gBattleStruct;
    struct BattleEnigmaBerry *battleBerry = &battleStruct->multiBuffer.linkBattlerHeader.battleEnigmaBerry;

    if (IsEnigmaBerryValid() == TRUE)
    {
        #if FREE_ENIGMA_BERRY == FALSE
        for (i = 0; i < BERRY_NAME_LENGTH; i++)
            battleBerry->name[i] = gSaveBlock1Ptr->enigmaBerry.berry.name[i];
        battleBerry->name[i] = EOS;
        for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            battleBerry->itemEffect[i] = gSaveBlock1Ptr->enigmaBerry.itemEffect[i];
        battleBerry->holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
        battleBerry->holdEffectParam = gSaveBlock1Ptr->enigmaBerry.holdEffectParam;
        #endif
    }
    else
    {
        const struct Berry *berryData = GetBerryInfo(ItemIdToBerryType(ITEM_ENIGMA_BERRY_E_READER));

        for (i = 0; i < BERRY_NAME_LENGTH; i++)
            battleBerry->name[i] = berryData->name[i];
        battleBerry->name[i] = EOS;
        for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            battleBerry->itemEffect[i] = 0;
        battleBerry->holdEffect = HOLD_EFFECT_NONE;
        battleBerry->holdEffectParam = 0;
    }
}

static void SetAllPlayersBerryData(void)
{
    s32 i, j;

    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        if (IsEnigmaBerryValid() == TRUE)
        {
            #if FREE_ENIGMA_BERRY == FALSE
            for (i = 0; i < BERRY_NAME_LENGTH; i++)
            {
                gEnigmaBerries[0].name[i] = gSaveBlock1Ptr->enigmaBerry.berry.name[i];
                gEnigmaBerries[2].name[i] = gSaveBlock1Ptr->enigmaBerry.berry.name[i];
            }
            gEnigmaBerries[0].name[i] = EOS;
            gEnigmaBerries[2].name[i] = EOS;

            for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            {
                gEnigmaBerries[0].itemEffect[i] = gSaveBlock1Ptr->enigmaBerry.itemEffect[i];
                gEnigmaBerries[2].itemEffect[i] = gSaveBlock1Ptr->enigmaBerry.itemEffect[i];
            }

            gEnigmaBerries[0].holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
            gEnigmaBerries[2].holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
            gEnigmaBerries[0].holdEffectParam = gSaveBlock1Ptr->enigmaBerry.holdEffectParam;
            gEnigmaBerries[2].holdEffectParam = gSaveBlock1Ptr->enigmaBerry.holdEffectParam;
            #endif
        }
        else
        {
            const struct Berry *berryData = GetBerryInfo(ItemIdToBerryType(ITEM_ENIGMA_BERRY_E_READER));

            for (i = 0; i < BERRY_NAME_LENGTH; i++)
            {
                gEnigmaBerries[0].name[i] = berryData->name[i];
                gEnigmaBerries[2].name[i] = berryData->name[i];
            }
            gEnigmaBerries[0].name[i] = EOS;
            gEnigmaBerries[2].name[i] = EOS;

            for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            {
                gEnigmaBerries[0].itemEffect[i] = 0;
                gEnigmaBerries[2].itemEffect[i] = 0;
            }

            gEnigmaBerries[0].holdEffect = HOLD_EFFECT_NONE;
            gEnigmaBerries[2].holdEffect = HOLD_EFFECT_NONE;
            gEnigmaBerries[0].holdEffectParam = 0;
            gEnigmaBerries[2].holdEffectParam = 0;
        }
    }
    else
    {
        s32 numPlayers;
        struct BattleEnigmaBerry *src;
        u32 battler;

        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            for (i = 0; i < 4; i++)
            {
                src = (struct BattleEnigmaBerry *)(gBlockRecvBuffer[i] + 2);
                battler = gLinkPlayers[i].id;
                for (j = 0; j < BERRY_NAME_LENGTH; j++)
                    gEnigmaBerries[battler].name[j] = src->name[j];
                gEnigmaBerries[battler].name[j] = EOS;
                for (j = 0; j < BERRY_ITEM_EFFECT_COUNT; j++)
                    gEnigmaBerries[battler].itemEffect[j] = src->itemEffect[j];
                gEnigmaBerries[battler].holdEffect = src->holdEffect;
                gEnigmaBerries[battler].holdEffectParam = src->holdEffectParam;
            }
        }
        else
        {
            for (i = 0; i < 2; i++)
            {
                src = (struct BattleEnigmaBerry *)(gBlockRecvBuffer[i] + 2);
                for (j = 0; j < BERRY_NAME_LENGTH; j++)
                {
                    gEnigmaBerries[i].name[j] = src->name[j];
                    gEnigmaBerries[i + 2].name[j] = src->name[j];
                }
                gEnigmaBerries[i].name[j] = EOS;
                gEnigmaBerries[i + 2].name[j] = EOS;
                for (j = 0; j < BERRY_ITEM_EFFECT_COUNT; j++)
                {
                    gEnigmaBerries[i].itemEffect[j] = src->itemEffect[j];
                    gEnigmaBerries[i + 2].itemEffect[j] = src->itemEffect[j];
                }
                gEnigmaBerries[i].holdEffect = src->holdEffect;
                gEnigmaBerries[i + 2].holdEffect = src->holdEffect;
                gEnigmaBerries[i].holdEffectParam = src->holdEffectParam;
                gEnigmaBerries[i + 2].holdEffectParam = src->holdEffectParam;
            }
        }
    }
}

static void LinkBattleComputeBattleTypeFlags(u8 numPlayers, u8 multiPlayerId)
{
    u8 found = 0;

    // If player 1 is playing the minimum version, player 1 is master.
    if (gBlockRecvBuffer[0][0] == 0x100)
    {
        if (multiPlayerId == 0)
            gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER | BATTLE_TYPE_TRAINER;
        else
            gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
        found++;
    }
    if (found == 0)
    {
        // If multiple different versions are being used, player 1 is master.
        s32 i;

        for (i = 0; i < numPlayers; i++)
            if (gBlockRecvBuffer[0][0] != gBlockRecvBuffer[i][0])
                break;
        if (i == numPlayers)
        {
            if (multiPlayerId == 0)
                gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER | BATTLE_TYPE_TRAINER;
            else
                gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
            found++;
        }
        if (found == 0)
        {
            // Lowest index player with the highest game version is master.
            for (i = 0; i < numPlayers; i++)
            {
                if (gBlockRecvBuffer[i][0] == 0x201 && i != multiPlayerId)
                    if (i < multiPlayerId)
                        break;
                if (gBlockRecvBuffer[i][0] > 0x201 && i != multiPlayerId)
                    break;
            }
            if (i == numPlayers)
                gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER | BATTLE_TYPE_TRAINER;
            else
                gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
        }
    }
}

static void CB2_HandleStartBattle(void)
{
    u8 playerMultiplayerId;
    u8 enemyMultiplayerId;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    playerMultiplayerId = GetMultiplayerId();
    gBattleStruct->multiplayerId = playerMultiplayerId;
    enemyMultiplayerId = playerMultiplayerId ^ BIT_SIDE;
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            ShowBg(0);
            ShowBg(1);
            ShowBg(2);
            ShowBg(3);
            BattleInterfaceSetWindowPals();
            gBattleCommunication[MULTIUSE_STATE] = 1;
        }
        if (gWirelessCommType)
            LoadWirelessStatusIndicatorSpriteGfx();
        break;
    case 1:
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            if (gReceivedRemoteLinkPlayers)
            {
                if (IsLinkTaskFinished())
                {
                    // 0x201
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureLo) = 1;
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureHi) = 2;
                    BufferPartyVsScreenHealth_AtStart();
                    SetPlayerBerryDataInBattleStruct();
                    SendBlock(BitmaskAllOtherLinkPlayers(), &gBattleStruct->multiBuffer.linkBattlerHeader, sizeof(gBattleStruct->multiBuffer.linkBattlerHeader));
                    gBattleCommunication[MULTIUSE_STATE] = 2;
                }
                if (gWirelessCommType != 0)
                    CreateWirelessStatusIndicatorSprite(0, 0);
            }
        }
        else
        {
            gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER;
            gBattleCommunication[MULTIUSE_STATE] = 15;
            SetAllPlayersBerryData();
        }
        break;
    case 2:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            u8 taskId;

            ResetBlockReceivedFlags();
            LinkBattleComputeBattleTypeFlags(2, playerMultiplayerId);
            SetAllPlayersBerryData();
            taskId = CreateTask(InitLinkBattleVsScreen, 0);
            gTasks[taskId].data[1] = 270;
            gTasks[taskId].data[2] = 90;
            gTasks[taskId].data[5] = 0;
            gTasks[taskId].data[3] = gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsLo | (gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsHi << 8);
            gTasks[taskId].data[4] = gBlockRecvBuffer[enemyMultiplayerId][1];
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 3:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gPlayerParty, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 4:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 7:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gPlayerParty + 2, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 8:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty + 2, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 11:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gPlayerParty + 4, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 12:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty + 4, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
            TryCorrectShedinjaLanguage(&gEnemyParty[0]);
            TryCorrectShedinjaLanguage(&gEnemyParty[1]);
            TryCorrectShedinjaLanguage(&gEnemyParty[2]);
            TryCorrectShedinjaLanguage(&gEnemyParty[3]);
            TryCorrectShedinjaLanguage(&gEnemyParty[4]);
            TryCorrectShedinjaLanguage(&gEnemyParty[5]);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 15:
        InitBattleControllers();
        gBattleCommunication[MULTIUSE_STATE]++;
        gBattleCommunication[SPRITES_INIT_STATE1] = 0;
        gBattleCommunication[SPRITES_INIT_STATE2] = 0;
        break;
    case 16:
        if (BattleInitAllSprites(&gBattleCommunication[SPRITES_INIT_STATE1], &gBattleCommunication[SPRITES_INIT_STATE2]))
        {
            gPreBattleCallback1 = gMain.callback1;
            gMain.callback1 = BattleMainCB1;
            SetMainCallback2(BattleMainCB2);
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                gBattleTypeFlags |= BATTLE_TYPE_LINK_IN_BATTLE;
            }
        }
        break;
    case 5:
    case 9:
    case 13:
        gBattleCommunication[MULTIUSE_STATE]++;
        gBattleCommunication[1] = 1;
    case 6:
    case 10:
    case 14:
        if (--gBattleCommunication[1] == 0)
            gBattleCommunication[MULTIUSE_STATE]++;
        break;
    }
}

static void PrepareOwnMultiPartnerBuffer(void)
{
    s32 i, j;
    u8 *nick, *cur;

    for (i = 0; i < 3; i++)
    {
        gMultiPartnerParty[i].species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        gMultiPartnerParty[i].heldItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
        nick = gMultiPartnerParty[i].nickname;
        GetMonData(&gPlayerParty[i], MON_DATA_NICKNAME, nick);
        gMultiPartnerParty[i].level = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
        gMultiPartnerParty[i].hp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
        gMultiPartnerParty[i].maxhp = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
        gMultiPartnerParty[i].status = GetMonData(&gPlayerParty[i], MON_DATA_STATUS);
        gMultiPartnerParty[i].personality = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY);
        gMultiPartnerParty[i].gender = GetMonGender(&gPlayerParty[i]);
        StripExtCtrlCodes(nick);
        if (GetMonData(&gPlayerParty[i], MON_DATA_LANGUAGE) != LANGUAGE_JAPANESE)
        {
            for (cur = nick, j = 0; cur[j] != EOS; j++)
                ;
            while (j < 6)
                cur[j++] = 0;
            cur[j] = EOS;
        }
    }
    memcpy(gBattleStruct->multiBuffer.multiBattleMons, gMultiPartnerParty, sizeof(gMultiPartnerParty));
}

static void CB2_PreInitMultiBattle(void)
{
    s32 i;
    u8 playerMultiplierId;
    u8 r4 = 0xF;
    u16 *savedBattleTypeFlags;
    void (**savedCallback)(void);

    playerMultiplierId = GetMultiplayerId();
    gBattleStruct->multiplayerId = playerMultiplierId;
    savedCallback = &gBattleStruct->savedCallback;
    savedBattleTypeFlags = &gBattleStruct->savedBattleTypeFlags;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        if (gReceivedRemoteLinkPlayers && IsLinkTaskFinished())
        {
            PrepareOwnMultiPartnerBuffer();
            SendBlock(BitmaskAllOtherLinkPlayers(), gBattleStruct->multiBuffer.multiBattleMons, sizeof(gBattleStruct->multiBuffer.multiBattleMons));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 1:
        if ((GetBlockReceivedStatus() & r4) == r4)
        {
            ResetBlockReceivedFlags();
            for (i = 0; i < 4; i++)
            {
                if (i == playerMultiplierId)
                    continue;
                if ((!(gLinkPlayers[i].id & 1) && !(gLinkPlayers[playerMultiplierId].id & 1))
                 || (gLinkPlayers[i].id & 1 && gLinkPlayers[playerMultiplierId].id & 1))
                    memcpy(gMultiPartnerParty, gBlockRecvBuffer[i], sizeof(gMultiPartnerParty));
            }
            gBattleCommunication[MULTIUSE_STATE]++;
            *savedCallback = gMain.savedCallback;
            *savedBattleTypeFlags = gBattleTypeFlags;
            gMain.savedCallback = CB2_PreInitMultiBattle;
            ShowPartyMenuToShowcaseMultiBattleParty();
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            gBattleCommunication[MULTIUSE_STATE]++;
            if (gWirelessCommType)
                SetLinkStandbyCallback();
            else
                SetCloseLinkCallback();
        }
        break;
    case 3:
        if (gWirelessCommType)
        {
            if (IsLinkRfuTaskFinished())
            {
                gBattleTypeFlags = *savedBattleTypeFlags;
                gMain.savedCallback = *savedCallback;
                SetMainCallback2(CB2_InitBattleInternal);
            }
        }
        else if (!gReceivedRemoteLinkPlayers)
        {
            gBattleTypeFlags = *savedBattleTypeFlags;
            gMain.savedCallback = *savedCallback;
            SetMainCallback2(CB2_InitBattleInternal);
        }
        break;
    }
}

static void CB2_HandleStartMultiBattle(void)
{
    u8 playerMultiplayerId;
    s32 id;
    u8 taskId;

    playerMultiplayerId = GetMultiplayerId();
    gBattleStruct->multiplayerId = playerMultiplayerId;
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            ShowBg(0);
            ShowBg(1);
            ShowBg(2);
            ShowBg(3);
            BattleInterfaceSetWindowPals();
            gBattleCommunication[MULTIUSE_STATE] = 1;
        }
        if (gWirelessCommType)
            LoadWirelessStatusIndicatorSpriteGfx();
        break;
    case 1:
        if (gReceivedRemoteLinkPlayers)
        {
            if (IsLinkTaskFinished())
            {
                // 0x201
                *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureLo) = 1;
                *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureHi) = 2;
                BufferPartyVsScreenHealth_AtStart();
                SetPlayerBerryDataInBattleStruct();
                SendBlock(BitmaskAllOtherLinkPlayers(), &gBattleStruct->multiBuffer.linkBattlerHeader, sizeof(gBattleStruct->multiBuffer.linkBattlerHeader));
                gBattleCommunication[MULTIUSE_STATE]++;
            }
            if (gWirelessCommType)
                CreateWirelessStatusIndicatorSprite(0, 0);
        }
        break;
    case 2:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            LinkBattleComputeBattleTypeFlags(4, playerMultiplayerId);
            SetAllPlayersBerryData();
            memcpy(gDecompressionBuffer, gPlayerParty, sizeof(struct Pokemon) * 3);
            taskId = CreateTask(InitLinkBattleVsScreen, 0);
            gTasks[taskId].data[1] = 270;
            gTasks[taskId].data[2] = 90;
            gTasks[taskId].data[5] = 0;
            gTasks[taskId].data[3] = 0;
            gTasks[taskId].data[4] = 0;
            for (id = 0; id < MAX_LINK_PLAYERS; ++id)
            {
                switch (gLinkPlayers[id].id)
                {
                case 0:
                    gTasks[taskId].data[3] |= gBlockRecvBuffer[id][1] & 0x3F;
                    break;
                case 1:
                    gTasks[taskId].data[4] |= gBlockRecvBuffer[id][1] & 0x3F;
                    break;
                case 2:
                    gTasks[taskId].data[3] |= (gBlockRecvBuffer[id][1] & 0x3F) << 6;
                    break;
                case 3:
                    gTasks[taskId].data[4] |= (gBlockRecvBuffer[id][1] & 0x3F) << 6;
                    break;
                }
            }
            ZeroPlayerPartyMons();
            ZeroEnemyPartyMons();
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        else
        {
            break;
        }
        // fall through
    case 3:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gDecompressionBuffer, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 4:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            for (id = 0; id < MAX_LINK_PLAYERS; ++id)
            {
                if (id == playerMultiplayerId)
                {
                    switch (gLinkPlayers[id].id)
                    {
                    case 0:
                    case 3:
                        memcpy(gPlayerParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                        break;
                    case 1:
                    case 2:
                        memcpy(gPlayerParty + 3, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                        break;
                    }
                }
                else
                {
                    if ((!(gLinkPlayers[id].id & 1) && !(gLinkPlayers[playerMultiplayerId].id & 1))
                     || ((gLinkPlayers[id].id & 1) && (gLinkPlayers[playerMultiplayerId].id & 1)))
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gPlayerParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        case 1:
                        case 2:
                            memcpy(gPlayerParty + 3, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        }
                    }
                    else
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gEnemyParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        case 1:
                        case 2:
                            memcpy(gEnemyParty + 3, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        }
                    }
                }
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 7:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gDecompressionBuffer + sizeof(struct Pokemon) * 2, sizeof(struct Pokemon));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 8:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            for (id = 0; id < MAX_LINK_PLAYERS; ++id)
            {
                if (id == playerMultiplayerId)
                {
                    switch (gLinkPlayers[id].id)
                    {
                    case 0:
                    case 3:
                        memcpy(gPlayerParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                        break;
                    case 1:
                    case 2:
                        memcpy(gPlayerParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                        break;
                    }
                }
                else
                {
                    if ((!(gLinkPlayers[id].id & 1) && !(gLinkPlayers[playerMultiplayerId].id & 1))
                     || ((gLinkPlayers[id].id & 1) && (gLinkPlayers[playerMultiplayerId].id & 1)))
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gPlayerParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        case 1:
                        case 2:
                            memcpy(gPlayerParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        }
                    }
                    else
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gEnemyParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        case 1:
                        case 2:
                            memcpy(gEnemyParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        }
                    }
                }
            }
            TryCorrectShedinjaLanguage(&gPlayerParty[0]);
            TryCorrectShedinjaLanguage(&gPlayerParty[1]);
            TryCorrectShedinjaLanguage(&gPlayerParty[2]);
            TryCorrectShedinjaLanguage(&gPlayerParty[3]);
            TryCorrectShedinjaLanguage(&gPlayerParty[4]);
            TryCorrectShedinjaLanguage(&gPlayerParty[5]);
            TryCorrectShedinjaLanguage(&gEnemyParty[0]);
            TryCorrectShedinjaLanguage(&gEnemyParty[1]);
            TryCorrectShedinjaLanguage(&gEnemyParty[2]);
            TryCorrectShedinjaLanguage(&gEnemyParty[3]);
            TryCorrectShedinjaLanguage(&gEnemyParty[4]);
            TryCorrectShedinjaLanguage(&gEnemyParty[5]);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 11:
        InitBattleControllers();
        gBattleCommunication[MULTIUSE_STATE]++;
        gBattleCommunication[SPRITES_INIT_STATE1] = 0;
        gBattleCommunication[SPRITES_INIT_STATE2] = 0;
        break;
    case 12:
        if (BattleInitAllSprites(&gBattleCommunication[SPRITES_INIT_STATE1], &gBattleCommunication[SPRITES_INIT_STATE2]))
        {
            gPreBattleCallback1 = gMain.callback1;
            gMain.callback1 = BattleMainCB1;
            SetMainCallback2(BattleMainCB2);
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
                gBattleTypeFlags |= BATTLE_TYPE_LINK_IN_BATTLE;
        }
        break;
    case 5:
    case 9:
        ++gBattleCommunication[0];
        gBattleCommunication[SPRITES_INIT_STATE1] = 1;
        // fall through
    case 6:
    case 10:
        if (--gBattleCommunication[SPRITES_INIT_STATE1] == 0)
            ++gBattleCommunication[0];
        break;
    }
}

void BattleMainCB2(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    UpdatePaletteFade();
    RunTasks();

    if (JOY_HELD(B_BUTTON) && gBattleTypeFlags & BATTLE_TYPE_POKEDUDE)
    {
        gSpecialVar_Result = gBattleOutcome = B_OUTCOME_DREW;
        ResetPaletteFadeControl();
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        SetMainCallback2(CB2_QuitPokedudeBattle);
    }
}

void FreeRestoreBattleData(void)
{
    gMain.callback1 = gPreBattleCallback1;
    gScanlineEffect.state = 3;
    gMain.inBattle = FALSE;
    ZeroEnemyPartyMons();
    m4aSongNumStop(SE_LOW_HEALTH);
    FreeMonSpritesGfx();
    FreeBattleSpritesData();
    FreeBattleResources();
}

static void CB2_QuitPokedudeBattle(void)
{
    UpdatePaletteFade();
    if (!gPaletteFade.active)
    {
        FreeRestoreBattleData();
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
    }
}

static void SpriteCB_UnusedDebugSprite(struct Sprite *sprite)
{
    sprite->data[0] = 0;
    sprite->callback = SpriteCB_UnusedDebugSprite_Step;
}

static void SpriteCB_UnusedDebugSprite_Step(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sUnknownDebugSpriteDataBuffer = AllocZeroed(0x1000);
        ++sprite->data[0];
        sprite->data[1] = 0;
        sprite->data[2] = 0x281;
        sprite->data[3] = 0;
        sprite->data[4] = 1;
        // fall through
    case 1:
        if (--sprite->data[4] == 0)
        {
            s32 i, r2, r0;

            sprite->data[4] = 2;
            r2 = sprite->data[1] + sprite->data[3] * 32;
            r0 = sprite->data[2] - sprite->data[3] * 32;
            for (i = 0; i <= 29; i += 2)
            {
                *(&sUnknownDebugSpriteDataBuffer[r2] + i) = 0x3D;
                *(&sUnknownDebugSpriteDataBuffer[r0] + i) = 0x3D;
            }
            if (++sprite->data[3] == 21)
            {
                ++sprite->data[0];
                sprite->data[1] = 32;
            }
        }
        break;
    case 2:
        if (--sprite->data[1] == 20)
        {
            if (sUnknownDebugSpriteDataBuffer != NULL)
            {
                memset(sUnknownDebugSpriteDataBuffer, 0, 0x1000);
                FREE_AND_SET_NULL(sUnknownDebugSpriteDataBuffer);
            }
            SetMainCallback2(CB2_InitBattle);
        }
        break;
    }
}

static u32 Crc32B (const u8 *data, u32 size)
{
   s32 i, j;
   u32 byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   for (i = 0; i < size; ++i)
   {
        byte = data[i];
        crc = crc ^ byte;
        for (j = 7; j >= 0; --j)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
   }
   return ~crc;
}

static u32 GeneratePartyHash(const struct Trainer *trainer, u32 i)
{
    const u8 *buffer = (const u8 *) &trainer->party[i];
    u32 n = sizeof(*trainer->party);
    return Crc32B(buffer, n);
}

void ModifyPersonalityForNature(u32 *personality, u32 newNature)
{
    u32 nature = GetNatureFromPersonality(*personality);
    s32 diff = abs((s32)nature - (s32)newNature);
    s32 sign = (nature > newNature) ? 1 : -1;
    if (diff > NUM_NATURES / 2)
    {
        diff = NUM_NATURES - diff;
        sign *= -1;
    }
    *personality -= (diff * sign);
}

u32 GeneratePersonalityForGender(u32 gender, u32 species)
{
    const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[species];
    if (gender == MON_MALE)
        return ((255 - speciesInfo->genderRatio) / 2) + speciesInfo->genderRatio;
    else
        return speciesInfo->genderRatio / 2;
}

void CustomTrainerPartyAssignMoves(struct Pokemon *mon, const struct TrainerMon *partyEntry)
{
    bool32 noMoveSet = TRUE;
    u32 j;

    for (j = 0; j < MAX_MON_MOVES; ++j)
    {
        if (partyEntry->moves[j] != MOVE_NONE)
            noMoveSet = FALSE;
    }
    if (noMoveSet)
    {
        // TODO: Figure out a default strategy when moves are not set, to generate a good moveset
        return;
    }

    for (j = 0; j < MAX_MON_MOVES; ++j)
    {
        SetMonData(mon, MON_DATA_MOVE1 + j, &partyEntry->moves[j]);
        SetMonData(mon, MON_DATA_PP1 + j, &gMovesInfo[partyEntry->moves[j]].pp);
    }
}

u8 CreateNPCTrainerPartyFromTrainer(struct Pokemon *party, const struct Trainer *trainer, bool32 firstTrainer, u32 battleTypeFlags)
{
    u32 personalityValue;
    s32 i;
    u8 monsCount;
    if (battleTypeFlags & BATTLE_TYPE_TRAINER && !(battleTypeFlags & BATTLE_TYPE_EREADER_TRAINER))
    {
        if (firstTrainer == TRUE)
            ZeroEnemyPartyMons();
        // TODO: Implement 2v1 battles
        /*if (battleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
        {
            if (trainer->partySize > PARTY_SIZE / 2)
                monsCount = PARTY_SIZE / 2;
            else
                monsCount = trainer->partySize;
        }
        else
        {*/
            monsCount = trainer->partySize;
        //}
        for (i = 0; i < monsCount; i++)
        {
            s32 ball = -1;
            u32 personalityHash = GeneratePartyHash(trainer, i);
            const struct TrainerMon *partyData = trainer->party;
            u32 otIdType = OT_ID_RANDOM_NO_SHINY;
            u32 fixedOtId = 0;
            u32 ability = 0;
            
            if (trainer->doubleBattle == TRUE)
                personalityValue = 0x80;
            else if (trainer->encounterMusic_gender & F_TRAINER_FEMALE)
                personalityValue = 0x78; // Use personality more likely to result in a female Pokémon
            else
                personalityValue = 0x88; // Use personality more likely to result in a male Pokémon

            personalityValue += personalityHash << 8;
            if (partyData[i].gender == TRAINER_MON_MALE)
                personalityValue = (personalityValue & 0xFFFFFF00) | GeneratePersonalityForGender(MON_MALE, partyData[i].species);
            else if (partyData[i].gender == TRAINER_MON_FEMALE)
                personalityValue = (personalityValue & 0xFFFFFF00) | GeneratePersonalityForGender(MON_FEMALE, partyData[i].species);
            ModifyPersonalityForNature(&personalityValue, partyData[i].nature);

            if (partyData[i].isShiny)
            {
                otIdType = OT_ID_PRESET;
                fixedOtId = HIHALF(personalityValue) ^ LOHALF(personalityValue);
            }
            CreateMon(&party[i], partyData[i].species, partyData[i].lvl, 0, TRUE, personalityValue, otIdType, fixedOtId);
            SetMonData(&party[i], MON_DATA_HELD_ITEM, &partyData[i].heldItem);

            CustomTrainerPartyAssignMoves(&party[i], &partyData[i]);
            SetMonData(&party[i], MON_DATA_IVS, &(partyData[i].iv));
            if (partyData[i].ev != NULL)
            {
                SetMonData(&party[i], MON_DATA_HP_EV, &(partyData[i].ev[0]));
                SetMonData(&party[i], MON_DATA_ATK_EV, &(partyData[i].ev[1]));
                SetMonData(&party[i], MON_DATA_DEF_EV, &(partyData[i].ev[2]));
                SetMonData(&party[i], MON_DATA_SPATK_EV, &(partyData[i].ev[3]));
                SetMonData(&party[i], MON_DATA_SPDEF_EV, &(partyData[i].ev[4]));
                SetMonData(&party[i], MON_DATA_SPEED_EV, &(partyData[i].ev[5]));
            }
            if (partyData[i].ability != ABILITY_NONE)
            {
                const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[partyData[i].species];
                u32 maxAbilities = ARRAY_COUNT(speciesInfo->abilities);
                for (ability = 0; ability < maxAbilities; ++ability)
                {
                    if (speciesInfo->abilities[ability] == partyData[i].ability)
                        break;
                }
                if (ability >= maxAbilities)
                    ability = 0;
            }
            else if (B_TRAINER_MON_RANDOM_ABILITY)
            {
                const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[partyData[i].species];
                ability = personalityHash % 3;
                while (speciesInfo->abilities[ability] == ABILITY_NONE)
                {
                    ability--;
                }
            }
            SetMonData(&party[i], MON_DATA_ABILITY_NUM, &ability);
            SetMonData(&party[i], MON_DATA_FRIENDSHIP, &(partyData[i].friendship));
            if (partyData[i].ball != ITEM_NONE)
            {
                ball = partyData[i].ball;
                SetMonData(&party[i], MON_DATA_POKEBALL, &ball);
            }
            if (partyData[i].nickname != NULL)
            {
                SetMonData(&party[i], MON_DATA_NICKNAME, partyData[i].nickname);
            }
            if (partyData[i].isShiny)
            {
                u32 data = TRUE;
                SetMonData(&party[i], MON_DATA_IS_SHINY, &data);
            }
            if (partyData[i].dynamaxLevel > 0)
            {
                u32 data = partyData[i].dynamaxLevel;
                SetMonData(&party[i], MON_DATA_DYNAMAX_LEVEL, &data);
            }
            if (partyData[i].gigantamaxFactor)
            {
                u32 data = partyData[i].gigantamaxFactor;
                SetMonData(&party[i], MON_DATA_GIGANTAMAX_FACTOR, &data);
            }
            CalculateMonStats(&party[i]);

            if (B_TRAINER_CLASS_POKE_BALLS >= GEN_7 && ball == -1)
            {
                ball = gTrainerClasses[trainer->trainerClass].ball ?: ITEM_POKE_BALL;
                SetMonData(&party[i], MON_DATA_POKEBALL, &ball);
            }
        }
    }

    return trainer->partySize;
}

static u8 CreateNPCTrainerParty(struct Pokemon *party, u16 trainerNum, bool8 firstTrainer)
{
    u8 retVal;
    if (trainerNum == TRAINER_SECRET_BASE)
        return 0;
    retVal = CreateNPCTrainerPartyFromTrainer(party, GetTrainerStructFromId(trainerNum), firstTrainer, gBattleTypeFlags);
    return retVal;
}

void CreateTrainerPartyForPlayer(void)
{
    ZeroPlayerPartyMons();
    // TODO: Implement partner battles
    //gPartnerTrainerId = gSpecialVar_0x8004;
    CreateNPCTrainerPartyFromTrainer(gPlayerParty, GetTrainerStructFromId(gSpecialVar_0x8004), TRUE, BATTLE_TYPE_TRAINER);
}

void VBlankCB_Battle(void)
{
    // Change gRngSeed every vblank.
    AdvanceRandom();

    SetGpuReg(REG_OFFSET_BG0HOFS, gBattle_BG0_X);
    SetGpuReg(REG_OFFSET_BG0VOFS, gBattle_BG0_Y);
    SetGpuReg(REG_OFFSET_BG1HOFS, gBattle_BG1_X);
    SetGpuReg(REG_OFFSET_BG1VOFS, gBattle_BG1_Y);
    SetGpuReg(REG_OFFSET_BG2HOFS, gBattle_BG2_X);
    SetGpuReg(REG_OFFSET_BG2VOFS, gBattle_BG2_Y);
    SetGpuReg(REG_OFFSET_BG3HOFS, gBattle_BG3_X);
    SetGpuReg(REG_OFFSET_BG3VOFS, gBattle_BG3_Y);
    SetGpuReg(REG_OFFSET_WIN0H, gBattle_WIN0H);
    SetGpuReg(REG_OFFSET_WIN0V, gBattle_WIN0V);
    SetGpuReg(REG_OFFSET_WIN1H, gBattle_WIN1H);
    SetGpuReg(REG_OFFSET_WIN1V, gBattle_WIN1V);
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void SpriteCB_VsLetterDummy(struct Sprite *sprite)
{

}

static void SpriteCB_VsLetter(struct Sprite *sprite)
{
    if (sprite->data[0] != 0)
        sprite->x = sprite->data[1] + ((sprite->data[2] & 0xFF00) >> 8);
    else
        sprite->x = sprite->data[1] - ((sprite->data[2] & 0xFF00) >> 8);

    sprite->data[2] += 0x180;

    if (sprite->affineAnimEnded)
    {
        FreeSpriteTilesByTag(ANIM_SPRITES_START);
        FreeSpritePaletteByTag(ANIM_SPRITES_START);
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
    }
}

void SpriteCB_VsLetterInit(struct Sprite *sprite)
{
    StartSpriteAffineAnim(sprite, 1);
    sprite->callback = SpriteCB_VsLetter;
    PlaySE(SE_MUGSHOT);
}

static void BufferPartyVsScreenHealth_AtEnd(u8 taskId)
{
    struct Pokemon *party1 = NULL;
    struct Pokemon *party2 = NULL;
    u8 multiplayerId = gBattleStruct->multiplayerId;
    u32 flags;
    s32 i;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        switch (gLinkPlayers[multiplayerId].id)
        {
        case 0:
        case 2:
            party1 = gPlayerParty;
            party2 = gEnemyParty;
            break;
        case 1:
        case 3:
            party1 = gEnemyParty;
            party2 = gPlayerParty;
            break;
        }
    }
    else
    {
        party1 = gPlayerParty;
        party2 = gEnemyParty;
    }

    flags = 0;
    BUFFER_PARTY_VS_SCREEN_STATUS(party1, flags, i);
    gTasks[taskId].data[3] = flags;

    flags = 0;
    BUFFER_PARTY_VS_SCREEN_STATUS(party2, flags, i);
    gTasks[taskId].data[4] = flags;
}

void CB2_InitEndLinkBattle(void)
{
    s32 i;
    u8 taskId;

    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);
    CpuFill32(0, (void *)VRAM, VRAM_SIZE);
    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, 0xF0));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0x50, 0x51));
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    gBattle_WIN0H = WIN_RANGE(0, 0xF0);
    gBattle_WIN0V = WIN_RANGE(0x50, 0x51);
    ScanlineEffect_Clear();
    for (i = 0; i < 80; i++)
    {
        gScanlineEffectRegBuffers[0][i] = 0xF0;
        gScanlineEffectRegBuffers[1][i] = 0xF0;
    }

    for (; i < 160; i++)
    {
        gScanlineEffectRegBuffers[0][i] = 0xFF10;
        gScanlineEffectRegBuffers[1][i] = 0xFF10;
    }
    ResetPaletteFade();
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;
    InitBattleBgsVideo();
    LoadCompressedPalette(gBattleInterface_Textbox_Pal, BG_PLTT_ID(0), 2 * PLTT_SIZE_4BPP);
    LoadBattleMenuWindowGfx();
    ResetSpriteData();
    ResetTasks();
    DrawBattleEntryBackground();
    SetGpuReg(REG_OFFSET_WINOUT, WININ_WIN0_BG0 | WININ_WIN0_BG1 | WININ_WIN0_BG2 | WININ_WIN0_OBJ | WININ_WIN0_CLR);
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 4;
    SetVBlankCallback(VBlankCB_Battle);
    taskId = CreateTask(InitLinkBattleVsScreen, 0);
    gTasks[taskId].data[1] = 270;
    gTasks[taskId].data[2] = 90;
    gTasks[taskId].data[5] = 1;
    BufferPartyVsScreenHealth_AtEnd(taskId);
    SetMainCallback2(CB2_EndLinkBattle);
    gBattleCommunication[MULTIUSE_STATE] = 0;
}

static void CB2_EndLinkBattle(void)
{
    EndLinkBattleInSteps();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    RunTasks();
}

static void EndLinkBattleInSteps(void)
{
    s32 i;

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        gBattleCommunication[1] = 0xFF;
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 1:
        if (--gBattleCommunication[1] == 0)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            SetMainCallback2(gMain.savedCallback);
            TrySetQuestLogLinkBattleEvent();
            FreeMonSpritesGfx();
            FreeBattleSpritesData();
            FreeBattleResources();
        }
        break;
    }
}

u32 GetBattleBgTemplateData(u8 arrayId, u8 caseId)
{
    u32 ret = 0;

    switch (caseId)
    {
    case 0:
        ret = gBattleBgTemplates[arrayId].bg;
        break;
    case 1:
        ret = gBattleBgTemplates[arrayId].charBaseIndex;
        break;
    case 2:
        ret = gBattleBgTemplates[arrayId].mapBaseIndex;
        break;
    case 3:
        ret = gBattleBgTemplates[arrayId].screenSize;
        break;
    case 4:
        ret = gBattleBgTemplates[arrayId].paletteMode;
        break;
    case 5:
        ret = gBattleBgTemplates[arrayId].priority;
        break;
    case 6:
        ret = gBattleBgTemplates[arrayId].baseTile;
        break;
    }
    return ret;
}

static void TryCorrectShedinjaLanguage(struct Pokemon *mon)
{
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u8 language = LANGUAGE_JAPANESE;

    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_SHEDINJA
     && GetMonData(mon, MON_DATA_LANGUAGE) != language)
    {
        GetMonData(mon, MON_DATA_NICKNAME, nickname);
        if (StringCompareWithoutExtCtrlCodes(nickname, sText_ShedinjaJpnName) == 0)
            SetMonData(mon, MON_DATA_LANGUAGE, &language);
    }
}

#define sBattler            data[0]
#define sSpeciesId          data[2]

void SpriteCB_EnemyMon(struct Sprite *sprite)
{
    sprite->callback = SpriteCB_MoveWildMonToRight;
    StartSpriteAnimIfDifferent(sprite, 0);
    if (WILD_DOUBLE_BATTLE)
        BeginNormalPaletteFade((0x10000 << sprite->sBattler) | (0x10000 << BATTLE_PARTNER(sprite->sBattler)), 0, 10, 10, RGB(8, 8, 8));
    else
        BeginNormalPaletteFade((0x10000 << sprite->sBattler), 0, 10, 10, RGB(8, 8, 8));
}

static void SpriteCB_MoveWildMonToRight(struct Sprite *sprite)
{
    if ((gIntroSlideFlags & 1) == 0)
    {
        sprite->x2 += 2;
        if (sprite->x2 == 0)
        {
            sprite->callback = SpriteCB_WildMonShowHealthbox;
            PlayCry_Normal(sprite->data[2], 25);
        }
    }
}

static void SpriteCB_WildMonShowHealthbox(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        StartHealthboxSlideIn(sprite->sBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[sprite->sBattler]);
        sprite->callback = SpriteCallbackDummy_2;
        StartSpriteAnimIfDifferent(sprite, 0);
        if (WILD_DOUBLE_BATTLE)
            BeginNormalPaletteFade((0x10000 << sprite->sBattler) | (0x10000 << BATTLE_PARTNER(sprite->sBattler)), 0, 10, 0, RGB(8, 8, 8));
        else
            BeginNormalPaletteFade((0x10000 << sprite->sBattler), 0, 10, 0, RGB(8, 8, 8));
    }
}

void SpriteCallbackDummy_2(struct Sprite *sprite)
{
}

void SpriteCB_FaintOpponentMon(struct Sprite *sprite)
{
    u8 battler = sprite->sBattler;
    u32 personality = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);
    u16 species;
    u8 yOffset;

    if (gBattleSpritesDataPtr->battlerData[battler].transformSpecies != 0)
        species = gBattleSpritesDataPtr->battlerData[battler].transformSpecies;
    else
        species = sprite->sSpeciesId;

    species = SanitizeSpeciesId(species);

    if (species == SPECIES_UNOWN)
        species = GetUnownSpeciesId(personality);
    yOffset = gSpeciesInfo[species].frontPicYOffset;

    sprite->data[3] = 8 - yOffset / 8;
    sprite->data[4] = 1;
    sprite->callback = SpriteCB_AnimFaintOpponent;
}

static void SpriteCB_AnimFaintOpponent(struct Sprite *sprite)
{
    s32 i;

    if (--sprite->data[4] == 0)
    {
        sprite->data[4] = 2;
        sprite->y2 += 8; // Move the sprite down.
        if (--sprite->data[3] < 0)
        {
            FreeSpriteOamMatrix(sprite);
            DestroySprite(sprite);
        }
        else // Erase bottom part of the sprite to create a smooth illusion of mon falling down.
        {
            u8 *dst = &gMonSpritesGfxPtr->spritesGfx[GetBattlerPosition(sprite->sBattler)][(sprite->data[3] << 8)];

            for (i = 0; i < 0x100; i++)
                *(dst++) = 0;
            StartSpriteAnim(sprite, 0);
        }
    }
}

// Used when selecting a move, which can hit multiple targets, in double battles.
void SpriteCB_ShowAsMoveTarget(struct Sprite *sprite)
{
    sprite->data[3] = 8;
    sprite->data[4] = sprite->invisible;
    sprite->callback = SpriteCB_BlinkVisible;
}

static void SpriteCB_BlinkVisible(struct Sprite *sprite)
{
    if (--sprite->data[3] == 0)
    {
        sprite->invisible ^= 1;
        sprite->data[3] = 8;
    }
}

void SpriteCB_HideAsMoveTarget(struct Sprite *sprite)
{
    sprite->invisible = sprite->data[4];
    sprite->data[4] = FALSE;
    sprite->callback = SpriteCallbackDummy_2;
}

void SpriteCB_AllyMon(struct Sprite *sprite)
{
    sprite->callback = oac_poke_ally_;
}

static void oac_poke_ally_(struct Sprite *sprite)
{
    if (!(gIntroSlideFlags & 1))
    {
        sprite->x2 -= 2;
        if (sprite->x2 == 0)
        {
            sprite->callback = SpriteCB_Idle;
            sprite->data[1] = 0;
        }
    }
}

void SetIdleSpriteCallback(struct Sprite *sprite)
{
    sprite->callback = SpriteCB_Idle;
}

static void SpriteCB_Idle(struct Sprite *sprite)
{
}

#define sSpeedX data[1]
#define sSpeedY data[2]

void SpriteCB_FaintSlideAnim(struct Sprite *sprite)
{
    if (!(gIntroSlideFlags & 1))
    {
        sprite->x2 += sprite->sSpeedX;
        sprite->y2 += sprite->sSpeedY;
    }
}

#define sSpeedX data[1]
#define sSpeedY data[2]

#define sSinIndex           data[3]
#define sDelta              data[4]
#define sAmplitude          data[5]
#define sBouncerSpriteId    data[6]
#define sWhich              data[7]

void DoBounceEffect(u8 battler, u8 which, s8 delta, s8 amplitude)
{
    u8 invisibleSpriteId;
    u8 bouncerSpriteId;

    switch (which)
    {
    case BOUNCE_HEALTHBOX:
    default:
        if (gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing)
            return;
        break;
    case BOUNCE_MON:
        if (gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing)
            return;
        break;
    }

    invisibleSpriteId = CreateInvisibleSpriteWithCallback(SpriteCB_BounceEffect);
    if (which == BOUNCE_HEALTHBOX)
    {
        bouncerSpriteId = gHealthboxSpriteIds[battler];
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxBounceSpriteId = invisibleSpriteId;
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing = 1;
        gSprites[invisibleSpriteId].sSinIndex = 128; // 0
    }
    else
    {
        bouncerSpriteId = gBattlerSpriteIds[battler];
        gBattleSpritesDataPtr->healthBoxesData[battler].battlerBounceSpriteId = invisibleSpriteId;
        gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing = 1;
        gSprites[invisibleSpriteId].sSinIndex = 192; // -1
    }
    gSprites[invisibleSpriteId].sDelta = delta;
    gSprites[invisibleSpriteId].sAmplitude = amplitude;
    gSprites[invisibleSpriteId].sBouncerSpriteId = bouncerSpriteId;
    gSprites[invisibleSpriteId].sWhich = which;
    gSprites[invisibleSpriteId].sBattler = battler;
    gSprites[bouncerSpriteId].x2 = 0;
    gSprites[bouncerSpriteId].y2 = 0;
}

void EndBounceEffect(u8 battler, u8 which)
{
    u8 bouncerSpriteId;

    if (which == BOUNCE_HEALTHBOX)
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing)
            return;

        bouncerSpriteId = gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].healthboxBounceSpriteId].sBouncerSpriteId;
        DestroySprite(&gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].healthboxBounceSpriteId]);
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing = 0;
    }
    else
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing)
            return;

        bouncerSpriteId = gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].battlerBounceSpriteId].sBouncerSpriteId;
        DestroySprite(&gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].battlerBounceSpriteId]);
        gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing = 0;
    }

    gSprites[bouncerSpriteId].x2 = 0;
    gSprites[bouncerSpriteId].y2 = 0;
}

static void SpriteCB_BounceEffect(struct Sprite *sprite)
{
    u8 bouncerSpriteId = sprite->sBouncerSpriteId;
    s32 index = sprite->sSinIndex;

    s32 y = Sin(index, sprite->sAmplitude) + sprite->sAmplitude;

    gSprites[bouncerSpriteId].y2 = y;
    sprite->sSinIndex = (sprite->sSinIndex + sprite->sDelta) & 0xFF;
}

void SpriteCB_PlayerThrowInit(struct Sprite *sprite)
{
    StartSpriteAnim(sprite, 1);
    sprite->callback = SpriteCB_PlayerThrowUpdate;
}

void UpdatePlayerPosInThrowAnim(struct Sprite *sprite)
{
    if (sprite->animDelayCounter == 0)
        sprite->centerToCornerVecX = sPlayerThrowXTranslation[sprite->animCmdIndex];
}

static void SpriteCB_PlayerThrowUpdate(struct Sprite *sprite)
{
    UpdatePlayerPosInThrowAnim(sprite);
    if (sprite->animEnded)
        sprite->callback = SpriteCB_Idle;
}

void AnimSetCenterToCornerVecX(struct Sprite *sprite)
{
    if (sprite->animDelayCounter == 0)
        sprite->centerToCornerVecX = sPlayerThrowXTranslation[sprite->animCmdIndex];
}

void BeginBattleIntroDummy(void)
{

}

void BeginBattleIntro(void)
{
    BattleStartClearSetData();
    gBattleCommunication[1] = 0;
    gBattleStruct->introState = 0;
    gBattleMainFunc = DoBattleIntro;
}

static void BattleMainCB1(void)
{
    u32 battler;
    
    gBattleMainFunc();

    for (battler = 0; battler < gBattlersCount; battler++)
        gBattlerControllerFuncs[battler](battler);
}

static void ClearSetBScriptingStruct(void)
{
    // windowsType is set up earlier in BattleInitBgsAndWindows, so we need to save the value
    u32 temp = gBattleScripting.windowsType;
    u32 specialBattleType = gBattleScripting.specialTrainerBattleType;
    memset(&gBattleScripting, 0, sizeof(gBattleScripting));

    gBattleScripting.windowsType = temp;
    gBattleScripting.battleStyle = gSaveBlock2Ptr->optionsBattleStyle;
    gBattleScripting.expOnCatch = (B_EXP_CATCH >= GEN_6);
    gBattleScripting.specialTrainerBattleType = specialBattleType;
}

static void BattleStartClearSetData(void)
{
    s32 i;

    TurnValuesCleanUp(FALSE);
    SpecialStatusesClear();

    memset(&gDisableStructs, 0, sizeof(gDisableStructs));
    memset(&gFieldTimers, 0, sizeof(gFieldTimers));
    memset(&gSideStatuses, 0, sizeof(gSideStatuses));
    memset(&gSideTimers, 0, sizeof(gSideTimers));
    memset(&gWishFutureKnock, 0, sizeof(gWishFutureKnock));
    memset(&gBattleResults, 0, sizeof(gBattleResults));
    ClearSetBScriptingStruct();

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gStatuses3[i] = 0;
        gStatuses4[i] = 0;

        gDisableStructs[i].isFirstTurn = 2;
        gLastMoves[i] = MOVE_NONE;
        gLastLandedMoves[i] = MOVE_NONE;
        gLastHitByType[i] = 0;
        gLastResultingMoves[i] = MOVE_NONE;
        gLastHitBy[i] = 0xFF;
        gLockedMoves[i] = MOVE_NONE;
        gLastPrintedMoves[i] = MOVE_NONE;
        gBattleResources->flags->flags[i] = 0;
        gBattleStruct->lastTakenMove[i] = MOVE_NONE;
        gBattleStruct->choicedMove[i] = MOVE_NONE;
        gBattleStruct->changedItems[i] = 0;
        gBattleStruct->lastTakenMoveFrom[i][0] = MOVE_NONE;
        gBattleStruct->lastTakenMoveFrom[i][1] = MOVE_NONE;
        gBattleStruct->lastTakenMoveFrom[i][2] = MOVE_NONE;
        gBattleStruct->lastTakenMoveFrom[i][3] = MOVE_NONE;
        gBattleStruct->AI_monToSwitchIntoId[i] = PARTY_SIZE;
        gBattleStruct->skyDropTargets[i] = 0xFF;
        gBattleStruct->overwrittenAbilities[i] = ABILITY_NONE;
    }

    gLastUsedMove = 0;
    gFieldStatuses = 0;

    gHasFetchedBall = FALSE;
    gLastUsedBall = 0;

    gBattlerAttacker = 0;
    gBattlerTarget = 0;
    gEffectBattler = 0;
    gBattlerAbility = 0;
    gBattleWeather = 0;
    gBattlerAbility = 0;

    gHitMarker = 0;

    if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_POKEDUDE)) && gSaveBlock2Ptr->optionsBattleSceneOff)
    {
        gHitMarker |= HITMARKER_NO_ANIMATIONS;
    }

    gMultiHitCounter = 0;
    gBattleOutcome = 0;
    gBattleControllerExecFlags = 0;
    gPaydayMoney = 0;
    gBattleResources->battleScriptsStack->size = 0;
    gBattleResources->battleCallbackStack->size = 0;

    for (i = 0; i < BATTLE_COMMUNICATION_ENTRIES_COUNT; i++)
        gBattleCommunication[i] = 0;

    gPauseCounterBattle = 0;
    gBattleMoveDamage = 0;
    gIntroSlideFlags = 0;
    gLeveledUpInBattle = 0;
    gAbsentBattlerFlags = 0;
    gBattleStruct->runTries = 0;
    gBattleStruct->safariRockThrowCounter = 0;
    gBattleStruct->safariBaitThrowCounter = 0;
    gBattleStruct->safariCatchFactor = gSpeciesInfo[GetMonData(&gEnemyParty[0], MON_DATA_SPECIES)].catchRate * 100 / 1275;
    gBattleStruct->safariEscapeFactor = gSpeciesInfo[GetMonData(&gEnemyParty[0], MON_DATA_SPECIES)].safariZoneFleeRate * 100 / 1275;
    if (gBattleStruct->safariEscapeFactor <= 1)
        gBattleStruct->safariEscapeFactor = 2;
    gBattleStruct->wildVictorySong = 0;
    gBattleStruct->moneyMultiplier = 1;
 
    gBattleStruct->givenExpMons = 0;
    for (i = 0; i < 11; i++)
        gBattleResults.catchAttempts[i] = 0;
    gBattleResults.battleTurnCounter = 0;
    gBattleResults.playerFaintCounter = 0;
    gBattleResults.opponentFaintCounter = 0;
    gBattleResults.playerSwitchesCounter = 0;
    gBattleResults.numHealingItemsUsed = 0;
    gBattleResults.numRevivesUsed = 0;
    gBattleResults.playerMonWasDamaged = FALSE;
    gBattleResults.lastOpponentSpecies = SPECIES_NONE;
    gBattleResults.lastUsedMovePlayer = MOVE_NONE;
    gBattleResults.lastUsedMoveOpponent = MOVE_NONE;
    gBattleResults.playerMon1Species = SPECIES_NONE;
    gBattleResults.playerMon2Species = SPECIES_NONE;
    gBattleResults.caughtMonSpecies = SPECIES_NONE;
    for (i = 0; i < POKEMON_NAME_LENGTH; ++i)
    {
        gBattleResults.playerMon1Name[i] = 0;
        gBattleResults.playerMon2Name[i] = 0;
        gBattleResults.caughtMonNick[i] = 0;
    }


    gBattleStruct->mega.triggerSpriteId = 0xFF;
    gBattleStruct->burst.triggerSpriteId = 0xFF;

    for (i = 0; i < ARRAY_COUNT(gSideTimers); i++)
    {
        gSideTimers[i].stickyWebBattlerId = 0xFF;
    }
    gBattleStruct->appearedInBattle = 0;
    gBattleStruct->beatUpSlot = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        gBattleStruct->usedHeldItems[i][B_SIDE_PLAYER] = 0;
        gBattleStruct->usedHeldItems[i][B_SIDE_OPPONENT] = 0;
        gBattleStruct->itemLost[B_SIDE_PLAYER][i].originalItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
        gBattleStruct->itemLost[B_SIDE_OPPONENT][i].originalItem = GetMonData(&gEnemyParty[i], MON_DATA_HELD_ITEM);
        gPartyCriticalHits[i] = 0;
        gBattleStruct->allowedToChangeFormInWeather[i][B_SIDE_PLAYER] = FALSE;
        gBattleStruct->allowedToChangeFormInWeather[i][B_SIDE_OPPONENT] = FALSE;
    }

    gBattleStruct->swapDamageCategory = FALSE; // Photon Geyser, Shell Side Arm, Light That Burns the Sky
    gSelectedMonPartyId = PARTY_SIZE; // Revival Blessing
}

void SwitchInClearSetData(u32 battler)
{
    s32 i;
    struct DisableStruct disableStructCopy = gDisableStructs[battler];

    ClearIllusionMon(battler);

    if (gMovesInfo[gCurrentMove].effect != EFFECT_BATON_PASS)
    {
        for (i = 0; i < NUM_BATTLE_STATS; i++)
            gBattleMons[battler].statStages[i] = DEFAULT_STAT_STAGE;
        for (i = 0; i < gBattlersCount; i++)
        {
            if ((gBattleMons[i].status2 & STATUS2_ESCAPE_PREVENTION) && gDisableStructs[i].battlerPreventingEscape == battler)
                gBattleMons[i].status2 &= ~STATUS2_ESCAPE_PREVENTION;
            if ((gStatuses3[i] & STATUS3_ALWAYS_HITS) && gDisableStructs[i].battlerWithSureHit == battler)
            {
                gStatuses3[i] &= ~STATUS3_ALWAYS_HITS;
                gDisableStructs[i].battlerWithSureHit = 0;
            }
        }
    }
    if (gMovesInfo[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        gBattleMons[battler].status2 &= (STATUS2_CONFUSION | STATUS2_FOCUS_ENERGY_ANY | STATUS2_SUBSTITUTE | STATUS2_ESCAPE_PREVENTION | STATUS2_CURSED);
        gStatuses3[battler] &= (STATUS3_LEECHSEED_BATTLER | STATUS3_LEECHSEED | STATUS3_ALWAYS_HITS | STATUS3_PERISH_SONG | STATUS3_ROOTED
                                       | STATUS3_GASTRO_ACID | STATUS3_EMBARGO | STATUS3_TELEKINESIS | STATUS3_MAGNET_RISE | STATUS3_HEAL_BLOCK
                                       | STATUS3_AQUA_RING | STATUS3_POWER_TRICK);
        gStatuses4[battler] &= (STATUS4_MUD_SPORT | STATUS4_WATER_SPORT | STATUS4_INFINITE_CONFUSION);
        for (i = 0; i < gBattlersCount; i++)
        {
            if (GetBattlerSide(battler) != GetBattlerSide(i)
             && (gStatuses3[i] & STATUS3_ALWAYS_HITS) != 0
             && (gDisableStructs[i].battlerWithSureHit == battler))
            {
                gStatuses3[i] &= ~STATUS3_ALWAYS_HITS;
                gStatuses3[i] |= STATUS3_ALWAYS_HITS_TURN(2);
            }
        }
        if (gStatuses3[battler] & STATUS3_POWER_TRICK)
            SWAP(gBattleMons[battler].attack, gBattleMons[battler].defense, i);
    }
    else
    {
        gBattleMons[battler].status2 = 0;
        gStatuses3[battler] = 0;
        gStatuses4[battler] = 0;
    }

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].status2 & STATUS2_INFATUATED_WITH(battler))
            gBattleMons[i].status2 &= ~STATUS2_INFATUATED_WITH(battler);
        if ((gBattleMons[i].status2 & STATUS2_WRAPPED) && *(gBattleStruct->wrappedBy + i) == battler)
            gBattleMons[i].status2 &= ~STATUS2_WRAPPED;
        if ((gStatuses4[i] & STATUS4_SYRUP_BOMB) && *(gBattleStruct->stickySyrupdBy + i) == battler)
            gStatuses4[i] &= ~STATUS4_SYRUP_BOMB;
    }

    gActionSelectionCursor[battler] = 0;
    gMoveSelectionCursor[battler] = 0;

    memset(&gDisableStructs[battler], 0, sizeof(struct DisableStruct));
    if (gMovesInfo[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        gDisableStructs[battler].substituteHP = disableStructCopy.substituteHP;
        gDisableStructs[battler].battlerWithSureHit = disableStructCopy.battlerWithSureHit;
        gDisableStructs[battler].perishSongTimer = disableStructCopy.perishSongTimer;
        gDisableStructs[battler].battlerPreventingEscape = disableStructCopy.battlerPreventingEscape;
        gDisableStructs[battler].embargoTimer = disableStructCopy.embargoTimer;
    }
    else if (gMovesInfo[gCurrentMove].effect == EFFECT_SHED_TAIL)
    {
        gBattleMons[battler].status2 |= STATUS2_SUBSTITUTE;
        gDisableStructs[battler].substituteHP = disableStructCopy.substituteHP;
    }

    gMoveResultFlags = 0;
    gDisableStructs[battler].isFirstTurn = 2;
    gLastMoves[battler] = MOVE_NONE;
    gLastLandedMoves[battler] = MOVE_NONE;
    gLastHitByType[battler] = 0;
    gLastResultingMoves[battler] = MOVE_NONE;
    gLastPrintedMoves[battler] = MOVE_NONE;
    gLastHitBy[battler] = 0xFF;

    gBattleStruct->lastTakenMove[battler] = 0;
    gBattleStruct->sameMoveTurns[battler] = 0;
    gBattleStruct->lastTakenMoveFrom[battler][0] = 0;
    gBattleStruct->lastTakenMoveFrom[battler][1] = 0;
    gBattleStruct->lastTakenMoveFrom[battler][2] = 0;
    gBattleStruct->lastTakenMoveFrom[battler][3] = 0;
    gBattleStruct->lastMoveFailed &= ~(gBitTable[battler]);

    for (i = 0; i < ARRAY_COUNT(gSideTimers); i++)
    {
        // Switched into sticky web user slot, so reset stored battler ID
        if (gSideTimers[i].stickyWebBattlerId == battler)
            gSideTimers[i].stickyWebBattlerId = 0xFF;
    }

    for (i = 0; i < gBattlersCount; i++)
    {
        if (i != battler && GetBattlerSide(i) != GetBattlerSide(battler))
            gBattleStruct->lastTakenMove[i] = MOVE_NONE;
        gBattleStruct->lastTakenMoveFrom[i][battler] = 0;
    }

    *((u8 *)(&gBattleStruct->choicedMove[battler]) + 0) = MOVE_NONE;
    *((u8 *)(&gBattleStruct->choicedMove[battler]) + 1) = MOVE_NONE;

    gBattleStruct->choicedMove[battler] = MOVE_NONE;
    gBattleResources->flags->flags[battler] = 0;
    gCurrentMove = MOVE_NONE;

    // Reset damage to prevent things like red card activating if the switched-in mon is holding it
    gSpecialStatuses[battler].physicalDmg = 0;
    gSpecialStatuses[battler].specialDmg = 0;
    gBattleStruct->enduredDamage &= ~gBitTable[battler];

    // Reset G-Max Chi Strike boosts.
    gBattleStruct->bonusCritStages[battler] = 0;

    // Reset Dynamax flags.
    UndoDynamax(battler);

    gBattleStruct->overwrittenAbilities[battler] = ABILITY_NONE;

    // Clear selected party ID so Revival Blessing doesn't get confused.
    gSelectedMonPartyId = PARTY_SIZE;

    // Allow for illegal abilities within tests.
    #if TESTING
    if (gTestRunnerEnabled)
    {
        u32 side = GetBattlerSide(battler);
        u32 partyIndex = gBattlerPartyIndexes[battler];
        if (TestRunner_Battle_GetForcedAbility(side, partyIndex))
            gBattleMons[i].ability = gBattleStruct->overwrittenAbilities[i] = TestRunner_Battle_GetForcedAbility(side, partyIndex);
    }
    #endif // TESTING

    Ai_UpdateSwitchInData(battler); 
}

const u8* FaintClearSetData(u32 battler)
{
    s32 i;
    const u8 *result = NULL;
    u8 battlerSide = GetBattlerSide(battler);

    for (i = 0; i < NUM_BATTLE_STATS; i++)
        gBattleMons[battler].statStages[i] = DEFAULT_STAT_STAGE;

    gBattleMons[battler].status2 = 0;
    gStatuses3[battler] &= STATUS3_GASTRO_ACID; // Edge case: Keep Gastro Acid if pokemon's ability can have effect after fainting, for example Innards Out.
    gStatuses4[battler] = 0;

    for (i = 0; i < gBattlersCount; i++)
    {
        if ((gBattleMons[i].status2 & STATUS2_ESCAPE_PREVENTION) && gDisableStructs[i].battlerPreventingEscape == battler)
            gBattleMons[i].status2 &= ~STATUS2_ESCAPE_PREVENTION;
        if (gBattleMons[i].status2 & STATUS2_INFATUATED_WITH(battler))
            gBattleMons[i].status2 &= ~STATUS2_INFATUATED_WITH(battler);
        if ((gBattleMons[i].status2 & STATUS2_WRAPPED) && *(gBattleStruct->wrappedBy + i) == battler)
            gBattleMons[i].status2 &= ~STATUS2_WRAPPED;
        if ((gStatuses4[i] & STATUS4_SYRUP_BOMB) && *(gBattleStruct->stickySyrupdBy + i) == battler)
            gStatuses4[i] &= ~STATUS4_SYRUP_BOMB;
    }

    gActionSelectionCursor[battler] = 0;
    gMoveSelectionCursor[battler] = 0;

    memset(&gDisableStructs[battler], 0, sizeof(struct DisableStruct));

    gProtectStructs[battler].protected = FALSE;
    gProtectStructs[battler].spikyShielded = FALSE;
    gProtectStructs[battler].kingsShielded = FALSE;
    gProtectStructs[battler].banefulBunkered = FALSE;
    gProtectStructs[battler].quash = FALSE;
    gProtectStructs[battler].obstructed = FALSE;
    gProtectStructs[battler].silkTrapped = FALSE;
    gProtectStructs[battler].burningBulwarked = FALSE;
    gProtectStructs[battler].endured = FALSE;
    gProtectStructs[battler].noValidMoves = FALSE;
    gProtectStructs[battler].helpingHand = FALSE;
    gProtectStructs[battler].bounceMove = FALSE;
    gProtectStructs[battler].stealMove = FALSE;
    gProtectStructs[battler].prlzImmobility = FALSE;
    gProtectStructs[battler].confusionSelfDmg = FALSE;
    gProtectStructs[battler].targetAffected = FALSE;
    gProtectStructs[battler].chargingTurn = FALSE;
    gProtectStructs[battler].fleeType = 0;
    gProtectStructs[battler].usedImprisonedMove = FALSE;
    gProtectStructs[battler].loveImmobility = FALSE;
    gProtectStructs[battler].usedDisabledMove = FALSE;
    gProtectStructs[battler].usedTauntedMove = FALSE;
    gProtectStructs[battler].flag2Unknown = FALSE;
    gProtectStructs[battler].flinchImmobility = FALSE;
    gProtectStructs[battler].notFirstStrike = FALSE;
    gProtectStructs[battler].usedHealBlockedMove = FALSE;
    gProtectStructs[battler].usedGravityPreventedMove = FALSE;
    gProtectStructs[battler].usedThroatChopPreventedMove = FALSE;
    gProtectStructs[battler].statRaised = FALSE;
    gProtectStructs[battler].statFell = FALSE;
    gProtectStructs[battler].pranksterElevated = FALSE;

    gDisableStructs[battler].isFirstTurn = 2;

    gLastMoves[battler] = MOVE_NONE;
    gLastLandedMoves[battler] = MOVE_NONE;
    gLastHitByType[battler] = 0;
    gLastResultingMoves[battler] = MOVE_NONE;
    gLastPrintedMoves[battler] = MOVE_NONE;
    gLastHitBy[battler] = 0xFF;

    gBattleStruct->choicedMove[battler] = MOVE_NONE;
    gBattleStruct->sameMoveTurns[battler] = 0;

    gBattleStruct->lastTakenMove[battler] = MOVE_NONE;
    gBattleStruct->lastTakenMoveFrom[battler][0] = 0;
    gBattleStruct->lastTakenMoveFrom[battler][1] = 0;
    gBattleStruct->lastTakenMoveFrom[battler][2] = 0;
    gBattleStruct->lastTakenMoveFrom[battler][3] = 0;

    for (i = 0; i < ARRAY_COUNT(gSideTimers); i++)
    {
        // User of sticky web fainted, so reset the stored battler ID
        if (gSideTimers[i].stickyWebBattlerId == battler)
            gSideTimers[i].stickyWebBattlerId = 0xFF;
    }

    for (i = 0; i < gBattlersCount; i++)
    {
        if (i != battler && GetBattlerSide(i) != battlerSide)
            gBattleStruct->lastTakenMove[i] = MOVE_NONE;

        gBattleStruct->lastTakenMoveFrom[i][battler] = 0;
    }
    gBattleResources->flags->flags[battler] = 0;

    gBattleMons[battler].type1 = gSpeciesInfo[gBattleMons[battler].species].types[0];
    gBattleMons[battler].type2 = gSpeciesInfo[gBattleMons[battler].species].types[1];
    gBattleMons[battler].type3 = TYPE_MYSTERY;

    Ai_UpdateFaintData(battler);
    TryBattleFormChange(battler, FORM_CHANGE_FAINT);

    gBattleStruct->overwrittenAbilities[battler] = ABILITY_NONE;

    // If the fainted mon was involved in a Sky Drop
    if (gBattleStruct->skyDropTargets[battler] != 0xFF)
    {
        // Get battler id of the other Pokemon involved in this Sky Drop
        u8 otherSkyDropper = gBattleStruct->skyDropTargets[battler];

        // Clear Sky Drop data
        gBattleStruct->skyDropTargets[battler] = 0xFF;
        gBattleStruct->skyDropTargets[otherSkyDropper] = 0xFF;

        // If the other Pokemon involved in this Sky Drop was the target, not the attacker
        if (gStatuses3[otherSkyDropper] & STATUS3_SKY_DROPPED)
        {
            // Release the target and take them out of the semi-invulnerable state
            gStatuses3[otherSkyDropper] &= ~(STATUS3_SKY_DROPPED | STATUS3_ON_AIR);

            // Make the target's sprite visible
            gSprites[gBattlerSpriteIds[otherSkyDropper]].invisible = FALSE;

            // If the target was sky dropped in the middle of using Outrage/Petal Dance/Thrash,
            // confuse them upon release and print "confused via fatigue" message and animation.
            if (gBattleMons[otherSkyDropper].status2 & STATUS2_LOCK_CONFUSE)
            {
                gBattleMons[otherSkyDropper].status2 &= ~(STATUS2_LOCK_CONFUSE);

                // If the released mon can be confused, do so.
                // Don't use CanBeConfused here, since it can cause issues in edge cases.
                if (!(GetBattlerAbility(otherSkyDropper) == ABILITY_OWN_TEMPO
                    || gBattleMons[otherSkyDropper].status2 & STATUS2_CONFUSION
                    || IsBattlerTerrainAffected(otherSkyDropper, STATUS_FIELD_MISTY_TERRAIN)))
                {
                    gBattleMons[otherSkyDropper].status2 |= STATUS2_CONFUSION_TURN(((Random()) % 4) + 2);
                    gBattlerAttacker = otherSkyDropper;
                    result = BattleScript_ThrashConfuses;
                }
            }
        }
    }

    // Clear Z-Move data
    gBattleStruct->zmove.active = FALSE;
    gBattleStruct->zmove.toBeUsed[battler] = MOVE_NONE;
    gBattleStruct->zmove.effect = EFFECT_HIT;
    // Clear Dynamax data
    UndoDynamax(battler);

    return result;
}

static void DoBattleIntro(void)
{
    s32 i;
    u32 battler;
    u8 *state = &gBattleStruct->introState;

    switch (*state)
    {
    case 0: // Get Data of all battlers.
        battler = gBattleCommunication[1];
        BtlController_EmitGetMonData(battler, BUFFER_A, REQUEST_ALL_BATTLE, 0);
        MarkBattlerForControllerExec(battler);
        (*state)++;
        break;
    case 1: // Loop through all battlers.
        if (!gBattleControllerExecFlags)
        {
            if (++gBattleCommunication[1] == gBattlersCount)
                (*state)++;
            else
                *state = 0;
        }
        break;
    case 2: // Start graphical intro slide.
        if (!gBattleControllerExecFlags)
        {
            battler = GetBattlerAtPosition(0);
            BtlController_EmitIntroSlide(battler, BUFFER_A, gBattleTerrain);
            MarkBattlerForControllerExec(battler);
            gBattleCommunication[0] = 0;
            gBattleCommunication[1] = 0;
            (*state)++;
        }
        break;
    case 3: // Wait for intro slide.
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 4: // Copy battler data gotten in cases 0 and 1. Draw trainer/mon sprite.
        for (battler = 0; battler < gBattlersCount; battler++)
        {
            if ((gBattleTypeFlags & BATTLE_TYPE_SAFARI) && GetBattlerSide(battler) == B_SIDE_PLAYER)
            {
                memset(&gBattleMons[battler], 0, sizeof(struct BattlePokemon));
            }
            else
            {
                memcpy(&gBattleMons[battler], &gBattleResources->bufferB[battler][4], sizeof(struct BattlePokemon));
                gBattleMons[battler].type1 = gSpeciesInfo[gBattleMons[battler].species].types[0];
                gBattleMons[battler].type2 = gSpeciesInfo[gBattleMons[battler].species].types[1];
                gBattleMons[battler].type3 = TYPE_MYSTERY;
                gBattleMons[battler].ability = GetAbilityBySpecies(gBattleMons[battler].species, gBattleMons[battler].abilityNum);
                gBattleStruct->hpOnSwitchout[GetBattlerSide(battler)] = gBattleMons[battler].hp;
                gBattleMons[battler].status2 = 0;
                for (i = 0; i < NUM_BATTLE_STATS; i++)
                    gBattleMons[battler].statStages[i] = DEFAULT_STAT_STAGE;
            }

            // Draw sprite.
            switch (GetBattlerPosition(battler))
            {
            case B_POSITION_PLAYER_LEFT: // player sprite
                BtlController_EmitDrawTrainerPic(battler, BUFFER_A);
                MarkBattlerForControllerExec(battler);
                break;
            case B_POSITION_OPPONENT_LEFT:
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER) // opponent 1 sprite
                {
                    BtlController_EmitDrawTrainerPic(battler, BUFFER_A);
                    MarkBattlerForControllerExec(battler);
                }
                else // wild mon 1
                {
                    BtlController_EmitLoadMonSprite(battler, BUFFER_A);
                    MarkBattlerForControllerExec(battler);
                    gBattleResults.lastOpponentSpecies = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES, NULL);
                }
                break;
            case B_POSITION_PLAYER_RIGHT:
                if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER)) // partner sprite
                {
                    BtlController_EmitDrawTrainerPic(battler, BUFFER_A);
                    MarkBattlerForControllerExec(battler);
                }
                break;
            case B_POSITION_OPPONENT_RIGHT:
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                {
                    // TODO: Implement 2v1 battles
                    /*if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_TWO_OPPONENTS) && !BATTLE_TWO_VS_ONE_OPPONENT) // opponent 2 if exists
                    {
                        BtlController_EmitDrawTrainerPic(battler, BUFFER_A);
                        MarkBattlerForControllerExec(battler);
                    }*/
                }
                else if (IsBattlerAlive(battler)) // wild mon 2 if alive
                {
                    BtlController_EmitLoadMonSprite(battler, BUFFER_A);
                    MarkBattlerForControllerExec(battler);
                    gBattleResults.lastOpponentSpecies = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES, NULL);
                }
                break;
            }
        }

        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            (*state)++;
        }
        else // Skip party summary since it is a wild battle.
        {
            if (B_FAST_INTRO == TRUE)
                *state = 7; // Don't wait for sprite, print message at the same time.
            else
                *state = 6; // Wait for sprite to load.
        }
        break;
    case 5: // draw party summary in trainer battles
        if (!gBattleControllerExecFlags)
        {
            struct HpAndStatus hpStatus[PARTY_SIZE];

            for (i = 0; i < PARTY_SIZE; i++)
            {
                if (GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_NONE
                 || GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
                {
                    hpStatus[i].hp = HP_EMPTY_SLOT;
                    hpStatus[i].status = 0;
                }
                else
                {
                    hpStatus[i].hp = GetMonData(&gEnemyParty[i], MON_DATA_HP);
                    hpStatus[i].status = GetMonData(&gEnemyParty[i], MON_DATA_STATUS);
                }
            }

            battler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            BtlController_EmitDrawPartyStatusSummary(battler, BUFFER_A, hpStatus, PARTY_SUMM_SKIP_DRAW_DELAY);
            MarkBattlerForControllerExec(battler);

            for (i = 0; i < PARTY_SIZE; i++)
            {
                if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_NONE
                 || GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
                {
                    hpStatus[i].hp = HP_EMPTY_SLOT;
                    hpStatus[i].status = 0;
                }
                else
                {
                    hpStatus[i].hp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
                    hpStatus[i].status = GetMonData(&gPlayerParty[i], MON_DATA_STATUS);
                }
            }

            battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            BtlController_EmitDrawPartyStatusSummary(battler, BUFFER_A, hpStatus, PARTY_SUMM_SKIP_DRAW_DELAY);
            MarkBattlerForControllerExec(battler);

            (*state)++;
        }
        break;
    case 6: // wait for previous action to complete
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 7: // print battle intro message
        if (!IsBattlerMarkedForControllerExec(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
        {
            PrepareStringBattle(STRINGID_INTROMSG, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
            (*state)++;
        }
        break;
    case 8: // wait for intro message to be printed
        if (!IsBattlerMarkedForControllerExec(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
            {
                (*state)++;
            }
            else
            {
                if (B_FAST_INTRO == TRUE)
                    *state = 15; // Wait for text to be printed.
                else
                    *state = 14; // Wait for text and sprite.
            }
        }
        break;
    case 9: // print opponent sends out
        PrepareStringBattle(STRINGID_INTROSENDOUT, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));
        (*state)++;
        break;
    case 10: // wait for opponent sends out text
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 11: // first opponent's mon send out animation
        battler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

        BtlController_EmitIntroTrainerBallThrow(battler, BUFFER_A);
        MarkBattlerForControllerExec(battler);
        (*state)++;
        break;
    case 12: // nothing
        (*state)++;
    case 13: // second opponent's mon send out
        // TODO: Implement 2v1 batles
        /*if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_TWO_OPPONENTS) && !BATTLE_TWO_VS_ONE_OPPONENT)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
                battler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            else
                battler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);

            BtlController_EmitIntroTrainerBallThrow(battler, BUFFER_A);
            MarkBattlerForControllerExec(battler);
        }*/
        if (B_FAST_INTRO == TRUE
          && !(gBattleTypeFlags & (BATTLE_TYPE_LINK)))
            *state = 15; // Print at the same time as trainer sends out second mon.
        else
            (*state)++;
        break;
    case 14: // wait for opponent 2 send out
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 15: // wait for wild battle message
        if (!IsBattlerMarkedForControllerExec(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
            (*state)++;
        break;
    case 16: // print player sends out
        if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
        {
            battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

            // A hack that makes fast intro work in trainer battles too.
            if (B_FAST_INTRO == TRUE
                && gBattleTypeFlags & BATTLE_TYPE_TRAINER
                && !(gBattleTypeFlags & (BATTLE_TYPE_LINK))
                && gSprites[gHealthboxSpriteIds[battler ^ BIT_SIDE]].callback == SpriteCallbackDummy)
            {
                return;
            }

            PrepareStringBattle(STRINGID_INTROSENDOUT, battler);
        }
        (*state)++;
        break;
    case 17: // wait for player send out message
        if (!(gBattleTypeFlags & BATTLE_TYPE_LINK && gBattleControllerExecFlags))
        {
            battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

            if (!IsBattlerMarkedForControllerExec(battler))
                (*state)++;
        }
        break;
    case 18: // player 1 send out
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

        BtlController_EmitIntroTrainerBallThrow(battler, BUFFER_A);
        MarkBattlerForControllerExec(battler);
        (*state)++;
        break;
    case 19: // player 2 send out
        if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER))
        {
            battler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);

            BtlController_EmitIntroTrainerBallThrow(battler, BUFFER_A);
            MarkBattlerForControllerExec(battler);
        }
        (*state)++;
        break;
    case 20: // set dex and battle vars
        if (!gBattleControllerExecFlags)
        {
            for (battler = 0; battler < gBattlersCount; battler++)
            {
                if (GetBattlerSide(battler) == B_SIDE_OPPONENT
                 && !(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER
                                          | BATTLE_TYPE_BATTLE_TOWER
                                          | BATTLE_TYPE_LINK
                                          | BATTLE_TYPE_TRAINER_TOWER)))
                {
                    HandleSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[battler].species), FLAG_SET_SEEN, gBattleMons[battler].personality);
                }
            }

            gBattleStruct->eventsBeforeFirstTurnState = 0;
            gBattleStruct->switchInBattlerCounter = 0;
            gBattleStruct->overworldWeatherDone = FALSE;
            SetAiLogicDataForTurn(AI_DATA); // get assumed abilities, hold effects, etc of all battlers
            Ai_InitPartyStruct(); // Save mons party counts, and first 2/4 mons on the battlefield.

            // Try to set a status to start the battle with
            gBattleStruct->startingStatus = 0;
            // TODO: Implement 2v1 battles
            /*if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && GetTrainerStartingStatusFromId(gTrainerBattleOpponent_B))
            {
                gBattleStruct->startingStatus = GetTrainerStartingStatusFromId(gTrainerBattleOpponent_B);
                gBattleStruct->startingStatusTimer = 0; // infinite
            }*/
            if (GetTrainerStartingStatusFromId(gTrainerBattleOpponent_A))
            {
                gBattleStruct->startingStatus = GetTrainerStartingStatusFromId(gTrainerBattleOpponent_A);
                gBattleStruct->startingStatusTimer = 0; // infinite
            }
            else if (B_VAR_STARTING_STATUS != 0)
            {
                gBattleStruct->startingStatus = VarGet(B_VAR_STARTING_STATUS);
                gBattleStruct->startingStatusTimer = VarGet(B_VAR_STARTING_STATUS_TIMER);
            }
            gBattleMainFunc = TryDoEventsBeforeFirstTurn;
        }
        break;
    }
}

const u8 sText_EmptyString3[] = _(""); //???

static void TryDoEventsBeforeFirstTurn(void)
{
    s32 i, j;
    if (gBattleControllerExecFlags)
        return;

    switch (gBattleStruct->eventsBeforeFirstTurnState)
    {
    case FIRST_TURN_EVENTS_START:
        // Set invalid mons as absent(for example when starting a double battle with only one pokemon).
        if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
        {
            for (i = 0; i < gBattlersCount; i++)
            {
                struct Pokemon *party = GetBattlerParty(i);
                struct Pokemon *mon = &party[gBattlerPartyIndexes[i]];
                if (gBattleMons[i].hp == 0 || gBattleMons[i].species == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG))
                    gAbsentBattlerFlags |= gBitTable[i];
            }
        }

        // Allow for illegal abilities within tests.
        #if TESTING
        if (gTestRunnerEnabled)
        {
            for (i = 0; i < gBattlersCount; ++i)
            {
                u32 side = GetBattlerSide(i);
                u32 partyIndex = gBattlerPartyIndexes[i];
                if (TestRunner_Battle_GetForcedAbility(side, partyIndex))
                    gBattleMons[i].ability = gBattleStruct->overwrittenAbilities[i] = TestRunner_Battle_GetForcedAbility(side, partyIndex);
            }
        }
        #endif // TESTING

        for (i = 0; i < gBattlersCount; i++)
            gBattlerByTurnOrder[i] = i;
        for (i = 0; i < gBattlersCount - 1; i++)
        {
            for (j = i + 1; j < gBattlersCount; j++)
            {
                if (GetWhichBattlerFaster(gBattlerByTurnOrder[i], gBattlerByTurnOrder[j], TRUE) == -1)
                    SwapTurnOrder(i, j);
            }
        }
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_OVERWORLD_WEATHER:
        if (!gBattleStruct->overworldWeatherDone
         && AbilityBattleEffects(ABILITYEFFECT_SWITCH_IN_WEATHER, 0, 0, ABILITYEFFECT_SWITCH_IN_WEATHER, 0) != 0)
        {
            gBattleStruct->overworldWeatherDone = TRUE;
            return;
        }
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_TERRAIN:
        if (!gBattleStruct->terrainDone
         && AbilityBattleEffects(ABILITYEFFECT_SWITCH_IN_TERRAIN, 0, 0, ABILITYEFFECT_SWITCH_IN_TERRAIN, 0) != 0)
        {
            gBattleStruct->terrainDone = TRUE;
            return;
        }
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_STARTING_STATUS:
        if (!gBattleStruct->startingStatusDone
         && gBattleStruct->startingStatus
         && AbilityBattleEffects(ABILITYEFFECT_SWITCH_IN_STATUSES, 0, 0, ABILITYEFFECT_SWITCH_IN_STATUSES, 0) != 0)
        {
            gBattleStruct->startingStatusDone = TRUE;
            return;
        }
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_TOTEM_BOOST:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (gQueuedStatBoosts[i].stats != 0 && !gProtectStructs[i].eatMirrorHerb && gProtectStructs[i].activateOpportunist == 0)
            {
                gBattlerAttacker = i;
                BattleScriptExecute(BattleScript_TotemVar);
                return;
            }
        }
        memset(gQueuedStatBoosts, 0, sizeof(gQueuedStatBoosts)); // erase all totem boosts for Mirror Herb and Opportunist
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_NEUTRALIZING_GAS:
        if (AbilityBattleEffects(ABILITYEFFECT_NEUTRALIZINGGAS, 0, 0, 0, 0) != 0)
            return;
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_SWITCH_IN_ABILITIES:
        while (gBattleStruct->switchInBattlerCounter < gBattlersCount) // From fastest to slowest
        {
            gBattlerAttacker = gBattlerByTurnOrder[gBattleStruct->switchInBattlerCounter++];

            if (TryPrimalReversion(gBattlerAttacker))
                return;
            if (AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, gBattlerAttacker, 0, 0, 0) != 0)
                return;
        }
        gBattleStruct->switchInBattlerCounter = 0;
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_OPPORTUNIST_1:
        if (AbilityBattleEffects(ABILITYEFFECT_OPPORTUNIST, 0, 0, 0, 0))
            return;
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_ITEM_EFFECTS:
        while (gBattleStruct->switchInBattlerCounter < gBattlersCount) // From fastest to slowest
        {
            if (ItemBattleEffects(ITEMEFFECT_ON_SWITCH_IN, gBattlerByTurnOrder[gBattleStruct->switchInBattlerCounter++], FALSE))
                return;
        }
        gBattleStruct->switchInBattlerCounter = 0;
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_OPPORTUNIST_2:
        if (AbilityBattleEffects(ABILITYEFFECT_OPPORTUNIST, 0, 0, 0, 0))
            return;
        gBattleStruct->eventsBeforeFirstTurnState++;
        break;
    case FIRST_TURN_EVENTS_END:
        for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            *(gBattleStruct->monToSwitchIntoId + i) = PARTY_SIZE;
            gChosenActionByBattler[i] = B_ACTION_NONE;
            gChosenMoveByBattler[i] = MOVE_NONE;
        }
        TurnValuesCleanUp(FALSE);
        SpecialStatusesClear();
        *(&gBattleStruct->absentBattlerFlags) = gAbsentBattlerFlags;
        BattlePutTextOnWindow(sText_EmptyString3, B_WIN_MSG);
        gBattleMainFunc = HandleTurnActionSelectionState;
        ResetSentPokesToOpponentValue();

        for (i = 0; i < BATTLE_COMMUNICATION_ENTRIES_COUNT; i++)
            gBattleCommunication[i] = 0;

        for (i = 0; i < gBattlersCount; i++)
        {
            gBattleMons[i].status2 &= ~STATUS2_FLINCHED;
            // Record party slots of player's mons that appeared in battle
            if (!BattlerHasAi(i))
                gBattleStruct->appearedInBattle |= gBitTable[gBattlerPartyIndexes[i]];
        }

        *(&gBattleStruct->turnEffectsTracker) = 0;
        *(&gBattleStruct->turnEffectsBattlerId) = 0;
        *(&gBattleStruct->wishPerishSongState) = 0;
        *(&gBattleStruct->wishPerishSongBattlerId) = 0;
        gBattleScripting.moveendState = 0;
        gBattleStruct->faintedActionsState = 0;
        gBattleStruct->turnCountersTracker = 0;
        gMoveResultFlags = 0;

        memset(gQueuedStatBoosts, 0, sizeof(gQueuedStatBoosts));
        SetShellSideArmCategory();
        SetAiLogicDataForTurn(AI_DATA); // get assumed abilities, hold effects, etc of all battlers

        if ((i = ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), TRAINER_SLIDE_BEFORE_FIRST_TURN)))
            BattleScriptExecute(i == 1 ? BattleScript_TrainerASlideMsgEnd2 : BattleScript_TrainerBSlideMsgEnd2);
        gBattleStruct->eventsBeforeFirstTurnState = 0;
        break;
    }
}

static void HandleEndTurn_ContinueBattle(void)
{
    s32 i;

    if (gBattleControllerExecFlags == 0)
    {
        gBattleMainFunc = BattleTurnPassed;
        for (i = 0; i < BATTLE_COMMUNICATION_ENTRIES_COUNT; i++)
            gBattleCommunication[i] = 0;
        for (i = 0; i < gBattlersCount; i++)
        {
            gBattleMons[i].status2 &= ~(STATUS2_FLINCHED);
            if ((gBattleMons[i].status1 & STATUS1_SLEEP) && (gBattleMons[i].status2 & STATUS2_MULTIPLETURNS))
                CancelMultiTurnMoves(i);
        }
        gBattleStruct->turnEffectsTracker = 0;
        gBattleStruct->turnEffectsBattlerId = 0;
        gBattleStruct->wishPerishSongState = 0;
        gBattleStruct->wishPerishSongBattlerId = 0;
        gBattleStruct->turnCountersTracker = 0;
        gMoveResultFlags = 0;
    }
}

void BattleTurnPassed(void)
{
    s32 i;

    TurnValuesCleanUp(TRUE);
    if (gBattleOutcome == 0)
    {
        if (DoFieldEndTurnEffects())
            return;
        if (DoBattlerEndTurnEffects())
            return;
    }
    if (HandleWishPerishSongOnTurnEnd())
        return;
    if (HandleFaintedMonActions())
        return;
    gBattleStruct->faintedActionsState = 0;
    TurnValuesCleanUp(FALSE);
    gHitMarker &= ~(HITMARKER_NO_ATTACKSTRING);
    gHitMarker &= ~(HITMARKER_UNABLE_TO_USE_MOVE);
    gHitMarker &= ~(HITMARKER_PLAYER_FAINTED);
    gHitMarker &= ~(HITMARKER_PASSIVE_DAMAGE);
    gBattleScripting.animTurn = 0;
    gBattleScripting.animTargetsHit = 0;
    gBattleScripting.moveendState = 0;
    gBattleMoveDamage = 0;
    gMoveResultFlags = 0;
    for (i = 0; i < 5; i++)
        gBattleCommunication[i] = 0;
    if (gBattleOutcome != 0)
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        gBattleMainFunc = RunTurnActionsFunctions;
        return;
    }
    if (gBattleResults.battleTurnCounter < 0xFF)
        ++gBattleResults.battleTurnCounter;
    for (i = 0; i < gBattlersCount; i++)
    {
        gChosenActionByBattler[i] = B_ACTION_NONE;
        gChosenMoveByBattler[i] = MOVE_NONE;
    }
    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        *(gBattleStruct->monToSwitchIntoId + i) = PARTY_SIZE;
    *(&gBattleStruct->absentBattlerFlags) = gAbsentBattlerFlags;
    SetShellSideArmCategory();
    SetAiLogicDataForTurn(AI_DATA); // get assumed abilities, hold effects, etc of all battlers
    gBattleMainFunc = HandleTurnActionSelectionState;

    if ((i = ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), TRAINER_SLIDE_LAST_LOW_HP)))
        BattleScriptExecute(i == 1 ? BattleScript_TrainerASlideMsgEnd2 : BattleScript_TrainerBSlideMsgEnd2);
    else if ((i = ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), TRAINER_SLIDE_LAST_HALF_HP)))
        BattleScriptExecute(i == 1 ? BattleScript_TrainerASlideMsgEnd2 : BattleScript_TrainerBSlideMsgEnd2);
    else if ((i = ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), TRAINER_SLIDE_FIRST_CRITICAL_HIT)))
        BattleScriptExecute(i == 1 ? BattleScript_TrainerASlideMsgEnd2 : BattleScript_TrainerBSlideMsgEnd2);
    else if ((i = ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), TRAINER_SLIDE_FIRST_SUPER_EFFECTIVE_HIT)))
        BattleScriptExecute(i == 1 ? BattleScript_TrainerASlideMsgEnd2 : BattleScript_TrainerBSlideMsgEnd2);
    else if ((i = ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), TRAINER_SLIDE_FIRST_STAB_MOVE)))
        BattleScriptExecute(i == 1 ? BattleScript_TrainerASlideMsgEnd2 : BattleScript_TrainerBSlideMsgEnd2);
    else if ((i = ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), TRAINER_SLIDE_PLAYER_MON_UNAFFECTED)))
        BattleScriptExecute(i == 1 ? BattleScript_TrainerASlideMsgEnd2 : BattleScript_TrainerBSlideMsgEnd2);
}

u8 IsRunningFromBattleImpossible(u32 battler)
{
    u32 holdEffect, i;

    if (gBattleMons[battler].item == ITEM_ENIGMA_BERRY_E_READER)
        holdEffect = gEnigmaBerries[battler].holdEffect;
    else
        holdEffect = ItemId_GetHoldEffect(gBattleMons[battler].item);

    gPotentialItemEffectBattler = battler;

    if (gBattleTypeFlags & (BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_POKEDUDE)) // Cannot ever run from saving Birch's battle.
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DONT_LEAVE_BIRCH;
        return BATTLE_RUN_FORBIDDEN;
    }
    if (GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT && WILD_DOUBLE_BATTLE
        && IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT))) // The second pokemon cannot run from a double wild battle, unless it's the only alive mon.
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_ESCAPE;
        return BATTLE_RUN_FORBIDDEN;
    }

    if (holdEffect == HOLD_EFFECT_CAN_ALWAYS_RUN)
        return BATTLE_RUN_SUCCESS;
    if (B_GHOSTS_ESCAPE >= GEN_6 && IS_BATTLER_OF_TYPE(battler, TYPE_GHOST))
        return BATTLE_RUN_SUCCESS;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        return BATTLE_RUN_SUCCESS;
    if (GetBattlerAbility(battler) == ABILITY_RUN_AWAY)
        return BATTLE_RUN_SUCCESS;

    if ((i = IsAbilityPreventingEscape(battler)))
    {
        gBattleScripting.battler = i - 1;
        gLastUsedAbility = gBattleMons[i - 1].ability;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_PREVENTS_ESCAPE;
        return BATTLE_RUN_FAILURE;
    }

    if (!CanBattlerEscape(battler))
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_ESCAPE;
        return BATTLE_RUN_FORBIDDEN;
    }
    return BATTLE_RUN_SUCCESS;
}

void SwitchTwoBattlersInParty(u32 battler, u32 battler2)
{
    s32 i;
    u32 partyId1, partyId2;

    for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
        gBattlePartyCurrentOrder[i] = *(battler * 3 + i + (u8 *)(gBattleStruct->battlerPartyOrders));

    partyId1 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battler]);
    partyId2 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battler2]);
    SwitchPartyMonSlots(partyId1, partyId2);

    for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
    {
        *(battler * 3 + i + (u8 *)(gBattleStruct->battlerPartyOrders)) = gBattlePartyCurrentOrder[i];
        *(BATTLE_PARTNER(battler) * 3 + i + (u8 *)(gBattleStruct->battlerPartyOrders)) = gBattlePartyCurrentOrder[i];
    }
}

void SwitchPartyOrder(u32 battler)
{
    s32 i;
    u32 partyId1, partyId2;

    for (i = 0; i < 3; i++)
        gBattlePartyCurrentOrder[i] = *(battler * 3 + i + (u8 *)(gBattleStruct->battlerPartyOrders));
    partyId1 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battler]);
    partyId2 = GetPartyIdFromBattlePartyId(*(gBattleStruct->monToSwitchIntoId + battler));
    SwitchPartyMonSlots(partyId1, partyId2);
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        for (i = 0; i < 3; i++)
        {
            *(battler * 3 + i + (u8 *)(gBattleStruct->battlerPartyOrders)) = gBattlePartyCurrentOrder[i];
            *(BATTLE_PARTNER(battler) * 3 + i + (u8 *)(gBattleStruct->battlerPartyOrders)) = gBattlePartyCurrentOrder[i];
        }
    }
    else
    {
        for (i = 0; i < 3; i++)
            *(battler * 3 + i + (u8 *)(gBattleStruct->battlerPartyOrders)) = gBattlePartyCurrentOrder[i];
    }
}

enum
{
    STATE_BEFORE_ACTION_CHOSEN,
    STATE_WAIT_ACTION_CHOSEN,
    STATE_WAIT_ACTION_CASE_CHOSEN,
    STATE_WAIT_ACTION_CONFIRMED_STANDBY,
    STATE_WAIT_ACTION_CONFIRMED,
    STATE_SELECTION_SCRIPT,
    STATE_WAIT_SET_BEFORE_ACTION,
    STATE_SELECTION_SCRIPT_MAY_RUN
};

static void HandleTurnActionSelectionState(void)
{
    s32 i, battler;

    gBattleCommunication[ACTIONS_CONFIRMED_COUNT] = 0;
    for (battler = 0; battler < gBattlersCount; battler++)
    {
        u32 position = GetBattlerPosition(battler);
        switch (gBattleCommunication[battler])
        {
        case STATE_BEFORE_ACTION_CHOSEN: // Choose an action.
            *(gBattleStruct->monToSwitchIntoId + battler) = PARTY_SIZE;
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI
             || (position & BIT_FLANK) == B_FLANK_LEFT
             || gBattleStruct->absentBattlerFlags & gBitTable[GetBattlerAtPosition(BATTLE_PARTNER(position))]
             || gBattleCommunication[GetBattlerAtPosition(BATTLE_PARTNER(position))] == STATE_WAIT_ACTION_CONFIRMED)
            {
                if (gBattleStruct->absentBattlerFlags & gBitTable[battler])
                {
                    gChosenActionByBattler[battler] = B_ACTION_NOTHING_FAINTED;
                    if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
                        gBattleCommunication[battler] = STATE_WAIT_ACTION_CONFIRMED;
                    else
                        gBattleCommunication[battler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                }
                else
                {
                    if (gBattleMons[battler].status2 & STATUS2_MULTIPLETURNS
                     || gBattleMons[battler].status2 & STATUS2_RECHARGE)
                    {
                        gChosenActionByBattler[battler] = B_ACTION_USE_MOVE;
                        gBattleCommunication[battler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                    }
                    else if (WILD_DOUBLE_BATTLE
                             && position == B_POSITION_PLAYER_RIGHT
                             && (gBattleStruct->throwingPokeBall || gChosenActionByBattler[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] == B_ACTION_RUN)
                             && gChosenActionByBattler[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] != B_ACTION_NOTHING_FAINTED)
                    {
                        gBattleStruct->throwingPokeBall = FALSE;
                        gChosenActionByBattler[battler] = B_ACTION_NOTHING_FAINTED; // Not fainted, but it cannot move, because of the throwing ball.
                        gBattleCommunication[battler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                    }
                    else
                    {
                        gBattleStruct->itemPartyIndex[battler] = PARTY_SIZE;
                        BtlController_EmitChooseAction(battler, BUFFER_A, gChosenActionByBattler[0], gBattleResources->bufferB[0][1] | (gBattleResources->bufferB[0][2] << 8));
                        MarkBattlerForControllerExec(battler);
                        gBattleCommunication[battler]++;
                    }
                }
            }
            break;
        case STATE_WAIT_ACTION_CHOSEN: // Try to perform an action.
            if (!(gBattleControllerExecFlags & ((gBitTable[battler]) | (0xF0000000) | (gBitTable[battler] << 4) | (gBitTable[battler] << 8) | (gBitTable[battler] << 0xC))))
            {
                gChosenActionByBattler[battler] = gBattleResources->bufferB[battler][1];

                switch (gBattleResources->bufferB[battler][1])
                {
                case B_ACTION_USE_MOVE:
                    if (AreAllMovesUnusable(battler))
                    {
                        gBattleCommunication[battler] = STATE_SELECTION_SCRIPT;
                        *(gBattleStruct->selectionScriptFinished + battler) = FALSE;
                        *(gBattleStruct->stateIdAfterSelScript + battler) = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                        *(gBattleStruct->moveTarget + battler) = gBattleResources->bufferB[battler][3];
                        return;
                    }
                    else if (gDisableStructs[battler].encoredMove != MOVE_NONE)
                    {
                        gChosenMoveByBattler[battler] = gDisableStructs[battler].encoredMove;
                        *(gBattleStruct->chosenMovePositions + battler) = gDisableStructs[battler].encoredMovePos;
                        gBattleCommunication[battler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                        return;
                    }
                    else
                    {
                        struct ChooseMoveStruct moveInfo;

                        moveInfo.zmove = gBattleStruct->zmove;
                        moveInfo.mega = gBattleStruct->mega;
                        moveInfo.species = gBattleMons[battler].species;
                        moveInfo.monType1 = gBattleMons[battler].type1;
                        moveInfo.monType2 = gBattleMons[battler].type2;
                        moveInfo.monType3 = gBattleMons[battler].type3;

                        for (i = 0; i < MAX_MON_MOVES; i++)
                        {
                            moveInfo.moves[i] = gBattleMons[battler].moves[i];
                            moveInfo.currentPp[i] = gBattleMons[battler].pp[i];
                            moveInfo.maxPp[i] = CalculatePPWithBonus(gBattleMons[battler].moves[i],
                                                                     gBattleMons[battler].ppBonuses,
                                                                     i);
                        }
                        BtlController_EmitChooseMove(battler, BUFFER_A, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) != 0, FALSE, &moveInfo);
                        MarkBattlerForControllerExec(battler);
                    }
                    break;
                case B_ACTION_USE_ITEM:
                    if (FlagGet(B_FLAG_NO_BAG_USE))
                    {
                        gSelectionBattleScripts[battler] = BattleScript_ActionSelectionItemsCantBeUsed;
                        gBattleCommunication[battler] = STATE_SELECTION_SCRIPT;
                        *(gBattleStruct->selectionScriptFinished + battler) = FALSE;
                        *(gBattleStruct->stateIdAfterSelScript + battler) = STATE_BEFORE_ACTION_CHOSEN;
                        return;
                    }

                    if (((gBattleTypeFlags & (BATTLE_TYPE_LINK
                                            | BATTLE_TYPE_EREADER_TRAINER))
                                            && !gTestRunnerEnabled)
                                            // Or if currently held by Sky Drop
                                            || gStatuses3[battler] & STATUS3_SKY_DROPPED)
                    {
                        gSelectionBattleScripts[battler] = BattleScript_ActionSelectionItemsCantBeUsed;
                        gBattleCommunication[battler] = STATE_SELECTION_SCRIPT;
                        *(gBattleStruct->selectionScriptFinished + battler) = FALSE;
                        *(gBattleStruct->stateIdAfterSelScript + battler) = STATE_BEFORE_ACTION_CHOSEN;
                        return;
                    }
                    else
                    {
                        BtlController_EmitChooseItem(battler, BUFFER_A, gBattleStruct->battlerPartyOrders[battler]);
                        MarkBattlerForControllerExec(battler);
                    }
                    break;
                case B_ACTION_SWITCH:
                    *(gBattleStruct->battlerPartyIndexes + battler) = gBattlerPartyIndexes[battler];
                    if (gBattleMons[battler].status2 & (STATUS2_WRAPPED | STATUS2_ESCAPE_PREVENTION) || gStatuses3[battler] & STATUS3_ROOTED)
                    {
                        BtlController_EmitChoosePokemon(battler, BUFFER_A, PARTY_ACTION_CANT_SWITCH, PARTY_SIZE, ABILITY_NONE, gBattleStruct->battlerPartyOrders[battler]);
                    }
                    else if (ItemId_GetHoldEffect(gBattleMons[battler].item) != HOLD_EFFECT_SHED_SHELL
                      && (i = IsAbilityPreventingEscape(battler)))   // must be last to keep i value integrity
                    {
                        BtlController_EmitChoosePokemon(battler, BUFFER_A, ((i - 1) << 4) | PARTY_ACTION_ABILITY_PREVENTS, PARTY_SIZE, gBattleMons[i - 1].ability, gBattleStruct->battlerPartyOrders[battler]);
                    }
                    else
                    {
                        if (battler == 2 && gChosenActionByBattler[0] == B_ACTION_SWITCH)
                            BtlController_EmitChoosePokemon(battler, BUFFER_A, PARTY_ACTION_CHOOSE_MON, *(gBattleStruct->monToSwitchIntoId + 0), ABILITY_NONE, gBattleStruct->battlerPartyOrders[battler]);
                        else if (battler == 3 && gChosenActionByBattler[1] == B_ACTION_SWITCH)
                            BtlController_EmitChoosePokemon(battler, BUFFER_A, PARTY_ACTION_CHOOSE_MON, *(gBattleStruct->monToSwitchIntoId + 1), ABILITY_NONE, gBattleStruct->battlerPartyOrders[battler]);
                        else
                            BtlController_EmitChoosePokemon(battler, BUFFER_A, PARTY_ACTION_CHOOSE_MON, PARTY_SIZE, ABILITY_NONE, gBattleStruct->battlerPartyOrders[battler]);
                    }
                    MarkBattlerForControllerExec(battler);
                    break;
                case B_ACTION_SAFARI_BALL:
                    if (IsPlayerPartyAndPokemonStorageFull())
                    {
                        gSelectionBattleScripts[battler] = BattleScript_PrintFullBox;
                        gBattleCommunication[battler] = STATE_SELECTION_SCRIPT;
                        *(gBattleStruct->selectionScriptFinished + battler) = FALSE;
                        *(gBattleStruct->stateIdAfterSelScript + battler) = STATE_BEFORE_ACTION_CHOSEN;
                        return;
                    }
                    break;
                case B_ACTION_CANCEL_PARTNER:
                    gBattleCommunication[battler] = STATE_WAIT_SET_BEFORE_ACTION;
                    gBattleCommunication[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(battler)))] = STATE_BEFORE_ACTION_CHOSEN;
                    BtlController_EmitEndBounceEffect(battler, 0);
                    gBattleStruct->mega.toEvolve &= ~(gBitTable[BATTLE_PARTNER(GetBattlerPosition(battler))]);
                    gBattleStruct->burst.toBurst &= ~(gBitTable[BATTLE_PARTNER(GetBattlerPosition(battler))]);
                    gBattleStruct->dynamax.toDynamax &= ~(gBitTable[BATTLE_PARTNER(GetBattlerPosition(battler))]);
                    gBattleStruct->dynamax.usingMaxMove[BATTLE_PARTNER(GetBattlerPosition(battler))] = FALSE;
                    gBattleStruct->zmove.toBeUsed[BATTLE_PARTNER(GetBattlerPosition(battler))] = MOVE_NONE;
                    MarkBattlerForControllerExec(battler);
                    return;
                case B_ACTION_DEBUG:
                    BtlController_EmitDebugMenu(battler, BUFFER_A);
                    MarkBattlerForControllerExec(battler);
                    break;
                }
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER
                         && !(gBattleTypeFlags & (BATTLE_TYPE_LINK))
                         && gBattleResources->bufferB[battler][1] == B_ACTION_RUN)
                {
                    BattleScriptExecute(BattleScript_PrintCantRunFromTrainer);
                    gBattleCommunication[battler] = STATE_BEFORE_ACTION_CHOSEN;
                }
                else if (IsRunningFromBattleImpossible(battler) != BATTLE_RUN_SUCCESS
                         && gBattleResources->bufferB[battler][1] == B_ACTION_RUN)
                {
                    gSelectionBattleScripts[battler] = BattleScript_PrintCantEscapeFromBattle;
                    gBattleCommunication[battler] = STATE_SELECTION_SCRIPT;
                    *(gBattleStruct->selectionScriptFinished + battler) = FALSE;
                    *(gBattleStruct->stateIdAfterSelScript + battler) = STATE_BEFORE_ACTION_CHOSEN;
                    return;
                }
                else
                {
                    gBattleCommunication[battler]++;
                }
            }
            break;
        case STATE_WAIT_ACTION_CASE_CHOSEN:
            if (!(gBattleControllerExecFlags & ((gBitTable[battler]) | (0xF0000000) | (gBitTable[battler] << 4) | (gBitTable[battler] << 8) | (gBitTable[battler] << 0xC))))
            {
                switch (gChosenActionByBattler[battler])
                {
                case B_ACTION_USE_MOVE:
                    switch (gBattleResources->bufferB[battler][1])
                    {
                    case 3 ... 9:
                        gChosenActionByBattler[battler] = gBattleResources->bufferB[battler][1];
                        return;
                    default:
                        if ((gBattleResources->bufferB[battler][2] | (gBattleResources->bufferB[battler][3] << 8)) == 0xFFFF)
                        {
                            gBattleCommunication[battler] = STATE_BEFORE_ACTION_CHOSEN;
                        }
                        else if (TrySetCantSelectMoveBattleScript(battler))
                        {
                            gBattleCommunication[battler] = STATE_SELECTION_SCRIPT;
                            *(gBattleStruct->selectionScriptFinished + battler) = FALSE;
                            gBattleResources->bufferB[battler][1] = B_ACTION_USE_MOVE;
                            *(gBattleStruct->stateIdAfterSelScript + battler) = STATE_WAIT_ACTION_CHOSEN;
                            return;
                        }
                        else
                        {
                            // Get the chosen move position (and thus the chosen move) and target from the returned buffer.
                            gBattleStruct->chosenMovePositions[battler] = gBattleResources->bufferB[battler][2] & ~(RET_MEGA_EVOLUTION | RET_ULTRA_BURST | RET_DYNAMAX);
                            gChosenMoveByBattler[battler] = gBattleMons[battler].moves[gBattleStruct->chosenMovePositions[battler]];
                            gBattleStruct->moveTarget[battler] = gBattleResources->bufferB[battler][3];

                            // Check to see if any gimmicks need to be prepared.
                            if (gBattleResources->bufferB[battler][2] & RET_MEGA_EVOLUTION)
                                gBattleStruct->mega.toEvolve |= gBitTable[battler];
                            else if (gBattleResources->bufferB[battler][2] & RET_ULTRA_BURST)
                                gBattleStruct->burst.toBurst |= gBitTable[battler];
                            else if (gBattleResources->bufferB[battler][2] & RET_DYNAMAX)
                                gBattleStruct->dynamax.toDynamax |= gBitTable[battler];

                            // Max Move check
                            if (ShouldUseMaxMove(battler, gChosenMoveByBattler[battler]))
                            {
                                gBattleStruct->dynamax.baseMove[battler] = gBattleMons[battler].moves[gBattleStruct->chosenMovePositions[battler]];
                                gBattleStruct->dynamax.usingMaxMove[battler] = TRUE;
                            }
                            gBattleCommunication[battler]++;

                            if (gTestRunnerEnabled)
                            {
                                TestRunner_Battle_CheckChosenMove(battler, gChosenMoveByBattler[battler], gBattleStruct->moveTarget[battler]);
                            }
                        }
                        break;
                    }
                    break;
                case B_ACTION_USE_ITEM:
                    if ((gBattleResources->bufferB[battler][1] | (gBattleResources->bufferB[battler][2] << 8)) == 0)
                    {
                        gBattleCommunication[battler] = STATE_BEFORE_ACTION_CHOSEN;
                    }
                    else
                    {
                        gLastUsedItem = (gBattleResources->bufferB[battler][1] | (gBattleResources->bufferB[battler][2] << 8));
                        if (ItemId_GetPocket(gLastUsedItem) == POCKET_POKE_BALLS)
                            gBattleStruct->throwingPokeBall = TRUE;
                        gBattleCommunication[battler]++;
                    }
                    break;
                case B_ACTION_SWITCH:
                    if (gBattleResources->bufferB[battler][1] == PARTY_SIZE)
                    {
                        gBattleCommunication[battler] = STATE_BEFORE_ACTION_CHOSEN;
                    }
                    else
                    {
                        *(gBattleStruct->monToSwitchIntoId + battler) = gBattleResources->bufferB[battler][1];
                        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                        {
                            *(battler * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) &= 0xF;
                            *(battler * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) |= (gBattleResources->bufferB[battler][2] & 0xF0);
                            *(battler * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 1) = gBattleResources->bufferB[battler][3];
                            *((battler ^ BIT_FLANK) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) &= (0xF0);
                            *((battler ^ BIT_FLANK) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) |= (gBattleResources->bufferB[battler][2] & 0xF0) >> 4;
                            *((battler ^ BIT_FLANK) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 2) = gBattleResources->bufferB[battler][3];
                        }
                        gBattleCommunication[battler]++;
                    }
                    break;
                case B_ACTION_RUN:
                    gHitMarker |= HITMARKER_RUN;
                    gBattleCommunication[battler]++;
                    break;
                case B_ACTION_SAFARI_WATCH_CAREFULLY:
                    gBattleCommunication[battler]++;
                    break;
                case B_ACTION_SAFARI_BALL:
                    gBattleCommunication[battler]++;
                    break;
                case B_ACTION_SAFARI_BAIT:
                case B_ACTION_SAFARI_GO_NEAR:
                    gBattleCommunication[battler]++;
                    break;
                case B_ACTION_SAFARI_RUN:
                    gHitMarker |= HITMARKER_RUN;
                    gBattleCommunication[battler]++;
                    break;
                case B_ACTION_OLDMAN_THROW:
                    gBattleCommunication[battler]++;
                    break;
                }
            }
            break;
        case STATE_WAIT_ACTION_CONFIRMED_STANDBY:
            if (!(gBattleControllerExecFlags & ((gBitTable[battler])
                                                | (0xF << 28)
                                                | (gBitTable[battler] << 4)
                                                | (gBitTable[battler] << 8)
                                                | (gBitTable[battler] << 12))))
            {
                if (AllAtActionConfirmed())
                    i = TRUE;
                else
                    i = FALSE;

                if (((gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE)) != BATTLE_TYPE_DOUBLE)
                 || (position & BIT_FLANK) != B_FLANK_LEFT
                 || (*(&gBattleStruct->absentBattlerFlags) & gBitTable[GetBattlerAtPosition(position ^ BIT_FLANK)]))
                    BtlController_EmitLinkStandbyMsg(battler, BUFFER_A, LINK_STANDBY_MSG_STOP_BOUNCE, i);
                else
                    BtlController_EmitLinkStandbyMsg(battler, BUFFER_A, LINK_STANDBY_STOP_BOUNCE_ONLY, i);
                MarkBattlerForControllerExec(battler);
                gBattleCommunication[battler]++;
            }
            break;
        case STATE_WAIT_ACTION_CONFIRMED:
            if (!(gBattleControllerExecFlags & ((gBitTable[battler]) | (0xF << 28) | (gBitTable[battler] << 4) | (gBitTable[battler] << 8) | (gBitTable[battler] << 12))))
                ++gBattleCommunication[ACTIONS_CONFIRMED_COUNT];
            break;
        case STATE_SELECTION_SCRIPT:
            if (*(gBattleStruct->selectionScriptFinished + battler))
            {
                gBattleCommunication[battler] = *(gBattleStruct->stateIdAfterSelScript + battler);
            }
            else
            {
                gBattlerAttacker = battler;
                gBattlescriptCurrInstr = gSelectionBattleScripts[battler];
                if (!(gBattleControllerExecFlags & ((gBitTable[battler]) | (0xF << 28) | (gBitTable[battler] << 4) | (gBitTable[battler] << 8) | (gBitTable[battler] << 12))))
                    gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
                gSelectionBattleScripts[battler] = gBattlescriptCurrInstr;
            }
            break;
        case STATE_WAIT_SET_BEFORE_ACTION:
            if (!(gBattleControllerExecFlags & ((gBitTable[battler]) | (0xF << 28) | (gBitTable[battler] << 4) | (gBitTable[battler] << 8) | (gBitTable[battler] << 12))))
                gBattleCommunication[battler] = STATE_BEFORE_ACTION_CHOSEN;
            break;
        }
    }
    // Check if everyone chose actions.
    if (gBattleCommunication[ACTIONS_CONFIRMED_COUNT] == gBattlersCount)
    {
        if (WILD_DOUBLE_BATTLE
            && gBattleStruct->throwingPokeBall
            && gChosenActionByBattler[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)] != B_ACTION_NOTHING_FAINTED)
        {
            // if we choose to throw a ball with our second mon, skip the action of the first
            // (if we have chosen throw ball with first, second's is already skipped)
            // if throwing a ball in a wild battle with an in-game partner, skip partner's turn when throwing a ball
            if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                gChosenActionByBattler[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)] = B_ACTION_NOTHING_FAINTED;
            else
                gChosenActionByBattler[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] = B_ACTION_NOTHING_FAINTED;
        }

        gBattleMainFunc = SetActionsAndBattlersTurnOrder;

        if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
        {
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gChosenActionByBattler[i] == B_ACTION_SWITCH)
                    SwitchPartyOrderInGameMulti(i, *(gBattleStruct->monToSwitchIntoId + i));
            }
        }
    }
}

static bool8 AllAtActionConfirmed(void)
{
    s32 i, count;

    for (count = 0, i = 0; i < gBattlersCount; i++)
    {
        if (gBattleCommunication[i] == STATE_WAIT_ACTION_CONFIRMED)
            count++;
    }

    if (count + 1 == gBattlersCount)
        return TRUE;
    else
        return FALSE;
}

static void UpdateBattlerPartyOrdersOnSwitch(u32 battler)
{
    gBattleStruct->monToSwitchIntoId[battler] = gBattleResources->bufferB[battler][1];

    if (gBattleTypeFlags & BATTLE_TYPE_LINK && gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        *(battler * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) &= 0xF;
        *(battler * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) |= (gBattleResources->bufferB[battler][2] & 0xF0);
        *(battler * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 1) = gBattleResources->bufferB[battler][3];

        *((BATTLE_PARTNER(battler)) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) &= (0xF0);
        *((BATTLE_PARTNER(battler)) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) |= (gBattleResources->bufferB[battler][2] & 0xF0) >> 4;
        *((BATTLE_PARTNER(battler)) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 2) = gBattleResources->bufferB[battler][3];
    }
}

void SwapTurnOrder(u8 id1, u8 id2)
{
    u32 temp;

    SWAP(gActionsByTurnOrder[id1], gActionsByTurnOrder[id2], temp);
    SWAP(gBattlerByTurnOrder[id1], gBattlerByTurnOrder[id2], temp);
}

// For AI, so it doesn't 'cheat' by knowing player's ability
u32 GetBattlerTotalSpeedStatArgs(u32 battler, u32 ability, u32 holdEffect)
{
    u32 speed = gBattleMons[battler].speed;
    u32 highestStat = GetHighestStatId(battler);

    // weather abilities
    if (WEATHER_HAS_EFFECT)
    {
        if (ability == ABILITY_SWIFT_SWIM       && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA && gBattleWeather & B_WEATHER_RAIN)
            speed *= 2;
        else if (ability == ABILITY_CHLOROPHYLL && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA && gBattleWeather & B_WEATHER_SUN)
            speed *= 2;
        else if (ability == ABILITY_SAND_RUSH   && gBattleWeather & B_WEATHER_SANDSTORM)
            speed *= 2;
        else if (ability == ABILITY_SLUSH_RUSH  && (gBattleWeather & (B_WEATHER_HAIL | B_WEATHER_SNOW)))
            speed *= 2;
    }

    // other abilities
    if (ability == ABILITY_QUICK_FEET && gBattleMons[battler].status1 & STATUS1_ANY)
        speed = (speed * 150) / 100;
    else if (ability == ABILITY_SURGE_SURFER && gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN)
        speed *= 2;
    else if (ability == ABILITY_SLOW_START && gDisableStructs[battler].slowStartTimer != 0)
        speed /= 2;
    else if (ability == ABILITY_PROTOSYNTHESIS && gBattleWeather & B_WEATHER_SUN && highestStat == STAT_SPEED)
        speed = (speed * 150) / 100;
    else if (ability == ABILITY_QUARK_DRIVE && gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN && highestStat == STAT_SPEED)
        speed = (speed * 150) / 100;

    // stat stages
    speed *= gStatStageRatios[gBattleMons[battler].statStages[STAT_SPEED]][0];
    speed /= gStatStageRatios[gBattleMons[battler].statStages[STAT_SPEED]][1];

    // player's badge boost
    if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_TRAINER_TOWER | BATTLE_TYPE_BATTLE_TOWER))
        && ShouldGetStatBadgeBoost(FLAG_BADGE03_GET, battler)
        && GetBattlerSide(battler) == B_SIDE_PLAYER)
    {
        speed = (speed * 110) / 100;
    }

    // item effects
    if (holdEffect == HOLD_EFFECT_MACHO_BRACE || holdEffect == HOLD_EFFECT_POWER_ITEM)
        speed /= 2;
    else if (holdEffect == HOLD_EFFECT_IRON_BALL)
        speed /= 2;
    else if (holdEffect == HOLD_EFFECT_CHOICE_SCARF && !IsDynamaxed(battler))
        speed = (speed * 150) / 100;
    else if (holdEffect == HOLD_EFFECT_QUICK_POWDER && gBattleMons[battler].species == SPECIES_DITTO && !(gBattleMons[battler].status2 & STATUS2_TRANSFORMED))
        speed *= 2;

    // various effects
    if (gSideStatuses[GetBattlerSide(battler)] & SIDE_STATUS_TAILWIND)
        speed *= 2;
    if (gBattleResources->flags->flags[battler] & RESOURCE_FLAG_UNBURDEN)
        speed *= 2;

    // paralysis drop
    if (gBattleMons[battler].status1 & STATUS1_PARALYSIS && ability != ABILITY_QUICK_FEET)
        speed /= B_PARALYSIS_SPEED >= GEN_7 ? 2 : 4;

    if (gSideStatuses[GetBattlerSide(battler)] & SIDE_STATUS_SWAMP)
        speed /= 4;

    return speed;
}

u32 GetBattlerTotalSpeedStat(u32 battler)
{
    u32 ability = GetBattlerAbility(battler);
    u32 holdEffect = GetBattlerHoldEffect(battler, TRUE);
    return GetBattlerTotalSpeedStatArgs(battler, ability, holdEffect);
}

s8 GetChosenMovePriority(u32 battler)
{
    u16 move;

    gProtectStructs[battler].pranksterElevated = 0;
    if (gProtectStructs[battler].noValidMoves)
        move = MOVE_STRUGGLE;
    else
        move = gBattleMons[battler].moves[*(gBattleStruct->chosenMovePositions + battler)];

    return GetMovePriority(battler, move);
}

s8 GetMovePriority(u32 battler, u16 move)
{
    s8 priority;
    u16 ability = GetBattlerAbility(battler);

    if (gBattleStruct->zmove.toBeUsed[battler] && gMovesInfo[move].power != 0)
        move = gBattleStruct->zmove.toBeUsed[battler];

    priority = gMovesInfo[move].priority;

    // Max Guard check
    if (gBattleStruct->dynamax.usingMaxMove[battler] && gMovesInfo[move].category == DAMAGE_CATEGORY_STATUS)
        return gMovesInfo[MOVE_MAX_GUARD].priority;

    if (ability == ABILITY_GALE_WINGS
        && (B_GALE_WINGS < GEN_7 || BATTLER_MAX_HP(battler))
        && gMovesInfo[move].type == TYPE_FLYING)
    {
        priority++;
    }
    else if (ability == ABILITY_PRANKSTER && IS_MOVE_STATUS(move))
    {
        gProtectStructs[battler].pranksterElevated = 1;
        priority++;
    }
    else if (gMovesInfo[move].effect == EFFECT_GRASSY_GLIDE && gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN && IsBattlerGrounded(battler) && !IsDynamaxed(battler) && !(gBattleStruct->dynamax.toDynamax & gBitTable[battler]))
    {
        priority++;
    }
    else if (ability == ABILITY_TRIAGE && IsHealingMove(move))
        priority += 3;

    if (gProtectStructs[battler].quash)
        priority = -8;

    return priority;
}

// Function for AI with variables provided as arguments to speed the computation time
s32 GetWhichBattlerFasterArgs(u32 battler1, u32 battler2, bool32 ignoreChosenMoves, u32 ability1, u32 ability2,
                              u32 holdEffectBattler1, u32 holdEffectBattler2, u32 speedBattler1, u32 speedBattler2, s32 priority1, s32 priority2)
{
    u32 strikesFirst = 0;

    if (priority1 == priority2)
    {
        // Quick Claw / Quick Draw / Custap Berry - always first
        // Stall / Mycelium Might - last but before Lagging Tail
        // Lagging Tail - always last
        bool32 battler1HasQuickEffect = gProtectStructs[battler1].quickDraw || gProtectStructs[battler1].usedCustapBerry;
        bool32 battler2HasQuickEffect = gProtectStructs[battler2].quickDraw || gProtectStructs[battler2].usedCustapBerry;
        bool32 battler1HasStallingAbility = ability1 == ABILITY_STALL || (ability1 == ABILITY_MYCELIUM_MIGHT && IS_MOVE_STATUS(gChosenMoveByBattler[battler1]));
        bool32 battler2HasStallingAbility = ability2 == ABILITY_STALL || (ability2 == ABILITY_MYCELIUM_MIGHT && IS_MOVE_STATUS(gChosenMoveByBattler[battler2]));

        if (battler1HasQuickEffect && !battler2HasQuickEffect)
            strikesFirst = 1;
        else if (battler2HasQuickEffect && !battler1HasQuickEffect)
            strikesFirst = -1;
        else if (holdEffectBattler1 == HOLD_EFFECT_LAGGING_TAIL && holdEffectBattler2 != HOLD_EFFECT_LAGGING_TAIL)
            strikesFirst = -1;
        else if (holdEffectBattler2 == HOLD_EFFECT_LAGGING_TAIL && holdEffectBattler1 != HOLD_EFFECT_LAGGING_TAIL)
            strikesFirst = 1;
        else if (battler1HasStallingAbility && !battler2HasStallingAbility)
            strikesFirst = -1;
        else if (battler2HasStallingAbility && !battler1HasStallingAbility)
            strikesFirst = 1;
        else
        {
            if (speedBattler1 == speedBattler2 && Random() & 1)
            {
                strikesFirst = 0; // same speeds, same priorities
            }
            else if (speedBattler1 < speedBattler2)
            {
                // battler2 has more speed
                if (gFieldStatuses & STATUS_FIELD_TRICK_ROOM)
                    strikesFirst = 1;
                else
                    strikesFirst = -1;
            }
            else
            {
                // battler1 has more speed
                if (gFieldStatuses & STATUS_FIELD_TRICK_ROOM)
                    strikesFirst = -1;
                else
                    strikesFirst = 1;
            }
        }
    }
    else if (priority1 < priority2)
    {
        strikesFirst = -1; // battler2's move has greater priority
    }
    else
    {
        strikesFirst = 1; // battler1's move has greater priority
    }
    return strikesFirst;
}

s32 GetWhichBattlerFaster(u32 battler1, u32 battler2, bool32 ignoreChosenMoves)
{
    s32 priority1 = 0, priority2 = 0;
    u32 ability1 = GetBattlerAbility(battler1);
    u32 speedBattler1 = GetBattlerTotalSpeedStat(battler1);
    u32 holdEffectBattler1 = GetBattlerHoldEffect(battler1, TRUE);
    u32 speedBattler2 = GetBattlerTotalSpeedStat(battler2);
    u32 holdEffectBattler2 = GetBattlerHoldEffect(battler2, TRUE);
    u32 ability2 = GetBattlerAbility(battler2);

    if (!ignoreChosenMoves)
    {
        if (gChosenActionByBattler[battler1] == B_ACTION_USE_MOVE)
            priority1 = GetChosenMovePriority(battler1);
        if (gChosenActionByBattler[battler2] == B_ACTION_USE_MOVE)
            priority2 = GetChosenMovePriority(battler2);
    }

    return GetWhichBattlerFasterArgs(battler1, battler2, ignoreChosenMoves, ability1, ability2,
                                     holdEffectBattler1, holdEffectBattler2, speedBattler1, speedBattler2, priority1, priority2);
}

static void SetActionsAndBattlersTurnOrder(void)
{
    s32 turnOrderId = 0;
    s32 i, j, battler;;

    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
    {
        for (battler = 0; battler < gBattlersCount; battler++)
        {
            gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[battler];
            gBattlerByTurnOrder[turnOrderId] = battler;
            turnOrderId++;
        }
    }
    else
    {
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            for (battler = 0; battler < gBattlersCount; battler++)
            {
                if (gChosenActionByBattler[battler] == B_ACTION_RUN)
                {
                    turnOrderId = 5;
                    break;
                }
            }
        }
        else
        {
            if (gChosenActionByBattler[0] == B_ACTION_RUN)
            {
                battler = 0;
                turnOrderId = 5;
            }
            if (gChosenActionByBattler[2] == B_ACTION_RUN)
            {
                battler = 2;
                turnOrderId = 5;
            }
        }
        if (turnOrderId == 5) // One of battlers wants to run.
        {
            gActionsByTurnOrder[0] = gChosenActionByBattler[battler];
            gBattlerByTurnOrder[0] = battler;
            turnOrderId = 1;
            for (i = 0; i < gBattlersCount; i++)
            {
                if (i != battler)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[i];
                    gBattlerByTurnOrder[turnOrderId] = i;
                    turnOrderId++;
                }
            }
        }
        else
        {
            for (battler = 0; battler < gBattlersCount; battler++)
            {
                if (gChosenActionByBattler[battler] == B_ACTION_USE_ITEM
                  || gChosenActionByBattler[battler] == B_ACTION_SWITCH
                  || gChosenActionByBattler[battler] == B_ACTION_THROW_BALL)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[battler];
                    gBattlerByTurnOrder[turnOrderId] = battler;
                    turnOrderId++;
                }
            }
            for (battler = 0; battler < gBattlersCount; battler++)
            {
                if (gChosenActionByBattler[battler] != B_ACTION_USE_ITEM
                  && gChosenActionByBattler[battler] != B_ACTION_SWITCH
                  && gChosenActionByBattler[battler] != B_ACTION_THROW_BALL)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[battler];
                    gBattlerByTurnOrder[turnOrderId] = battler;
                    gBattleStruct->quickClawRandom[battler] = RandomPercentage(RNG_QUICK_CLAW, GetBattlerHoldEffectParam(battler));
                    gBattleStruct->quickDrawRandom[battler] = RandomPercentage(RNG_QUICK_DRAW, 30);
                    turnOrderId++;
                }
            }
            for (i = 0; i < gBattlersCount - 1; i++)
            {
                for (j = i + 1; j < gBattlersCount; j++)
                {
                    u8 battler1 = gBattlerByTurnOrder[i];
                    u8 battler2 = gBattlerByTurnOrder[j];
                    TryChangingTurnOrderEffects(battler1, battler2);

                    if (gActionsByTurnOrder[i] != B_ACTION_USE_ITEM
                     && gActionsByTurnOrder[j] != B_ACTION_USE_ITEM
                     && gActionsByTurnOrder[i] != B_ACTION_SWITCH
                     && gActionsByTurnOrder[j] != B_ACTION_SWITCH
                     && gActionsByTurnOrder[i] != B_ACTION_THROW_BALL
                     && gActionsByTurnOrder[j] != B_ACTION_THROW_BALL)
                    {
                        if (GetWhichBattlerFaster(battler1, battler2, FALSE) == -1)
                            SwapTurnOrder(i, j);
                    }
                }
            }
        }
    }
    gBattleMainFunc = CheckChangingTurnOrderEffects;
    gBattleStruct->quickClawBattlerId = 0;
}

static void TurnValuesCleanUp(bool8 var0)
{
    s32 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (var0)
        {
            gProtectStructs[i].protected = FALSE;
            gProtectStructs[i].spikyShielded = FALSE;
            gProtectStructs[i].kingsShielded = FALSE;
            gProtectStructs[i].banefulBunkered = FALSE;
            gProtectStructs[i].quash = FALSE;
            gProtectStructs[i].usedCustapBerry = FALSE;
            gProtectStructs[i].quickDraw = FALSE;
            memset(&gQueuedStatBoosts[i], 0, sizeof(struct QueuedStatBoost));
        }
        else
        {
            memset(&gProtectStructs[i], 0, sizeof(struct ProtectStruct));

            if (gDisableStructs[i].isFirstTurn)
                gDisableStructs[i].isFirstTurn--;

            if (gDisableStructs[i].rechargeTimer)
            {
                gDisableStructs[i].rechargeTimer--;
                if (gDisableStructs[i].rechargeTimer == 0)
                    gBattleMons[i].status2 &= ~STATUS2_RECHARGE;
            }
        }

        if (gDisableStructs[i].substituteHP == 0)
            gBattleMons[i].status2 &= ~STATUS2_SUBSTITUTE;

        gSpecialStatuses[i].parentalBondState = PARENTAL_BOND_OFF;
    }

    gSideStatuses[B_SIDE_PLAYER] &= ~(SIDE_STATUS_QUICK_GUARD | SIDE_STATUS_WIDE_GUARD | SIDE_STATUS_CRAFTY_SHIELD | SIDE_STATUS_MAT_BLOCK);
    gSideStatuses[B_SIDE_OPPONENT] &= ~(SIDE_STATUS_QUICK_GUARD | SIDE_STATUS_WIDE_GUARD | SIDE_STATUS_CRAFTY_SHIELD | SIDE_STATUS_MAT_BLOCK);
    gSideTimers[B_SIDE_PLAYER].followmeTimer = 0;
    gSideTimers[B_SIDE_OPPONENT].followmeTimer = 0;

    gBattleStruct->pledgeMove = FALSE; // combined pledge move may not have been used due to a canceller
}
static void SpecialStatusesClear(void)
{
    memset(&gSpecialStatuses, 0, sizeof(gSpecialStatuses));
}

static void PopulateArrayWithBattlers(u8 *battlers)
{
    u32 i;
    for (i = 0; i < gBattlersCount; i++)
        battlers[i] = i;
}

static bool32 TryDoGimmicksBeforeMoves(void)
{
    if (!(gHitMarker & HITMARKER_RUN)
        && (gBattleStruct->mega.toEvolve || gBattleStruct->burst.toBurst || gBattleStruct->dynamax.toDynamax))
    {
        u32 i, battler;
        u8 order[MAX_BATTLERS_COUNT];

        PopulateArrayWithBattlers(order);
        SortBattlersBySpeed(order, FALSE);
        for (i = 0; i < gBattlersCount; i++)
        {
            // Dynamax Check
            if (gBattleStruct->dynamax.toDynamax & gBitTable[order[i]])
            {
                gBattlerAttacker = order[i];
                gBattleScripting.battler = gBattlerAttacker;
                gBattleStruct->dynamax.toDynamax &= ~(gBitTable[gBattlerAttacker]);
                PrepareBattlerForDynamax(gBattlerAttacker);
                BattleScriptExecute(BattleScript_DynamaxBegins);
                return TRUE;
            }
            // Mega Evo Check
            if (gBattleStruct->mega.toEvolve & gBitTable[order[i]]
                && !(gProtectStructs[order[i]].noValidMoves))
            {
                gBattlerAttacker = order[i];
                gBattleStruct->mega.toEvolve &= ~(gBitTable[gBattlerAttacker]);
                gLastUsedItem = gBattleMons[gBattlerAttacker].item;
                if (GetBattleFormChangeTargetSpecies(gBattlerAttacker, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE) != SPECIES_NONE)
                    BattleScriptExecute(BattleScript_WishMegaEvolution);
                else
                    BattleScriptExecute(BattleScript_MegaEvolution);
                return TRUE;
            }
            // Ultra Burst Check
            if (gBattleStruct->burst.toBurst & gBitTable[order[i]]
                && !(gProtectStructs[order[i]].noValidMoves))
            {
                battler = gBattlerAttacker = order[i];
                gBattleStruct->burst.toBurst &= ~(gBitTable[battler]);
                gLastUsedItem = gBattleMons[battler].item;
                BattleScriptExecute(BattleScript_UltraBurst);
                return TRUE;
            }
        }
    }

    if (B_MEGA_EVO_TURN_ORDER >= GEN_7)
        TryChangeTurnOrder(); // This will just do nothing if no mon has mega evolved.
    return FALSE;
}

static bool32 TryDoMoveEffectsBeforeMoves(void)
{
    if (!(gHitMarker & HITMARKER_RUN))
    {
        u32 i;
        u8 battlers[MAX_BATTLERS_COUNT];

        PopulateArrayWithBattlers(battlers);
        SortBattlersBySpeed(battlers, FALSE);
        for (i = 0; i < gBattlersCount; i++)
        {
            if (!(gBattleStruct->focusPunchBattlers & gBitTable[battlers[i]])
                && !(gBattleMons[battlers[i]].status1 & STATUS1_SLEEP)
                && !(gDisableStructs[battlers[i]].truantCounter)
                && !(gProtectStructs[battlers[i]].noValidMoves))
            {
                gBattleStruct->focusPunchBattlers |= gBitTable[battlers[i]];
                gBattlerAttacker = battlers[i];
                switch (gChosenMoveByBattler[gBattlerAttacker])
                {
                case MOVE_FOCUS_PUNCH:
                    BattleScriptExecute(BattleScript_FocusPunchSetUp);
                    return TRUE;
                case MOVE_BEAK_BLAST:
                    BattleScriptExecute(BattleScript_BeakBlastSetUp);
                    return TRUE;
                case MOVE_SHELL_TRAP:
                    BattleScriptExecute(BattleScript_ShellTrapSetUp);
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

// In gen7, priority and speed are recalculated during the turn in which a pokemon mega evolves
static void TryChangeTurnOrder(void)
{
    u32 i, j;
    for (i = 0; i < gBattlersCount - 1; i++)
    {
        for (j = i + 1; j < gBattlersCount; j++)
        {
            u32 battler1 = gBattlerByTurnOrder[i];
            u32 battler2 = gBattlerByTurnOrder[j];

            if (gActionsByTurnOrder[i] == B_ACTION_USE_MOVE
                && gActionsByTurnOrder[j] == B_ACTION_USE_MOVE)
            {
                if (GetWhichBattlerFaster(battler1, battler2, FALSE) == -1)
                    SwapTurnOrder(i, j);
            }
        }
    }
}

static void TryChangingTurnOrderEffects(u32 battler1, u32 battler2)
{
    u32 ability1 = GetBattlerAbility(battler1);
    u32 holdEffectBattler1 = GetBattlerHoldEffect(battler1, TRUE);
    u32 holdEffectBattler2 = GetBattlerHoldEffect(battler2, TRUE);
    u32 ability2 = GetBattlerAbility(battler2);

    // Battler 1
    // Quick Draw
    if (ability1 == ABILITY_QUICK_DRAW && !IS_MOVE_STATUS(gChosenMoveByBattler[battler1]) && gBattleStruct->quickDrawRandom[battler1])
        gProtectStructs[battler1].quickDraw = TRUE;
    // Quick Claw and Custap Berry
    if (!gProtectStructs[battler1].quickDraw
     && ((holdEffectBattler1 == HOLD_EFFECT_QUICK_CLAW && gBattleStruct->quickClawRandom[battler1])
     || (holdEffectBattler1 == HOLD_EFFECT_CUSTAP_BERRY && HasEnoughHpToEatBerry(battler1, 4, gBattleMons[battler1].item))))
        gProtectStructs[battler1].usedCustapBerry = TRUE;

    // Battler 2
    // Quick Draw
    if (ability2 == ABILITY_QUICK_DRAW && !IS_MOVE_STATUS(gChosenMoveByBattler[battler2]) && gBattleStruct->quickDrawRandom[battler2])
        gProtectStructs[battler2].quickDraw = TRUE;
    // Quick Claw and Custap Berry
    if (!gProtectStructs[battler2].quickDraw
     && ((holdEffectBattler2 == HOLD_EFFECT_QUICK_CLAW && gBattleStruct->quickClawRandom[battler2])
     || (holdEffectBattler2 == HOLD_EFFECT_CUSTAP_BERRY && HasEnoughHpToEatBerry(battler2, 4, gBattleMons[battler2].item))))
        gProtectStructs[battler2].usedCustapBerry = TRUE;
}

static void CheckChangingTurnOrderEffects(void)
{
    u32 i, battler;

    if (!(gHitMarker & HITMARKER_RUN))
    {
        while (gBattleStruct->quickClawBattlerId < gBattlersCount)
        {
            battler = gBattlerAttacker = gBattleStruct->quickClawBattlerId;
            gBattleStruct->quickClawBattlerId++;
            if (gChosenActionByBattler[battler] == B_ACTION_USE_MOVE
             && gChosenMoveByBattler[battler] != MOVE_FOCUS_PUNCH   // quick claw message doesn't need to activate here
             && (gProtectStructs[battler].usedCustapBerry || gProtectStructs[battler].quickDraw)
             && !(gBattleMons[battler].status1 & STATUS1_SLEEP)
             && !(gDisableStructs[gBattlerAttacker].truantCounter)
             && !(gProtectStructs[battler].noValidMoves))
            {
                if (gProtectStructs[battler].usedCustapBerry)
                {
                    gLastUsedItem = gBattleMons[battler].item;
                    PREPARE_ITEM_BUFFER(gBattleTextBuff1, gLastUsedItem);
                    if (GetBattlerHoldEffect(battler, FALSE) == HOLD_EFFECT_CUSTAP_BERRY)
                    {
                        // don't record berry since its gone now
                        BattleScriptExecute(BattleScript_CustapBerryActivation);
                    }
                    else
                    {
                        RecordItemEffectBattle(battler, GetBattlerHoldEffect(battler, FALSE));
                        BattleScriptExecute(BattleScript_QuickClawActivation);
                    }
                }
                else if (gProtectStructs[battler].quickDraw)
                {
                    gBattlerAbility = battler;
                    gLastUsedAbility = gBattleMons[battler].ability;
                    PREPARE_ABILITY_BUFFER(gBattleTextBuff1, gLastUsedAbility);
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    BattleScriptExecute(BattleScript_QuickDrawActivation);
                }
                return;
            }
        }
    }

    // setup stuff before turns/actions
    TryClearRageAndFuryCutter();
    gCurrentTurnActionNumber = 0;
    gCurrentActionFuncId = gActionsByTurnOrder[0];
    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->effectsBeforeUsingMoveDone = FALSE;
    gBattleStruct->focusPunchBattlers = 0;
    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gBattleStruct->ateBoost[i] = FALSE;
        gSpecialStatuses[i].gemBoost = FALSE;
    }

    gBattleMainFunc = RunTurnActionsFunctions;
    gBattleCommunication[3] = 0;
    gBattleCommunication[4] = 0;
    gBattleScripting.multihitMoveEffect = 0;
    gBattleResources->battleScriptsStack->size = 0;
}

static void RunTurnActionsFunctions(void)
{
    if (gBattleOutcome != 0)
        gCurrentActionFuncId = B_ACTION_FINISHED;

    // Mega Evolve / Focus Punch-like moves after switching, items, running, but before using a move.
    if (gCurrentActionFuncId == B_ACTION_USE_MOVE && !gBattleStruct->effectsBeforeUsingMoveDone)
    {
        if (TryDoGimmicksBeforeMoves())
            return;
        else if (TryDoMoveEffectsBeforeMoves())
            return;
        gBattleStruct->effectsBeforeUsingMoveDone = TRUE;
    }

    *(&gBattleStruct->savedTurnActionNumber) = gCurrentTurnActionNumber;
    sTurnActionsFuncsTable[gCurrentActionFuncId]();

    if (gCurrentTurnActionNumber >= gBattlersCount) // everyone did their actions, turn finished
    {
        gHitMarker &= ~(HITMARKER_PASSIVE_DAMAGE);
        gBattleMainFunc = sEndTurnFuncsTable[gBattleOutcome & 0x7F];
    }
    else
    {
        if (gBattleStruct->savedTurnActionNumber != gCurrentTurnActionNumber) // action turn has been done, clear hitmarker bits for another battler
        {
            gHitMarker &= ~(HITMARKER_NO_ATTACKSTRING);
            gHitMarker &= ~(HITMARKER_UNABLE_TO_USE_MOVE);
        }
    }
}

static void HandleEndTurn_BattleWon(void)
{
    gCurrentActionFuncId = 0;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gBattleTextBuff1[0] = gBattleOutcome;
        gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
        gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
    }
    else if (gBattleTypeFlags & (BATTLE_TYPE_TRAINER_TOWER | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER))
    {
        BattleStopLowHpSound();
        PlayBGM(MUS_VICTORY_TRAINER);
        gBattlescriptCurrInstr = BattleScript_BattleTowerTrainerBattleWon;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        BattleStopLowHpSound();
        gBattlescriptCurrInstr = BattleScript_LocalTrainerBattleWon;
        switch (GetTrainerClassFromId(gTrainerBattleOpponent_A))
        {
        case TRAINER_CLASS_LEADER:
        case TRAINER_CLASS_CHAMPION:
            PlayBGM(MUS_VICTORY_GYM_LEADER);
            break;
        case TRAINER_CLASS_BOSS:
        case TRAINER_CLASS_TEAM_ROCKET:
        case TRAINER_CLASS_COOLTRAINER:
        case TRAINER_CLASS_ELITE_FOUR:
        case TRAINER_CLASS_GENTLEMAN:
        default:
            PlayBGM(MUS_VICTORY_TRAINER);
            break;
        }
    }
    else
    {
        gBattlescriptCurrInstr = BattleScript_PayDayMoneyAndPickUpItems;
    }
    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_BattleLost(void)
{
    gCurrentActionFuncId = 0;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gBattleTextBuff1[0] = gBattleOutcome;
        gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
        gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
    }
    else
    {
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && GetTrainerBattleMode() == TRAINER_BATTLE_EARLY_RIVAL)
        {
            if (GetRivalBattleFlags() & RIVAL_BATTLE_HEAL_AFTER)
                gBattleCommunication[MULTISTRING_CHOOSER] = 1; // Dont do white out text
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = 2; // Do white out text
            gBattlerAttacker = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = 0;
        }
        gBattlescriptCurrInstr = BattleScript_LocalBattleLost;
    }
    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_RanFromBattle(void)
{
    gCurrentActionFuncId = 0;
    switch (gProtectStructs[gBattlerAttacker].fleeType)
    {
    default:
        gBattlescriptCurrInstr = BattleScript_GotAwaySafely;
        break;
    case 1:
        gBattlescriptCurrInstr = BattleScript_SmokeBallEscape;
        break;
    case 2:
        gBattlescriptCurrInstr = BattleScript_RanAwayUsingMonAbility;
        break;
    }
    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_MonFled(void)
{
    gCurrentActionFuncId = 0;
    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker]);
    gBattlescriptCurrInstr = BattleScript_WildMonFled;
    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_FinishBattle(void)
{
    u32 i, battler;

    if (gCurrentActionFuncId == B_ACTION_TRY_FINISH || gCurrentActionFuncId == B_ACTION_FINISHED)
    {
        if (!(gBattleTypeFlags & (BATTLE_TYPE_TRAINER_TOWER | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_SAFARI | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_LINK)))
        {
            for (battler = 0; battler < gBattlersCount; battler++)
            {
                if (GetBattlerSide(battler) == B_SIDE_PLAYER)
                {
                    if (gBattleResults.playerMon1Species == SPECIES_NONE)
                    {
                        gBattleResults.playerMon1Species = gBattleMons[battler].species;
                        StringCopy(gBattleResults.playerMon1Name, gBattleMons[battler].nickname);
                    }
                    else
                    {
                        gBattleResults.playerMon2Species = gBattleMons[battler].species;
                        StringCopy(gBattleResults.playerMon2Name, gBattleMons[battler].nickname);
                    }
                }
            }
        }
        TrySetQuestLogBattleEvent();
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
            ClearRematchStateByTrainerId();
        if (gTestRunnerEnabled)
            TestRunner_Battle_AfterLastTurn();
        BeginFastPaletteFade(3);
        FadeOutMapMusic(5);
        if (B_TRAINERS_KNOCK_OFF_ITEMS == TRUE || B_RESTORE_HELD_BATTLE_ITEMS >= GEN_9)
            TryRestoreHeldItems();

        // Undo Dynamax HP multiplier before recalculating stats.
        for (i = 0; i < gBattlersCount; ++i)
        {
            if (IsDynamaxed(i))
                UndoDynamax(i);
        }

        for (i = 0; i < PARTY_SIZE; i++)
        {
            bool8 changedForm = FALSE;

            // Appeared in battle and didn't faint
            if ((gBattleStruct->appearedInBattle & gBitTable[i]) && GetMonData(&gPlayerParty[i], MON_DATA_HP, NULL) != 0)
                changedForm = TryFormChange(i, B_SIDE_PLAYER, FORM_CHANGE_END_BATTLE_TERRAIN);

            if (!changedForm)
                changedForm = TryFormChange(i, B_SIDE_PLAYER, FORM_CHANGE_END_BATTLE);

            // Clear original species field
            gBattleStruct->changedSpecies[B_SIDE_PLAYER][i] = SPECIES_NONE;
            gBattleStruct->changedSpecies[B_SIDE_OPPONENT][i] = SPECIES_NONE;

            // Recalculate the stats of every party member before the end
            if (!changedForm && B_RECALCULATE_STATS >= GEN_5)
                CalculateMonStats(&gPlayerParty[i]);
        }
        // Clear battle mon species to avoid a bug on the next battle that causes
        // healthboxes loading incorrectly due to it trying to create a Mega Indicator
        // if the previous battler would've had it.
        for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            gBattleMons[i].species = SPECIES_NONE;
        }
        gBattleMainFunc = FreeResetData_ReturnToOvOrDoEvolutions;
        gCB2_AfterEvolution = BattleMainCB2;
    }
    else if (gBattleControllerExecFlags == 0)
    {
        gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
    }
}

static void FreeResetData_ReturnToOvOrDoEvolutions(void)
{
    if (!gPaletteFade.active)
    {
        gIsFishingEncounter = FALSE;
        gIsSurfingEncounter = FALSE;
        ResetSpriteData();
        if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK
                                  | BATTLE_TYPE_OLD_MAN_TUTORIAL
                                  | BATTLE_TYPE_SAFARI
                                  | BATTLE_TYPE_EREADER_TRAINER
                                  | BATTLE_TYPE_TRAINER_TOWER
                                  | BATTLE_TYPE_BATTLE_TOWER
                                  | BATTLE_TYPE_POKEDUDE))
            && (B_EVOLUTION_AFTER_WHITEOUT >= GEN_6
                || gBattleOutcome == B_OUTCOME_WON
                || gBattleOutcome == B_OUTCOME_CAUGHT))
        {
            gBattleMainFunc = TryEvolvePokemon;
        }
        else
        {
            gBattleMainFunc = ReturnFromBattleToOverworld;
            return;
        }
    }
    FreeAllWindowBuffers();
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        FreeMonSpritesGfx();
        FreeBattleSpritesData();
        FreeBattleResources();
    }
}

static void TryEvolvePokemon(void)
{
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (!(sTriedEvolving & gBitTable[i]))
        {
            u16 species = GetEvolutionTargetSpecies(&gPlayerParty[i], EVO_MODE_BATTLE_SPECIAL, i, NULL);
            sTriedEvolving |= gBitTable[i];

            if (species == SPECIES_NONE && (gLeveledUpInBattle & gBitTable[i]))
            {
                gLeveledUpInBattle &= ~(gBitTable[i]);
                species = GetEvolutionTargetSpecies(&gPlayerParty[i], EVO_MODE_BATTLE_ONLY, gLeveledUpInBattle, NULL);
            }

            if (species != SPECIES_NONE)
            {
                gBattleMainFunc = WaitForEvoSceneToFinish;
                EvolutionScene(&gPlayerParty[i], species, 0x81, i);
                return;
            }
        }
    }
    sTriedEvolving = 0;
    gLeveledUpInBattle = 0;
    gBattleMainFunc = ReturnFromBattleToOverworld;
}

static void WaitForEvoSceneToFinish(void)
{
    if (gMain.callback2 == BattleMainCB2)
        gBattleMainFunc = TryEvolvePokemon;
}

static void ReturnFromBattleToOverworld(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        RandomlyGivePartyPokerus(gPlayerParty);
        PartySpreadPokerus(gPlayerParty);
    }
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK) || !gReceivedRemoteLinkPlayers)
    {
        gSpecialVar_Result = gBattleOutcome;
        gMain.inBattle = FALSE;
        gMain.callback1 = gPreBattleCallback1;
        if (gBattleTypeFlags & BATTLE_TYPE_ROAMER)
        {
            UpdateRoamerHPStatus(&gEnemyParty[0]);
#ifdef BUGFIX
            if ((gBattleOutcome == B_OUTCOME_WON) || gBattleOutcome == B_OUTCOME_CAUGHT)
#else
            if ((gBattleOutcome & B_OUTCOME_WON) || gBattleOutcome == B_OUTCOME_CAUGHT) // Bug: When Roar is used by roamer, gBattleOutcome is B_OUTCOME_PLAYER_TELEPORTED (5).
#endif                                                                                  // & with B_OUTCOME_WON (1) will return TRUE and deactivates the roamer.
                SetRoamerInactive();
        }
        m4aSongNumStop(SE_LOW_HEALTH);
        SetMainCallback2(gMain.savedCallback);
    }
}

void RunBattleScriptCommands_PopCallbacksStack(void)
{
    if (gCurrentActionFuncId == B_ACTION_TRY_FINISH || gCurrentActionFuncId == B_ACTION_FINISHED)
    {
        if (gBattleResources->battleCallbackStack->size != 0)
            gBattleResources->battleCallbackStack->size--;
        gBattleMainFunc = gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size];
    }
    else
    {
        if (gBattleControllerExecFlags == 0)
            gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
    }
}

void RunBattleScriptCommands(void)
{
    if (gBattleControllerExecFlags == 0)
        gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
}

static void HandleAction_UseMove(void)
{
    u32 battler, i, side, moveType, var = 4;
    u16 moveTarget;

    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    if (gBattleStruct->absentBattlerFlags & gBitTable[gBattlerAttacker] || !IsBattlerAlive(gBattlerAttacker))
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }
    gIsCriticalHit = FALSE;
    gBattleStruct->atkCancellerTracker = 0;
    gMoveResultFlags = 0;
    gMultiHitCounter = 0;
    gBattleScripting.savedDmg = 0;
    gBattleCommunication[MISS_TYPE] = 0;
    gBattleScripting.savedMoveEffect = 0;
    gCurrMovePos = gChosenMovePos = *(gBattleStruct->chosenMovePositions + gBattlerAttacker);
    // choose move
    if (gProtectStructs[gBattlerAttacker].noValidMoves)
    {
        gProtectStructs[gBattlerAttacker].noValidMoves = 0;
        gCurrentMove = gChosenMove = MOVE_STRUGGLE;
        gHitMarker |= HITMARKER_NO_PPDEDUCT;
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(MOVE_STRUGGLE, NO_TARGET_OVERRIDE);
    }
    else if (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS || gBattleMons[gBattlerAttacker].status2 & STATUS2_RECHARGE)
    {
        gCurrentMove = gChosenMove = gLockedMoves[gBattlerAttacker];
    }
    // encore forces you to use the same move
    else if (!gBattleStruct->zmove.active && gDisableStructs[gBattlerAttacker].encoredMove != MOVE_NONE
          && gDisableStructs[gBattlerAttacker].encoredMove == gBattleMons[gBattlerAttacker].moves[gDisableStructs[gBattlerAttacker].encoredMovePos])
    {
        gCurrentMove = gChosenMove = gDisableStructs[gBattlerAttacker].encoredMove;
        gCurrMovePos = gChosenMovePos = gDisableStructs[gBattlerAttacker].encoredMovePos;
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
    }
    // check if the encored move wasn't overwritten
    else if (!gBattleStruct->zmove.active && gDisableStructs[gBattlerAttacker].encoredMove != MOVE_NONE
          && gDisableStructs[gBattlerAttacker].encoredMove != gBattleMons[gBattlerAttacker].moves[gDisableStructs[gBattlerAttacker].encoredMovePos])
    {
        gCurrMovePos = gChosenMovePos = gDisableStructs[gBattlerAttacker].encoredMovePos;
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        gDisableStructs[gBattlerAttacker].encoredMove = MOVE_NONE;
        gDisableStructs[gBattlerAttacker].encoredMovePos = 0;
        gDisableStructs[gBattlerAttacker].encoreTimer = 0;
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
    }
    else if (gBattleMons[gBattlerAttacker].moves[gCurrMovePos] != gChosenMoveByBattler[gBattlerAttacker])
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
    }
    else
    {
        
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
    }

    // check z move used
    if (gBattleStruct->zmove.toBeUsed[gBattlerAttacker] != MOVE_NONE && !IS_MOVE_STATUS(gCurrentMove))
    {
        gCurrentMove = gBattleStruct->zmove.toBeUsed[gBattlerAttacker];
    }

    if (gBattleMons[gBattlerAttacker].hp != 0)
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            gBattleResults.lastUsedMovePlayer = gCurrentMove;
        else
            gBattleResults.lastUsedMoveOpponent = gCurrentMove;
    }

    // Set dynamic move type.
    SetTypeBeforeUsingMove(gChosenMove, gBattlerAttacker);
    GET_MOVE_TYPE(gChosenMove, moveType);

    // check max move used
    if (gBattleStruct->dynamax.usingMaxMove[gBattlerAttacker])
    {
        gCurrentMove = gChosenMove = GetMaxMove(gBattlerAttacker, gCurrentMove);
        gBattleStruct->dynamax.activeCategory = gBattleStruct->dynamax.categories[gBattlerAttacker];
    }

    moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);

    // choose target
    side = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
    if (IsAffectedByFollowMe(gBattlerAttacker, side, gCurrentMove)
        && moveTarget == MOVE_TARGET_SELECTED
        && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gSideTimers[side].followmeTarget))
    {
        gBattleStruct->moveTarget[gBattlerAttacker] = gBattlerTarget = gSideTimers[side].followmeTarget; // follow me moxie fix
    }
    else if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
          && gSideTimers[side].followmeTimer == 0
        && (gMovesInfo[gCurrentMove].power != 0 || (moveTarget != MOVE_TARGET_USER && moveTarget != MOVE_TARGET_ALL_BATTLERS))
          && ((GetBattlerAbility(*(gBattleStruct->moveTarget + gBattlerAttacker)) != ABILITY_LIGHTNING_ROD && moveType == TYPE_ELECTRIC)
            || (GetBattlerAbility(*(gBattleStruct->moveTarget + gBattlerAttacker)) != ABILITY_STORM_DRAIN && moveType == TYPE_WATER)))
    {
        side = GetBattlerSide(gBattlerAttacker);
        for (battler = 0; battler < gBattlersCount; battler++)
        {
            if (side != GetBattlerSide(battler)
                && *(gBattleStruct->moveTarget + gBattlerAttacker) != battler
                && ((GetBattlerAbility(battler) == ABILITY_LIGHTNING_ROD && moveType == TYPE_ELECTRIC)
                 || (GetBattlerAbility(battler) == ABILITY_STORM_DRAIN && moveType == TYPE_WATER))
                && GetBattlerTurnOrderNum(battler) < var
                && gMovesInfo[gCurrentMove].effect != EFFECT_SNIPE_SHOT
                && gMovesInfo[gCurrentMove].effect != EFFECT_PLEDGE
                && (GetBattlerAbility(gBattlerAttacker) != ABILITY_PROPELLER_TAIL
                 || GetBattlerAbility(gBattlerAttacker) != ABILITY_STALWART))
            {
                var = GetBattlerTurnOrderNum(battler);
            }
        }
        if (var == 4)
        {
            if (moveTarget & MOVE_TARGET_RANDOM)
            {
                if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                {
                    if (Random() & 1)
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
                    else
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
                }
                else
                {
                    if (Random() & 1)
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
                    else
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
                }
            }
            else if (moveTarget & MOVE_TARGET_FOES_AND_ALLY)
            {
                for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount; gBattlerTarget++)
                {
                    if (gBattlerTarget == gBattlerAttacker)
                        continue;
                    if (IsBattlerAlive(gBattlerTarget))
                        break;
                }
            }
            else
            {
                gBattlerTarget = *(gBattleStruct->moveTarget + gBattlerAttacker);
            }
            if (!IsBattlerAlive(gBattlerTarget))
            {
                if (GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
                {
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
                }
                else
                {
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_SIDE);
                    if (!IsBattlerAlive(gBattlerTarget))
                        gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
                }
            }
        }
        else
        {
            u16 battlerAbility;
            battler = gBattlerByTurnOrder[var];
            battlerAbility = GetBattlerAbility(battler);

            RecordAbilityBattle(battler, gBattleMons[battler].ability);
            if (battlerAbility == ABILITY_LIGHTNING_ROD && gCurrentMove != MOVE_TEATIME)
                gSpecialStatuses[battler].lightningRodRedirected = TRUE;
            else if (battlerAbility == ABILITY_STORM_DRAIN)
                gSpecialStatuses[battler].stormDrainRedirected = TRUE;
            gBattlerTarget = battler;
        }
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE
          && moveTarget & MOVE_TARGET_RANDOM)
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        {
            if (Random() & 1)
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            else
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        }
        else
        {
            if (Random() & 1)
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            else
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        }
        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget]
         && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
            gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
    }
    else if (moveTarget == MOVE_TARGET_ALLY)
    {
        if (IsBattlerAlive(BATTLE_PARTNER(gBattlerAttacker)))
            gBattlerTarget = BATTLE_PARTNER(gBattlerAttacker);
        else
            gBattlerTarget = gBattlerAttacker;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE
          && moveTarget == MOVE_TARGET_FOES_AND_ALLY)
    {
        for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount; gBattlerTarget++)
        {
            if (gBattlerTarget == gBattlerAttacker)
                continue;
            if (IsBattlerAlive(gBattlerTarget))
                break;
        }
    }
    else
    {
        gBattlerTarget = *(gBattleStruct->moveTarget + gBattlerAttacker);
        if (!IsBattlerAlive(gBattlerTarget))
        {
            if (GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
            {
                gBattlerTarget = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gBattlerTarget)));
            }
            else
            {
                gBattlerTarget = GetBattlerAtPosition(BATTLE_OPPOSITE(GetBattlerPosition(gBattlerAttacker)));
                if (!IsBattlerAlive(gBattlerTarget))
                    gBattlerTarget = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gBattlerTarget)));
            }
        }
    }
    // Edge case: moves targeting the ally fail after a successful Ally Switch.
    if (moveTarget == MOVE_TARGET_ALLY && gProtectStructs[BATTLE_PARTNER(gBattlerAttacker)].usedAllySwitch)
    {
        gBattlescriptCurrInstr = BattleScript_FailedFromAtkCanceler;
    }
    else
    {
        gBattlescriptCurrInstr = GET_MOVE_BATTLESCRIPT(gCurrentMove);
    }

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gBattleStruct->hpBefore[i] = gBattleMons[i].hp;
        gSpecialStatuses[i].emergencyExited = FALSE;
    }

    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

static void HandleAction_Switch(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gActionSelectionCursor[gBattlerAttacker] = 0;
    gMoveSelectionCursor[gBattlerAttacker] = 0;
    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, *(gBattleStruct->battlerPartyIndexes + gBattlerAttacker));
    gBattleScripting.battler = gBattlerAttacker;
    gBattlescriptCurrInstr = BattleScript_ActionSwitch;
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
    if (gBattleResults.playerSwitchesCounter < 255)
        ++gBattleResults.playerSwitchesCounter;

    TryBattleFormChange(gBattlerAttacker, FORM_CHANGE_BATTLE_SWITCH);
}

void HandleAction_UseItem(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    ClearVariousBattlerFlags(gBattlerAttacker);

    gLastUsedItem = gBattleResources->bufferB[gBattlerAttacker][1] | (gBattleResources->bufferB[gBattlerAttacker][2] << 8);
    gBattlescriptCurrInstr = gBattlescriptsForUsingItem[ItemId_GetBattleUsage(gLastUsedItem) - 1];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

bool32 TryRunFromBattle(u32 battler)
{
    bool32 effect = FALSE;
    u8 holdEffect;
    u8 speedVar;

    if (gBattleMons[battler].item == ITEM_ENIGMA_BERRY_E_READER)
        holdEffect = gEnigmaBerries[battler].holdEffect;
    else
        holdEffect = ItemId_GetHoldEffect(gBattleMons[battler].item);
    gPotentialItemEffectBattler = battler;
    if (holdEffect == HOLD_EFFECT_CAN_ALWAYS_RUN)
    {
        gLastUsedItem = gBattleMons[battler].item;
        gProtectStructs[battler].fleeType = FLEE_ITEM;
        effect++;
    }
    else if (B_GHOSTS_ESCAPE >= GEN_6 && IS_BATTLER_OF_TYPE(battler, TYPE_GHOST))
    {
        effect++;
    }
    else if (GetBattlerAbility(battler) == ABILITY_RUN_AWAY)
    {
        gLastUsedAbility = ABILITY_RUN_AWAY;
        gProtectStructs[battler].fleeType = FLEE_ABILITY;
        effect++;
    }
    else if (IS_BATTLE_TYPE_GHOST_WITHOUT_SCOPE(gBattleTypeFlags))
    {
        if (GetBattlerSide(battler) == B_SIDE_PLAYER)
            effect++;
    }
    else
    {
        u8 runningFromBattler = BATTLE_OPPOSITE(battler);
        if (!IsBattlerAlive(runningFromBattler))
            runningFromBattler |= BIT_FLANK;
        if (gBattleMons[battler].speed < gBattleMons[runningFromBattler].speed)
        {
            speedVar = (gBattleMons[battler].speed * 128) / (gBattleMons[runningFromBattler].speed) + (gBattleStruct->runTries * 30);
            if (speedVar > (Random() & 0xFF))
                effect++;
        }
        else // same speed or faster
        {
            effect++;
        }


        ++gBattleStruct->runTries;
    }
    if (effect != 0)
    {
        gCurrentTurnActionNumber = gBattlersCount;
        gBattleOutcome = B_OUTCOME_RAN;
    }
    return effect;
}

static void HandleAction_Run(void)
{
    s32 i;

    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gCurrentTurnActionNumber = gBattlersCount;
        for (i = 0; i < gBattlersCount; i++)
        {
            if (GetBattlerSide(i) == B_SIDE_PLAYER)
            {
                if (gChosenActionByBattler[i] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_LOST;
            }
            else
            {
                if (gChosenActionByBattler[i] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_WON;
            }
        }
        gBattleOutcome |= B_OUTCOME_LINK_BATTLE_RAN;
    }
    else
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        {
            if (!TryRunFromBattle(gBattlerAttacker)) // failed to run away
            {
                ClearVariousBattlerFlags(gBattlerAttacker);
                gBattleCommunication[MULTISTRING_CHOOSER] = 3;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
        }
        else
        {
            if (!CanBattlerEscape(gBattlerAttacker))
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = 4;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
            else
            {
                gCurrentTurnActionNumber = gBattlersCount;
                gBattleOutcome = B_OUTCOME_MON_FLED;
            }
        }
    }
}

static void HandleAction_WatchesCarefully(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    if (gBattleStruct->safariRockThrowCounter != 0)
    {
        --gBattleStruct->safariRockThrowCounter;
        if (gBattleStruct->safariRockThrowCounter == 0)
        {
            *(&gBattleStruct->safariCatchFactor) = gSpeciesInfo[GetMonData(gEnemyParty, MON_DATA_SPECIES)].catchRate * 100 / 1275;
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_WATCHING;
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_ANGRY;
        }
    }
    else
    {
        if (gBattleStruct->safariBaitThrowCounter != 0)
        {
            --gBattleStruct->safariBaitThrowCounter;
            if (gBattleStruct->safariBaitThrowCounter == 0)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_WATCHING;
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_EATING;
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_WATCHING;
        }
    }
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[0];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

static void HandleAction_SafariZoneBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    --gNumSafariBalls;
    gLastUsedItem = ITEM_SAFARI_BALL;
    gBattlescriptCurrInstr = BattleScript_SafariBallThrow;
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

static void HandleAction_ThrowBait(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattleStruct->safariBaitThrowCounter += Random() % 5 + 2;
    if (gBattleStruct->safariBaitThrowCounter > 6)
        gBattleStruct->safariBaitThrowCounter = 6;
    gBattleStruct->safariRockThrowCounter = 0;
    gBattleStruct->safariCatchFactor >>= 1;
    if (gBattleStruct->safariCatchFactor <= 2)
        gBattleStruct->safariCatchFactor = 3;
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[2];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

static void HandleAction_ThrowRock(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattleStruct->safariRockThrowCounter += Random() % 5 + 2;
    if (gBattleStruct->safariRockThrowCounter > 6)
        gBattleStruct->safariRockThrowCounter = 6;
    gBattleStruct->safariBaitThrowCounter = 0;
    gBattleStruct->safariCatchFactor <<= 1;
    if (gBattleStruct->safariCatchFactor > 20)
        gBattleStruct->safariCatchFactor = 20;
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[1];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

static void HandleAction_SafariZoneRun(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    PlaySE(SE_FLEE);
    gCurrentTurnActionNumber = gBattlersCount;
    gBattleOutcome = B_OUTCOME_RAN;
}

static void HandleAction_OldManBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker])
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[3];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
    gActionsByTurnOrder[1] = B_ACTION_FINISHED;
}

static void HandleAction_TryFinish(void)
{
    if (!HandleFaintedMonActions())
    {
        gBattleStruct->faintedActionsState = 0;
        gCurrentActionFuncId = B_ACTION_FINISHED;
    }
}

static void HandleAction_NothingIsFainted(void)
{
    ++gCurrentTurnActionNumber;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
    gHitMarker &= ~(HITMARKER_DESTINYBOND | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_ATTACKSTRING_PRINTED
                    | HITMARKER_NO_PPDEDUCT | HITMARKER_STATUS_ABILITY_EFFECT | HITMARKER_PASSIVE_DAMAGE
                    | HITMARKER_OBEYS | HITMARKER_WAKE_UP_CLEAR | HITMARKER_SYNCHRONISE_EFFECT
                    | HITMARKER_CHARGING | HITMARKER_NEVER_SET);
}

static void HandleAction_ActionFinished(void)
{
    u32 i, j;
    bool32 afterYouActive = gSpecialStatuses[gBattlerByTurnOrder[gCurrentTurnActionNumber + 1]].afterYou;
    *(gBattleStruct->monToSwitchIntoId + gBattlerByTurnOrder[gCurrentTurnActionNumber]) = gSelectedMonPartyId = PARTY_SIZE;
    ++gCurrentTurnActionNumber;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
    SpecialStatusesClear();
    gHitMarker &= ~(HITMARKER_DESTINYBOND | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_ATTACKSTRING_PRINTED
                    | HITMARKER_NO_PPDEDUCT | HITMARKER_STATUS_ABILITY_EFFECT | HITMARKER_PASSIVE_DAMAGE
                    | HITMARKER_OBEYS | HITMARKER_WAKE_UP_CLEAR | HITMARKER_SYNCHRONISE_EFFECT
                    | HITMARKER_CHARGING | HITMARKER_NEVER_SET | HITMARKER_IGNORE_DISGUISE);
    gCurrentMove = MOVE_NONE;
    gBattleMoveDamage = 0;
    gMoveResultFlags = 0;
    gBattleScripting.animTurn = 0;
    gBattleScripting.animTargetsHit = 0;
    gLastLandedMoves[gBattlerAttacker] = 0;
    gLastHitByType[gBattlerAttacker] = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleScripting.moveendState = 0;
    gBattleCommunication[MOVE_EFFECT_BYTE] = 0;
    gBattleCommunication[ACTIONS_CONFIRMED_COUNT] = 0;
    gBattleScripting.multihitMoveEffect = 0;
    gBattleResources->battleScriptsStack->size = 0;
    gBattleStruct->dynamax.usingMaxMove[gBattlerAttacker] = 0;

    if (B_RECALC_TURN_AFTER_ACTIONS >= GEN_8 && !afterYouActive && !gBattleStruct->pledgeMove)
    {
        // i starts at `gCurrentTurnActionNumber` because we don't want to recalculate turn order for mon that have already
        // taken action. It's been previously increased, which we want in order to not recalculate the turn of the mon that just finished its action
        for (i = gCurrentTurnActionNumber; i < gBattlersCount - 1; i++)
        {
            for (j = i + 1; j < gBattlersCount; j++)
            {
                u32 battler1 = gBattlerByTurnOrder[i];
                u32 battler2 = gBattlerByTurnOrder[j];

                if (gProtectStructs[battler1].quash || gProtectStructs[battler2].quash
                    || gProtectStructs[battler1].shellTrap || gProtectStructs[battler2].shellTrap)
                    continue;

                // We recalculate order only for action of the same priority. If any action other than switch/move has been taken, they should
                // have been executed before. The only recalculation needed is for moves/switch. Mega evolution is handled in src/battle_main.c/TryChangeOrder
                if((gActionsByTurnOrder[i] == B_ACTION_USE_MOVE && gActionsByTurnOrder[j] == B_ACTION_USE_MOVE))
                {
                    if (GetWhichBattlerFaster(battler1, battler2, FALSE) == -1)
                        SwapTurnOrder(i, j);
                }
                else if ((gActionsByTurnOrder[i] == B_ACTION_SWITCH && gActionsByTurnOrder[j] == B_ACTION_SWITCH))
                {
                    if (GetWhichBattlerFaster(battler1, battler2, TRUE) == -1) // If the actions chosen are switching, we recalc order but ignoring the moves
                        SwapTurnOrder(i, j);
                }
            }
        }
    }
}

void SetTypeBeforeUsingMove(u32 move, u32 battlerAtk)
{
    u32 moveType, ateType, attackerAbility;
    u16 holdEffect = GetBattlerHoldEffect(battlerAtk, TRUE);

    if (move == MOVE_STRUGGLE)
        return;

    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->ateBoost[battlerAtk] = 0;
    gSpecialStatuses[battlerAtk].gemBoost = FALSE;

    if (gMovesInfo[move].effect == EFFECT_WEATHER_BALL)
    {
        if (WEATHER_HAS_EFFECT)
        {
            if (gBattleWeather & B_WEATHER_RAIN && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA)
                gBattleStruct->dynamicMoveType = TYPE_WATER | F_DYNAMIC_TYPE_SET;
            else if (gBattleWeather & B_WEATHER_SANDSTORM)
                gBattleStruct->dynamicMoveType = TYPE_ROCK | F_DYNAMIC_TYPE_SET;
            else if (gBattleWeather & B_WEATHER_SUN && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA)
                gBattleStruct->dynamicMoveType = TYPE_FIRE | F_DYNAMIC_TYPE_SET;
            else if (gBattleWeather & (B_WEATHER_HAIL |B_WEATHER_SNOW))
                gBattleStruct->dynamicMoveType = TYPE_ICE | F_DYNAMIC_TYPE_SET;
            else
                gBattleStruct->dynamicMoveType = TYPE_NORMAL | F_DYNAMIC_TYPE_SET;
        }
    }
    else if (gMovesInfo[move].effect == EFFECT_HIDDEN_POWER)
    {
        u8 typeBits  = ((gBattleMons[battlerAtk].hpIV & 1) << 0)
                     | ((gBattleMons[battlerAtk].attackIV & 1) << 1)
                     | ((gBattleMons[battlerAtk].defenseIV & 1) << 2)
                     | ((gBattleMons[battlerAtk].speedIV & 1) << 3)
                     | ((gBattleMons[battlerAtk].spAttackIV & 1) << 4)
                     | ((gBattleMons[battlerAtk].spDefenseIV & 1) << 5);

        // Subtract 4 instead of 1 below because 3 types are excluded (TYPE_NORMAL and TYPE_MYSTERY and TYPE_FAIRY)
        // The final + 1 skips past Normal, and the following conditional skips TYPE_MYSTERY
        gBattleStruct->dynamicMoveType = ((NUMBER_OF_MON_TYPES - 4) * typeBits) / 63 + 1;
        if (gBattleStruct->dynamicMoveType >= TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType++;
        gBattleStruct->dynamicMoveType |= F_DYNAMIC_TYPE_IGNORE_PHYSICALITY | F_DYNAMIC_TYPE_SET;
    }
    else if (gMovesInfo[move].effect == EFFECT_CHANGE_TYPE_ON_ITEM && holdEffect == gMovesInfo[move].argument)
    {
        gBattleStruct->dynamicMoveType = ItemId_GetSecondaryId(gBattleMons[battlerAtk].item) | F_DYNAMIC_TYPE_SET;
    }
    else if (gMovesInfo[move].effect == EFFECT_REVELATION_DANCE)
    {
        if (gBattleMons[battlerAtk].type1 != TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type1 | F_DYNAMIC_TYPE_SET;
        else if (gBattleMons[battlerAtk].type2 != TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type2 | F_DYNAMIC_TYPE_SET;
        else if (gBattleMons[battlerAtk].type3 != TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type3 | F_DYNAMIC_TYPE_SET;
    }
    else if (gMovesInfo[move].effect == EFFECT_RAGING_BULL
            && (gBattleMons[battlerAtk].species == SPECIES_TAUROS_PALDEAN_COMBAT_BREED
             || gBattleMons[battlerAtk].species == SPECIES_TAUROS_PALDEAN_BLAZE_BREED
             || gBattleMons[battlerAtk].species == SPECIES_TAUROS_PALDEAN_AQUA_BREED))
    {
            gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type2 | F_DYNAMIC_TYPE_SET;
    }
    else if (gMovesInfo[move].effect == EFFECT_IVY_CUDGEL
            && (gBattleMons[battlerAtk].species == SPECIES_OGERPON_WELLSPRING_MASK || gBattleMons[battlerAtk].species == SPECIES_OGERPON_WELLSPRING_MASK_TERA
             || gBattleMons[battlerAtk].species == SPECIES_OGERPON_HEARTHFLAME_MASK || gBattleMons[battlerAtk].species == SPECIES_OGERPON_HEARTHFLAME_MASK_TERA
             || gBattleMons[battlerAtk].species == SPECIES_OGERPON_CORNERSTONE_MASK || gBattleMons[battlerAtk].species == SPECIES_OGERPON_CORNERSTONE_MASK_TERA ))
    {
        gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type2 | F_DYNAMIC_TYPE_SET;
    }
    else if (gMovesInfo[move].effect == EFFECT_NATURAL_GIFT)
    {
        if (ItemId_GetPocket(gBattleMons[battlerAtk].item) == POCKET_BERRY_POUCH)
            gBattleStruct->dynamicMoveType = gNaturalGiftTable[ITEM_TO_BERRY(gBattleMons[battlerAtk].item)].type;
    }
    else if (gMovesInfo[move].effect == EFFECT_TERRAIN_PULSE)
    {
        if (IsBattlerTerrainAffected(battlerAtk, STATUS_FIELD_TERRAIN_ANY))
        {
            if (gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_ELECTRIC | F_DYNAMIC_TYPE_SET;
            else if (gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_GRASS | F_DYNAMIC_TYPE_SET;
            else if (gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_FAIRY | F_DYNAMIC_TYPE_SET;
            else if (gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_PSYCHIC | F_DYNAMIC_TYPE_SET;
            else //failsafe
                gBattleStruct->dynamicMoveType = TYPE_NORMAL | F_DYNAMIC_TYPE_SET;
        }
    }

    attackerAbility = GetBattlerAbility(battlerAtk);

    if (gMovesInfo[move].type == TYPE_NORMAL
             && gMovesInfo[move].effect != EFFECT_HIDDEN_POWER
             && gMovesInfo[move].effect != EFFECT_WEATHER_BALL
             && gMovesInfo[move].effect != EFFECT_CHANGE_TYPE_ON_ITEM
             && gMovesInfo[move].effect != EFFECT_NATURAL_GIFT
             && ((attackerAbility == ABILITY_PIXILATE && (ateType = TYPE_FAIRY))
                 || (attackerAbility == ABILITY_REFRIGERATE && (ateType = TYPE_ICE))
                 || (attackerAbility == ABILITY_AERILATE && (ateType = TYPE_FLYING))
                 || ((attackerAbility == ABILITY_GALVANIZE) && (ateType = TYPE_ELECTRIC))
                )
             )
    {
        gBattleStruct->dynamicMoveType = ateType | F_DYNAMIC_TYPE_SET;
        if (!IsDynamaxed(battlerAtk))
            gBattleStruct->ateBoost[battlerAtk] = 1;
    }
    else if (gMovesInfo[move].type != TYPE_NORMAL
             && gMovesInfo[move].effect != EFFECT_HIDDEN_POWER
             && gMovesInfo[move].effect != EFFECT_WEATHER_BALL
             && attackerAbility == ABILITY_NORMALIZE)
    {
        gBattleStruct->dynamicMoveType = TYPE_NORMAL | F_DYNAMIC_TYPE_SET;
        if (!IsDynamaxed(battlerAtk))
            gBattleStruct->ateBoost[battlerAtk] = 1;
    }
    else if (gMovesInfo[move].soundMove && attackerAbility == ABILITY_LIQUID_VOICE)
    {
        gBattleStruct->dynamicMoveType = TYPE_WATER | F_DYNAMIC_TYPE_SET;
    }
    else if (gMovesInfo[move].effect == EFFECT_AURA_WHEEL && gBattleMons[battlerAtk].species == SPECIES_MORPEKO_HANGRY)
    {
        gBattleStruct->dynamicMoveType = TYPE_DARK | F_DYNAMIC_TYPE_SET;
    }

    GET_MOVE_TYPE(move, moveType);
    if ((gFieldStatuses & STATUS_FIELD_ION_DELUGE && moveType == TYPE_NORMAL)
        || gStatuses4[battlerAtk] & STATUS4_ELECTRIFIED)
        gBattleStruct->dynamicMoveType = TYPE_ELECTRIC | F_DYNAMIC_TYPE_SET;

    // Check if a gem should activate.
    GET_MOVE_TYPE(move, moveType);
    if (holdEffect == HOLD_EFFECT_GEMS
        && moveType == ItemId_GetSecondaryId(gBattleMons[battlerAtk].item))
    {
        gSpecialStatuses[battlerAtk].gemParam = GetBattlerHoldEffectParam(battlerAtk);
        gSpecialStatuses[battlerAtk].gemBoost = TRUE;
    }
}

// special to set a field's totem boost(s)
// inputs:
//  var8000: battler
//  var8001 - var8007: stat changes
void SetTotemBoost(void)
{
    u32 battler = gSpecialVar_0x8000;
    u32 i;

    for (i = 0; i < (NUM_BATTLE_STATS - 1); i++)
    {
        if (*(&gSpecialVar_0x8001 + i))
        {
            gQueuedStatBoosts[battler].stats |= (1 << i);
            gQueuedStatBoosts[battler].statChanges[i] = *(&gSpecialVar_0x8001 + i);
            gQueuedStatBoosts[battler].stats |= 0x80;  // used as a flag for the "totem flared to life" script
        }
    }
}

bool32 IsWildMonSmart(void)
{
#if B_SMART_WILD_AI_FLAG != 0
    return (FlagGet(B_SMART_WILD_AI_FLAG));
#else
    return FALSE;
#endif
}
