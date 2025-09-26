/**
 * @file lv_abnorml_dev.c
 * @author your name (you@domain.com)
 * @brief UI异常情况源文件
 * @version 0.1
 * @date 2025-09-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_abnorml_dev.h"

#if (LV_CHILD_CAMERA != 0) && (LV_CHILD_CAMERA_ABNORMAL != 0)

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_init();
static void lv_page_open();
static void lv_page_close();
static void lv_abnorml_low_battery(lv_obj_t *cont);
static void lv_low_battery_event(lv_event_t *e);
static void lv_abnorml_minute_battery(lv_obj_t *cont);
static void lv_minute_battery_event(lv_event_t *e);
static void lv_abnorml_high_temperature(lv_obj_t *cont);
static void lv_abnorml_low_temperature(lv_obj_t *cont);
static void lv_abnorml_storage_exhausted(lv_obj_t *cont);
static void lv_abnorml_storage_exhausted_toast(lv_obj_t *cont);
static void lv_storage_exhausted_click_event(lv_event_t *e);
static void lv_abnorml_storage_updating(lv_obj_t *cont);
static void lv_abnorml_transmit_usb(lv_obj_t *cont);
static void lv_usb_back_click_event(lv_event_t *e);
static void lv_switch_select_checkbox_event(lv_event_t *e);
static void lv_abnorml_usb_flash_mode(lv_obj_t *cont);
static void lv_abnorml_network_exception(lv_obj_t *cont);
static void lv_network_exception_event(lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *abnormal_page;
static lv_obj_t *select;
static lv_obj_t *unselect;
static lv_obj_t *charge;//仅充电
static lv_obj_t *transmit;//数据传输
static lv_obj_t *current_select;//记录当前选项
static lv_style_t style;
static const lv_font_t *font_24;
static const lv_font_t *font_26;
static const lv_font_t *font_28;
static const lv_font_t *font_30;
static const lv_font_t *font_30B;
static const lv_font_t *font_32;
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

}

static void lv_page_open()
{
    style_init();

    //屏幕对象
    abnormal_page = lv_obj_create(NULL);
    lv_obj_clear_flag(abnormal_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(abnormal_page, &style, 0);
    lv_obj_center(abnormal_page);

    // TODO: 根据业务区分调用
    {
        //电量小于20%
        // lv_abnorml_low_battery(abnormal_page);
        //电量小于3%
        // lv_abnorml_minute_battery(abnormal_page);
        //高温异常
        // lv_abnorml_high_temperature(abnormal_page);
        //低温异常
        // lv_abnorml_low_temperature(abnormal_page);
        //存储耗尽
        // lv_abnorml_storage_exhausted(abnormal_page);
        //拍摄提示存储耗尽
        // lv_abnorml_storage_exhausted_toast(abnormal_page);
        //升级中
        // lv_abnorml_storage_updating(abnormal_page);
        //USB传输
        // lv_abnorml_transmit_usb(abnormal_page);
        //U盘模式中
        // lv_abnorml_usb_flash_mode(abnormal_page);
        //网络异常
        lv_abnorml_network_exception(abnormal_page);
    }

    return;
}

static void lv_page_close()
{
    lv_font_manager_del_font(font_24);
    lv_font_manager_del_font(font_26);
    lv_font_manager_del_font(font_28);
    lv_font_manager_del_font(font_30);
    lv_font_manager_del_font(font_30B);
    lv_font_manager_del_font(font_32);
    lv_font_deinit();
}

static void lv_abnorml_low_battery(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *photo = lv_img_create(cont);
    lv_img_set_src(photo, "V:tk1/icon/battery_20.png");
    lv_img_set_zoom(photo, 128);
    lv_obj_set_size(photo, 380, 210);
    lv_obj_align(photo, LV_ALIGN_TOP_MID, 0, 25);

    //文字提示：电量较低请及时充电
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "电量较低请及时充电");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 254);

    //知道了
    if (NULL == font_30) font_30 = font_get_regular(30);
    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 209, 70);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 312);
    lv_obj_set_style_radius(btn, 51, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_t *tip2_label = lv_label_create(btn);
    lv_label_set_text(tip2_label, "知道了");
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip2_label, font_30, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, lv_low_battery_event, LV_EVENT_CLICKED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_low_battery_event(lv_event_t *e)
{
    //TODO: 回到当前页面
    lv_obj_clean(abnormal_page);
    abnormal_page = NULL;

}

static void lv_abnorml_minute_battery(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *photo = lv_img_create(cont);
    lv_img_set_src(photo, "V:tk1/icon/battery_3.png");
    lv_img_set_zoom(photo, 128);
    lv_obj_set_size(photo, 380, 210);
    lv_obj_align(photo, LV_ALIGN_TOP_MID, 0, 25);

    //文字提示：电量较低即将关机
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "电量较低即将关机");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 254);

    //知道了
    if (NULL == font_30) font_30 = font_get_regular(30);
    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 209, 70);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 312);
    lv_obj_set_style_radius(btn, 51, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_t *tip2_label = lv_label_create(btn);
    lv_label_set_text(tip2_label, "知道了");
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip2_label, font_30, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, lv_minute_battery_event, LV_EVENT_CLICKED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_minute_battery_event(lv_event_t *e)
{
    //TODO: 回到当前页面
    lv_obj_clean(abnormal_page);
    abnormal_page = NULL;

}

static void lv_abnorml_high_temperature(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *photo = lv_img_create(cont);
    lv_img_set_src(photo, "V:tk1/icon/high_temperature.png");
    lv_img_set_zoom(photo, 128);
    lv_obj_set_size(photo, 380, 210);
    lv_obj_align(photo, LV_ALIGN_TOP_MID, 0, 60);

    //文字提示：相机温度较高，无法进行操作
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "相机温度较高，无法进行操作");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 290);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_abnorml_low_temperature(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *photo = lv_img_create(cont);
    lv_img_set_src(photo, "V:tk1/icon/low_temperature.png");
    lv_img_set_zoom(photo, 128);
    lv_obj_set_size(photo, 380, 210);
    lv_obj_align(photo, LV_ALIGN_TOP_MID, 0, 60);

    //文字提示：相机温度较低，无法进行操作
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "相机温度较低，无法进行操作");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 290);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_abnorml_storage_exhausted(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *storage = lv_img_create(cont);
    lv_img_set_src(storage, "V:tk1/icon/storage.png");
    lv_img_set_zoom(storage, 128);
    lv_obj_set_size(storage, 380, 210);
    lv_obj_align(storage, LV_ALIGN_TOP_MID, 0, 25);

    //文字提示：存储容量即将耗尽，即将停止录像
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "存储容量即将耗尽，即将停止录像");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 259);

    //知道了
    if (NULL == font_30) font_30 = font_get_regular(30);
    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 209, 70);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 309);
    lv_obj_set_style_radius(btn, 51, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_t *tip2_label = lv_label_create(btn);
    lv_label_set_text(tip2_label, "知道了");
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip2_label, font_30, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, lv_storage_exhausted_click_event, LV_EVENT_CLICKED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_storage_exhausted_click_event(lv_event_t *e)
{
    //TODO: 回到当前页面
    lv_obj_clean(abnormal_page);
    abnormal_page = NULL;

}

static void lv_abnorml_storage_exhausted_toast(lv_obj_t *cont)
{
    //文字提示：存储容量耗尽无法拍摄
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 400, 92);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 164);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "存储容量耗尽无法拍摄");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

     //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "V:tk1/icon/toast_fail.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 31, 124);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_abnorml_storage_updating(lv_obj_t *cont)
{
    //背景图

    //文字提示：升级中，完成后相机即将自动重启
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "升级中，完成后相机即将自动重启");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_26, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 325);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_abnorml_transmit_usb(lv_obj_t *cont)
{
    //返回
    lv_obj_t *back_btn = lv_img_create(cont);
    lv_img_set_src(back_btn, "V:tk1/icon/common_icon_back.png");
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back_btn, lv_usb_back_click_event, LV_EVENT_CLICKED, NULL);
    
    //文字提示：USB用于
    if (NULL == font_30B) font_30B = font_get_bold(30);
    lv_obj_t *label1_usb = lv_label_create(cont);
    lv_label_set_text(label1_usb, "USB用于");
    lv_obj_set_style_text_opa(label1_usb, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label1_usb, font_30B, 0);
    lv_obj_set_style_text_color(label1_usb, lv_color_white(), 0);
    lv_obj_set_style_text_align(label1_usb, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label1_usb, back_btn, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    //1.仅充电
    charge = lv_obj_create(cont);
    lv_obj_remove_style_all(charge);
    lv_obj_clear_flag(charge, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(charge, 1, 0);
    lv_obj_set_style_border_color(charge, lv_color_hex(0x404040), 0);
    lv_obj_set_size(charge, 422, 110);
    lv_obj_align(charge, LV_ALIGN_TOP_MID, 0, 78);
    lv_obj_set_style_radius(charge, 20, 0);
    lv_obj_set_style_bg_opa(charge, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(charge, lv_color_hex(0x0A0B0D), 0);
    lv_obj_set_style_bg_grad_color(charge, lv_color_hex(0x202124), 0);
    lv_obj_set_style_bg_grad_dir(charge, LV_GRAD_DIR_HOR, 0);

    lv_obj_t *battery = lv_img_create(charge);
    lv_img_set_src(battery, "V:tk1/icon/usb_icon_charge.png");
    lv_img_set_zoom(battery, 128);
    lv_obj_set_size(battery, 70, 70);
    lv_obj_align_to(battery, charge, LV_ALIGN_LEFT_MID, 19, 0);

    if (NULL == font_28) font_28 = font_get_regular(28);
    lv_obj_t *label1 = lv_label_create(charge);
    lv_label_set_text(label1, "仅充电");
    lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label1, font_28, 0);
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label1, charge, LV_ALIGN_LEFT_MID, 110, 0);

    //2.传输照片录像
    if (NULL == font_28) font_28 = font_get_regular(28);
    transmit = lv_obj_create(cont);
    lv_obj_remove_style_all(transmit);
    lv_obj_clear_flag(transmit, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(transmit, 1, 0);
    lv_obj_set_style_border_color(transmit, lv_color_hex(0x404040), 0);
    lv_obj_set_size(transmit, 422, 110);
    lv_obj_align(transmit, LV_ALIGN_TOP_MID, 0, 204);
    lv_obj_set_style_radius(transmit, 20, 0);
    lv_obj_set_style_bg_opa(transmit, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(transmit, lv_color_hex(0x0A0B0D), 0);
    lv_obj_set_style_bg_grad_color(transmit, lv_color_hex(0x202124), 0);
    lv_obj_set_style_bg_grad_dir(transmit, LV_GRAD_DIR_HOR, 0);

    lv_obj_t *photo = lv_img_create(transmit);
    lv_img_set_src(photo, "V:tk1/icon/usb_icon_photo.png");
    lv_img_set_zoom(photo, 128);
    lv_obj_set_size(photo, 70, 70);
    lv_obj_align_to(photo, transmit, LV_ALIGN_LEFT_MID, 19, 0);

    if (NULL == font_28) font_28 = font_get_regular(28);
    lv_obj_t *label2 = lv_label_create(transmit);
    lv_label_set_text(label2, "传输照片录像");
    lv_obj_set_style_text_opa(label2, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label2, font_28, 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label2, transmit, LV_ALIGN_LEFT_MID, 104, 0);

    //复选框
    select = lv_img_create(charge);
    unselect = lv_img_create(transmit);
    lv_img_set_src(select, "V:tk1/icon/photograph_icon_select_green.png");
    lv_img_set_src(unselect, "V:tk1/icon/photograph_icon_unselect.png");
    lv_obj_align_to(select, charge, LV_ALIGN_RIGHT_MID, -34, 0);
    lv_obj_align_to(unselect, transmit, LV_ALIGN_RIGHT_MID, -34, 0);
    lv_obj_add_flag(select, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(unselect, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(select, lv_switch_select_checkbox_event, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(unselect, lv_switch_select_checkbox_event, LV_EVENT_CLICKED, NULL);
    current_select = charge;

    //确认
    lv_obj_t *confirm_btn = lv_btn_create(cont);
    lv_obj_set_size(confirm_btn, 148, 70);
    lv_obj_align(confirm_btn, LV_ALIGN_TOP_MID, 0, 328);
    lv_obj_set_style_radius(confirm_btn, 51, 0);
    lv_obj_set_style_shadow_opa(confirm_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(confirm_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(confirm_btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_t *img_ok = lv_img_create(confirm_btn);
    lv_img_set_src(img_ok, "V:tk1/icon/common_icon_ok_button.png");
    lv_obj_set_size(img_ok, 50, 50);
    lv_obj_align_to(img_ok, confirm_btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_image_recolor_opa(img_ok, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img_ok, lv_color_hex(0x0A0B0D), 0);
    lv_obj_add_event_cb(confirm_btn, lv_usb_back_click_event, LV_EVENT_CLICKED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_usb_back_click_event(lv_event_t *e)
{
    if (current_select == charge)
    {
        //仅充电
        printf("当前选择： 仅充电\n");
    }
    else if (current_select == transmit)
    {
        //传输照片录像
        printf("当前选择： 传输照片录像\n");
    }
    //TODO: 回到当前页面
    lv_obj_clean(abnormal_page);
    abnormal_page = NULL;

}

static void lv_switch_select_checkbox_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    if(target == select) return;

    //获取当前父对象
    lv_obj_t *select_parent = lv_obj_get_parent(select);
    lv_obj_t *unselect_parent = lv_obj_get_parent(unselect);
    //切换图片源
    lv_img_set_src(select, "V:tk1/icon/photograph_icon_unselect.png");
    lv_img_set_src(unselect, "V:tk1/icon/photograph_icon_select_green.png");
    //交换指针
    lv_obj_t *temp = select;
    select = unselect;
    unselect = temp;
    //交换父对象
    lv_obj_set_parent(select, unselect_parent);
    lv_obj_set_parent(unselect, select_parent);
    //更新当前选项的父对象
    current_select = lv_obj_get_parent(select);
}

static void lv_abnorml_usb_flash_mode(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *photo = lv_img_create(cont);
    lv_img_set_src(photo, "V:tk1/icon/USB.png");
    lv_img_set_zoom(photo, 128);
    lv_obj_set_size(photo, 380, 210);
    lv_obj_align(photo, LV_ALIGN_TOP_MID, 0, 60);

    //文字提示：U盘模式中…
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip_label = lv_label_create(cont);
    lv_label_set_text(tip_label, "U盘模式中…");
    lv_obj_set_style_text_opa(tip_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip_label, font_26, 0);
    lv_obj_set_style_text_color(tip_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip_label, LV_ALIGN_TOP_MID, 0, 290);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_abnorml_network_exception(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *photo = lv_img_create(cont);
    lv_img_set_src(photo, "V:tk1/icon/network_anomaly.png");
    lv_img_set_zoom(photo, 128);
    lv_obj_set_size(photo, 342, 189);
    lv_obj_align(photo, LV_ALIGN_TOP_MID, 0, 0);

    //文字提示：设备网络异常
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "设备网络异常");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 189);

    if (NULL == font_24) font_24 = font_get_regular(24);
    lv_obj_t *tip2_label = lv_label_create(cont);
    lv_obj_set_size(tip2_label, 375, 72);
    lv_obj_set_style_opa(tip2_label, LV_OPA_80, 0);
    lv_obj_set_style_text_line_space(tip2_label, 6, 0);
    lv_label_set_text(tip2_label, "1、请确保设备所摆放位置信号良好2、请确保流量卡中有可用流量");
    lv_label_set_long_mode(tip2_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_60, 0);
    lv_obj_set_style_text_font(tip2_label, font_24, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 229);

    //重试
    if (NULL == font_30) font_30 = font_get_regular(30);
    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 148, 70);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 320);
    lv_obj_set_style_radius(btn, 51, 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_t *tip3_label = lv_label_create(btn);
    lv_label_set_text(tip3_label, "重试");
    lv_obj_set_style_text_opa(tip3_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip3_label, font_30, 0);
    lv_obj_set_style_text_color(tip3_label, lv_color_hex(0x1C1C1E), 0);
    lv_obj_set_style_text_align(tip3_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, lv_network_exception_event, LV_EVENT_CLICKED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_network_exception_event(lv_event_t *e)
{
    //TODO: 回到当前页面
    lv_obj_clean(abnormal_page);
    abnormal_page = NULL;

}

#endif