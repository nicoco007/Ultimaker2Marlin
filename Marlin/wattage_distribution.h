#ifndef WATTAGEDIST_H
#define WATTAGEDIST_H

// default values for wattage distribution for all but um2go
#ifndef DEFAULT_WATTAGE_BUDGET
    #define DEFAULT_WATTAGE_BUDGET     190
#endif

#ifndef DEFAULT_WATTAGE_EXTRUDER
    #define DEFAULT_WATTAGE_EXTRUDER    35
#endif

#ifndef DEFAULT_WATTAGE_BUILDPLATE
    #define DEFAULT_WATTAGE_BUILDPLATE 150
#endif

// min/max allowed input value
#define WATTAGE_MINVALUE             0
#define WATTAGE_MAXVALUE           999

extern uint16_t wattage_budget;
extern uint16_t wattage_buildplate;
extern uint16_t wattage_extruder[EXTRUDERS];

void Wattage_RetrieveSettings();

#ifdef ENABLE_ULTILCD2
// menu function
void lcd_menu_wattage();
#endif

#endif //ULTILCD2_MENU_PREFS_H
