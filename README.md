# pokefirered-expansion

### Important: DO NOT use GitHub's "Download Zip" option. Using this option will not download the commit history required to update your expansion version or merge other feature branches. Instead, please read [this guide](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/wiki/The-Basics-of-GitHub) to learn how to fork the repository and clone locally from there.

## What is pokeemerald-expansion?

pokefirered-expansion is a very much WIP progress porting the pokeemerald-expansion to pokefirered. This includes Pokemons, items, moves, abilities, etc. pokeemerald-expansion is a decomp hack base project based off pret's [pokeemerald](https://github.com/pret/pokeemerald) decompilation project. It's recommended that any new projects that plan on using it, to clone this repository instead of pret's vanilla repository, as we regurlarly incorporate pret's documentation changes. This is ***NOT*** a standalone romhack, and as such, most features will be unavailable and/or unbalanced if played as is.

For now, if used, please credit RHH (Rom Hacking Hideout) for their pokeemerald-expansion, and me (Greenphx9) for my port. Keep it this way until this project is officially mantained / owned by RHH.
You can phrase it as the following:
```
Based of Greenphx9's pokefirered-expansion v1.0.0, which is based off RHH's pokeemerald-expansion v1.8.5 https://github.com/rh-hideout/pokeemerald-expansion/
```

## What features are included?
This will be updated as I port things, the full list of features can be found at https://github.com/rh-hideout/pokeemerald-expansion/.
For now, these features have been implemeneted:
- ***IMPORTANT*❗❗ Read through these to learn what features you can toggle**:
    - [Battle configurations](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/config/battle.h)
    - [Pokémon configurations](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/config/pokemon.h)
    - [Item configurations](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/config/item.h)
    - [Overworld configurations](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/config/overworld.h)
    - [Debug configurations](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/config/debug.h)
- ***Upgraded battle engine.***
    - Gen5+ damage calculation.
    ~~- 2v2 Wild battles support.~~
    ~~- 1v2/2v1 battles support.~~
    - Fairy Type (configurable).
    - Physical/Special/Status Category (configurable).
    - New moves and abilities up to Scarlet and Violet.
        ~~- Custom Contest data up to SwSh, newer moves are WIP. ([source](https://pokemonurpg.com/info/contests/rse-move-list/))~~ (Not in Fire Red)
    - Battle gimmick support:
        - Mega Evolution
        - Primal Reversion
        ~~- Ultra Burst~~ (implemented but untested)
        ~~- Z-Moves~~ (implemented but untested)
            ~~- Gen 8+ damaging moves are given power extrapolated from Gen 7.~~ (implemented but untested)
            ~~- Gen 8+ status moves have no additional effects, like Healing Wish.~~ (implemented but untested)
        ~~- Dynamax and Gigantamax~~ (implemented but untested)
    - Initial battle parameters
        ~~- Queueing stat boosts (aka, Totem Boosts)~~ (implemented but untested)
        ~~- Setting Terrains.~~ (implemented but untested)
    - Mid-turn speed recalculation.
    - Quick Poké Ball selection in Wild Battles
        - Hold `R` to change selection with the D-Pad.
        - Press `R` to use last selected Poké Ball.
    ~~- Run option shortcut~~
    - Faster battle intro - Message and animation/cry happens at the same time.
    - Faster HP drain.
    ~~- Battle Debug menu.~~
        ~~- Accessed by pressing `Select` on the "Fight/Bag/Pokémon/Run" menu.~~
    - Option to use AI flags in wild Pokémon battles.
    - FRLG/Gen4+ whiteout money calculation.
    - Configurable experience settings
        - Experience on catch.
        - Splitting experience.
        - Trainer experience.
        - Scaled experience.
        - Unevolved experience boost.
    - Frostbite.
        - Doesn't replace freezing unless a config is enabled, so you can mix and match.
    - Critical capture.
    - Removed badge boosts (configurable).
    - Recalculating stats at the end of every battle.
    - Level 100 Pokémon can earn EVs.
    ~~- Inverse battle support.~~
    - TONS of other features listed [here](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/config/battle.h).
- ***Full Trainer customization***
    - Nickname, EVs, IVs, moves, ability, ball, friendship, nature, gender, shininess.
    ~~- Custom tag battle support (teaming up an NPC in a double battle).~~
    ~~- Sliding trainer messages.~~
    - Upgraded Trainer AI
        - Considers newer move effects.
        - New flag options to let you customize the intelligence of your trainers.
        - Faster calculations.
    - Specify Poké Balls by Trainer class.
- ***Pokémon Species from Generations 1-9.***
    - Simplified process to add new Pokémon.
    - Option to disable unwanted families.
    - Updated sprites to DS style.
    - Updated stats, types, abilities and egg groups (configurable).
    - Updated Hoenn's Regional Dex to match ORAS' (configurable).
    - Updated National Dex incorporating the new species.
    - Sprite and animation visualizer.
        - Accesible by pressing `Select` on a Pokémon's Summary screen.
    - Gen4+ evolution methods, with some changes:
        - Mossy Rock, Icy Rock and Magnetic Field locations match ORAS'.
            - Leaf, Ice and Thunder Stones may also be used.
        - Inkay just needs level 30 to evolve.
            - You can't physically have both the RTC and gyroscope, so we skip this requirement.
        - Sylveon uses Gen8+'s evolution method (friendship + Fairy Move).
        - Option to use hold evolution items directly like stones.
    - Hidden Abilities.
        - Available via Ability Patch.
        - Compatible with Ghoul's DexNav branch.
    - All gender differences.
        - Custom female icons for female Hippopotas Hippowdon, Pikachu and Wobbufett
    - 3 Perfect IVs on Legendaries, Mythicals and Ultra Beasts.
- ***Customizable form change tables. Full list of methods [here](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/constants/form_change_types.h).***
    - Item holding (eg. Giratina/Arceus)
    - Item using (eg. Oricorio)
        - Time of day option for Shaymin
    - Fainting
    - Battle begin and end (eg. Xerneas)
        - Move change option for Zacian/Zamazenta
    - Battle end in terrains (eg. Burmy)
    - Switched in battle (eg. Palafin)
    - HP Threshold (eg. Darmanitan)
    - Weather (eg. Castform)
    - End of turn (eg. Morpeko)
    ~~- Time of day (eg. Shaymin)~~ (No RTC yet)
    ~~- Fusions (eg. Kyurem)~~
- ***Breeding Improvements***
    ~~- Incense Baby Pokémon now happen automatically (configurable).~~
    ~~- Level 1 eggs (configurable).~~
    ~~- Poké Ball inheriting (configurable).~~
    ~~- Egg Move Transfer, including Mirror Herb (configurable).~~
    ~~- Nature inheriting 100% of the time with Everstone (configurable)~~
    ~~- Gen6+ Ability inheriting (configurable).~~
- ***Items from newer Generations. Full list [here](https://github.com/rh-hideout/pokeemerald-expansion/blob/master/include/constants/items.h).***
    ~~- ***Gen 6+ Exp. Share*** (configurable)~~
    ~~- Berserk Gene~~
    - Most battle items from Gen 4+
    - Existing item data but missing effects:
        - Gimmighoul Coin
        - Booster Energy
        - Tera Shards
        - Tera Orb
- ***Feature branches incorporated (with permission):***
    ~~- [RHH intro credits](https://github.com/Xhyzi/pokeemerald/tree/rhh-intro-credits) by @Xhyzi.~~ (this won't be added unless this becomes an official RHH project)
        ~~- A small signature from all of us to show the collective effort in the project :)~~
    - [Overworld debug](https://github.com/TheXaman/pokeemerald/tree/tx_debug_system) by @TheXaman
        - May be disabled.
        - Accesible by pressing `R + Start` in the overworld by default.
        - **Additional features**:
            - *Clear Boxes*: cleans every Pokémon from the Boxes.
            - *Hatch an Egg*: lets you choose an Egg in your party and immediately hatch it.
    ~~- [HGSS Pokédex](https://github.com/TheXaman/pokeemerald/tree/tx_pokedexPlus_hgss) by @TheXaman~~
        ~~- May be disabled.~~
        ~~- **Additional features**:~~
            ~~- *Support for new evolution methods*.~~
            ~~- *Dark Mode*.~~
    - [Nature Colors](https://github.com/DizzyEggg/pokeemerald/tree/nature_color) in summary screen by @DizzyEggg (adapted for Fire Red)
    - [Dynamic Multichoice](https://github.com/SBird1337/pokeemerald/tree/feature/dynmulti) by @SBird1337 (adapted for Fire Red)
    - [Saveblock Cleansing](https://github.com/ghoulslash/pokeemerald/tree/saveblock) by @ghoulslash (adapted for Fire Red)
- ***Other features***
    ~~- Pressing B while holding a Pokémon drops them like in modern games (configurable).~~
    ~~- Running indoors (configurable).~~
    ~~- Configurable overworld poison damage.~~
    ~~- Configurable flags for disabling Wild encounters and Trainer battles.~~
    ~~- Configurable flags for forcing or disabling Shinies.~~
    - Reusable TM (configurable).
    - B2W2+ Repel system that also supports LGPE's Lures
    - Gen6+'s EV cap.
    ~~- All bugfixes from pret included.~~
    ~~- Fixed overworld snow effect.~~
