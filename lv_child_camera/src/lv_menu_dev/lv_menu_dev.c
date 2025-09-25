/**
 * @file lv_menu_dev.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_menu_dev.h"

#if (LV_CHILD_CAMERA != 0) && (LV_CHILD_CAMERA_MENU != 0)

/*********************
 *      DEFINES
 *********************/
#define APP_NUM     7

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    const char *name;
    const char *color_icon;
    const char *grey_icon;
} lv_menu_dev_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_init();
static void lv_page_open();
static void lv_page_close();
static void lv_menu_main(lv_obj_t *cont);
static void scroll_app_item_event_cb(lv_event_t * e);
static void scroll_end_favorites_app_item_event_cb(lv_event_t * e);
static void app_icon_event_cb(lv_event_t * e);
static lv_obj_t *lv_app_create(int i, lv_obj_t *cont, const char *name, const char *path);
static void set_gray_app_style(lv_obj_t *obj, lv_menu_dev_t *iterm_ptr);
static void set_color_app_style(int i, lv_obj_t *obj, lv_menu_dev_t *iterm_ptr);
static void set_indicator_light(int i);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *menu_page;
static lv_style_t style;
static lv_style_t style_mask;
static const lv_font_t *font_30;

static lv_menu_dev_t menu_app_list[APP_NUM] = {
    {"拍摄", "V:tk1/icon/photograph_icon_screenshot_black.png", "V:tk1/icon/photograph_icon_screenshot.png"},
    {"AI问答", "V:tk1/icon/photograph_icon_ai_black.png", "V:tk1/icon/photograph_icon_ai.png"},
    {"视频通话", "V:tk1/icon/photograph_icon_videocall_filled_black.png", "V:tk1/icon/photograph_icon_videocall_filled.png"},
    {"留言板", "V:tk1/icon/photograph_icon_message_board_black.png", "V:tk1/icon/photograph_icon_message_board.png"},
    {"闹钟提醒", "V:tk1/icon/photograph_icon_alarm_clock_black.png", "V:tk1/icon/photograph_icon_alarm_clock.png"},
    {"相册", "V:tk1/icon/photograph_icon_album_black.png", "V:tk1/icon/photograph_icon_album.png"},
    {"消息中心", "V:tk1/icon/photograph_icon_ring_filled_black.png", "V:tk1/icon/photograph_icon_ring_filled.png"}
};

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
static void style_init()
{
    lv_style_init(&style);
    lv_style_set_radius(&style, 0);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_border_width(&style, 0);
    lv_style_set_bg_color(&style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&style, LV_OPA_COVER);

    //图层蒙板
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 3;
    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[0].opa = LV_OPA_90;
    grad.stops[1].color = lv_color_hex(0x000000);
    grad.stops[1].opa = LV_OPA_TRANSP;
    grad.stops[2].color = lv_color_hex(0x000000);
    grad.stops[2].opa = LV_OPA_90;
    grad.stops[0].frac = 0;
    grad.stops[1].frac = 127;
    grad.stops[2].frac = 255;
    lv_style_init(&style_mask);
    lv_style_copy(&style_mask, &style);
    lv_style_set_bg_grad(&style_mask, &grad);
    lv_style_set_bg_grad_dir(&style_mask, LV_GRAD_DIR_VER);

}

static void lv_page_open()
{
    style_init();

    //屏幕对象
    menu_page = lv_obj_create(NULL);
    lv_obj_clear_flag(menu_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(menu_page, &style, 0);
    lv_obj_center(menu_page);

    // TODO: 根据业务区分调用
    {
        //彩单转动
        lv_menu_main(menu_page);
    }

    return;
}

static void lv_page_close()
{
    lv_font_manager_del_font(font_30);
    lv_font_deinit();
}

static void lv_menu_main(lv_obj_t *cont)
{
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, 365, lv_pct(100));
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(cont_col, &style, 0);
    lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 80, 0);
    lv_obj_set_style_clip_corner(cont_col, true, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont_col, scroll_app_item_event_cb, LV_EVENT_SCROLL, NULL);

    lv_obj_t *image = NULL;
    for (uint8_t i = 0; i < APP_NUM; i++)
    {
        //创建子功能按钮
        lv_obj_t *btn = lv_app_create(i, cont_col, menu_app_list[i].name, menu_app_list[i].grey_icon);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_event_cb(btn, app_icon_event_cb, LV_EVENT_CLICKED, &menu_app_list[i]);
    }

    //绘制刻度圆盘
    lv_obj_t *scale = lv_img_create(cont);
    lv_img_set_src(scale, "V:tk1/icon/menu_knob_2x.png");
    lv_img_set_zoom(scale, 128);
    lv_obj_align_to(scale, cont, LV_ALIGN_LEFT_MID, 267, 0);
    //图层蒙板
    lv_obj_t *mask = lv_obj_create(cont);
    lv_obj_set_size(mask, 55, 330);
    lv_obj_align(mask, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(mask, &style_mask, 0);
    //首次更新位置
    lv_obj_send_event(cont_col, LV_EVENT_SCROLL, NULL);
    //首个居于中央
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, 0), LV_ANIM_OFF);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void app_icon_event_cb(lv_event_t * e)
{
    lv_obj_t *app_obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_menu_dev_t *iterm_data = lv_event_get_user_data(e);
    //点击按钮滚动置于中央
    lv_obj_scroll_to_view(app_obj, LV_ANIM_OFF);

    if (code == LV_EVENT_CLICKED) {
        printf("===>点击: %s, 进入子菜单\n", iterm_data->name);
        // lv_obj_clean(menu_page);
        // menu_page = NULL;
        //TODO: 进入子菜单
    }
}

static lv_obj_t *lv_app_create(int i, lv_obj_t *cont, const char *name, const char *path)
{
    //背景
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 300, 120);
    lv_obj_set_style_radius(btn, 65, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2A3534), 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    //圆形
    lv_obj_t *image1 = lv_img_create(btn);
    lv_obj_set_size(image1, 80, 80);
    if ((i % 2) == 0) {
        lv_img_set_src(image1, "V:tk1/icon/purple_circle.png");
    } else {
        lv_img_set_src(image1, "V:tk1/icon/green_circle.png");
    }
    lv_obj_align_to(image1, btn, LV_ALIGN_LEFT_MID, 20, 0);
    //叠加图标
    lv_obj_t *image2 = lv_img_create(btn);
    lv_obj_set_size(image2, 50, 50);
    lv_img_set_src(image2, path);
    lv_obj_align_to(image2, btn, LV_ALIGN_LEFT_MID, 35, 0);
    //文字
    if (NULL == font_30) font_30 = font_get_regular(30);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, font_30, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 116, 0);

    return btn;
}

static void set_gray_app_style(lv_obj_t *obj, lv_menu_dev_t *iterm_ptr)
{
    //背景
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2A3534), 0);
    //圆形
    lv_obj_t *image1 = lv_obj_get_child(obj, 0);
    lv_obj_set_style_image_opa(image1, LV_OPA_30, 0);
    //叠加图标
    lv_obj_t *image2 = lv_obj_get_child(obj, 1);
    lv_img_set_src(image2, iterm_ptr->grey_icon);
    lv_obj_set_style_image_opa(image2, LV_OPA_30, 0);
    //文字
    lv_obj_t *label = lv_obj_get_child(obj, 2);
    lv_obj_set_style_text_opa(label, LV_OPA_60, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
}

static void set_color_app_style(int i, lv_obj_t *obj, lv_menu_dev_t *iterm_ptr)
{
    //背景
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    if ((i % 2) == 0) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xCE94F8), 0);
    } else {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xAFF99C), 0);
    }
    //圆形
    lv_obj_t *image1 = lv_obj_get_child(obj, 0);
    lv_obj_set_style_image_opa(image1, LV_OPA_COVER, 0);
    //叠加图标
    lv_obj_t *image2 = lv_obj_get_child(obj, 1);
    lv_img_set_src(image2, iterm_ptr->color_icon);
    lv_obj_set_style_image_opa(image2, LV_OPA_COVER, 0);
    //文字
    lv_obj_t *label = lv_obj_get_child(obj, 2);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
}

static void set_indicator_light(int i)
{
    //指示灯
    lv_obj_t *image = lv_img_create(menu_page);
    // lv_obj_set_size(image, 80, 80);
    if ((i % 2) == 0) {
        lv_img_set_src(image, "V:tk1/icon/photograph_icon_guide_purple.png");
    } else {
        lv_img_set_src(image, "V:tk1/icon/photograph_icon_guide_green.png");
    }
    lv_obj_align_to(image, menu_page, LV_ALIGN_RIGHT_MID, -36, -2);
}

static void scroll_app_item_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    uint32_t child_cnt = lv_obj_get_child_count(cont);
    for (uint32_t i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = child_y_center - cont_y_center;
        // printf("i = %d, y = %d, %d\n", i, LV_ABS(diff_y), diff_y);

        int32_t r = lv_obj_get_height(cont);
        uint32_t x_sqr = r * r - LV_ABS(diff_y) * LV_ABS(diff_y);

        lv_sqrt_res_t res;
        lv_sqrt(x_sqr, &res, 0x8000);
        int32_t x = r - res.i;

        // 设置旋转中心为右侧边缘中间点
        lv_obj_set_style_transform_pivot_x(child, 502, 0);
        lv_obj_set_style_transform_pivot_y(child, 205, 0);
        if (LV_ABS(diff_y) >= 50)//阈值范围可调
        {
            set_gray_app_style(child, &menu_app_list[i]);
            int32_t angle = -(diff_y) / 2;
            if (diff_y < 0)
            {
                lv_obj_set_style_translate_x(child, x, 0);
                lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
            }
            else
            {
                lv_obj_set_style_translate_x(child, x + 25, 0);
                lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
            }
        }
        else
        {
            set_indicator_light(i);
            set_color_app_style(i, child, &menu_app_list[i]);
            lv_obj_set_style_translate_x(child, x, 0);
            lv_obj_set_style_transform_rotation(child, 0, LV_PART_MAIN);
        }
    }
}

#endif