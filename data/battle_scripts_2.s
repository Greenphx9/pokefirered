#include "config/battle.h"
#include "constants/moves.h"
#include "constants/battle.h"
#include "constants/battle_script_commands.h"
#include "constants/battle_anim.h"
#include "constants/items.h"
#include "constants/abilities.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "constants/songs.h"
#include "constants/game_stat.h"
#include "constants/battle_string_ids.h"
	.include "asm/macros/battle_script.inc"
@ Define these here since misc_constants.inc conflicts with the C headers
	.set NULL, 0
	.set FALSE, 0
	.set TRUE, 1
	.section script_data, "aw", %progbits

	.align 2
gBattlescriptsForUsingItem::
	.4byte BattleScript_BallThrow                @ EFFECT_ITEM_RESTORE_HP
	.4byte BattleScript_BallThrow               @ EFFECT_ITEM_CURE_STATUS
	.4byte BattleScript_BallThrow        @ EFFECT_ITEM_HEAL_AND_CURE_STATUS
	.4byte BattleScript_BallThrow             @ EFFECT_ITEM_INCREASE_STAT
	.4byte BattleScript_BallThrow                  @ EFFECT_ITEM_SET_MIST
	.4byte BattleScript_BallThrow           @ EFFECT_ITEM_SET_FOCUS_ENERGY
	.4byte BattleScript_BallThrow               @ EFFECT_ITEM_ESCAPE
	.4byte BattleScript_BallThrow                    @ EFFECT_ITEM_THROW_BALL
	.4byte BattleScript_BallThrow                @ EFFECT_ITEM_REVIVE
	.4byte BattleScript_BallThrow                @ EFFECT_ITEM_RESTORE_PP
	.4byte BattleScript_BallThrow         @ EFFECT_ITEM_INCREASE_ALL_STATS

gBattlescriptsForSafariActions::
	.4byte BattleScript_WatchesCarefully
	.4byte BattleScript_ThrowRock
	.4byte BattleScript_ThrowBait
	.4byte BattleScript_LeftoverWallyPrepToThrow

BattleScript_ItemEnd:
	end


BattleScript_BallThrow::
	printstring STRINGID_PLAYERUSEDITEM
	handleballthrow

BattleScript_SafariBallThrow::
	printstring STRINGID_PLAYERUSEDITEM
	updatestatusicon BS_ATTACKER
	handleballthrow

BattleScript_SuccessBallThrow::
	setbyte sMON_CAUGHT, TRUE
	incrementgamestat GAME_STAT_POKEMON_CAPTURES
	printstring STRINGID_GOTCHAPKMNCAUGHT
	@jumpifbyte CMP_NOT_EQUAL, sEXP_CATCH, TRUE, BattleScript_TryPrintCaughtMonInfo
	setbyte sGIVEEXP_STATE, 0
	getexp BS_TARGET
	sethword gBattle_BG2_X, 0
BattleScript_TryPrintCaughtMonInfo:
	@jumpifbattletype BATTLE_TYPE_RECORDED, BattleScript_GiveCaughtMonEnd
	trysetcaughtmondexflags BattleScript_TryNicknameCaughtMon
	printstring STRINGID_PKMNDATAADDEDTODEX
	waitstate
	setbyte gBattleCommunication, 0
	displaydexinfo
BattleScript_TryNicknameCaughtMon::
	printstring STRINGID_GIVENICKNAMECAPTURED
	waitstate
	setbyte gBattleCommunication, 0
	trygivecaughtmonnick BattleScript_GiveCaughtMonEnd
	givecaughtmon
	printfromtable gCaughtMonStringIds
	waitmessage B_WAIT_TIME_LONG
	goto BattleScript_SuccessBallThrowEnd
BattleScript_GiveCaughtMonEnd::
	givecaughtmon
BattleScript_SuccessBallThrowEnd::
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	finishturn

BattleScript_ShakeBallThrow::
	printfromtable gBallEscapeStringIds
	waitmessage B_WAIT_TIME_LONG
	jumpifword CMP_NO_COMMON_BITS, gBattleTypeFlags, BATTLE_TYPE_SAFARI, BattleScript_ShakeBallThrowEnd
	jumpifbyte CMP_NOT_EQUAL, gNumSafariBalls, 0, BattleScript_ShakeBallThrowEnd
	printstring STRINGID_OUTOFSAFARIBALLS
	waitmessage B_WAIT_TIME_LONG
	setbyte gBattleOutcome, B_OUTCOME_NO_SAFARI_BALLS
BattleScript_ShakeBallThrowEnd::
	finishaction

BattleScript_TrainerBallBlock::
	waitmessage B_WAIT_TIME_LONG
	printstring STRINGID_TRAINERBLOCKEDBALL
	waitmessage B_WAIT_TIME_LONG
	printstring STRINGID_DONTBEATHIEF
	waitmessage B_WAIT_TIME_LONG
	finishaction

BattleScript_GhostBallDodge::
	waitmessage B_WAIT_TIME_LONG
	printstring STRINGID_ITDODGEDBALL
	waitmessage B_WAIT_TIME_LONG
	finishaction

BattleScript_OldMan_Pokedude_CaughtMessage::
	printstring STRINGID_GOTCHAPKMNCAUGHT2
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	endlinkbattle
	finishturn

BattleScript_WatchesCarefully::
	printfromtable gSafariReactionStringIds
	waitmessage B_WAIT_TIME_LONG
	playanimation BS_OPPONENT1, B_ANIM_SAFARI_REACTION
	end2

BattleScript_ThrowRock::
	printstring STRINGID_THREWROCK
	waitmessage B_WAIT_TIME_LONG
	playanimation BS_ATTACKER, B_ANIM_ROCK_THROW
	end2

BattleScript_ThrowBait::
	printstring STRINGID_THREWBAIT
	waitmessage B_WAIT_TIME_LONG
	@playanimation BS_ATTACKER, B_ANIM_BAIT_THROW
	end2

BattleScript_LeftoverWallyPrepToThrow::
	printstring STRINGID_RETURNMON
	waitmessage B_WAIT_TIME_LONG
	returnatktoball
	waitstate
	trainerslidein BS_TARGET
	waitstate
	printstring STRINGID_YOUTHROWABALLNOWRIGHT
	waitmessage B_WAIT_TIME_LONG
	end2

BattleScript_TrainerASlideMsgRet::
	handletrainerslidemsg BS_SCRIPTING, 0
	trainerslidein B_POSITION_OPPONENT_LEFT
	handletrainerslidemsg BS_SCRIPTING, 1
	waitstate
	trainerslideout B_POSITION_OPPONENT_LEFT
	waitstate
	handletrainerslidemsg BS_SCRIPTING, 2
	return

BattleScript_TrainerASlideMsgEnd2::
	call BattleScript_TrainerASlideMsgRet
	end2

BattleScript_TrainerBSlideMsgRet::
	handletrainerslidemsg BS_SCRIPTING, 0
	trainerslidein B_POSITION_OPPONENT_RIGHT
	handletrainerslidemsg BS_SCRIPTING, 1
	waitstate
	trainerslideout B_POSITION_OPPONENT_RIGHT
	waitstate
	handletrainerslidemsg BS_SCRIPTING, 2
	return

BattleScript_TrainerBSlideMsgEnd2::
	call BattleScript_TrainerBSlideMsgRet
	end2
