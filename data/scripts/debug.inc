Debug_ShowFieldMessageStringVar4::
	special ShowFieldMessageStringVar4
	waitmessage
	waitbuttonpress
	releaseall
	end

Debug_CheckWeekDay_Text:
	.string "Aaaaah, yes!\l"
	.string "{STR_VAR_1}s are made for dads!\l"
	.string "And dad's car!$"

Debug_CheatStart::
	lockall
	setflag FLAG_SYS_POKEMON_GET
	setvar VAR_MAP_SCENE_PALLET_TOWN_OAK, 1
	givemon SPECIES_BULBASAUR, 20, ITEM_NONE
	givemon SPECIES_CHARMANDER, 20, ITEM_NONE
	givemon SPECIES_SQUIRTLE, 20, ITEM_NONE
	setflag FLAG_SYS_POKEDEX_GET
	special SetUnlockedPokedexFlags
	special EnableNationalPokedex
	setflag FLAG_HIDE_PEWTER_CITY_RUNNING_SHOES_GUY
	setflag FLAG_SYS_B_DASH
	setflag FLAG_GOT_BICYCLE
	giveitem ITEM_BICYCLE
	additem ITEM_MAX_REPEL, 90
	additem ITEM_ESCAPE_ROPE, 20
	setflag FLAG_BADGE01_GET
	setflag FLAG_BADGE02_GET
	setflag FLAG_BADGE03_GET
	setflag FLAG_BADGE04_GET
	setflag FLAG_BADGE05_GET
	setflag FLAG_BADGE06_GET
	setflag FLAG_BADGE07_GET
	setflag FLAG_BADGE08_GET
	closemessage
	release
	end

Debug_Script_1::
	end

Debug_Script_2::
	end

Debug_Script_3::
	end

Debug_Script_4::
	end

Debug_Script_5::
	end

Debug_Script_6::
	end

Debug_Script_7::
	end

Debug_Script_8::
	end
