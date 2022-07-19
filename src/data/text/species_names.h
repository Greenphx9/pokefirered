const u8 gSpeciesNames[][11] = {
    _("??????"),
    _("Bulbasaur"),
    _("Ivysaur"),
    _("Venusaur"),
    _("Charmander"),
    _("Charmeleon"),
    _("Charizard"),
    _("Squirtle"),
    _("Wartortle"),
    _("Blastoise"),
    _("Caterpie"),
    _("Metapod"),
    _("Butterfree"),
    _("Weedle"),
    _("Kakuna"),
    _("Beedrill"),
    _("Pidgey"),
    _("Pidgeotto"),
    _("Pidgeot"),
    _("Rattata"),
    _("Raticate"),
    _("Spearow"),
    _("Fearow"),
    _("Ekans"),
    _("Arbok"),
    _("Pikachu"),
    _("Raichu"),
    _("Sandshrew"),
    _("Sandslash"),
    _("Nidoran♀"),
    _("Nidorina"),
    _("Nidoqueen"),
    _("Nidoran♂"),
    _("Nidorino"),
    _("Nidoking"),
    _("Clefairy"),
    _("Clefable"),
    _("Vulpix"),
    _("Ninetales"),
    _("Jigglypuff"),
    _("Wigglytuff"),
    _("Zubat"),
    _("Golbat"),
    _("Oddish"),
    _("Gloom"),
    _("Vileplume"),
    _("Paras"),
    _("Parasect"),
    _("Venonat"),
    _("Venomoth"),
    _("Diglett"),
    _("Dugtrio"),
    _("Meowth"),
    _("Persian"),
    _("Psyduck"),
    _("Golduck"),
    _("Mankey"),
    _("Primeape"),
    _("Growlithe"),
    _("Arcanine"),
    _("Poliwag"),
    _("Poliwhirl"),
    _("Poliwrath"),
    _("Abra"),
    _("Kadabra"),
    _("Alakazam"),
    _("Machop"),
    _("Machoke"),
    _("Machamp"),
    _("Bellsprout"),
    _("Weepinbell"),
    _("Victreebel"),
    _("Tentacool"),
    _("Tentacruel"),
    _("Geodude"),
    _("Graveler"),
    _("Golem"),
    _("Ponyta"),
    _("Rapidash"),
    _("Slowpoke"),
    _("Slowbro"),
    _("Magnemite"),
    _("Magneton"),
    _("Farfetch'd"),
    _("Doduo"),
    _("Dodrio"),
    _("Seel"),
    _("Dewgong"),
    _("Grimer"),
    _("Muk"),
    _("Shellder"),
    _("Cloyster"),
    _("Gastly"),
    _("Haunter"),
    _("Gengar"),
    _("Onix"),
    _("Drowzee"),
    _("Hypno"),
    _("Krabby"),
    _("Kingler"),
    _("Voltorb"),
    _("Electrode"),
    _("Exeggcute"),
    _("Exeggutor"),
    _("Cubone"),
    _("Marowak"),
    _("Hitmonlee"),
    _("Hitmonchan"),
    _("Lickitung"),
    _("Koffing"),
    _("Weezing"),
    _("Rhyhorn"),
    _("Rhydon"),
    _("Chansey"),
    _("Tangela"),
    _("Kangaskhan"),
    _("Horsea"),
    _("Seadra"),
    _("Goldeen"),
    _("Seaking"),
    _("Staryu"),
    _("Starmie"),
    _("Mr. Mime"),
    _("Scyther"),
    _("Jynx"),
    _("Electabuzz"),
    _("Magmar"),
    _("Pinsir"),
    _("Tauros"),
    _("Magikarp"),
    _("Gyarados"),
    _("Lapras"),
    _("Ditto"),
    _("Eevee"),
    _("Vaporeon"),
    _("Jolteon"),
    _("Flareon"),
    _("Porygon"),
    _("Omanyte"),
    _("Omastar"),
    _("Kabuto"),
    _("Kabutops"),
    _("Aerodactyl"),
    _("Snorlax"),
    _("Articuno"),
    _("Zapdos"),
    _("Moltres"),
    _("Dratini"),
    _("Dragonair"),
    _("Dragonite"),
    _("Mewtwo"),
    _("Mew"),
    _("Chikorita"),
    _("Bayleef"),
    _("Meganium"),
    _("Cyndaquil"),
    _("Quilava"),
    _("Typhlosion"),
    _("Totodile"),
    _("Croconaw"),
    _("Feraligatr"),
    _("Sentret"),
    _("Furret"),
    _("Hoothoot"),
    _("Noctowl"),
    _("Ledyba"),
    _("Ledian"),
    _("Spinarak"),
    _("Ariados"),
    _("Crobat"),
    _("Chinchou"),
    _("Lanturn"),
    _("Pichu"),
    _("Cleffa"),
    _("Igglybuff"),
    _("Togepi"),
    _("Togetic"),
    _("Natu"),
    _("Xatu"),
    _("Mareep"),
    _("Flaaffy"),
    _("Ampharos"),
    _("Bellossom"),
    _("Marill"),
    _("Azumarill"),
    _("Sudowoodo"),
    _("Politoed"),
    _("Hoppip"),
    _("Skiploom"),
    _("Jumpluff"),
    _("Aipom"),
    _("Sunkern"),
    _("Sunflora"),
    _("Yanma"),
    _("Wooper"),
    _("Quagsire"),
    _("Espeon"),
    _("Umbreon"),
    _("Murkrow"),
    _("Slowking"),
    _("Misdreavus"),
    _("Unown"),
    _("Wobbuffet"),
    _("Girafarig"),
    _("Pineco"),
    _("Forretress"),
    _("Dunsparce"),
    _("Gligar"),
    _("Steelix"),
    _("Snubbull"),
    _("Granbull"),
    _("Qwilfish"),
    _("Scizor"),
    _("Shuckle"),
    _("Heracross"),
    _("Sneasel"),
    _("Teddiursa"),
    _("Ursaring"),
    _("Slugma"),
    _("Magcargo"),
    _("Swinub"),
    _("Piloswine"),
    _("Corsola"),
    _("Remoraid"),
    _("Octillery"),
    _("Delibird"),
    _("Mantine"),
    _("Skarmory"),
    _("Houndour"),
    _("Houndoom"),
    _("Kingdra"),
    _("Phanpy"),
    _("Donphan"),
    _("Porygon2"),
    _("Stantler"),
    _("Smeargle"),
    _("Tyrogue"),
    _("Hitmontop"),
    _("Smoochum"),
    _("Elekid"),
    _("Magby"),
    _("Miltank"),
    _("Blissey"),
    _("Raikou"),
    _("Entei"),
    _("Suicune"),
    _("Larvitar"),
    _("Pupitar"),
    _("Tyranitar"),
    _("Lugia"),
    _("Ho-Oh"),
    _("Celebi"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("?"),
    _("Treecko"),
    _("Grovyle"),
    _("Sceptile"),
    _("Torchic"),
    _("Combusken"),
    _("Blaziken"),
    _("Mudkip"),
    _("Marshtomp"),
    _("Swampert"),
    _("Poochyena"),
    _("Mightyena"),
    _("Zigzagoon"),
    _("Linoone"),
    _("Wurmple"),
    _("Silcoon"),
    _("Beautifly"),
    _("Cascoon"),
    _("Dustox"),
    _("Lotad"),
    _("Lombre"),
    _("Ludicolo"),
    _("Seedot"),
    _("Nuzleaf"),
    _("Shiftry"),
    _("Nincada"),
    _("Ninjask"),
    _("Shedinja"),
    _("Taillow"),
    _("Swellow"),
    _("Shroomish"),
    _("Breloom"),
    _("Spinda"),
    _("Wingull"),
    _("Pelipper"),
    _("Surskit"),
    _("Masquerain"),
    _("Wailmer"),
    _("Wailord"),
    _("Skitty"),
    _("Delcatty"),
    _("Kecleon"),
    _("Baltoy"),
    _("Claydol"),
    _("Nosepass"),
    _("Torkoal"),
    _("Sableye"),
    _("Barboach"),
    _("Whiscash"),
    _("Luvdisc"),
    _("Corphish"),
    _("Crawdaunt"),
    _("Feebas"),
    _("Milotic"),
    _("Carvanha"),
    _("Sharpedo"),
    _("Trapinch"),
    _("Vibrava"),
    _("Flygon"),
    _("Makuhita"),
    _("Hariyama"),
    _("Electrike"),
    _("Manectric"),
    _("Numel"),
    _("Camerupt"),
    _("Spheal"),
    _("Sealeo"),
    _("Walrein"),
    _("Cacnea"),
    _("Cacturne"),
    _("Snorunt"),
    _("Glalie"),
    _("Lunatone"),
    _("Solrock"),
    _("Azurill"),
    _("Spoink"),
    _("Grumpig"),
    _("Plusle"),
    _("Minun"),
    _("Mawile"),
    _("Meditite"),
    _("Medicham"),
    _("Swablu"),
    _("Altaria"),
    _("Wynaut"),
    _("Duskull"),
    _("Dusclops"),
    _("Roselia"),
    _("Slakoth"),
    _("Vigoroth"),
    _("Slaking"),
    _("Gulpin"),
    _("Swalot"),
    _("Tropius"),
    _("Whismur"),
    _("Loudred"),
    _("Exploud"),
    _("Clamperl"),
    _("Huntail"),
    _("Gorebyss"),
    _("Absol"),
    _("Shuppet"),
    _("Banette"),
    _("Seviper"),
    _("Zangoose"),
    _("Relicanth"),
    _("Aron"),
    _("Lairon"),
    _("Aggron"),
    _("Castform"),
    _("Volbeat"),
    _("Illumise"),
    _("Lileep"),
    _("Cradily"),
    _("Anorith"),
    _("Armaldo"),
    _("Ralts"),
    _("Kirlia"),
    _("Gardevoir"),
    _("Bagon"),
    _("Shelgon"),
    _("Salamence"),
    _("Beldum"),
    _("Metang"),
    _("Metagross"),
    _("Regirock"),
    _("Regice"),
    _("Registeel"),
    _("Kyogre"),
    _("Groudon"),
    _("Rayquaza"),
    _("Latias"),
    _("Latios"),
    _("Jirachi"),
    _("Deoxys"),
    _("Chimecho"),
    _("Bad Egg"),
    _("Unown B"),
    _("Unown C"),
    _("Unown D"),
    _("Unown E"),
    _("Unown F"),
    _("Unown G"),
    _("Unown H"),
    _("Unown I"),
    _("Unown J"),
    _("Unown K"),
    _("Unown L"),
    _("Unown M"),
    _("Unown N"),
    _("Unown O"),
    _("Unown P"),
    _("Unown Q"),
    _("Unown R"),
    _("Unown S"),
    _("Unown T"),
    _("Unown U"),
    _("Unown V"),
    _("Unown W"),
    _("Unown X"),
    _("Unown Y"),
    _("Unown Z"),
    _("Unown !"),
    _("Unown ?"),
    _("Turtwig"),
    _("Grotle"),
    _("Torterra"),
    _("Chimchar"),
    _("Monferno"),
    _("Infernape"),
    _("Piplup"),
    _("Prinplup"),
    _("Empoleon"),
    _("Starly"),
    _("Staravia"),
    _("Staraptor"),
    _("Bidoof"),
    _("Bibarel"),
    _("Kricketot"),
    _("Kricketune"),
    _("Shinx"),
    _("Luxio"),
    _("Luxray"),
    _("Budew"),
    _("Roserade"),
    _("Cranidos"),
    _("Rampardos"),
    _("Shieldon"),
    _("Bastiodon"),
    _("Burmy"),
    _("Wormadam"),
    _("Mothim"),
    _("Combee"),
    _("Vespiquen"),
    _("Pachirisu"),
    _("Buizel"),
    _("Floatzel"),
    _("Cherubi"),
    _("Cherrim"),
    _("Shellos"),
    _("Gastrodon"),
    _("Ambipom"),
    _("Drifloon"),
    _("Drifblim"),
    _("Buneary"),
    _("Lopunny"),
    _("Mismagius"),
    _("Honchkrow"),
    _("Glameow"),
    _("Purugly"),
    _("Chingling"),
    _("Stunky"),
    _("Skuntank"),
    _("Bronzor"),
    _("Bronzong"),
    _("Bonsly"),
    _("Mime Jr."),
    _("Happiny"),
    _("Chatot"),
    _("Spiritomb"),
    _("Gible"),
    _("Gabite"),
    _("Garchomp"),
    _("Munchlax"),
    _("Riolu"),
    _("Lucario"),
    _("Hippopotas"),
    _("Hippowdon"),
    _("Skorupi"),
    _("Drapion"),
    _("Croagunk"),
    _("Toxicroak"),
    _("Carnivine"),
    _("Finneon"),
    _("Lumineon"),
    _("Mantyke"),
    _("Snover"),
    _("Abomasnow"),
    _("Weavile"),
    _("Magnezone"),
    _("Lickilicky"),
    _("Rhyperior"),
    _("Tangrowth"),
    _("Electivire"),
    _("Magmortar"),
    _("Togekiss"),
    _("Yanmega"),
    _("Leafeon"),
    _("Glaceon"),
    _("Gliscor"),
    _("Mamoswine"),
    _("Porygon-Z"),
    _("Gallade"),
    _("Probopass"),
    _("Dusknoir"),
    _("Froslass"),
    _("Rotom"),
    _("Uxie"),
    _("Mesprit"),
    _("Azelf"),
    _("Dialga"),
    _("Palkia"),
    _("Heatran"),
    _("Regigigas"),
    _("Giratina"),
    _("Cresselia"),
    _("Phione"),
    _("Manaphy"),
    _("Darkrai"),
    _("Shaymin"),
    _("Arceus"),
    _("Victini"),
    _("Snivy"),
    _("Servine"),
    _("Serperior"),
    _("Tepig"),
    _("Pignite"),
    _("Emboar"),
    _("Oshawott"),
    _("Dewott"),
    _("Samurott"),
    _("Patrat"),
    _("Watchog"),
    _("Lillipup"),
    _("Herdier"),
    _("Stoutland"),
    _("Purrloin"),
    _("Liepard"),
    _("Pansage"),
    _("Simisage"),
    _("Pansear"),
    _("Simisear"),
    _("Panpour"),
    _("Simipour"),
    _("Munna"),
    _("Musharna"),
    _("Pidove"),
    _("Tranquill"),
    _("Unfezant"),
    _("Blitzle"),
    _("Zebstrika"),
    _("Roggenrola"),
    _("Boldore"),
    _("Gigalith"),
    _("Woobat"),
    _("Swoobat"),
    _("Drilbur"),
    _("Excadrill"),
    _("Audino"),
    _("Timburr"),
    _("Gurdurr"),
    _("Conkeldurr"),
    _("Tympole"),
    _("Palpitoad"),
    _("Seismitoad"),
    _("Throh"),
    _("Sawk"),
    _("Sewaddle"),
    _("Swadloon"),
    _("Leavanny"),
    _("Venipede"),
    _("Whirlipede"),
    _("Scolipede"),
    _("Cottonee"),
    _("Whimsicott"),
    _("Petilil"),
    _("Lilligant"),
    _("Basculin"),
    _("Sandile"),
    _("Krokorok"),
    _("Krookodile"),
    _("Darumaka"),
    _("Darmanitan"),
    _("Maractus"),
    _("Dwebble"),
    _("Crustle"),
    _("Scraggy"),
    _("Scrafty"),
    _("Sigilyph"),
    _("Yamask"),
    _("Cofagrigus"),
    _("Tirtouga"),
    _("Carracosta"),
    _("Archen"),
    _("Archeops"),
    _("Trubbish"),
    _("Garbodor"),
    _("Zorua"),
    _("Zoroark"),
    _("Minccino"),
    _("Cinccino"),
    _("Gothita"),
    _("Gothorita"),
    _("Gothitelle"),
    _("Solosis"),
    _("Duosion"),
    _("Reuniclus"),
    _("Ducklett"),
    _("Swanna"),
    _("Vanillite"),
    _("Vanillish"),
    _("Vanilluxe"),
    _("Deerling"),
    _("Sawsbuck"),
    _("Emolga"),
    _("Karrablast"),
    _("Escavalier"),
    _("Foongus"),
    _("Amoonguss"),
    _("Frillish"),
    _("Jellicent"),
    _("Alomomola"),
    _("Joltik"),
    _("Galvantula"),
    _("Ferroseed"),
    _("Ferrothorn"),
    _("Klink"),
    _("Klang"),
    _("Klinklang"),
    _("Tynamo"),
    _("Eelektrik"),
    _("Eelektross"),
    _("Elgyem"),
    _("Beheeyem"),
    _("Litwick"),
    _("Lampent"),
    _("Chandelure"),
    _("Axew"),
    _("Fraxure"),
    _("Haxorus"),
    _("Cubchoo"),
    _("Beartic"),
    _("Cryogonal"),
    _("Shelmet"),
    _("Accelgor"),
    _("Stunfisk"),
    _("Mienfoo"),
    _("Mienshao"),
    _("Druddigon"),
    _("Golett"),
    _("Golurk"),
    _("Pawniard"),
    _("Bisharp"),
    _("Bouffalant"),
    _("Rufflet"),
    _("Braviary"),
    _("Vullaby"),
    _("Mandibuzz"),
    _("Heatmor"),
    _("Durant"),
    _("Deino"),
    _("Zweilous"),
    _("Hydreigon"),
    _("Larvesta"),
    _("Volcarona"),
    _("Cobalion"),
    _("Terrakion"),
    _("Virizion"),
    _("Tornadus"),
    _("Thundurus"),
    _("Reshiram"),
    _("Zekrom"),
    _("Landorus"),
    _("Kyurem"),
    _("Keldeo"),
    _("Meloetta"),
    _("Genesect"),
    _("Unfezant"),
    _("Frillish"),
    _("Jellicent"),
    _("Warrior"),
    _("Burmy"),
    _("Burmy"),
    _("Wormadam"),
    _("Wormadam"),
    _("Shellos"),
    _("Gastrodon"),
    _("Rotom"),
    _("Rotom"),
    _("Rotom"),
    _("Rotom"),
    _("Rotom"),
    _("Giratina"),
    _("Shaymin"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Arceus"),
    _("Basculin"),
    _("Darmanitan"),
    _("Deerling"),
    _("Deerling"),
    _("Deerling"),
    _("Sawsbuck"),
    _("Sawsbuck"),
    _("Sawsbuck"),
    _("Hippopotas"),
    _("Hippowdon"),
    _("Meloetta"),
    _("Genesect"),
    _("Genesect"),
    _("Genesect"),
    _("Genesect"),
    _("Cherrim"),
    _("Kyurem"),
    _("Kyurem"),
    _("Tornadus"),
    _("Thundurus"),
    _("Landorus"),
    _("Keldeo"),
    _("Chespin"),
    _("Quilladin"),
    _("Chesnaught"),
    _("Fennekin"),
    _("Braixen"),
    _("Delphox"),
    _("Froakie"),
    _("Frogadier"),
    _("Greninja"),
    _("Bunnelby"),
    _("Diggersby"),
    _("Fletchling"),
    _("Fletchindr"),
    _("Talonflame"),
    _("Scatterbug"),
    _("Spewpa"),
    _("Vivillon"),
    _("Litleo"),
    _("Pyroar"),
    _("Flabébé"),
    _("Floette"),
    _("Florges"),
    _("Skiddo"),
    _("Gogoat"),
    _("Pancham"),
    _("Pangoro"),
    _("Furfrou"),
    _("Espurr"),
    _("Meowstic"),
    _("Honedge"),
    _("Doublade"),
    _("Aegislash"),
    _("Spritzee"),
    _("Aromatisse"),
    _("Swirlix"),
    _("Slurpuff"),
    _("Inkay"),
    _("Malamar"),
    _("Binacle"),
    _("Barbaracle"),
    _("Skrelp"),
    _("Dragalge"),
    _("Clauncher"),
    _("Clawitzer"),
    _("Helioptile"),
    _("Heliolisk"),
    _("Tyrunt"),
    _("Tyrantrum"),
    _("Amaura"),
    _("Aurorus"),
    _("Sylveon"),
    _("Hawlucha"),
    _("Dedenne"),
    _("Carbink"),
    _("Goomy"),
    _("Sliggoo"),
    _("Goodra"),
    _("Klefki"),
    _("Phantump"),
    _("Trevenant"),
    _("Pumpkaboo"),
    _("Gourgeist"),
    _("Bergmite"),
    _("Avalugg"),
    _("Noibat"),
    _("Noivern"),
    _("Xerneas"),
    _("Yveltal"),
    _("Zygarde"),
    _("Diancie"),
    _("Hoopa"),
    _("Hoopa"),
    _("Volcanion"),
    _("Pyroar"),
    _("Meowstic"),
    _("Aegislash"),
    _("Arceus"),
    _("Zygarde"),
    _("Zygarde"),
    _("Zygarde"),
    _("Zygarde"),
    _("Greninja"),
    _("Flabébé"),
    _("Flabébé"),
    _("Flabébé"),
    _("Flabébé"),
    _("Floette"),
    _("Floette"),
    _("Floette"),
    _("Floette"),
    _("Floette"),
    _("Florges"),
    _("Florges"),
    _("Florges"),
    _("Florges"),
    _("Pumpkaboo"),
    _("Pumpkaboo"),
    _("Pumpkaboo"),
    _("Gourgeist"),
    _("Gourgeist"),
    _("Gourgeist"),
    _("Furfrou"),
    _("Furfrou"),
    _("Furfrou"),
    _("Furfrou"),
    _("Furfrou"),
    _("Furfrou"),
    _("Furfrou"),
    _("Furfrou"),
    _("Furfrou"),
    _("Vivillon"),
    _("Venusaur"),
    _("Charizard"),
    _("Charizard"),
    _("Blastoise"),
    _("Beedrill"),
    _("Pidgeot"),
    _("Alakazam"),
    _("Slowbro"),
    _("Gengar"),
    _("Kangaskhan"),
    _("Pinsir"),
    _("Gyarados"),
    _("Aerodactyl"),
    _("Mewtwo"),
    _("Mewtwo"),
    _("Ampharos"),
    _("Steelix"),
    _("Scizor"),
    _("Heracross"),
    _("Houndoom"),
    _("Tyranitar"),
    _("Sceptile"),
    _("Blaziken"),
    _("Swampert"),
    _("Gardevoir"),
    _("Sableye"),
    _("Mawile"),
    _("Aggron"),
    _("Medicham"),
    _("Manectric"),
    _("Sharpedo"),
    _("Camerupt"),
    _("Altaria"),
    _("Banette"),
    _("Absol"),
    _("Glalie"),
    _("Salamence"),
    _("Metagross"),
    _("Latias"),
    _("Latios"),
    _("Groudon"),
    _("Kyogre"),
    _("Rayquaza"),
    _("Lopunny"),
    _("Garchomp"),
    _("Lucario"),
    _("Abomasnow"),
    _("Gallade"),
    _("Audino"),
    _("Diancie"),
    _("Dialga"),
    _("Palkia"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Vivillon"),
    _("Rowlet"),
    _("Dartrix"),
    _("Decidueye"),
    _("Litten"),
    _("Torracat"),
    _("Incineroar"),
    _("Popplio"),
    _("Brionne"),
    _("Primarina"),
    _("Pikipek"),
    _("Trumbeak"),
    _("Toucannon"),
    _("Yungoos"),
    _("Gumshoos"),
    _("Grubbin"),
    _("Charjabug"),
    _("Vikavolt"),
    _("Crabrawler"),
    _("Crabminble"),
    _("Oricorio"),
    _("Cutiefly"),
    _("Ribombee"),
    _("Rockruff"),
    _("Lycanroc"),
    _("Wishiwashi"),
    _("Mareanie"),
    _("Toxapex"),
    _("Mudbray"),
    _("Mudsdale"),
    _("Dewpider"),
    _("Araquanid"),
    _("Fomantis"),
    _("Lurantis"),
    _("Morelull"),
    _("Shiinotic"),
    _("Salandit"),
    _("Salazzle"),
    _("Stufful"),
    _("Bewear"),
    _("Bounsweet"),
    _("Steenee"),
    _("Tsareena"),
    _("Comfey"),
    _("Oranguru"),
    _("Passimian"),
    _("Wimpod"),
    _("Golisopod"),
    _("Sandygast"),
    _("Palossand"),
    _("Pyukumuku"),
    _("Type: Null"),
    _("Silvally"),
    _("Minior"),
    _("Komala"),
    _("Turtonator"),
    _("Togedemaru"),
    _("Mimikyu"),
    _("Bruxish"),
    _("Drampa"),
    _("Dhelmise"),
    _("Jangmo-o"),
    _("Hakamo-o"),
    _("Kommo-o"),
    _("Tapu Koko"),
    _("Tapu Lele"),
    _("Tapu Bulu"),
    _("Tapu Fini"),
    _("Cosmog"),
    _("Cosmoem"),
    _("Solgaleo"),
    _("Lunala"),
    _("Nihilego"),
    _("Buzzwole"),
    _("Pheromosa"),
    _("Xurkitree"),
    _("Celesteela"),
    _("Kartana"),
    _("Guzzlord"),
    _("Necrozma"),
    _("Magearna"),
    _("Marshadow"),
    _("Rattata"),
    _("Raticate"),
    _("Raichu"),
    _("Sandshrew"),
    _("Sandslash"),
    _("Vulpix"),
    _("Ninetales"),
    _("Diglett"),
    _("Dugtrio"),
    _("Meowth"),
    _("Persian"),
    _("Geodude"),
    _("Graveler"),
    _("Golem"),
    _("Grimer"),
    _("Muk"),
    _("Exeggcute"),
    _("Exeggutor"),
    _("Cubone"),
    _("Marowak"),
    _("Deoxys"),
    _("Deoxys"),
    _("Deoxys"),
    _("Oricorio"),
    _("Oricorio"),
    _("Oricorio"),
    _("Lycanroc"),
    _("Wishiwashi"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Silvally"),
    _("Minior"),
    _("Minior"),
    _("Minior"),
    _("Minior"),
    _("Minior"),
    _("Minior"),
    _("Minior"),
    _("Mimikyu"),
    _("Magearna"),
    _("Poipole"),
    _("Naganadel"),
    _("Stakataka"),
    _("Blacphalon"),
    _("Zeraora"),
    _("Necrozma"),
    _("Necrozma"),
    _("Necrozma"),
    _("Lycanroc"),
    _("Meltan"),
    _("Melmetal"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pikachu"),
    _("Pichu"),
    _("Xerneas"),
    _("Grookey"),
    _("Thwackey"),
    _("Rillaboom"),
    _("Scorbunny"),
    _("Raboot"),
    _("Cinderace"),
    _("Sobble"),
    _("Drizzile"),
    _("Inteleon"),
    _("Skwovet"),
    _("Greedent"),
    _("Rookidee"),
    _("Corvsquire"),
    _("Corvknight"),
    _("Blipbug"),
    _("Dottler"),
    _("Orbeetle"),
    _("Nickit"),
    _("Thievul"),
    _("Gossifleur"),
    _("Eldegoss"),
    _("Wooloo"),
    _("Dubwool"),
    _("Chewtle"),
    _("Drednaw"),
    _("Yamper"),
    _("Boltund"),
    _("Rolycoly"),
    _("Carkol"),
    _("Coalossal"),
    _("Applin"),
    _("Flapple"),
    _("Appletun"),
    _("Silicobra"),
    _("Sandaconda"),
    _("Cramorant"),
    _("Arrokuda"),
    _("Baraskewda"),
    _("Toxel"),
    _("Toxtricity"),
    _("Sizzlipede"),
    _("Centskorch"),
    _("Clobbopus"),
    _("Grapploct"),
    _("Sinistea"),
    _("Poltegeist"),
    _("Hatenna"),
    _("Hattrem"),
    _("Hatterene"),
    _("Impidimp"),
    _("Morgrem"),
    _("Grimmsnarl"),
    _("Obstagoon"),
    _("Perrserker"),
    _("Cursola"),
    _("Sirfetch'd"),
    _("Mr. Rime"),
    _("Runerigus"),
    _("Milcery"),
    _("Alcremie"),
    _("Falinks"),
    _("Pincurchin"),
    _("Snom"),
    _("Frosmoth"),
    _("Stonjorner"),
    _("Eiscue"),
    _("Indeedee"),
    _("Morpeko"),
    _("Cufant"),
    _("Copperajah"),
    _("Dracozolt"),
    _("Arctozolt"),
    _("Dracovish"),
    _("Arctovish"),
    _("Duraludon"),
    _("Dreepy"),
    _("Drakloak"),
    _("Dragapult"),
    _("Zacian"),
    _("Zamazenta"),
    _("Eternatus"),
    _("Kubfu"),
    _("Urshifu"),
    _("Zarude"),
    _("Regieleki"),
    _("Regidrago"),
    _("Glastrier"),
    _("Spectrier"),
    _("Calyrex"),
    _("Cramorant"),
    _("Cramorant"),
    _("Toxtricity"),
    _("Sinistea"),
    _("Poltegeist"),
    _("Alcremie"),
    _("Alcremie"),
    _("Alcremie"),
    _("Alcremie"),
    _("Alcremie"),
    _("Alcremie"),
    _("Eiscue"),
    _("Indeedee"),
    _("Morpeko"),
    _("Zacian"),
    _("Zamazenta"),
    _("Eternatus"),
    _("Urshifu"),
    _("Zarude"),
    _("Calyrex"),
    _("Calyrex"),
    _("Meowth"),
    _("Ponyta"),
    _("Rapidash"),
    _("Slowpoke"),
    _("Slowbro"),
    _("Farfetch'd"),
    _("Koffing"),
    _("Weezing"),
    _("Mr. Mime"),
    _("Articuno"),
    _("Zapdos"),
    _("Moltres"),
    _("Slowking"),
    _("Corsola"),
    _("Zigzagoon"),
    _("Linoone"),
    _("Mime Jr."),
    _("Darumaka"),
    _("Darmanitan"),
    _("Darmanitan"),
    _("Yamask"),
    _("Stunfisk"),
    _("Venusaur"),
    _("Charizard"),
    _("Blastoise"),
    _("Butterfree"),
    _("Pikachu"),
    _("Meowth"),
    _("Machamp"),
    _("Gengar"),
    _("Kingler"),
    _("Lapras"),
    _("Eevee"),
    _("Snorlax"),
    _("Garbodor"),
    _("Melmetal"),
    _("Rillaboom"),
    _("Cinderace"),
    _("Inteleon"),
    _("Corvknight"),
    _("Orbeetle"),
    _("Drednaw"),
    _("Coalossal"),
    _("Flapple"),
    _("Appletun"),
    _("Sandaconda"),
    _("Toxtricity"),
    _("Toxtricity"),
    _("Centskorch"),
    _("Hatterene"),
    _("Grimmsnarl"),
    _("Alcremie"),
    _("Copperajah"),
    _("Duraludon"),
    _("Urshifu"),
    _("Urshifu"),
};
