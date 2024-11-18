#include "globals.h"

#ifdef USE_EU_NUTRITIONFACT
#if defined(COUNTRY_POLAND) || defined(USE_POLAND_STOKROTKA_MART) || defined(USE_POLAND_LOGISTICS)
#ifdef USE_POL_NUTRITIONFACT_ENG_CAP
char eu_nutrition_menu_caption[39][21] = {
	"Energy",
	"   Fat",
	"       -saturated",
	"-mono-unsaturated",
	" -polyunsatureted",
	"Carbohydrate    ",
	"         -sugars",
	"        -polyols",
	"         -starch",
	"Fibre           ",
	"Protein         ",
	"Salt            ",
	"Vitamin A       ",
	"Vitamin D       ",
	"Vitamin E       ",
	"Vitamin K       ",
	"Vitamin C       ",
	"Thiamin         ",
	"Riboflavin      ",
	"Niacin          ",
	"Vitamin B6      ",
	"Folic acid      ",
	"Vitamin B12     ",
	"Biotin	         ",
	"Pantothenic acid",
	"Potassium       ",
	"Chloride        ",
	"Calcium         ",
	"Phosphorus      ",
	"Magnesium       ",
	"Iron            ",
	"Zinc            ",
	"Copper          ",
	"Manganese       ",
	"Fluoride        ",
	"Selenium        ",
	"Chromium        ",
	"Molybdenum      ",
	"Iodine          "
};

char eu_nutrition_print_caption[39][21] = {
	"Energy",
	"Fat",
	"-saturated",
	"-mono-unsaturated",
	"-polyunsatureted",
	"Carbohydrate",
	"-sugars",
	"-polyols",
	"-starch",
	"Fibre",
	"Protein",
	"Salt",
	"Vitamin A",
	"Vitamin D",
	"Vitamin E",
	"Vitamin K",
	"Vitamin C",
	"Thiamin",
	"Riboflavin",
	"Niacin",
	"Vitamin B6",
	"Folic acid",
	"Vitamin B12",
	"Biotin",
	"Pantothenic acid",
	"Potassium",
	"Chloride",
	"Calcium",
	"Phosphorus",
	"Magnesium",
	"Iron",
	"Zinc",
	"Copper",
	"Manganese",
	"Fluoride",
	"Selenium",
	"Chromium",
	"Molybdenum",
	"Iodine"
};

char eu_nutrition_print_nutrition_facts[] = "Nutrition facts:";
char eu_nutrition_print_serving_size[] = "Serving size";
char eu_nutrition_print_serving_per_container[] = "serving size per container";
char eu_nutrition_print_of_which[] = "of which";
char eu_nutrition_print_fat_prefix[] = "fat";
char eu_nutrition_print_vitamins_and_minerals[] = "Vitamins and Minerals:";
char eu_nutrition_print_per[] = "per:";
char eu_nutrition_print_per_portion[] = "per portion:";
char eu_nutrition_print_percent_ri[] = "%DV";

#else//USE_POL_NUTRITIONFACT_ENG_CAP
char eu_nutrition_menu_caption[39][8] = {
    "Energia",				//"Enegy",
	"Tluszcz",				//"Fat",
	"nasyc. ",				//"saturat",
	"jedno n",				//"mono-unsaturates",
	"wielo n",				//"-polyunsaturetes",
	"Weglow ",				//"Carbohydrate",
	"cukry  ",				//"-sugars",
	"poliole",				//"-polyols",
	"skrobia",				//"-starch",
	"Blonnik",				//"Fibre",
	"Bialko ",				//"Protein",
	"Sól    ",					//"Salt",
	"Wit. A ",				//"Vit-A",
	"Wit. D ",				//"Vit-D",
	"Wit. E ",				//"Vit-E",
	"Wit. K ",				//"Vit-K",
	"Wit. C ",				//"Vit-C",
	"Tiamina",				//"Thiamin",
	"Rybofl.",				//"Riboflavin",
	"Niacyna",				//"Niacin",
	"Wit. B6",				//"Vit-B6",
	"Foliowy",				//"Folic acid",
	"Wit.B12",				//"Vit-B12",
	"Biotyna",				//"Biotin",
	"Pantot.",				//"Panthothenic acid",
	"Potas  ",				//"Potassium",
	"Chlor  ",				//"Chloride",
	"Wapn   ",					//"Calcium",
	"Fosfor ",				//"Phosphorus",
	"Magnez ",				//"Magnesium",
	"Zelazo ",				//"Iron",
	"Cynk   ",					//"Zinc",
	"Miedz  ",				//"Copper",
	"Mangan ",				//"Manganese",
	"Fluor  ",				//"Fluoride",
	"Selen  ",				//"Selenium",
	"Chrom  ",				//"Chromium",
	"Molib. ",				//"Molybdenum",
	"Jod    "					//"Iodine"
};

char eu_nutrition_print_caption[39][21] = {
	"Wartoœæ energetyczna",	//"Enegy",
	"T³uszcz",				//"Fat",
	"nasycone",				//"-saturates",
	"jednonienasycone",		//"-mono-unsaturates",
	"wielonienasycone",		//"-polyunsaturetes",
	"Wêglowodany",			//"Carbohydrate",
	"cukry",				//"-sugars",
	"poliole",				//"-polyols",
	"skrobia",				//"-starch",
	"B³onnik",				//"Fibre",
	"Bia³ko",				//"Protein",
	"Sól",					//"Salt",
	"Witamina A",			//"Vitamin A",
	"Witamina D",			//"Vitamin D",
	"Witamina E",			//"Vitamin E",
	"Witamina K",			//"Vitamin K",
	"Witamina C",			//"Vit-C",
	"Tiamina",				//"Thiamin",
	"Ryboflawina",			//"Riboflavin",
	"Niacyna",				//"Niacin",
	"Witamina B6",			//"Vit-B6",
	"Kwas Foliowy",			//"Folic acid",
	"Witamina B12",			//"Vit-B12",
	"Biotyna",				//"Biotin",
	"Kwas Pantotenowy",		//"Panthothenic acid",
	"Potas",				//"Potassium",
	"Chlor",				//"Chloride",
	"Wapñ",					//"Calcium",
	"Fosfor",				//"Phosphorus",
	"Magnez",				//"Magnesium",
	"¯elazo",				//"Iron",
	"Cynk",					//"Zinc",
	"MiedŸ",				//"Copper",
	"Mangan",				//"Manganese",
	"Fluor",				//"Fluoride",
	"Selen",				//"Selenium",
	"Chrom",				//"Chromium",
	"Molibden",				//"Molybdenum",
	"Jod"					//"Iodine"
};

char eu_nutrition_print_nutrition_facts[] = "Wartoœæ od¿ywcza:";
char eu_nutrition_print_serving_size[] = "1 porcja =";
char eu_nutrition_print_serving_per_container[] = "Iloœæ porcji w opak.";
char eu_nutrition_print_of_which[] = "w tym";
char eu_nutrition_print_fat_prefix[] = "Kwasy t³uszczowe";
char eu_nutrition_print_vitamins_and_minerals[] = "Witaminy i sk³adniki mineralne:";
char eu_nutrition_print_per[] = "W:";
char eu_nutrition_print_per_portion[] = "W porcji:";
char eu_nutrition_print_percent_ri[] = "%RWS";
#endif
#endif
#endif
