#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl/lvgl.h"
#include "../../lv_child_camera_ui_conf.h"

#if (LV_CHILD_CAMERA != 0) && (LV_USE_FONT_MANAGER != 0)

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_font_init();

const lv_font_t *font_get_regular(uint32_t size);

const lv_font_t *font_get_bold(uint32_t size);

void lv_font_manager_del_font(lv_font_t *font);

void lv_font_deinit();

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FONT_MANAGER_H */