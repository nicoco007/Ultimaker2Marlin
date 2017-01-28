#ifndef POWERBUDGET_H
#define POWERBUDGET_H

// default values for wattage distribution
#define DEFAULT_POWER_BUDGET     160
#define DEFAULT_POWER_EXTRUDER    35
#define DEFAULT_POWER_BUILDPLATE 120

// min/max allowed input value
#define POWER_MINVALUE             0
#define POWER_MAXVALUE           999

extern uint16_t power_budget;
extern uint16_t power_buildplate;
extern uint16_t power_extruder[EXTRUDERS];

void PowerBudget_RetrieveSettings();

#ifdef ENABLE_ULTILCD2
// menu function
void lcd_menu_powerbudget();
#endif

#endif //POWERBUDGET_H
