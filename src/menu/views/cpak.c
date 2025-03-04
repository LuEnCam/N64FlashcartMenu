#include <stdbool.h>
#include <stdio.h>
#include <libdragon.h>
#include "views.h"
#include "../sound.h"
#include "../fonts.h"
#include <fatfs/ff.h>

#define WAITING_TIME 0
#define u8 unsigned char
#define u32 unsigned long
#define MAX_NUM_NOTES 16
#define MAX_STRING_LENGTH 50
#define EXTENSION ".mpk"   
#define NOTE_EXTENSION ".smpk"

char * CPAK_PATH = "sd:/cpak_saves";
char * CPAK_PATH_NO_PRE = "/cpak_saves";
char * CPAK_NOTES_PATH = "sd:/cpak_saves/notes";
char * CPAK_NOTES_PATH_NO_PRE = "cpak_saves/notes";

bool use_rtc;
rtc_time_t rtc_time;
char string_datetime_cpak[26];

char controller_pak_name_notes[MAX_NUM_NOTES][MAX_STRING_LENGTH];

short controller_selected;

bool has_rumble;
bool has_transfert;
bool has_bio_sensor;

bool has_mem;
bool ctr_p_data_loop;
int free_space_cpak;
bool validate_pak;
int total_elements;
bool process_completed;
bool start_complete_dump;

bool show_complete_dump_confirm_message;
bool show_complete_write_confirm_message;

u8 fmLoadDir(const TCHAR* path, FILINFO *inf, u32 max_items);

void create_directory(const char *dirpath) {
    FRESULT res = f_mkdir(dirpath);
    
    if (res == FR_OK) {
        printf("Directory created: %s\n", dirpath);
    } else if (res == FR_EXIST) {
        printf("Directory already exists: %s\n", dirpath);
    } else {
        printf("Failed to create directory: %s (Error Code: %d)\n", dirpath, res);
    }
}

void get_rtc_time(char* formatted_time) {
    rtc_get(&rtc_time);

    sprintf(formatted_time, "%04d.%02d.%02d_%02dh%02dm%02ds",
            rtc_time.year, rtc_time.month + 1, rtc_time.day,
            rtc_time.hour, rtc_time.min, rtc_time.sec);
}

void utils_truncate_string(const char *source, char *destination, int new_length) {
    // Copy the first `new_length` characters from `source` to `destination`
    strncpy(destination, source, new_length);
    destination[new_length] = '\0'; // Null-terminate the truncated string
}

void free_controller_pak_name_notes() {

    // Set \0 to each note
    for (int i = 0; i < MAX_NUM_NOTES; ++i) {
        sprintf(controller_pak_name_notes[i], " ");
    }

}

char* get_cpak_save_region(char _code) {
    switch (_code) {
        case 'A': return "A = All"; //(Only used in 1080 Snowboarding [USA/JAP] - later \"region-free\" in Wii)";
        case 'B': return "B = Brazil"; //(Not in GC/Wii thus possibly unlicensed, but exists in ROM data)";
        case 'D': return "D = Germany"; //(German only)";
        case 'E': return "E = N.Am"; //(USA, Canada, Mexico)";
        case 'F': return "F = France"; //(French only)";
        case 'I': return "I = Italy"; //(Italian only)";
        case 'J': return "J = Japan";
        case 'P': return "P = Europe"; //(sometimes including Australia)";
        case 'S': return "S = Spain"; //(Spanish only)";
        case 'U': return "U = Aus."; //(English-only PAL games)";
        case 'X': return "X = Europe"; //(Alt. Languages 1)";
        case 'Y': return "Y = Europe"; //(Alt. Languages 2)";
        case 'G': return "G = Lodgenet NTSC"; //(NTSC, mentioned in N64 SDKs)"; G = Lodgenet/Gateway 64 NTSC
        case 'L': return "L = Lodgenet PAL"; //(PAL, mentioned in N64 SDKs)"; L = Lodgenet/Gateway 64 PAL
        default: return  "? = Unknown";
    }
}

void dump_complete_cpak(int _port) {
    process_completed = false;

    uint8_t* data = malloc(MEMPAK_BLOCK_SIZE * 128 * sizeof(uint8_t));

    if (!data) {
        //"Memory allocation failed!"
        return;
    }
    
    for (int i = 0; i < 128; i++) {

        surface_t *d = display_try_get();
        rdpq_attach(d, NULL);

        ui_components_layout_draw();

        
        ui_components_messagebox_draw(
            "Do you want to dump the CPAK?\n\n"
            "A: Yes     B: No"
        );   
        

        if (read_mempak_sector(0, i, data + (i * MEMPAK_BLOCK_SIZE)) != 0) {
            //"Failed to read mempak sector!"
            free(data);
            return;
        }

        ui_components_loader_draw((float) i / 128.0f);

        rdpq_detach_show();
    }

    get_rtc_time(string_datetime_cpak);

    char complete_filename[200];
    sprintf(complete_filename, "%s/CPAK_%s%s", CPAK_PATH, string_datetime_cpak, EXTENSION);


    FILE *fp = fopen(complete_filename, "w");
    if (!fp) {
        //"Failed to open file for writing!"
        free(data);
        return;
    }

    if (fwrite(data, 1, MEMPAK_BLOCK_SIZE * 128, fp) != MEMPAK_BLOCK_SIZE * 128) {
        //"Failed to write data to file!"
    } else {
        process_completed = true;
    }
    
    fclose(fp);
    free(data);
}

void write_complete_cpak(int _port) {








    /*
    process_completed = false;

    char *mpkFiles[200];
    int mpkCount;
    unsigned short selected_index = 0;

    //struct controller_data output;

    //page_cpak = 0;
    //total_elements_cpak = 0;

    listFiles(mpkFiles, &mpkCount , CPAK_PATH_NO_PRE, "mpk");

    //total_elements_cpak = mpkCount;

    
    while(true) {

        graphics_draw_text(utils_disp, 10, h_space_cpak, "Which dump would you like to write ?");
        inc_h_txt(&h_space_cpak);

        draw_action_info(&utils_disp, &h_space_cpak);

        inc_h_txt(&h_space_cpak);

        int elements_per_page = 4;
        int tot_page = (total_elements_cpak / elements_per_page); 
        if ((total_elements_cpak % elements_per_page) == 0 ) tot_page --;

        if (left_value && page_cpak > 0){
            left_value = false;
            page_cpak--;
            wait_ms(WAITING_TIME);
        }

        if (right_value && page_cpak < tot_page) {
            right_value = false;
            page_cpak++;
            wait_ms(WAITING_TIME);
        }

        int j = page_cpak * elements_per_page;
        int k = ((page_cpak + 1) * elements_per_page);

        for (int i = j; i < k && i < total_elements_cpak; i++) {
            sprintf(text_output, "[%d] - %s", i, mpkFiles[i]);
            graphics_draw_text(utils_disp, 10, h_space_cpak, text_output);
            inc_h_txt(&h_space_cpak);
        }

        inc_h_txt(&h_space_cpak);
        sprintf(text_output, "Page %d/%d", page_cpak + 1, tot_page + 1);
        graphics_draw_text(utils_disp, 240, h_space_cpak, text_output);
        inc_h_txt(&h_space_cpak);

        sprintf(text_output, "Selected index : [%d]", selected_index);
        graphics_draw_text(utils_disp, 10, h_space_cpak, text_output);
        inc_h_txt(&h_space_cpak);

        utils_screen_render(&utils_disp);


        if (a_value) {
            a_value = false;
            uint8_t* data = malloc(MEMPAK_BLOCK_SIZE * 128 * sizeof(uint8_t));
            if (!data) {
                //"Memory allocation failed!"
                return;
            }

            char filename[255];

            sprintf(filename, "%s/%s", CPAK_PATH, mpkFiles[selected_index]);

            FILE *fp = fopen(filename, "r");
            if (!fp) {
                //"Failed to open file for reading!"
                free(data);
                return;
            }

            if (fread(data, 1, MEMPAK_BLOCK_SIZE * 128, fp) != MEMPAK_BLOCK_SIZE * 128) {
                //"Failed to read data from file!"
                fclose(fp);
                free(data);
                return;
            }
            fclose(fp);

            for (int i = 0; i < 128; i++) { 
                if (write_mempak_sector(0, i, data + (i * MEMPAK_BLOCK_SIZE)) != 0) {
                    //"Failed to write to mempak sector!"
                    free(data);
                    return;
                }
                utils_disp = display_get();
                sprintf(text_output, "Processing... (%d%%)", (100 * i) / 128);
                graphics_draw_text(utils_disp, 10, h_space_cpak, text_output);
                utils_screen_render(&utils_disp);
            }

            process_completed = true;

            free(data);
            free_controller_pak_name_entries_manager(); 
            break;
        }

        if (up_value && selected_index < mpkCount -1) {
            up_value = false;
            selected_index += 1;
            wait_ms(WAITING_TIME);
        } 
        if (down_value && selected_index > 0) {
            down_value = false;
            selected_index -= 1;
            wait_ms(WAITING_TIME);
        }

        if (z_value) {
            z_value = false;
            graphics_draw_text(utils_disp, 160, h_space_cpak, "Please wait...");
            wait_ms(WAITING_TIME);
            break;
        }


    }

    for (int i = 0; i < mpkCount; i++) {
        free(mpkFiles[i]); // Don't forget to free the allocated memory
    }
    */
}

bool check_accessories(int port) {
    
    joypad_accessory_type_t val =  joypad_get_accessory_type(controller_selected);

    has_rumble    = val == JOYPAD_ACCESSORY_TYPE_RUMBLE_PAK;
    has_transfert = val == JOYPAD_ACCESSORY_TYPE_TRANSFER_PAK;
    has_mem       = val == JOYPAD_ACCESSORY_TYPE_CONTROLLER_PAK;
    has_bio_sensor = val == JOYPAD_ACCESSORY_TYPE_BIO_SENSOR;

    return has_rumble || has_transfert || has_bio_sensor || has_mem;
}


static void process (menu_t *menu) {

    if (!show_complete_dump_confirm_message && !show_complete_write_confirm_message) {
        if(menu->actions.go_c_left) {
            sound_play_effect(SFX_SETTING);
            controller_selected = ((controller_selected - 1) + 4) % 4;
            ctr_p_data_loop = false;
            validate_pak = false;
            has_mem = false;
        } else if (menu->actions.go_c_right) {
            sound_play_effect(SFX_SETTING);
            controller_selected = ((controller_selected + 1) + 4) % 4;
            ctr_p_data_loop = false;
            validate_pak = false;
            has_mem = false;    
        } else if (menu->actions.back) {
            sound_play_effect(SFX_EXIT);
            menu->next_mode = MENU_MODE_BROWSER;
        }
    }

    check_accessories(controller_selected);

    if (has_mem) {

        // Pressing A : dump the controller pak
        if (menu->actions.enter && use_rtc && !show_complete_dump_confirm_message && !show_complete_write_confirm_message) {
            sound_play_effect(SFX_ENTER);
            show_complete_dump_confirm_message = true;
            return;
        } 
        // Pressing L : write a controller pak dump
        else if (menu->actions.l && use_rtc && !show_complete_write_confirm_message && !show_complete_dump_confirm_message) {
            sound_play_effect(SFX_ENTER);
            show_complete_write_confirm_message = true;
            return;
        }

        if (show_complete_dump_confirm_message && !show_complete_write_confirm_message) {
            if (menu->actions.enter) {
                show_complete_dump_confirm_message = false;
                sound_play_effect(SFX_ENTER);
                start_complete_dump = true;
            } else if (menu->actions.back) {
                sound_play_effect(SFX_EXIT);
                show_complete_dump_confirm_message = false;
            }
            return;
        } else if (show_complete_write_confirm_message && !show_complete_dump_confirm_message) {
            if (menu->actions.back) {
                show_complete_write_confirm_message = false;
                sound_play_effect(SFX_EXIT);
            }
            return;
        }
    }
}

static void draw (menu_t *menu, surface_t *d) {
    rdpq_attach(d, NULL);

    ui_components_background_draw();

    ui_components_layout_draw();

    char has_mem_text[64];
    char free_space_cpak_text[64];
    menu_font_type_t style;

    if (has_mem) {
        sprintf(has_mem_text, "CPAK detected");

        if (ctr_p_data_loop) {
            sprintf(has_mem_text, "%s %s", has_mem_text, " (is valid)");
            style = STL_GREEN;
            sprintf(free_space_cpak_text, "It has %d/123 free blocks", free_space_cpak);

        } else {
            sprintf(free_space_cpak_text, " ");
        }

        if (validate_pak == false && validate_mempak(controller_selected) == 0) {
            validate_pak = true;
            
            if (ctr_p_data_loop == false) {
                free_space_cpak = get_mempak_free_space(controller_selected);

                free_controller_pak_name_notes();

                bool has_tot_element_checked = false;
                if (total_elements > 0) has_tot_element_checked = true;
                for (int i = 0; i < MAX_NUM_NOTES; i++) {
                    entry_structure_t note;
                    get_mempak_entry(controller_selected, i, &note);
                    if (note.valid) {
                        char temp[16];
                        utils_truncate_string(note.name, temp, 15);
                        snprintf(controller_pak_name_notes[i], MAX_STRING_LENGTH, "%s %s", temp , get_cpak_save_region(note.region));
                        //snprintf(controller_pak_name_notes[i], MAX_STRING_LENGTH, "%s %s", note.name , get_cpak_save_region(note.region));
                        if (!has_tot_element_checked) total_elements++;
                    } else {
                        strcpy(controller_pak_name_notes[i], " ");
                    }
                }
                ctr_p_data_loop = true;
            }

        } else if (validate_pak == false) {
            sprintf(has_mem_text, "%s %s", has_mem_text, " (is NOT valid. Corrupted)");
            style = STL_ORANGE;
            sprintf(free_space_cpak_text, " ");
        }

    } else {
        sprintf(has_mem_text, "NO CPAK detected");
        style = STL_ORANGE;
        sprintf(free_space_cpak_text, " ");
        validate_pak = false;
        ctr_p_data_loop = false;
        free_space_cpak = 0;
    }

    ui_components_main_text_draw(STL_DEFAULT,
        ALIGN_CENTER, VALIGN_TOP,
        "CONTROLLER PAK MANAGEMENT\n"
    );

    ui_components_main_text_draw(STL_DEFAULT,
        ALIGN_RIGHT, VALIGN_TOP,
        "B: Back\n"
    );

    ui_components_main_text_draw(STL_DEFAULT,
        ALIGN_LEFT, VALIGN_TOP,
        "\n"
        "Controller selected: %d\n",
            controller_selected + 1
    );

    ui_components_main_text_draw(style,
        ALIGN_LEFT, VALIGN_TOP,
        "\n"
        "                        %s\n",
        has_mem_text
    );

    if (has_mem) {
        ui_components_main_text_draw(STL_DEFAULT,
            ALIGN_CENTER, VALIGN_TOP,
            "\n"
            "\n"
            "%s\n",
            free_space_cpak_text
        );

        ui_components_main_text_draw(style,
            ALIGN_LEFT, VALIGN_TOP,
            "\n"
            "\n"
            "\n"
            "N.01: %s\n"
            "N.02: %s\n"
            "N.03: %s\n"
            "N.04: %s\n"
            "N.05: %s\n"
            "N.06: %s\n"
            "N.07: %s\n"
            "N.08: %s\n"
            "N.09: %s\n"
            "N.10: %s\n"
            "N.11: %s\n"
            "N.12: %s\n"
            "N.13: %s\n"
            "N.14: %s\n"
            "N.15: %s\n"
            "N.16: %s\n",
            controller_pak_name_notes[0],
            controller_pak_name_notes[1],
            controller_pak_name_notes[2],
            controller_pak_name_notes[3],
            controller_pak_name_notes[4],
            controller_pak_name_notes[5],
            controller_pak_name_notes[6],
            controller_pak_name_notes[7],
            controller_pak_name_notes[8],
            controller_pak_name_notes[9],
            controller_pak_name_notes[10],
            controller_pak_name_notes[11],
            controller_pak_name_notes[12],
            controller_pak_name_notes[13],
            controller_pak_name_notes[14],
            controller_pak_name_notes[15],
            controller_pak_name_notes[16]
        );
    }

    style = (has_mem && validate_pak) ? STL_DEFAULT : STL_GRAY;
    

    ui_components_actions_bar_text_draw(style,
        ALIGN_LEFT, VALIGN_TOP,
        "A: Dump Pak\n"
        "L: Dump single Note\n"
    );
    ui_components_actions_bar_text_draw(style,
        ALIGN_RIGHT, VALIGN_TOP,
        "START: Restore Pak\n"
        "Z: Restore single Note\n"
    );

    if (show_complete_dump_confirm_message && !start_complete_dump) {
        ui_components_messagebox_draw(
            "Do you want to dump the CPAK?\n\n"
            "A: Yes        B: No"
        );   
    } else if (show_complete_write_confirm_message) {
        ui_components_messagebox_draw(
            "To write a complete dump, go to the directory \"/cpak_saves\"\n"
            "and select a file with the extension \".mpk\".\n\n"
            "B: Back"
        );   
    } 

    if (start_complete_dump) {
        rdpq_detach_show();
        dump_complete_cpak(controller_selected);
        start_complete_dump = false;
        return;
    }
        
    rdpq_detach_show();
}

void view_controller_pak_init (menu_t *menu) {
    controller_selected = 0;
    ctr_p_data_loop = false;
    validate_pak = false;
    has_mem = false;
    free_space_cpak = 0;
    total_elements = 0;
    process_completed = false;
    start_complete_dump = false;
    show_complete_dump_confirm_message = false;
    show_complete_write_confirm_message = false;

    use_rtc = menu->current_time >= 0 ? true : false;

    
    create_directory(CPAK_PATH_NO_PRE);
    create_directory(CPAK_NOTES_PATH_NO_PRE);


}

void view_controller_pak_display (menu_t *menu, surface_t *display) {
    process(menu);
    draw(menu, display);
}