/**
 * @file lv_startup_dev.c
 * @author your name (you@domain.com)
 * @brief UI添加流程源文件
 * @version 0.1
 * @date 2025-09-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_startup_dev.h"

#if (LV_CHILD_CAMERA != 0) && (LV_CHILD_CAMERA_STARTUP != 0)

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_page_open();
static void lv_page_close();

/**********************
 *  STATIC VARIABLES
 **********************/
static const lv_font_t *font;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_child_camera()
{
    lv_font_init();
    lv_page_open();
}

/**********************
 *  STATIC FUNCTIONS
 **********************/
static void lv_page_open()
{
    font = font_get_regular(30);
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, LVGL!");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    return;
}

static void lv_page_close()
{
    lv_font_manager_del_font(font);
    lv_font_deinit();
}

#endif