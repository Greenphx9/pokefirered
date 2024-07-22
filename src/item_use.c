#include "global.h"
#include "gflib.h"
#include "bag.h"
#include "battle.h"
#include "berry_pouch.h"
#include "berry_powder.h"
#include "bike.h"
#include "coins.h"
#include "event_data.h"
#include "field_effect.h"
#include "field_fadetransition.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "item.h"
#include "item_menu.h"
#include "item_use.h"
#include "itemfinder.h"
#include "mail.h"
#include "menu_helpers.h"
#include "event_object_lock.h"
#include "metatile_behavior.h"
#include "new_menu_helpers.h"
#include "overworld.h"
#include "party_menu.h"
#include "quest_log.h"
#include "region_map.h"
#include "script.h"
#include "strings.h"
#include "task.h"
#include "teachy_tv.h"
#include "tm_case.h"
#include "vs_seeker.h"
#include "constants/sound.h"
#include "constants/items.h"
#include "constants/item_effects.h"
#include "constants/maps.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/field_weather.h"

static EWRAM_DATA void (*sItemUseOnFieldCB)(u8 taskId) = NULL;

// Below is set TRUE by UseRegisteredKeyItemOnField
#define tUsingRegisteredKeyItem  data[3]

static void FieldCB_FadeInFromBlack(void);
static void Task_WaitFadeIn_CallItemUseOnFieldCB(u8 taskId);
static void Task_ItemUse_CloseMessageBoxAndReturnToField(u8 taskId);
static void Task_ItemUseWaitForFade(u8 taskId);
static bool8 FieldCB2_UseItemFromField(void);
static void CB2_CheckMail(void);
static void ItemUseOnFieldCB_Bicycle(u8 taskId);
static bool8 CanFish(void);
static void ItemUseOnFieldCB_Rod(u8 taskId);
static void Task_PlayPokeFlute(u8 taskId);
static void Task_DisplayPokeFluteMessage(u8 taskId);
static void InitTMCaseFromBag(void);
static void Task_InitTMCaseFromField(u8 taskId);
static void InitBerryPouchFromBag(void);
static void Task_InitBerryPouchFromField(u8 taskId);
static void InitBerryPouchFromBattle(void);
static void InitTeachyTvFromBag(void);
static void Task_InitTeachyTvFromField(u8 taskId);
static void Task_UseRepel(u8 taskId);
static void RemoveUsedItem(void);
static void Task_UsedBlackWhiteFlute(u8 taskId);
static void ItemUseOnFieldCB_EscapeRope(u8 taskId);
static void UseTownMapFromBag(void);
static void Task_UseTownMapFromField(u8 taskId);
static void UseFameCheckerFromBag(void);
static void Task_UseFameCheckerFromField(u8 taskId);
static void Task_BattleUse_StatBooster_DelayAndPrint(u8 taskId);
static void Task_BattleUse_StatBooster_WaitButton_ReturnToBattle(u8 taskId);
static void Task_AccessPokemonBoxLink(u8);
static void BootUpSoundTMHM(u8);
static void Task_ShowTMHMContainedMessage(u8);
static void UseTMHMYesNo(u8);
static void UseTMHM(u8);

// unknown unused data.
// It's curiously about the size of an array of values indexed by species (including padding),
// but the arrangement of values is not sensible (e.g., not giving all "old unown" the same value).
static const u8 sUnused[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x04, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x40, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x40, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1f, 0x00, 0xe0, 0x03, 0x00, 0x7c,
    0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void (*const sExitCallbackByItemType[])(void) = {
    [ITEM_TYPE_PARTY_MENU - 1] = CB2_ShowPartyMenuForItemUse,
    [ITEM_TYPE_FIELD      - 1] = CB2_ReturnToField,
    [ITEM_TYPE_UNUSED     - 1] = NULL,
    [ITEM_TYPE_BAG_MENU   - 1] = NULL,
};

static void SetUpItemUseCallback(u8 taskId)
{
    u8 itemType;
    if (gSpecialVar_ItemId == ITEM_ENIGMA_BERRY)
        itemType = gTasks[taskId].data[4] - 1;
    else
        itemType = ItemId_GetType(gSpecialVar_ItemId) - 1;
    if (GetPocketByItemId(gSpecialVar_ItemId) == POCKET_BERRY_POUCH)
    {
        BerryPouch_SetExitCallback(sExitCallbackByItemType[itemType]);
        BerryPouch_StartFadeToExitCallback(taskId);
    }
    else
    {
        ItemMenu_SetExitCallback(sExitCallbackByItemType[itemType]);
        if (itemType == ITEM_TYPE_FIELD - 1)
            Bag_BeginCloseWin0Animation();
        ItemMenu_StartFadeToExitCallback(taskId);
    }
}

static void SetUpItemUseOnFieldCallback(u8 taskId)
{
    if (gTasks[taskId].data[3] != 1)
    {
        gFieldCallback = FieldCB_FadeInFromBlack;
        SetUpItemUseCallback(taskId);
    }
    else
        sItemUseOnFieldCB(taskId);
}

static void FieldCB_FadeInFromBlack(void)
{
    FadeInFromBlack();
    CreateTask(Task_WaitFadeIn_CallItemUseOnFieldCB, 8);
}

static void Task_WaitFadeIn_CallItemUseOnFieldCB(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
        sItemUseOnFieldCB(taskId);
}

static void DisplayItemMessageInCurrentContext(u8 taskId, bool8 inField, u8 fontId, const u8 *str)
{
    StringExpandPlaceholders(gStringVar4, str);
    if (inField == FALSE)
        DisplayItemMessageInBag(taskId, fontId, gStringVar4, Task_ReturnToBagFromContextMenu);
    else
        DisplayItemMessageOnField(taskId, fontId, gStringVar4, Task_ItemUse_CloseMessageBoxAndReturnToField);
}

static void PrintNotTheTimeToUseThat(u8 taskId, bool8 inField)
{
    DisplayItemMessageInCurrentContext(taskId, inField, FONT_MALE, gText_OakForbidsUseOfItemHere);
}

static void Task_ItemUse_CloseMessageBoxAndReturnToField(u8 taskId)
{
    ClearDialogWindowAndFrame(0, 1);
    DestroyTask(taskId);
    ClearPlayerHeldMovementAndUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

u8 CheckIfItemIsTMHMOrEvolutionStone(u16 itemId)
{
    if (ItemId_GetPocket(itemId) == POCKET_TM_CASE)
        return 1;
    else if (ItemId_GetFieldFunc(itemId) == FieldUseFunc_EvoItem)
        return 2;
    else
        return 0;
}

static void SetFieldCallback2ForItemUse(void)
{
    gFieldCallback2 = FieldCB2_UseItemFromField;
}

static bool8 FieldCB2_UseItemFromField(void)
{
    FreezeObjectEvents();
    LockPlayerFieldControls();
    FadeInFromBlack();
    CreateTask(Task_ItemUseWaitForFade, 10);
    gExitStairsMovementDisabled = FALSE;
    return TRUE;
}

static void Task_ItemUseWaitForFade(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        UnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
    }
}

void FieldUseFunc_Mail(u8 taskId)
{
    ItemMenu_SetExitCallback(CB2_CheckMail);
    ItemMenu_StartFadeToExitCallback(taskId);
}

static void CB2_CheckMail(void)
{
    struct Mail mail;

    mail.itemId = gSpecialVar_ItemId;
    ReadMail(&mail, CB2_BagMenuFromStartMenu, FALSE);
}

void FieldUseFunc_Bike(u8 taskId)
{
    s16 x, y;
    u8 behavior;

    PlayerGetDestCoords(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);

    if (FlagGet(FLAG_SYS_ON_CYCLING_ROAD) == TRUE
     || MetatileBehavior_IsVerticalRail(behavior) == TRUE
     || MetatileBehavior_IsHorizontalRail(behavior) == TRUE
     || MetatileBehavior_IsIsolatedVerticalRail(behavior) == TRUE
     || MetatileBehavior_IsIsolatedHorizontalRail(behavior) == TRUE)
        DisplayItemMessageInCurrentContext(taskId, gTasks[taskId].data[3], FONT_NORMAL, gText_CantDismountBike);
    else if (Overworld_IsBikingAllowed() == TRUE && !IsBikingDisallowedByPlayer())
    {
        sItemUseOnFieldCB = ItemUseOnFieldCB_Bicycle;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
        PrintNotTheTimeToUseThat(taskId, gTasks[taskId].data[3]);
}

static void ItemUseOnFieldCB_Bicycle(u8 taskId)
{
    if (!TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE))
        PlaySE(SE_BIKE_BELL);
    GetOnOffBike(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE);
    ClearPlayerHeldMovementAndUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

void FieldUseFunc_Rod(u8 taskId)
{
    if (CanFish() == TRUE)
    {
        sItemUseOnFieldCB = ItemUseOnFieldCB_Rod;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
        PrintNotTheTimeToUseThat(taskId, gTasks[taskId].data[3]);
}

static bool8 CanFish(void)
{
    s16 x, y;
    u16 behavior;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);

    if (MetatileBehavior_IsWaterfall(behavior))
        return FALSE;
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_UNDERWATER))
        return FALSE;
    if (!TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
    {
        if (IsPlayerFacingSurfableFishableWater())
            return TRUE;
    }
    else
    {
        if (MetatileBehavior_IsSurfable(behavior) && MapGridGetCollisionAt(x, y) == 0)
            return TRUE;
        if (MetatileBehavior_IsBridge(behavior) == TRUE)
            return TRUE;
    }
    return FALSE;
}

static void ItemUseOnFieldCB_Rod(u8 taskId)
{
    StartFishing(ItemId_GetSecondaryId(gSpecialVar_ItemId));
    DestroyTask(taskId);
}

void ItemUseOutOfBattle_Itemfinder(u8 taskId)
{
    IncrementGameStat(GAME_STAT_USED_ITEMFINDER);
    sItemUseOnFieldCB = ItemUseOnFieldCB_Itemfinder;
    SetUpItemUseOnFieldCallback(taskId);
}

void FieldUseFunc_CoinCase(u8 taskId)
{
    ConvertIntToDecimalStringN(gStringVar1, GetCoins(), STR_CONV_MODE_LEFT_ALIGN, 4);
    StringExpandPlaceholders(gStringVar4, gText_CoinCase);
    ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, 0xFFFF);
    if (gTasks[taskId].data[3] == 0)
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gStringVar4, Task_ReturnToBagFromContextMenu);
    else
        DisplayItemMessageOnField(taskId, FONT_NORMAL, gStringVar4, Task_ItemUse_CloseMessageBoxAndReturnToField);
}

void FieldUseFunc_PowderJar(u8 taskId)
{
    ConvertIntToDecimalStringN(gStringVar1, GetBerryPowder(), STR_CONV_MODE_LEFT_ALIGN, 5);
    StringExpandPlaceholders(gStringVar4, gText_PowderQty);
    ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, 0xFFFF);
    if (gTasks[taskId].data[3] == 0)
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gStringVar4, Task_ReturnToBagFromContextMenu);
    else
        DisplayItemMessageOnField(taskId, FONT_NORMAL, gStringVar4, Task_ItemUse_CloseMessageBoxAndReturnToField);
}

void FieldUseFunc_PokeFlute(u8 taskId)
{
    bool8 wokeSomeoneUp = FALSE;
    u8 i;

    for (i = 0; i < CalculatePlayerPartyCount(); i++)
    {
        if (!ExecuteTableBasedItemEffect(&gPlayerParty[i], ITEM_AWAKENING, i, 0))
            wokeSomeoneUp = TRUE;
    }

    if (wokeSomeoneUp)
    {
        ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, 0xFFFF);
        if (gTasks[taskId].data[3] == 0)
            DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_PlayedPokeFlute, Task_PlayPokeFlute);
        else
            DisplayItemMessageOnField(taskId, FONT_NORMAL, gText_PlayedPokeFlute, Task_PlayPokeFlute);
    }
    else
    {
        if (gTasks[taskId].data[3] == 0)
            DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_PlayedPokeFluteCatchy, Task_ReturnToBagFromContextMenu);
        else
            DisplayItemMessageOnField(taskId, FONT_NORMAL, gText_PlayedPokeFluteCatchy, Task_ItemUse_CloseMessageBoxAndReturnToField);
    }
}

static void Task_PlayPokeFlute(u8 taskId)
{
    PlayFanfareByFanfareNum(FANFARE_POKE_FLUTE);
    gTasks[taskId].func = Task_DisplayPokeFluteMessage;
}

static void Task_DisplayPokeFluteMessage(u8 taskId)
{
    if (WaitFanfare(FALSE))
    {
        if (gTasks[taskId].data[3] == 0)
            DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_PokeFluteAwakenedMon, Task_ReturnToBagFromContextMenu);
        else
            DisplayItemMessageOnField(taskId, FONT_NORMAL, gText_PokeFluteAwakenedMon, Task_ItemUse_CloseMessageBoxAndReturnToField);
    }
}

static void DoSetUpItemUseCallback(u8 taskId)
{
    SetUpItemUseCallback(taskId);
}

void FieldUseFunc_Medicine(u8 taskId)
{
    gItemUseCB = ItemUseCB_Medicine;
    DoSetUpItemUseCallback(taskId);
}

void FieldUseFunc_Ether(u8 taskId)
{
    gItemUseCB = ItemUseCB_TryRestorePP;
    DoSetUpItemUseCallback(taskId);
}

void FieldUseFunc_PpUp(u8 taskId)
{
    gItemUseCB = ItemUseCB_PPUp;
    DoSetUpItemUseCallback(taskId);
}

void FieldUseFunc_RareCandy(u8 taskId)
{
    gItemUseCB = ItemUseCB_RareCandy;
    DoSetUpItemUseCallback(taskId);
}

void FieldUseFunc_EvoItem(u8 taskId)
{
    gItemUseCB = ItemUseCB_EvolutionStone;
    DoSetUpItemUseCallback(taskId);
}

void FieldUseFunc_SacredAsh(u8 taskId)
{
    gItemUseCB = ItemUseCB_SacredAsh;
    SetUpItemUseCallback(taskId);
}

void FieldUseFunc_TmCase(u8 taskId)
{
    if (gTasks[taskId].data[3] == 0)
    {
        ItemMenu_SetExitCallback(InitTMCaseFromBag);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else
    {
        StopPokemonLeagueLightingEffectTask();
        FadeScreen(FADE_TO_BLACK, 0);
        gTasks[taskId].func = Task_InitTMCaseFromField;
    }
}

static void InitTMCaseFromBag(void)
{
    InitTMCase(TMCASE_FIELD, CB2_BagMenuFromStartMenu, FALSE);
}

static void Task_InitTMCaseFromField(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        SetFieldCallback2ForItemUse();
        InitTMCase(TMCASE_FIELD, CB2_ReturnToField, TRUE);
        DestroyTask(taskId);
    }
}

void FieldUseFunc_BerryPouch(u8 taskId)
{
    if (gTasks[taskId].data[3] == 0)
    {
        ItemMenu_SetExitCallback(InitBerryPouchFromBag);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else
    {
        StopPokemonLeagueLightingEffectTask();
        FadeScreen(FADE_TO_BLACK, 0);
        gTasks[taskId].func = Task_InitBerryPouchFromField;
    }
}

static void InitBerryPouchFromBag(void)
{
    InitBerryPouch(BERRYPOUCH_FROMFIELD, CB2_BagMenuFromStartMenu, 0);
}

static void Task_InitBerryPouchFromField(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        SetFieldCallback2ForItemUse();
        InitBerryPouch(BERRYPOUCH_FROMFIELD, CB2_ReturnToField, 1);
        DestroyTask(taskId);
    }
}

void BattleUseFunc_BerryPouch(u8 taskId)
{
    ItemMenu_SetExitCallback(InitBerryPouchFromBattle);
    ItemMenu_StartFadeToExitCallback(taskId);
}

static void InitBerryPouchFromBattle(void)
{
    InitBerryPouch(BERRYPOUCH_FROMBATTLE, CB2_BagMenuFromBattle, 0);
}

void FieldUseFunc_TeachyTv(u8 taskId)
{
    ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, 0xFFFF);
    if (gTasks[taskId].data[3] == 0)
    {
        ItemMenu_SetExitCallback(InitTeachyTvFromBag);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else
    {
        StopPokemonLeagueLightingEffectTask();
        FadeScreen(FADE_TO_BLACK, 0);
        gTasks[taskId].func = Task_InitTeachyTvFromField;
    }
}

static void InitTeachyTvFromBag(void)
{
    InitTeachyTvController(0, CB2_BagMenuFromStartMenu);
}

static void Task_InitTeachyTvFromField(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        SetFieldCallback2ForItemUse();
        InitTeachyTvController(0, CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

void FieldUseFunc_Repel(u8 taskId)
{
    if (VarGet(VAR_REPEL_STEP_COUNT) == 0)
    {
        PlaySE(SE_REPEL);
        gTasks[taskId].func = Task_UseRepel;
    }
    else
        // An earlier repel is still in effect
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_RepelEffectsLingered, Task_ReturnToBagFromContextMenu);
}

static void Task_UseRepel(u8 taskId)
{
    if (!IsSEPlaying())
    {
        ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, 0xFFFF);
        VarSet(VAR_REPEL_STEP_COUNT, ItemId_GetHoldEffectParam(gSpecialVar_ItemId));
        RemoveUsedItem();
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gStringVar4, Task_ReturnToBagFromContextMenu);
    }
}

static void RemoveUsedItem(void)
{
    RemoveBagItem(gSpecialVar_ItemId, 1);
    Pocket_CalculateNItemsAndMaxShowed(ItemId_GetPocket(gSpecialVar_ItemId));
    PocketCalculateInitialCursorPosAndItemsAbove(ItemId_GetPocket(gSpecialVar_ItemId));
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gText_PlayerUsedVar2);
}

void FieldUseFunc_BlackWhiteFlute(u8 taskId)
{
    ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, 0xFFFF);
    if (gSpecialVar_ItemId == ITEM_WHITE_FLUTE)
    {
        FlagSet(FLAG_SYS_WHITE_FLUTE_ACTIVE);
        FlagClear(FLAG_SYS_BLACK_FLUTE_ACTIVE);
        CopyItemName(gSpecialVar_ItemId, gStringVar2);
        StringExpandPlaceholders(gStringVar4, gText_UsedVar2WildLured);
        gTasks[taskId].func = Task_UsedBlackWhiteFlute;
        gTasks[taskId].data[8] = 0;
    }
    else if (gSpecialVar_ItemId == ITEM_BLACK_FLUTE)
    {
        FlagSet(FLAG_SYS_BLACK_FLUTE_ACTIVE);
        FlagClear(FLAG_SYS_WHITE_FLUTE_ACTIVE);
        CopyItemName(gSpecialVar_ItemId, gStringVar2);
        StringExpandPlaceholders(gStringVar4, gText_UsedVar2WildRepelled);
        gTasks[taskId].func = Task_UsedBlackWhiteFlute;
        gTasks[taskId].data[8] = 0;
    }
}

static void Task_UsedBlackWhiteFlute(u8 taskId)
{
    if (++gTasks[taskId].data[8] > 7)
    {
        PlaySE(SE_GLASS_FLUTE);
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gStringVar4, Task_ReturnToBagFromContextMenu);
    }
}

bool8 CanUseEscapeRopeOnCurrMap(void)
{
    if (gMapHeader.allowEscaping)
        return TRUE;
    else
        return FALSE;
}

void ItemUseOutOfBattle_EscapeRope(u8 taskId)
{
    if (CanUseEscapeRopeOnCurrMap() == TRUE)
    {
        ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, gMapHeader.regionMapSectionId);
        sItemUseOnFieldCB = ItemUseOnFieldCB_EscapeRope;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
        PrintNotTheTimeToUseThat(taskId, gTasks[taskId].data[3]);
}

static void ItemUseOnFieldCB_EscapeRope(u8 taskId)
{
    Overworld_ResetStateAfterDigEscRope();
    RemoveUsedItem();
    gTasks[taskId].data[0] = 0;
    DisplayItemMessageOnField(taskId, FONT_NORMAL, gStringVar4, Task_UseDigEscapeRopeOnField);
}

void Task_UseDigEscapeRopeOnField(u8 taskId)
{
    ResetInitialPlayerAvatarState();
    StartEscapeRopeFieldEffect();
    DestroyTask(taskId);
}

void FieldUseFunc_TownMap(u8 taskId)
{
    if (gTasks[taskId].data[3] == 0)
    {
        ItemMenu_SetExitCallback(UseTownMapFromBag);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else
    {
        StopPokemonLeagueLightingEffectTask();
        FadeScreen(FADE_TO_BLACK, 0);
        gTasks[taskId].func = Task_UseTownMapFromField;
    }
}

static void UseTownMapFromBag(void)
{
    InitRegionMapWithExitCB(REGIONMAP_TYPE_NORMAL, CB2_BagMenuFromStartMenu);
}

static void Task_UseTownMapFromField(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        SetFieldCallback2ForItemUse();
        InitRegionMapWithExitCB(REGIONMAP_TYPE_NORMAL, CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

void FieldUseFunc_FameChecker(u8 taskId)
{
    ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, NULL, gSpecialVar_ItemId, 0xFFFF);
    if (gTasks[taskId].data[3] == 0)
    {
        ItemMenu_SetExitCallback(UseFameCheckerFromBag);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else
    {
        StopPokemonLeagueLightingEffectTask();
        FadeScreen(FADE_TO_BLACK, 0);
        gTasks[taskId].func = Task_UseFameCheckerFromField;
    }
}

static void UseFameCheckerFromBag(void)
{
    UseFameChecker(CB2_BagMenuFromStartMenu);
}

static void Task_UseFameCheckerFromField(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        SetFieldCallback2ForItemUse();
        UseFameChecker(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

void FieldUseFunc_VsSeeker(u8 taskId)
{
    if ((gMapHeader.mapType != MAP_TYPE_ROUTE
      && gMapHeader.mapType != MAP_TYPE_TOWN
      && gMapHeader.mapType != MAP_TYPE_CITY)
     || (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(VIRIDIAN_FOREST)
      && (gSaveBlock1Ptr->location.mapNum == MAP_NUM(VIRIDIAN_FOREST)
       || gSaveBlock1Ptr->location.mapNum == MAP_NUM(MT_EMBER_EXTERIOR)
       || gSaveBlock1Ptr->location.mapNum == MAP_NUM(THREE_ISLAND_BERRY_FOREST)
       || gSaveBlock1Ptr->location.mapNum == MAP_NUM(SIX_ISLAND_PATTERN_BUSH))))
    {
        PrintNotTheTimeToUseThat(taskId, gTasks[taskId].data[3]);
    }
    else
    {
        sItemUseOnFieldCB = Task_VsSeeker_0;
        SetUpItemUseOnFieldCallback(taskId);
    }
}

void Task_ItemUse_CloseMessageBoxAndReturnToField_VsSeeker(u8 taskId)
{
    Task_ItemUse_CloseMessageBoxAndReturnToField(taskId);
}

void BattleUseFunc_PokeBallEtc(u8 taskId)
{
    if (!IsPlayerPartyAndPokemonStorageFull())
    {
        RemoveBagItem(gSpecialVar_ItemId, 1);
        Bag_BeginCloseWin0Animation();
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_BoxFull, Task_ReturnToBagFromContextMenu);
}

void BattleUseFunc_PokeFlute(u8 taskId)
{
    Bag_BeginCloseWin0Animation();
    ItemMenu_StartFadeToExitCallback(taskId);
}

void BattleUseFunc_StatBooster(u8 taskId)
{
    if (ExecuteTableBasedItemEffect(&gPlayerParty[gBattlerPartyIndexes[gBattlerInMenuId]], gSpecialVar_ItemId, gBattlerPartyIndexes[gBattlerInMenuId], 0))
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_WontHaveEffect, Task_ReturnToBagFromContextMenu);
    else
    {
        gTasks[taskId].data[8] = 0;
        gTasks[taskId].func = Task_BattleUse_StatBooster_DelayAndPrint;
    }
}

static void Task_BattleUse_StatBooster_DelayAndPrint(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (++data[8] > 7)
    {
        u16 itemId = gSpecialVar_ItemId;
        PlaySE(SE_USE_ITEM);
        RemoveBagItem(itemId, 1);
        DisplayItemMessageInBag(taskId, FONT_NORMAL, Battle_PrintStatBoosterEffectMessage(itemId), Task_BattleUse_StatBooster_WaitButton_ReturnToBattle);
    }
}

static void Task_BattleUse_StatBooster_WaitButton_ReturnToBattle(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        Bag_BeginCloseWin0Animation();
        ItemMenu_StartFadeToExitCallback(taskId);
    }
}

static void ItemUse_SwitchToPartyMenuInBattle(u8 taskId)
{
    if (GetPocketByItemId(gSpecialVar_ItemId) == POCKET_BERRY_POUCH)
    {
        BerryPouch_SetExitCallback(EnterPartyFromItemMenuInBattle);
        BerryPouch_StartFadeToExitCallback(taskId);
    }
    else
    {
        ItemMenu_SetExitCallback(EnterPartyFromItemMenuInBattle);
        ItemMenu_StartFadeToExitCallback(taskId);
    }
}

void BattleUseFunc_Medicine(u8 taskId)
{
    gItemUseCB = ItemUseCB_MedicineStep;
    ItemUse_SwitchToPartyMenuInBattle(taskId);
}

// Unused. Sacred Ash cannot be used in battle
static void BattleUseFunc_SacredAsh(u8 taskId)
{
    gItemUseCB = ItemUseCB_SacredAsh;
    ItemUse_SwitchToPartyMenuInBattle(taskId);
}

void BattleUseFunc_Ether(u8 taskId)
{
    gItemUseCB = ItemUseCB_TryRestorePP;
    ItemUse_SwitchToPartyMenuInBattle(taskId);
}

void BattleUseFunc_PokeDoll(u8 taskId)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
    {
        RemoveUsedItem();
        ItemUse_SetQuestLogEvent(QL_EVENT_USED_ITEM, 0, gSpecialVar_ItemId, 0xFFFF);
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gStringVar4, ItemMenu_StartFadeToExitCallback);
    }
    else
        PrintNotTheTimeToUseThat(taskId, 0);
}

void ItemUseOutOfBattle_EnigmaBerry(u8 taskId)
{
    switch (GetItemEffectType(gSpecialVar_ItemId))
    {
    case ITEM_EFFECT_HEAL_HP:
    case ITEM_EFFECT_CURE_POISON:
    case ITEM_EFFECT_CURE_SLEEP:
    case ITEM_EFFECT_CURE_BURN:
    case ITEM_EFFECT_CURE_FREEZE_FROSTBITE:
    case ITEM_EFFECT_CURE_PARALYSIS:
    case ITEM_EFFECT_CURE_ALL_STATUS:
    case ITEM_EFFECT_ATK_EV:
    case ITEM_EFFECT_HP_EV:
    case ITEM_EFFECT_SPATK_EV:
    case ITEM_EFFECT_SPDEF_EV:
    case ITEM_EFFECT_SPEED_EV:
    case ITEM_EFFECT_DEF_EV:
        gTasks[taskId].data[4] = 1;
        FieldUseFunc_Medicine(taskId);
        break;
    case ITEM_EFFECT_SACRED_ASH:
        gTasks[taskId].data[4] = 1;
        FieldUseFunc_SacredAsh(taskId);
        break;
    case ITEM_EFFECT_RAISE_LEVEL:
        gTasks[taskId].data[4] = 1;
        FieldUseFunc_RareCandy(taskId);
        break;
    case ITEM_EFFECT_PP_UP:
    case ITEM_EFFECT_PP_MAX:
        gTasks[taskId].data[4] = 1;
        FieldUseFunc_PpUp(taskId);
        break;
    case ITEM_EFFECT_HEAL_PP:
        gTasks[taskId].data[4] = 1;
        FieldUseFunc_Ether(taskId);
        break;
    default:
        gTasks[taskId].data[4] = 4;
        FieldUseFunc_OakStopsYou(taskId);
    }
}

void ItemUseInBattle_EnigmaBerry(u8 taskId)
{
    switch (GetItemEffectType(gSpecialVar_ItemId))
    {
    case ITEM_EFFECT_X_ITEM:
        BattleUseFunc_StatBooster(taskId);
        break;
    case ITEM_EFFECT_HEAL_HP:
    case ITEM_EFFECT_CURE_POISON:
    case ITEM_EFFECT_CURE_SLEEP:
    case ITEM_EFFECT_CURE_BURN:
    case ITEM_EFFECT_CURE_FREEZE_FROSTBITE:
    case ITEM_EFFECT_CURE_PARALYSIS:
    case ITEM_EFFECT_CURE_CONFUSION:
    case ITEM_EFFECT_CURE_INFATUATION:
    case ITEM_EFFECT_CURE_ALL_STATUS:
        BattleUseFunc_Medicine(taskId);
        break;
    case ITEM_EFFECT_HEAL_PP:
        BattleUseFunc_Ether(taskId);
        break;
    default:
        FieldUseFunc_OakStopsYou(taskId);
    }
}

void FieldUseFunc_OakStopsYou(u8 taskId)
{
    if (GetPocketByItemId(gSpecialVar_ItemId) == POCKET_BERRY_POUCH)
    {
        StringExpandPlaceholders(gStringVar4, gText_OakForbidsUseOfItemHere);
        DisplayItemMessageInBerryPouch(taskId, FONT_MALE, gStringVar4, Task_BerryPouch_DestroyDialogueWindowAndRefreshListMenu);
    }
    else
        PrintNotTheTimeToUseThat(taskId, gTasks[taskId].data[3]);
}

void ItemUse_SetQuestLogEvent(u8 eventId, struct Pokemon *pokemon, u16 itemId, u16 param)
{
    struct QuestLogEvent_Item *data = Alloc(sizeof(*data));

    data->itemId = itemId;
    data->itemParam = param;
    if (pokemon != NULL)
        data->species = GetMonData(pokemon, MON_DATA_SPECIES_OR_EGG);
    else
        data->species = 0xFFFF;
    SetQuestLogEvent(eventId, (void *)data);
    Free(data);
}

// TODO: Implement new item effects
void ItemUseOutOfBattle_PokemonBoxLink(u8 taskId)
{
    sItemUseOnFieldCB = Task_AccessPokemonBoxLink;
    SetUpItemUseOnFieldCallback(taskId); 
}

static void Task_AccessPokemonBoxLink(u8 taskId)
{
    ScriptContext_SetupScript(EventScript_AccessPokemonBoxLink);
    DestroyTask(taskId); 
}

void ItemUseOutOfBattle_ExpShare(u8 taskId)
{
    /*
#if I_EXP_SHARE_ITEM >= GEN_6
    if (IsGen6ExpShareEnabled())
    {
        PlaySE(SE_PC_OFF);
        if (!gTasks[taskId].data[2]) // to account for pressing select in the overworld
            DisplayItemMessageOnField(taskId, gText_ExpShareOff, Task_CloseCantUseKeyItemMessage);
        else
            DisplayItemMessage(taskId, FONT_NORMAL, gText_ExpShareOff, CloseItemMessage);
    }
    else
    {
        PlaySE(SE_EXP_MAX);
        if (!gTasks[taskId].data[2]) // to account for pressing select in the overworld
            DisplayItemMessageOnField(taskId, gText_ExpShareOn, Task_CloseCantUseKeyItemMessage);
        else
            DisplayItemMessage(taskId, FONT_NORMAL, gText_ExpShareOn, CloseItemMessage);
    }
    FlagToggle(I_EXP_SHARE_FLAG);
#else
    DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
#endif
    */
}

void ItemUseOutOfBattle_AbilityCapsule(u8 taskId)
{   
    gItemUseCB = ItemUseCB_AbilityCapsule;
    SetUpItemUseCallback(taskId);
}

void ItemUseOutOfBattle_AbilityPatch(u8 taskId)
{
    gItemUseCB = ItemUseCB_AbilityPatch;
    SetUpItemUseCallback(taskId);
}

void ItemUseOutOfBattle_Mint(u8 taskId)
{
    gItemUseCB = ItemUseCB_Mint;
    SetUpItemUseCallback(taskId);
}

void ItemUseOutOfBattle_ResetEVs(u8 taskId)
{
    /* 
    gItemUseCB = ItemUseCB_ResetEVs;
    SetUpItemUseCallback(taskId); 
    */
}

void ItemUseOutOfBattle_ReduceEV(u8 taskId)
{
    /* 
    gItemUseCB = ItemUseCB_ReduceEV;
    SetUpItemUseCallback(taskId); 
    */
}

void ItemUseOutOfBattle_DynamaxCandy(u8 taskId)
{
    /* 
    gItemUseCB = ItemUseCB_DynamaxCandy;
    SetUpItemUseCallback(taskId); 
    */
}

//TMs are handled in tm_case.c, not needed
void ItemUseOutOfBattle_TMHM(u8 taskId)
{    

}

void ItemUseOutOfBattle_EvolutionStone(u8 taskId)
{
    gItemUseCB = ItemUseCB_EvolutionStone;
    SetUpItemUseCallback(taskId); 
}

void ItemUseOutOfBattle_Lure(u8 taskId)
{
    /* 
    if (LURE_STEP_COUNT == 0)
        gTasks[taskId].func = Task_StartUseLure;
    else if (!InBattlePyramid())
        DisplayItemMessage(taskId, FONT_NORMAL, gText_LureEffectsLingered, CloseItemMessage);
    else
        DisplayItemMessageInBattlePyramid(taskId, gText_LureEffectsLingered, Task_CloseBattlePyramidBagMessage); 
    */
}

static void Task_StartUseLure(u8 taskId)
{
    /* 
    s16* data = gTasks[taskId].data;

    if (++data[8] > 7)
    {
        data[8] = 0;
        PlaySE(SE_REPEL);
        gTasks[taskId].func = Task_UseLure;
    } 
    */
}

static void Task_UseLure(u8 taskId)
{
    /* 
    if (!IsSEPlaying())
    {
        VarSet(VAR_REPEL_STEP_COUNT, ItemId_GetHoldEffectParam(gSpecialVar_ItemId) | REPEL_LURE_MASK);
    #if VAR_LAST_REPEL_LURE_USED != 0
        VarSet(VAR_LAST_REPEL_LURE_USED, gSpecialVar_ItemId);
    #endif
        RemoveUsedItem();
        if (!InBattlePyramid())
            DisplayItemMessage(taskId, FONT_NORMAL, gStringVar4, CloseItemMessage);
        else
            DisplayItemMessageInBattlePyramid(taskId, gStringVar4, Task_CloseBattlePyramidBagMessage);
    } 
    */
}

void HandleUseExpiredLure(struct ScriptContext *ctx)
{
    /* 
#if VAR_LAST_REPEL_LURE_USED != 0
    VarSet(VAR_REPEL_STEP_COUNT, ItemId_GetHoldEffectParam(VarGet(VAR_LAST_REPEL_LURE_USED)) | REPEL_LURE_MASK);
#endif 
    */
}

static u32 GetBallThrowableState(void)
{
    /* 
    if (IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT))
     && IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))
        return BALL_THROW_UNABLE_TWO_MONS;
    else if (IsPlayerPartyAndPokemonStorageFull() == TRUE)
        return BALL_THROW_UNABLE_NO_ROOM;
    else if (B_SEMI_INVULNERABLE_CATCH >= GEN_4 && (gStatuses3[GetCatchingBattler()] & STATUS3_SEMI_INVULNERABLE))
        return BALL_THROW_UNABLE_SEMI_INVULNERABLE;
    else if (FlagGet(B_FLAG_NO_CATCHING))
        return BALL_THROW_UNABLE_DISABLED_FLAG;

    return BALL_THROW_ABLE; 
    */
    return 0;
}

bool32 CanThrowBall(void)
{
    /* 
    return (GetBallThrowableState() == BALL_THROW_ABLE); 
    */
    return TRUE;
}

static const u8 sText_CantThrowPokeBall_TwoMons[] = _("Cannot throw a ball!\nThere are two Pokémon out there!\p");
static const u8 sText_CantThrowPokeBall_SemiInvulnerable[] = _("Cannot throw a ball!\nThere's no Pokémon in sight!\p");
static const u8 sText_CantThrowPokeBall_Disabled[] = _("POKé BALLS cannot be used\nright now!\p");
void ItemUseInBattle_PokeBall(u8 taskId)
{
    /* 
    switch (GetBallThrowableState())
    {
    case BALL_THROW_ABLE:
    default:
        RemoveBagItem(gSpecialVar_ItemId, 1);
        if (!InBattlePyramid())
            Task_FadeAndCloseBagMenu(taskId);
        else
            CloseBattlePyramidBag(taskId);
        break;
    case BALL_THROW_UNABLE_TWO_MONS:
        if (!InBattlePyramid())
            DisplayItemMessage(taskId, FONT_NORMAL, sText_CantThrowPokeBall_TwoMons, CloseItemMessage);
        else
            DisplayItemMessageInBattlePyramid(taskId, sText_CantThrowPokeBall_TwoMons, Task_CloseBattlePyramidBagMessage);
        break;
    case BALL_THROW_UNABLE_NO_ROOM:
        if (!InBattlePyramid())
            DisplayItemMessage(taskId, FONT_NORMAL, gText_BoxFull, CloseItemMessage);
        else
            DisplayItemMessageInBattlePyramid(taskId, gText_BoxFull, Task_CloseBattlePyramidBagMessage);
        break;
    case BALL_THROW_UNABLE_SEMI_INVULNERABLE:
        if (!InBattlePyramid())
            DisplayItemMessage(taskId, FONT_NORMAL, sText_CantThrowPokeBall_SemiInvulnerable, CloseItemMessage);
        else
            DisplayItemMessageInBattlePyramid(taskId, sText_CantThrowPokeBall_SemiInvulnerable, Task_CloseBattlePyramidBagMessage);
        break;
    case BALL_THROW_UNABLE_DISABLED_FLAG:
        if (!InBattlePyramid())
            DisplayItemMessage(taskId, FONT_NORMAL, sText_CantThrowPokeBall_Disabled, CloseItemMessage);
        else
            DisplayItemMessageInBattlePyramid(taskId, sText_CantThrowPokeBall_Disabled, Task_CloseBattlePyramidBagMessage);
        break;
    } 
    */
}

static void ItemUseInBattle_ShowPartyMenu(u8 taskId)
{
    /* 
    if (!InBattlePyramid())
    {
        gBagMenu->newScreenCallback = ChooseMonForInBattleItem;
        Task_FadeAndCloseBagMenu(taskId);
    }
    else
    {
        gPyramidBagMenu->newScreenCallback = ChooseMonForInBattleItem;
        CloseBattlePyramidBag(taskId);
    } 
    */
}

void ItemUseInBattle_PartyMenu(u8 taskId)
{
    /* 
    gItemUseCB = ItemUseCB_BattleScript;
    ItemUseInBattle_ShowPartyMenu(taskId);
    */
}

void ItemUseInBattle_PartyMenuChooseMove(u8 taskId)
{
    /* 
    gItemUseCB = ItemUseCB_BattleChooseMove;
    ItemUseInBattle_ShowPartyMenu(taskId); 
    */
}

// Returns whether an item can be used in battle and sets the fail text.
bool32 CannotUseItemsInBattle(u16 itemId, struct Pokemon *mon)
{
    /* 
    u16 battleUsage = ItemId_GetBattleUsage(itemId);
    bool8 cannotUse = FALSE;
    const u8* failStr = NULL;
    u32 i;
    u16 hp = GetMonData(mon, MON_DATA_HP);

    // Embargo Check
    if ((gPartyMenu.slotId == 0 && gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_EMBARGO)
        || (gPartyMenu.slotId == 1 && gStatuses3[B_POSITION_PLAYER_RIGHT] & STATUS3_EMBARGO))
    {
        return TRUE;
    }

    // battleUsage checks
    switch (battleUsage)
    {
    case EFFECT_ITEM_INCREASE_STAT:
        if (gBattleMons[gBattlerInMenuId].statStages[ItemId_GetEffect(itemId)[1]] == MAX_STAT_STAGE)
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_SET_FOCUS_ENERGY:
        if (gBattleMons[gBattlerInMenuId].status2 & STATUS2_FOCUS_ENERGY_ANY)
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_SET_MIST:
        if (gSideStatuses[GetBattlerSide(gBattlerInMenuId)] & SIDE_STATUS_MIST)
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_ESCAPE:
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_THROW_BALL:
        switch (GetBallThrowableState())
        {
        case BALL_THROW_UNABLE_TWO_MONS:
            failStr = sText_CantThrowPokeBall_TwoMons;
            cannotUse = TRUE;
            break;
        case BALL_THROW_UNABLE_NO_ROOM:
            failStr = gText_BoxFull;
            cannotUse = TRUE;
            break;
        case BALL_THROW_UNABLE_SEMI_INVULNERABLE:
            failStr = sText_CantThrowPokeBall_SemiInvulnerable;
            cannotUse = TRUE;
            break;
        case BALL_THROW_UNABLE_DISABLED_FLAG:
            failStr = sText_CantThrowPokeBall_Disabled;
            cannotUse = TRUE;
            break;
        }
        break;
    case EFFECT_ITEM_INCREASE_ALL_STATS:
        for (i = STAT_ATK; i < NUM_STATS; i++)
        {
            if (CompareStat(gBattlerInMenuId, i, MAX_STAT_STAGE, CMP_EQUAL))
            {
                cannotUse = TRUE;
                break;
            }
        }
        break;
    case EFFECT_ITEM_RESTORE_HP:
        if (hp == 0 || hp == GetMonData(mon, MON_DATA_MAX_HP))
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_CURE_STATUS:
        if (!((GetMonData(mon, MON_DATA_STATUS) & GetItemStatus1Mask(itemId))
            || (gPartyMenu.slotId == 0 && gBattleMons[gBattlerInMenuId].status2 & GetItemStatus2Mask(itemId))))
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_HEAL_AND_CURE_STATUS:
        if ((hp == 0 || hp == GetMonData(mon, MON_DATA_MAX_HP))
            && !((GetMonData(mon, MON_DATA_STATUS) & GetItemStatus1Mask(itemId))
            || (gPartyMenu.slotId == 0 && gBattleMons[gBattlerInMenuId].status2 & GetItemStatus2Mask(itemId))))
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_REVIVE:
        if (hp != 0)
            cannotUse = TRUE;
        break;
    case EFFECT_ITEM_RESTORE_PP:
        if (ItemId_GetEffect(itemId)[4] == ITEM4_HEAL_PP)
        {
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                if (GetMonData(mon, MON_DATA_PP1 + i) < CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + i), GetMonData(mon, MON_DATA_PP_BONUSES), i))
                    break;
            }
            if (i == MAX_MON_MOVES)
                cannotUse = TRUE;
        }
        else if (GetMonData(mon, MON_DATA_PP1 + gPartyMenu.data1) == CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + gPartyMenu.data1), GetMonData(mon, MON_DATA_PP_BONUSES), gPartyMenu.data1))
        {
            cannotUse = TRUE;
        }
        break;
    }

    if (failStr != NULL)
        StringExpandPlaceholders(gStringVar4, failStr);
    else
        StringExpandPlaceholders(gStringVar4, gText_WontHaveEffect);

    return cannotUse; 
    */
    return FALSE;
}

void ItemUseInBattle_BagMenu(u8 taskId)
{
    /* 
    if (CannotUseItemsInBattle(gSpecialVar_ItemId, NULL))
    {
        if (!InBattlePyramid())
            DisplayItemMessage(taskId, FONT_NORMAL, gStringVar4, CloseItemMessage);
        else
            DisplayItemMessageInBattlePyramid(taskId, gStringVar4, Task_CloseBattlePyramidBagMessage);
    }
    else
    {
        PlaySE(SE_SELECT);
        if (!(B_TRY_CATCH_TRAINER_BALL >= GEN_4 && (ItemId_GetBattleUsage(gSpecialVar_ItemId) == EFFECT_ITEM_THROW_BALL) && (gBattleTypeFlags & BATTLE_TYPE_TRAINER)))
            RemoveUsedItem();
        ScheduleBgCopyTilemapToVram(2);
        if (!InBattlePyramid())
            gTasks[taskId].func = Task_FadeAndCloseBagMenu;
        else
            gTasks[taskId].func = CloseBattlePyramidBag;
    } 
    */
}

void ItemUseOutOfBattle_FormChange(u8 taskId)
{
    if (!gTasks[taskId].tUsingRegisteredKeyItem)
    {
        gItemUseCB = ItemUseCB_FormChange;
        gTasks[taskId].data[0] = FALSE;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
    {
        // TODO: handle key items with callbacks to menus allow to be used by registering them.
        FieldUseFunc_OakStopsYou(taskId);
    } 
    
}

void ItemUseOutOfBattle_FormChange_ConsumedOnUse(u8 taskId)
{
    if (!gTasks[taskId].tUsingRegisteredKeyItem)
    {
        gItemUseCB = ItemUseCB_FormChange_ConsumedOnUse;
        gTasks[taskId].data[0] = TRUE;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
    {
        // TODO: handle key items with callbacks to menus allow to be used by registering them.
        FieldUseFunc_OakStopsYou(taskId);
    } 
    
}

void ItemUseOutOfBattle_RotomCatalog(u8 taskId)
{
    if (!gTasks[taskId].tUsingRegisteredKeyItem)
    {
        gItemUseCB = ItemUseCB_RotomCatalog;
        gTasks[taskId].data[0] = TRUE;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
    {
        // TODO: handle key items with callbacks to menus allow to be used by registering them.
        FieldUseFunc_OakStopsYou(taskId);
    } 
    
}

void ItemUseOutOfBattle_ZygardeCube(u8 taskId)
{
    if (!gTasks[taskId].tUsingRegisteredKeyItem)
    {
        gItemUseCB = ItemUseCB_ZygardeCube;
        gTasks[taskId].data[0] = TRUE;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
    {
        // TODO: handle key items with callbacks to menus allow to be used by registering them.
        FieldUseFunc_OakStopsYou(taskId);
    } 
    
}

void ItemUseOutOfBattle_Fusion(u8 taskId)
{
    /* 
    gItemUseCB = ItemUseCB_Fusion;
    gTasks[taskId].data[0] = FALSE;
    SetUpItemUseCallback(taskId); 
    */
}

void Task_UseHoneyOnField(u8 taskId)
{
    /* 
    //ResetInitialPlayerAvatarState();
    StartSweetScentFieldEffect();
    DestroyTask(taskId); 
    */
}

static void ItemUseOnFieldCB_Honey(u8 taskId)
{
    /* 
    Overworld_ResetStateAfterDigEscRope();
    RemoveBagItem(gSpecialVar_ItemId, 1);
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gText_PlayerUsedVar2);
    gTasks[taskId].data[0] = 0;
    DisplayItemMessageOnField(taskId, gStringVar4, Task_UseHoneyOnField); 
    */
}

void ItemUseOutOfBattle_Honey(u8 taskId)
{
    /* 
    sItemUseOnFieldCB = ItemUseOnFieldCB_Honey;
    gFieldCallback = FieldCB_UseItemOnField;
    gBagMenu->newScreenCallback = CB2_ReturnToField;
    Task_FadeAndCloseBagMenu(taskId); 
    */
}