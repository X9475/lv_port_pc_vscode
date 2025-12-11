#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lvgl/lvgl.h"
#include "../../lv_child_camera_ui_conf.h"

#if (LV_CHILD_CAMERA != 0) && (LV_USE_FONT_MANAGER != 0)

//瘦体
extern const lv_font_t *fzlthr_20;
extern const lv_font_t *fzlthr_22;
extern const lv_font_t *fzlthr_24;
extern const lv_font_t *fzlthr_26;
extern const lv_font_t *fzlthr_28;
extern const lv_font_t *fzlthr_30;
extern const lv_font_t *fzlthr_32;
extern const lv_font_t *fzlthr_34;
extern const lv_font_t *fzlthr_36;
extern const lv_font_t *fzlthr_40;
extern const lv_font_t *fzlthr_50;
extern const lv_font_t *fzlthr_56;
extern const lv_font_t *fzlthr_118;
extern const lv_font_t *fzlthr_170;
extern const lv_font_t *oswaldr_20;
extern const lv_font_t *oswaldr_24;
extern const lv_font_t *oswaldr_36;
extern const lv_font_t *oswaldr_50;
extern const lv_font_t *oswaldr_178;
//粗体
extern const lv_font_t *fzlthb_20;
extern const lv_font_t *fzlthb_22;
extern const lv_font_t *fzlthb_24;
extern const lv_font_t *fzlthb_26;
extern const lv_font_t *fzlthb_28;
extern const lv_font_t *fzlthb_30;
extern const lv_font_t *fzlthb_32;
extern const lv_font_t *fzlthb_34;
extern const lv_font_t *fzlthb_36;
extern const lv_font_t *fzlthb_40;
extern const lv_font_t *fzlthb_50;
extern const lv_font_t *fzlthb_56;
extern const lv_font_t *fzlthb_118;
extern const lv_font_t *fzlthb_170;
extern const lv_font_t *oswaldb_20;
extern const lv_font_t *oswaldb_24;
extern const lv_font_t *oswaldb_36;
extern const lv_font_t *oswaldb_50;
extern const lv_font_t *oswaldb_178;

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