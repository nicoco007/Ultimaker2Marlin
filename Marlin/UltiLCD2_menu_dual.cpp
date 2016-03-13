#include "Configuration.h"
#if (EXTRUDERS > 1) && defined(ENABLE_ULTILCD2)
#include "ConfigurationStore.h"
#include "ConfigurationDual.h"
#include "planner.h"
#include "stepper.h"
#include "language.h"
#include "machinesettings.h"
#include "commandbuffer.h"
#include "tinkergnome.h"
#include "UltiLCD2_hi_lib.h"
#include "UltiLCD2_menu_utils.h"
#include "UltiLCD2_menu_maintenance.h"
#include "UltiLCD2_menu_dual.h"

void lcd_menu_dual();

static void lcd_store_dualstate()
{
    Dual_StoreState();
    menu.return_to_previous();
}

static void lcd_store_dockposition()
{
    Dual_StoreDockPosition();
    menu.return_to_previous();
}

static void lcd_store_wipeposition()
{
    Dual_StoreWipePosition();
    menu.return_to_previous();
}

static void lcd_store_extruderoffset()
{
    Dual_StoreExtruderOffset();
    Dual_StoreAddHomeingZ2();
    menu.return_to_previous();
}

static void lcd_store_tcretract()
{
    Dual_StoreRetract();
    menu.return_to_previous();
}

//////////////////

static void lcd_extruderoffset_x()
{
    lcd_tune_value(extruder_offset[X_AXIS][1], -99.99f, 99.99f, 0.01f);
}

static void lcd_extruderoffset_y()
{
    lcd_tune_value(extruder_offset[Y_AXIS][1], -99.99f, 99.99f, 0.01f);
}

static void lcd_extruderoffset_z()
{
    float zoffset = add_homeing[Z_AXIS] - add_homeing_z2;
    if (lcd_tune_value(zoffset, -10.0f, 10.0f, 0.01f))
    {
        add_homeing_z2 = add_homeing[Z_AXIS] - zoffset;
    }
}

// create menu options for "extruder offset"
static const menu_t & get_extruderoffset_menuoption(uint8_t nr, menu_t &opt)
{
    uint8_t index(0);
    if (nr == index++)
    {
        // STORE
        opt.setData(MENU_NORMAL, lcd_store_extruderoffset);
    }
    else if (nr == index++)
    {
        // RETURN
        opt.setData(MENU_NORMAL, lcd_change_to_previous_menu);
    }
    else if (nr == index++)
    {
        // x offset
        opt.setData(MENU_INPLACE_EDIT, lcd_extruderoffset_x, 8);
    }
    else if (nr == index++)
    {
        // y offset
        opt.setData(MENU_INPLACE_EDIT, lcd_extruderoffset_y, 8);
    }
    else if (nr == index++)
    {
        // z offset
        opt.setData(MENU_INPLACE_EDIT, lcd_extruderoffset_z, 8);
    }
    return opt;
}

static void drawExtruderOffsetSubmenu(uint8_t nr, uint8_t &flags)
{
    uint8_t index(0);
    char buffer[32] = {0};
    if (nr == index++)
    {
        // Store
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Store offsets"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("STORE")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // RETURN
        LCDMenu::drawMenuBox(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                           , BOTTOM_MENU_YPOS
                           , 52
                           , LCD_CHAR_HEIGHT
                           , flags);
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Click to return"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("RETURN")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // x offset
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("X offset"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(17, PSTR("X"));
        float_to_string2(extruder_offset[X_AXIS][1], buffer, PSTR("mm"));
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*3
                                , 17
                                , LCD_CHAR_SPACING*8
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // y offset
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Y offset"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(28, PSTR("Y"));
        float_to_string2(extruder_offset[Y_AXIS][1], buffer, PSTR("mm"));
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*3
                                , 28
                                , LCD_CHAR_SPACING*8
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // z offset
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Z offset"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(39, PSTR("Z"));
        float_to_string2(add_homeing[Z_AXIS] - add_homeing_z2, buffer, PSTR("mm"));
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*3
                                , 39
                                , LCD_CHAR_SPACING*8
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
}

void lcd_menu_extruderoffset()
{
    lcd_basic_screen();
    lcd_lib_draw_hline(3, 124, 13);

    menu.process_submenu(get_extruderoffset_menuoption, 5);

    uint8_t flags = 0;
    for (uint8_t index=0; index<5; ++index) {
        menu.drawSubMenu(drawExtruderOffsetSubmenu, index, flags);
    }
    if (!(flags & MENU_STATUSLINE))
    {
        lcd_lib_draw_string_leftP(5, PSTR("Extruder offset"));
    }

    lcd_lib_update_screen();
}

//////////////////

static void lcd_dockposition_x()
{
    lcd_tune_value(dock_position[X_AXIS], 0.0f, max_pos[X_AXIS], 0.1f);
}

static void lcd_dockposition_y()
{
    lcd_tune_value(dock_position[Y_AXIS], 0.0f, max_pos[Y_AXIS], 0.1f);
}

// create menu options for "axis steps/mm"
static const menu_t & get_dockposition_menuoption(uint8_t nr, menu_t &opt)
{
    uint8_t index(0);
    if (nr == index++)
    {
        // STORE
        opt.setData(MENU_NORMAL, lcd_store_dockposition);
    }
    else if (nr == index++)
    {
        // RETURN
        opt.setData(MENU_NORMAL, lcd_change_to_previous_menu);
    }
    else if (nr == index++)
    {
        // x pos
        opt.setData(MENU_INPLACE_EDIT, lcd_dockposition_x, 2);
    }
    else if (nr == index++)
    {
        // y pos
        opt.setData(MENU_INPLACE_EDIT, lcd_dockposition_y, 2);
    }
    return opt;
}

static void drawDockPositionSubmenu(uint8_t nr, uint8_t &flags)
{
    uint8_t index(0);
    char buffer[32] = {0};
    if (nr == index++)
    {
        // Store
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Store dock position"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("STORE")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // RETURN
        LCDMenu::drawMenuBox(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                           , BOTTOM_MENU_YPOS
                           , 52
                           , LCD_CHAR_HEIGHT
                           , flags);
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Click to return"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("RETURN")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // x position
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("dock position x"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(20, PSTR("X"));
        float_to_string2(dock_position[X_AXIS], buffer, PSTR("mm"));
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*3
                                , 20
                                , LCD_CHAR_SPACING*8
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // y position
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("dock position y"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(32, PSTR("Y"));
        float_to_string2(dock_position[Y_AXIS], buffer, PSTR("mm"));
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*3
                                , 32
                                , LCD_CHAR_SPACING*8
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
}

static void lcd_menu_dockposition()
{
    lcd_basic_screen();
    lcd_lib_draw_hline(3, 124, 13);

    menu.process_submenu(get_dockposition_menuoption, 4);

    uint8_t flags = 0;
    for (uint8_t index=0; index<4; ++index) {
        menu.drawSubMenu(drawDockPositionSubmenu, index, flags);
    }
    if (!(flags & MENU_STATUSLINE))
    {
        lcd_lib_draw_string_leftP(5, PSTR("Docking position"));
    }

    lcd_lib_update_screen();
}

//////////////////

static void lcd_wipeposition_x()
{
    lcd_tune_value(wipe_position[X_AXIS], 0.0f, max_pos[X_AXIS], 0.1f);
}

static void lcd_wipeposition_y()
{
    lcd_tune_value(wipe_position[Y_AXIS], 0.0f, max_pos[Y_AXIS], 0.1f);
}

// create menu options for "axis steps/mm"
static const menu_t & get_wipeposition_menuoption(uint8_t nr, menu_t &opt)
{
    uint8_t index(0);
    if (nr == index++)
    {
        // STORE
        opt.setData(MENU_NORMAL, lcd_store_wipeposition);
    }
    else if (nr == index++)
    {
        // RETURN
        opt.setData(MENU_NORMAL, lcd_change_to_previous_menu);
    }
    else if (nr == index++)
    {
        // x pos
        opt.setData(MENU_INPLACE_EDIT, lcd_wipeposition_x, 2);
    }
    else if (nr == index++)
    {
        // y pos
        opt.setData(MENU_INPLACE_EDIT, lcd_wipeposition_y, 2);
    }
    return opt;
}

static void drawWipePositionSubmenu(uint8_t nr, uint8_t &flags)
{
    uint8_t index(0);
    char buffer[32] = {0};
    if (nr == index++)
    {
        // Store
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Store wipe position"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("STORE")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // RETURN
        LCDMenu::drawMenuBox(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                           , BOTTOM_MENU_YPOS
                           , 52
                           , LCD_CHAR_HEIGHT
                           , flags);
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Click to return"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("RETURN")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // x position
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("wipe position x"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(20, PSTR("X"));
        float_to_string2(wipe_position[X_AXIS], buffer, PSTR("mm"));
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*3
                                , 20
                                , LCD_CHAR_SPACING*8
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // y position
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("wipe position y"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(32, PSTR("Y"));
        float_to_string2(wipe_position[Y_AXIS], buffer, PSTR("mm"));
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*3
                                , 32
                                , LCD_CHAR_SPACING*8
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
}

static void lcd_menu_wipeposition()
{
    lcd_basic_screen();
    lcd_lib_draw_hline(3, 124, 13);

    menu.process_submenu(get_wipeposition_menuoption, 4);

    uint8_t flags = 0;
    for (uint8_t index=0; index<4; ++index) {
        menu.drawSubMenu(drawWipePositionSubmenu, index, flags);
    }
    if (!(flags & MENU_STATUSLINE))
    {
        lcd_lib_draw_string_leftP(5, PSTR("Wipe position"));
    }

    lcd_lib_update_screen();
}

//////////////////

static void lcd_tune_tcretractlen()
{
    lcd_tune_value(toolchange_retractlen[menu_extruder], 0.0f, 50.0, 0.1f);
}

static void lcd_tune_tcretractfeed()
{
    lcd_tune_value(toolchange_retractfeedrate[menu_extruder], 0.0f, max_feedrate[E_AXIS]*60, 60.0f);
}

static void lcd_tune_tcprime()
{
    lcd_tune_value(toolchange_prime[menu_extruder], 0.0f, 20.0f, 0.1f);
}

// create menu options for "axis steps/mm"
static const menu_t & get_tcretract_menuoption(uint8_t nr, menu_t &opt)
{
    uint8_t index(0);
    if (nr == index++)
    {
        // STORE
        opt.setData(MENU_NORMAL, lcd_store_tcretract);
    }
    else if (nr == index++)
    {
        // RETURN
        opt.setData(MENU_NORMAL, lcd_change_to_previous_menu);
    }
    else if (nr == index++)
    {
        // nozzle 1 retract len
        opt.setData(MENU_INPLACE_EDIT, lcd_tune_tcretractlen, 2);
    }
    else if (nr == index++)
    {
        // nozzle 1 retract feedrate
        opt.setData(MENU_INPLACE_EDIT, lcd_tune_tcretractfeed, 2);
    }
    else if (nr == index++)
    {
        // nozzle 2 retract len
        opt.setData(MENU_INPLACE_EDIT, lcd_tune_tcprime, 2);
    }
    return opt;
}

static void drawTCRetractSubmenu(uint8_t nr, uint8_t &flags)
{
    uint8_t index(0);
    char buffer[32] = {0};
    if (nr == index++)
    {
        // Store
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Store retract values"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("STORE")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // RETURN
        LCDMenu::drawMenuBox(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                           , BOTTOM_MENU_YPOS
                           , 52
                           , LCD_CHAR_HEIGHT
                           , flags);
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Click to return"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("RETURN")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // retract len
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Retract len (mm)"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(17, PSTR("Len"));
        float_to_string2(toolchange_retractlen[menu_extruder], buffer, NULL);
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*7
                                , 17
                                , LCD_CHAR_SPACING*5
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // retract feedrate
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Retract speed (mm/s)"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(27, PSTR("Speed"));
        float_to_string2(toolchange_retractfeedrate[menu_extruder]/60, buffer, NULL);
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*7
                                , 27
                                , LCD_CHAR_SPACING*5
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // extra prime len
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Extra priming (mm)"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(37, PSTR("Prime"));
        float_to_string2(toolchange_prime[menu_extruder], buffer, NULL);
        LCDMenu::drawMenuString(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*7
                                , 37
                                , LCD_CHAR_SPACING*5
                                , LCD_CHAR_HEIGHT
                                , buffer
                                , ALIGN_RIGHT | ALIGN_VCENTER
                                , flags);
    }
}

void lcd_menu_tune_tcretract()
{
    lcd_basic_screen();
    lcd_lib_draw_hline(3, 124, 13);

    char buffer[4] = {0};
    strcpy_P(buffer, PSTR("("));
    int_to_string(menu_extruder+1, buffer+1, PSTR(")"));
    lcd_lib_draw_string(LCD_GFX_WIDTH-LCD_CHAR_MARGIN_RIGHT-LCD_CHAR_SPACING*3, 17, buffer);


    menu.process_submenu(get_tcretract_menuoption, 5);

    uint8_t flags = 0;
    for (uint8_t index=0; index<5; ++index) {
        menu.drawSubMenu(drawTCRetractSubmenu, index, flags);
    }
    if (!(flags & MENU_STATUSLINE))
    {
        lcd_lib_draw_string_leftP(5, PSTR("Toolchange retract"));
    }

    lcd_lib_update_screen();
}

//////////////////

static void lcd_toggle_dual()
{
    dual_state ^= DUAL_ENABLED;
}

static void lcd_toggle_toolchange()
{
    dual_state ^= DUAL_TOOLCHANGE;
}

static void lcd_toggle_wipe()
{
    dual_state ^= DUAL_WIPE;
}

// create menu options for "axis steps/mm"
static const menu_t & get_dualstate_menuoption(uint8_t nr, menu_t &opt)
{
    uint8_t index(0);
    if (nr == index++)
    {
        // STORE
        opt.setData(MENU_NORMAL, lcd_store_dualstate);
    }
    else if (nr == index++)
    {
        // RETURN
        opt.setData(MENU_NORMAL, lcd_change_to_previous_menu);
    }
    else if (nr == index++)
    {
        // enable dual mode
        opt.setData(MENU_NORMAL, lcd_toggle_dual);
    }
    else if (nr == index++)
    {
        // enable toolchange scripts
        opt.setData(MENU_NORMAL, lcd_toggle_toolchange);
    }
    else if (nr == index++)
    {
        // enable wipe script
        opt.setData(MENU_NORMAL, lcd_toggle_wipe);
    }
    return opt;
}

static void drawDualStateSubmenu(uint8_t nr, uint8_t &flags)
{
    uint8_t index(0);
    if (nr == index++)
    {
        // Store
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Store dual mode"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("STORE")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // RETURN
        LCDMenu::drawMenuBox(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                           , BOTTOM_MENU_YPOS
                           , 52
                           , LCD_CHAR_HEIGHT
                           , flags);
        if (flags & MENU_SELECTED)
        {
            lcd_lib_draw_string_leftP(5, PSTR("Click to return"));
            flags |= MENU_STATUSLINE;
        }
        LCDMenu::drawMenuString_P(LCD_GFX_WIDTH/2 + 2*LCD_CHAR_MARGIN_LEFT
                                , BOTTOM_MENU_YPOS
                                , 52
                                , LCD_CHAR_HEIGHT
                                , PSTR("RETURN")
                                , ALIGN_CENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // dual mode
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Dual mode"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(17, PSTR("Dual mode:"));
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*13
                                , 17
                                , LCD_CHAR_SPACING*7
                                , LCD_CHAR_HEIGHT
                                , IS_DUAL_ENABLED ? PSTR("enabled") : PSTR("off")
                                , ALIGN_LEFT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // wipe device
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Toolchange script"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(28, PSTR("Toolchange:"));
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*13
                                , 28
                                , LCD_CHAR_SPACING*7
                                , LCD_CHAR_HEIGHT
                                , IS_TOOLCHANGE_ENABLED ? PSTR("enabled") : PSTR("off")
                                , ALIGN_LEFT | ALIGN_VCENTER
                                , flags);
    }
    else if (nr == index++)
    {
        // wipe device
        if ((flags & MENU_ACTIVE) | (flags & MENU_SELECTED))
        {
            lcd_lib_draw_string_leftP(5, PSTR("Wipe device"));
            flags |= MENU_STATUSLINE;
        }
        lcd_lib_draw_string_leftP(39, PSTR("Wipe device:"));
        LCDMenu::drawMenuString_P(LCD_CHAR_MARGIN_LEFT+LCD_CHAR_SPACING*13
                                , 39
                                , LCD_CHAR_SPACING*7
                                , LCD_CHAR_HEIGHT
                                , IS_WIPE_ENABLED ? PSTR("enabled") : PSTR("off")
                                , ALIGN_LEFT | ALIGN_VCENTER
                                , flags);
    }
}

static void lcd_menu_dualstate()
{
    lcd_basic_screen();
    lcd_lib_draw_hline(3, 124, 13);

    uint8_t len = IS_DUAL_ENABLED ? 5 : 3;
    menu.process_submenu(get_dualstate_menuoption, len);

    uint8_t flags = 0;
    for (uint8_t index=0; index<len; ++index) {
        menu.drawSubMenu(drawDualStateSubmenu, index, flags);
    }
    if (!(flags & MENU_STATUSLINE))
    {
        lcd_lib_draw_string_leftP(5, PSTR("Dual state"));
    }

    lcd_lib_update_screen();
}

//////////////////

void switch_extruder(uint8_t newExtruder, bool moveZ)
{
    if (newExtruder != active_extruder)
    {
        st_synchronize();
        if (!(position_state & (KNOWNPOS_X | KNOWNPOS_Y)))
        {
            // home head
            enquecommand_P(PSTR("G28 X0 Y0"));
            cmd_synchronize();
            st_synchronize();
        }
        changeExtruder(newExtruder, moveZ);
        SERIAL_ECHO_START;
        SERIAL_ECHOPGM(MSG_ACTIVE_EXTRUDER);
        SERIAL_PROTOCOLLN((int)active_extruder);
    }
}

static void lcd_switch_extruder()
{
    switch_extruder(menu_extruder, false);
}


FORCE_INLINE static void lcd_dual_switch_extruder()
{
    lcd_select_nozzle(lcd_switch_extruder, NULL);
}

//////////////////

static void lcd_dual_item(uint8_t nr, uint8_t offsetY, uint8_t flags)
{
    uint8_t index(0);
    char buffer[32] = {0};

    if (nr == index++)
        strcpy_P(buffer, PSTR("< RETURN"));
    else if (nr == index++)
        strcpy_P(buffer, PSTR("Dual mode"));
    else if (nr == index++)
        strcpy_P(buffer, PSTR("Change extruder"));
    else if (nr == index++)
        strcpy_P(buffer, PSTR("Toolchange retract"));
    else if (nr == index++)
        strcpy_P(buffer, PSTR("Extruder offset"));
    else if (nr == index++)
        strcpy_P(buffer, PSTR("Docking position"));
    else if (nr == index++)
        strcpy_P(buffer, PSTR("Wipe position"));
    else if (nr == index++)
    {
        strcpy_P(buffer, PSTR("Adjust Z (nozzle "));
        int_to_string(active_extruder+1, buffer+strlen(buffer), PSTR(")"));
    }
    else
        strcpy_P(buffer, PSTR("???"));

    lcd_draw_scroll_entry(offsetY, buffer, flags);
}

static void lcd_dual_details(uint8_t nr)
{
    if (nr == 1)
    {
        // dual mode
        lcd_lib_draw_string_leftP(BOTTOM_MENU_YPOS, IS_DUAL_ENABLED ? PSTR("enabled") : PSTR("off"));
    }
}

static void start_menu_tcretract()
{
    menu.add_menu(menu_t(lcd_menu_tune_tcretract, MAIN_MENU_ITEM_POS(1)));
}

static void lcd_menu_tcretraction()
{
    lcd_select_nozzle(start_menu_tcretract, NULL);
}

void lcd_menu_dual()
{
    lcd_scroll_menu(PSTR("Dual extrusion"), 8, lcd_dual_item, lcd_dual_details);
    if (lcd_lib_button_pressed)
    {
        if (IS_SELECTED_SCROLL(0))
            menu.return_to_previous();
        else if (IS_SELECTED_SCROLL(1))
            menu.add_menu(menu_t(lcd_menu_dualstate, MAIN_MENU_ITEM_POS(1)));
        else if (IS_SELECTED_SCROLL(2))
            menu.add_menu(menu_t(lcd_dual_switch_extruder, MAIN_MENU_ITEM_POS(active_extruder ? 1 : 0)));
        else if (IS_SELECTED_SCROLL(3))
            menu.add_menu(menu_t(lcd_menu_tcretraction, MAIN_MENU_ITEM_POS(menu_extruder)));
        else if (IS_SELECTED_SCROLL(4))
            menu.add_menu(menu_t(lcd_menu_extruderoffset, MAIN_MENU_ITEM_POS(1)));
        else if (IS_SELECTED_SCROLL(5))
            menu.add_menu(menu_t(lcd_menu_dockposition, MAIN_MENU_ITEM_POS(1)));
        else if (IS_SELECTED_SCROLL(6))
            menu.add_menu(menu_t(lcd_menu_wipeposition, MAIN_MENU_ITEM_POS(1)));
        else if (IS_SELECTED_SCROLL(7))
        {
            lcd_prepare_buildplate_adjust();
            menu.add_menu(menu_t(lcd_menu_simple_buildplate_init, ENCODER_NO_SELECTION));
        }
    }
    lcd_lib_update_screen();
}

void lcd_select_nozzle(menuFunc_t callbackOnSelect, menuFunc_t callbackOnAbort)
{
    lcd_tripple_menu(PSTR("EXTRUDER|1"), PSTR("EXTRUDER|2"), PSTR("RETURN"));

    if (lcd_lib_button_pressed)
    {
        uint8_t index(SELECTED_MAIN_MENU_ITEM());
        if (index < 2)
        {
            menu_extruder = index;
            if (callbackOnSelect) callbackOnSelect();
        }
        else
        {
            if (callbackOnAbort)
                callbackOnAbort();
            else
                menu.return_to_previous();
        }
    }

    lcd_lib_update_screen();
}

#endif//EXTRUDERS
