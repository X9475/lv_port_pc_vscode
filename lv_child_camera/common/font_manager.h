#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lvgl/lvgl.h"
#include "../../lv_child_camera_ui_conf.h"

#if (LV_CHILD_CAMERA != 0) && (LV_USE_FONT_MANAGER != 0)

//瘦体
extern const lv_font_t *fzlthr_22;
extern const lv_font_t *fzlthr_24;
extern const lv_font_t *fzlthr_26;
extern const lv_font_t *fzlthr_28;
extern const lv_font_t *fzlthr_30;
extern const lv_font_t *fzlthr_34;
extern const lv_font_t *fzlthr_170;
//粗体
extern const lv_font_t *fzlthb_22;
extern const lv_font_t *fzlthb_24;
extern const lv_font_t *fzlthb_26;
extern const lv_font_t *fzlthb_28;
extern const lv_font_t *fzlthb_30;
extern const lv_font_t *fzlthb_34;
extern const lv_font_t *fzlthb_170;

void lv_font_init();

const lv_font_t *font_get_regular(uint32_t size);

const lv_font_t *font_get_bold(uint32_t size);

void lv_font_manager_del_font(lv_font_t *font);

void lv_font_deinit();

#endif

#ifdef __cplusplus
}
#endif

#endif