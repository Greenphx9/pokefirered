#ifndef GUARD_CONFIG_BW_SUMMARY_SCREEN_CONFIG_H
#define GUARD_CONFIG_BW_SUMMARY_SCREEN_CONFIG_H

/*=== Custom BW Summary Screen Options ===*/
#define BW_SUMMARY_SCREEN                   TRUE  //KEEP THIS SET. Used by other branches. Disabling does not do anything.
#define MULTILINE_ABILITY_DESCRIPTIONS      FALSE //If set, \n will work in ability descriptions. Also modifies the max ability description to 100 characters.
#define NATURE_COLOURED_STATS               FALSE //If set, stats on the skills page will be blue if decreased by a nature, and red if increased by a nature.
#define IV_RANKINGS                         FALSE //If set, displays grades that range from E- to S for each stat. They use the type icons palette. Make sure
                                                  //To change them if you change the type icons.
#define SPLIT_ICONS                         FALSE //If set, Displays the move split above the move power.
#define GEN_4_PLUS_PSS                      FALSE //If set, uses the Gen 4 Physical Special Split for the Summary Screen.
                                                  //This does not implement Gen 4 PSS. Just adds support for it. Uses the "split" field in BattleMove, change
                                                  //it in GetMoveSplitPSS if yours is called something else!
#define UNHIDDEN_POWER_PSS                  FALSE //If set, shows Hidden Power's type on the summary screen.


#endif  // GUARD_CONFIG_BW_SUMMARY_SCREEN_CONFIG_H