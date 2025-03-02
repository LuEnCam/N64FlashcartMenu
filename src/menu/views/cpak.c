#include <stdbool.h>
#include <stdio.h>
#include <libdragon.h>
#include "views.h"
#include "../sound.h"
#include "../fonts.h"

#define ACCESSORY_BIO_SENSOR 5

short controller_selected;

static bool is_editing_mode;
bool has_mem;
bool has_rumble;
bool has_transfert;
bool has_bio_sensor;

typedef enum {
    COL_DEFAULT,
    COL_YELLOW,
    COL_GREEN,
    COL_BLUE,
    COL_ORANGE,
    COL_GRAY,
} style_colors_t;

/*
for (int i = 0; i < LIST_ENTRIES; i++) {
    int entry_index = starting_position + i;

    entry_t *entry = &list[entry_index];

    menu_font_style_t style;

    switch (entry->type) {
        case ENTRY_TYPE_DIR: style = STL_YELLOW; break;
        case ENTRY_TYPE_ROM: style = STL_DEFAULT; break;
        case ENTRY_TYPE_DISK: style = STL_DEFAULT; break;
        case ENTRY_TYPE_EMULATOR: style = STL_DEFAULT; break;
        case ENTRY_TYPE_SAVE: style = STL_GREEN; break;
        case ENTRY_TYPE_IMAGE: style = STL_BLUE; break;
        case ENTRY_TYPE_MUSIC: style = STL_BLUE; break;
        case ENTRY_TYPE_TEXT: style = STL_ORANGE; break;
        case ENTRY_TYPE_OTHER: style = STL_GRAY; break;
        default: style = STL_GRAY; break;
    }

    rdpq_paragraph_builder_style(style);

    if (entry->type == ENTRY_TYPE_DIR) {
        rdpq_paragraph_builder_span(dir_prefix, strlen(dir_prefix));
    }

    rdpq_paragraph_builder_span(entry->name, name_lengths[i]);

    if ((entry_index + 1) >= entries) {
        break;
    }

    rdpq_paragraph_builder_newline();
}
*/





bool check_accessories(int port) {
    
    joypad_accessory_type_t val =  joypad_get_accessory_type(controller_selected);

    has_rumble    = val == JOYPAD_ACCESSORY_TYPE_RUMBLE_PAK;
    has_transfert = val == JOYPAD_ACCESSORY_TYPE_TRANSFER_PAK;
    has_mem       = val == JOYPAD_ACCESSORY_TYPE_CONTROLLER_PAK;
    has_bio_sensor = val == JOYPAD_ACCESSORY_TYPE_BIO_SENSOR;

    return has_rumble || has_transfert || has_bio_sensor || has_mem;
}




static void process (menu_t *menu) {
    if(menu->actions.go_c_left) {
        sound_play_effect(SFX_SETTING);
        controller_selected = ((controller_selected - 1) + 4) % 4;
    } 
    else if (menu->actions.go_c_right) {
        sound_play_effect(SFX_SETTING);
        controller_selected = ((controller_selected + 1) + 4) % 4;
    }

    if (menu->actions.back && !is_editing_mode) {
        sound_play_effect(SFX_EXIT);
        menu->next_mode = MENU_MODE_BROWSER;
    }

    check_accessories(controller_selected);

    /*
    else if (menu->actions.enter && !is_editing_mode && menu->current_time >= 0) {
        rtc_tm = *gmtime(&menu->current_time);
        is_editing_mode = true;
    }
    
    
    if (is_editing_mode) {
        if (menu->actions.go_left) {
            if ( editing_field_type <= RTC_EDIT_YEAR ) { editing_field_type = RTC_EDIT_SEC; }
            else { editing_field_type = editing_field_type - 1; }
        }
        else if (menu->actions.go_right) {
            if ( editing_field_type >= RTC_EDIT_SEC ) { editing_field_type = RTC_EDIT_YEAR; }
            else { editing_field_type = editing_field_type + 1; }
        }
        else if (menu->actions.go_up) {
            adjust_rtc_time( &rtc_tm, +1 );
        }
        else if (menu->actions.go_down) {
            adjust_rtc_time( &rtc_tm, -1 );
        }
        else if (menu->actions.options) { // R button = save
            if(rtc_is_writable()) {
                // FIXME: settimeofday is not available in libdragon yet.
                // struct timeval new_time = { .tv_sec = mktime(&rtc_tm) };
                // int res = settimeofday(&new_time, NULL);

                rtc_time_t rtc_time = rtc_time_from_tm(&rtc_tm);
                int res = rtc_set(&rtc_time);
                if (res != 1) {
                    menu_show_error(menu, "Failed to set RTC time");
                }
            }
            else {
                menu_show_error(menu, "RTC is not writable");
            }
            is_editing_mode = false;
        }
        else if (menu->actions.back) { // cancel
            is_editing_mode = false;
        }
    }
    */
}

static void draw (menu_t *menu, surface_t *d) {
    rdpq_attach(d, NULL);

    ui_components_background_draw();

    ui_components_layout_draw();

    if (!is_editing_mode) {
         if( menu->current_time >= 0 ) {

            ui_components_main_text_draw(STL_DEFAULT,
                ALIGN_CENTER, VALIGN_TOP,
                "CONTROLLER PAK MANAGEMENT\n"
                "\n"
                "To dump your Contr. Pak, press A.\n"
                "To restore a dump to your Contr. Pak, press START.\n"
                "To dump a single entry of your Contr. Pak, press L.\n"
                "To restore a single entry, press Z\n"
                "\n"
                "Controller SELECTED: %d\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n",
                 controller_selected + 1
            );

            char has_mem_text[64];
            menu_font_type_t style;

            if (has_mem) {
                sprintf(has_mem_text, "This controller has a memory card\n");
                style = STL_GREEN;
            } else {
                sprintf(has_mem_text, "This controller has NOT a memory card\n");
                style = STL_ORANGE;
            }

            
            ui_components_main_text_draw(style,
                ALIGN_CENTER, VALIGN_TOP,
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "%s\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n"
                "\n",
                has_mem_text
            );
            

            ui_components_actions_bar_text_draw(
                ALIGN_LEFT, VALIGN_TOP,
                "A: Dump Contr. Pak\n"
                "L: Dump single entry\n"
            );
            ui_components_actions_bar_text_draw(
                ALIGN_RIGHT, VALIGN_TOP,
                "START: Restore Contr. Pak\n"
                "Z: Dump single entry\n"
            );
         }
         else {

            ui_components_main_text_draw(STL_DEFAULT,
                ALIGN_CENTER, VALIGN_TOP,
                "ADJUST REAL TIME CLOCK\n"
                "\n"
                "\n"
                "This cart does not support a real time clock."
                "\n"
                "Current date & time: %s\n"
                "\n",
                menu->current_time >= 0 ? ctime(&menu->current_time) : "Unknown"
            );

            ui_components_actions_bar_text_draw(
                ALIGN_LEFT, VALIGN_TOP,
                "\n"
                "B: Back"
            );
         }
    }
    else {
        ui_components_actions_bar_text_draw(
            ALIGN_RIGHT, VALIGN_TOP,
            "Up/Down: Adjust Field\n"
            "Left/Right: Switch Field"
        );
        ui_components_actions_bar_text_draw(
            ALIGN_LEFT, VALIGN_TOP,
            "R: Save\n"
            "B: Back"
        );
    }

    /*
    if (is_editing_mode) {
        rtc_ui_component_editdatetime_draw(rtc_tm, editing_field_type);
    }
    */

    rdpq_detach_show();
}

void view_controller_pak_init (menu_t *menu) {
    is_editing_mode = false;
    controller_selected = 0;
}

void view_controller_pak_display (menu_t *menu, surface_t *display) {
    process(menu);
    draw(menu, display);
}