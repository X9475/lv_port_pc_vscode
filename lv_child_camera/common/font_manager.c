/**
 * @file lv_font_manager.c
 * @author your name (you@domain.com)
 * @brief 字体管理
 * @version 0.1
 * @date 2025-09-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "font_manager.h"

#if (LV_CHILD_CAMERA != 0) && (LV_USE_FONT_MANAGER != 0)

#define FZLTH_GB18030L2_B_DIR   "../assert/font/FZLTH_GB18030L2/FZLTH_GB18030L2_B.ttf"
#define FZLTH_GB18030L2_R_DIR   "../assert/font/FZLTH_GB18030L2/FZLTH_GB18030L2_R.ttf"
#define Oswald_Bold_DIR         "../assert/font/Oswald/Oswald_Bold.ttf"
#define Oswald_Regular_DIR      "../assert/font/Oswald/Oswald_Regular.ttf"

//字体
//瘦体
const lv_font_t *fzlthr_20;
const lv_font_t *fzlthr_22;
const lv_font_t *fzlthr_24;
const lv_font_t *fzlthr_26;
const lv_font_t *fzlthr_28;
const lv_font_t *fzlthr_30;
const lv_font_t *fzlthr_32;
const lv_font_t *fzlthr_34;
const lv_font_t *fzlthr_36;
const lv_font_t *fzlthr_40;
const lv_font_t *fzlthr_50;
const lv_font_t *fzlthr_56;
const lv_font_t *fzlthr_118;
const lv_font_t *fzlthr_170;
const lv_font_t *oswaldr_20;
const lv_font_t *oswaldr_24;
const lv_font_t *oswaldr_36;
const lv_font_t *oswaldr_48;
const lv_font_t *oswaldr_50;
const lv_font_t *oswaldr_60;
const lv_font_t *oswaldr_70;
const lv_font_t *oswaldr_178;

//粗体
const lv_font_t *fzlthb_20;
const lv_font_t *fzlthb_22;
const lv_font_t *fzlthb_24;
const lv_font_t *fzlthb_26;
const lv_font_t *fzlthb_28;
const lv_font_t *fzlthb_30;
const lv_font_t *fzlthb_32;
const lv_font_t *fzlthb_34;
const lv_font_t *fzlthb_36;
const lv_font_t *fzlthb_40;
const lv_font_t *fzlthb_50;
const lv_font_t *fzlthb_56;
const lv_font_t *fzlthb_118;
const lv_font_t *fzlthb_170;
const lv_font_t *oswaldb_20;
const lv_font_t *oswaldb_24;
const lv_font_t *oswaldb_36;
const lv_font_t *oswaldb_48;
const lv_font_t *oswaldb_50;
const lv_font_t *oswaldb_60;
const lv_font_t *oswaldb_70;
const lv_font_t *oswaldb_178;

/// @brief 字体管理器
static lv_font_manager_t *g_font_manager;

static void lv_fzlth_font_generic();
static const lv_font_t *lv_font_generic(const char *font_family, uint32_t size);

void lv_font_init()
{
    g_font_manager = lv_font_manager_create(2);

    lv_font_manager_add_src_static(g_font_manager, "fzlthr", FZLTH_GB18030L2_R_DIR, &lv_freetype_font_class);
    lv_font_manager_add_src_static(g_font_manager, "fzlthb", FZLTH_GB18030L2_B_DIR, &lv_freetype_font_class);
    lv_font_manager_add_src_static(g_font_manager, "oswaldr", Oswald_Regular_DIR, &lv_freetype_font_class);
    lv_font_manager_add_src_static(g_font_manager, "oswaldb", Oswald_Bold_DIR, &lv_freetype_font_class);

    lv_fzlth_font_generic();

    return;
}

void lv_font_deinit()
{
    //销毁之前，确保已使用lv_font_manager_delete_font() 删除所有字体
    lv_font_manager_delete(g_font_manager);
    g_font_manager = NULL;

    return;
}

static void lv_fzlth_font_generic()
{
    fzlthr_20 = lv_font_generic("fzlthr", 20);
    fzlthr_22 = lv_font_generic("fzlthr", 22);
    fzlthr_24 = lv_font_generic("fzlthr", 24);
    fzlthr_26 = lv_font_generic("fzlthr", 26);
    fzlthr_28 = lv_font_generic("fzlthr", 28);
    fzlthr_30 = lv_font_generic("fzlthr", 30);
    fzlthr_32 = lv_font_generic("fzlthr", 32);
    fzlthr_34 = lv_font_generic("fzlthr", 34);
    fzlthr_36 = lv_font_generic("fzlthr", 36);
    fzlthr_40 = lv_font_generic("fzlthr", 40);
    fzlthr_50 = lv_font_generic("fzlthr", 50);
    fzlthr_56 = lv_font_generic("fzlthr", 56);
    fzlthr_118 = lv_font_generic("fzlthr", 118);
    fzlthr_170 = lv_font_generic("fzlthr", 170);

    fzlthb_20 = lv_font_generic("fzlthb", 20);
    fzlthb_22 = lv_font_generic("fzlthb", 22);
    fzlthb_24 = lv_font_generic("fzlthb", 24);
    fzlthb_26 = lv_font_generic("fzlthb", 26);
    fzlthb_28 = lv_font_generic("fzlthb", 28);
    fzlthb_30 = lv_font_generic("fzlthb", 30);
    fzlthb_32 = lv_font_generic("fzlthb", 32);
    fzlthb_34 = lv_font_generic("fzlthb", 34);
    fzlthb_36 = lv_font_generic("fzlthb", 36);
    fzlthb_40 = lv_font_generic("fzlthb", 40);
    fzlthb_50 = lv_font_generic("fzlthb", 50);
    fzlthb_56 = lv_font_generic("fzlthb", 56);
    fzlthb_118 = lv_font_generic("fzlthb", 118);
    fzlthb_170 = lv_font_generic("fzlthb", 170);

    oswaldr_20 = lv_font_generic("oswaldr", 20);
    oswaldr_24 = lv_font_generic("oswaldr", 24);
    oswaldr_36 = lv_font_generic("oswaldr", 36);
    oswaldr_48 = lv_font_generic("oswaldr", 48);
    oswaldr_50 = lv_font_generic("oswaldr", 50);
    oswaldr_60 = lv_font_generic("oswaldr", 60);
    oswaldr_70 = lv_font_generic("oswaldr", 70);
    oswaldr_178 = lv_font_generic("oswaldr", 178);

    oswaldb_20 = lv_font_generic("oswaldb", 20);
    oswaldb_24 = lv_font_generic("oswaldb", 24);
    oswaldb_36 = lv_font_generic("oswaldb", 36);
    oswaldb_48 = lv_font_generic("oswaldb", 48);
    oswaldb_50 = lv_font_generic("oswaldb", 50);
    oswaldb_60 = lv_font_generic("oswaldb", 60);
    oswaldb_70 = lv_font_generic("oswaldb", 70);
    oswaldb_178 = lv_font_generic("oswaldb", 178);
}

const lv_font_t *font_get_regular(uint32_t size)
{
    return lv_font_generic("fzlthr", size);
}

const lv_font_t *font_get_bold(uint32_t size)
{
    return lv_font_generic("fzlthb", size);
}

void lv_font_manager_del_font(lv_font_t *font)
{
    if (NULL == g_font_manager || NULL == font) return;

    lv_font_manager_delete_font(g_font_manager, font);
}

static const lv_font_t *lv_font_generic(const char *font_family, uint32_t size)
{
    if (g_font_manager == NULL) return LV_FONT_DEFAULT;

    lv_freetype_font_style_t style;
    if (lv_strcmp(font_family, "oswaldr") == 0) style = LV_FREETYPE_FONT_STYLE_NORMAL;
    if (lv_strcmp(font_family, "oswaldb") == 0) style = LV_FREETYPE_FONT_STYLE_BOLD;
    if (lv_strcmp(font_family, "fzlthr") == 0) style = LV_FREETYPE_FONT_STYLE_NORMAL;
    if (lv_strcmp(font_family, "fzlthb") == 0) style = LV_FREETYPE_FONT_STYLE_BOLD;

    lv_font_t *new_font = lv_font_manager_create_font(g_font_manager,
                                                      font_family,
                                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                      size,
                                                      style,
                                                      LV_FONT_KERNING_NONE);
    // if NULL, return &lv_font_montserrat_14
    if (new_font == NULL) return LV_FONT_DEFAULT;

    return new_font;
}

#endif