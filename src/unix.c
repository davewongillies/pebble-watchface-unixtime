/*
Copyright (c) 2013 WhyIsThisOpen

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define TOTAL_DIGITS 10
#define TOTAL_COLUMNS 4
#define TOTAL_ROWS 3

#define TIME_ZONE_OFFSET 0

#define MY_UUID { 0x2D, 0xF2, 0x62, 0xCE, 0xA2, 0xF3, 0x4B, 0x6F, 0xA5, 0xB7, 0x46, 0xBE, 0xFD, 0xBC, 0xBE, 0x0A }
PBL_APP_INFO(MY_UUID,
             "Unix Time", "WhyIsThisOpen",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
BmpContainer background;
BmpContainer digits[TOTAL_DIGITS];

const int BLOCK_NUMBER[] = {
    RESOURCE_ID_IMAGE_BLOCK_0,
    RESOURCE_ID_IMAGE_BLOCK_1,
    RESOURCE_ID_IMAGE_BLOCK_2,
    RESOURCE_ID_IMAGE_BLOCK_3,
    RESOURCE_ID_IMAGE_BLOCK_4,
    RESOURCE_ID_IMAGE_BLOCK_5,
    RESOURCE_ID_IMAGE_BLOCK_6,
    RESOURCE_ID_IMAGE_BLOCK_7,
    RESOURCE_ID_IMAGE_BLOCK_8,
    RESOURCE_ID_IMAGE_BLOCK_9
};

const int ROW[] = {
    12,
    63,
    114
};

const int COLUMN[] = {
    6,
    41,
    76,
    111
};

void set_container_image(BmpContainer *bmp_container, const int resource_id, int row_y, int column_x) {
    layer_remove_from_parent(&bmp_container->layer.layer);
    bmp_deinit_container(bmp_container);

    bmp_init_container(resource_id, bmp_container);

    GRect frame = layer_get_frame(&bmp_container->layer.layer);
    frame.origin.x = column_x;
    frame.origin.y = row_y;
    layer_set_frame(&bmp_container->layer.layer, frame);

    layer_add_child(&window.layer, &bmp_container->layer.layer);
}

int my_pow(int base, int exp) {
    int result = 1;
    for(int i=exp; i>0; i--) {
        result = result * base;
    }
    return result;
}

void update_display(PblTm *current_time) {
    unsigned int unix_time;
    /* Convert time to seconds since epoch. */
    unix_time = ((0-TIME_ZONE_OFFSET)*3600) + /* time zone offset */ 
              + current_time->tm_sec /* start with seconds */
              + current_time->tm_min*60 /* add minutes */
              + current_time->tm_hour*3600 /* add hours */
              + current_time->tm_yday*86400 /* add days */
              + (current_time->tm_year-70)*31536000 /* add years since 1970 */
              + ((current_time->tm_year-69)/4)*86400 /* add a day after leap years, starting in 1973 */
              - ((current_time->tm_year-1)/100)*86400 /* remove a leap day every 100 years, starting in 2001 */
              + ((current_time->tm_year+299)/400)*86400; /* add a leap day back every 400 years, starting in 2001*/

    /* Draw each digit in the correct location. */
    for(int i=0; i<TOTAL_DIGITS; i++) {
        int digit_row = (int)(i / TOTAL_COLUMNS);
        int digit_column = (i > (TOTAL_DIGITS - 3)) ? (3 - (TOTAL_DIGITS - i)) : (i % TOTAL_COLUMNS); /* This centers the bottom row digits. Use the next line to have them left aligned. */
        /* int digit_colum = i % TOTAL_COLUMNS; */
        int denominator = my_pow(10,i); /* The loop starts at the most significant digit and goes down from there. */
        int digit_value = (int)unix_time/(1000000000 / denominator); /* This gives the value for the current digit. (Casting should give us the floor of the value.) */
        unix_time = unix_time % (1000000000 / denominator); /* This subtracts the value for the current digit so that it doesn't interfere with the next iteration of the loop. */
        set_container_image(&digits[i], BLOCK_NUMBER[digit_value], ROW[digit_row], COLUMN[digit_column]); /* Now we set this digit. */
    }
}

void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
    (void)ctx;

    update_display(t->tick_time);
}

void handle_init(AppContextRef ctx) {
    (void)ctx;

    window_init(&window, "Unix Time");
    window_stack_push(&window, true);

    resource_init_current_app(&APP_RESOURCES);

    bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background);
    layer_add_child(&window.layer, &background.layer.layer);

    PblTm tick_time;
    get_time(&tick_time);
    update_display(&tick_time);
}

void handle_deinit(AppContextRef ctx) {
    (void)ctx;

    bmp_deinit_container(&background);

    for (int i=0; i<TOTAL_DIGITS; i++) {
        bmp_deinit_container(&digits[i]);
    }
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,

        .tick_info = {
            .tick_handler = &handle_second_tick,
            .tick_units = SECOND_UNIT
        }
    };
    app_event_loop(params, &handlers);
}
