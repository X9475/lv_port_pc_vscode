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
static void style_init();
static void lv_page_open();
static void lv_page_close();
static void lv_startup_dev(lv_obj_t *cont);
static void lv_startup_login_success(lv_obj_t *cont);
static void lv_startup_login_failed(lv_obj_t *cont);
static void lv_startup_neterror(lv_obj_t *cont);
static void lv_startup_first_user(lv_obj_t *cont);
static void login_success_timer_callback(lv_timer_t *timer);
static void lv_long_pressed_event(lv_event_t *e);
static void set_arc_value(void* obj, int32_t v);
static void anim_finish_callback(lv_anim_t *anim);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *startup_page;
static lv_obj_t *arc;
static lv_style_t style;
static lv_style_t indc_style;
static lv_style_t main_style;
static const lv_font_t *font_24;
static const lv_font_t *font_26;
static const lv_font_t *font_30;
static const lv_font_t *font_34;
static lv_timer_t *timer;
static lv_anim_t arc_anim;
/// @brief 业务相关变量
static uint32_t tick_sec = 5;

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

    //indc_style
    lv_style_init(&indc_style);
    lv_style_set_arc_color(&indc_style, lv_color_hex(0x56D6AA));
    lv_style_set_arc_opa(&indc_style, LV_OPA_60);
    lv_style_set_arc_width(&indc_style, 12);

    //main_style
    lv_style_init(&main_style);
    lv_style_set_arc_color(&main_style, lv_color_hex(0xABABAB));
    lv_style_set_arc_opa(&main_style, LV_OPA_60);
    lv_style_set_arc_width(&main_style, 12);
}

static void lv_page_open()
{
    style_init();

    //屏幕对象
    startup_page = lv_obj_create(NULL);
    lv_obj_clear_flag(startup_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(startup_page, &style, 0);
    lv_obj_center(startup_page);

    // TODO: 根据业务区分调用
    {
        // //网络异常页面
        // lv_startup_neterror(startup_page);
        // //二维码展示
        // lv_startup_dev(startup_page);
        //登录成功页面
        // lv_startup_login_success(startup_page);
        //登录失败页面
        // lv_startup_login_failed(startup_page);
        //登录成功后第一次使用召唤智能体
        lv_startup_first_user(startup_page);
    }

    return;
}

static void lv_page_close()
{
    if (NULL != timer) lv_timer_del(timer);

    lv_font_manager_del_font(font_24);
    lv_font_manager_del_font(font_26);
    lv_font_manager_del_font(font_30);
    lv_font_manager_del_font(font_34);
    lv_font_deinit();

}

static void lv_startup_dev(lv_obj_t *cont)
{
    //识别框
    lv_obj_t *scanning_box = lv_img_create(cont);
    lv_img_set_src(scanning_box, "V:tk1/icon/scanning_box_2x.png");
    lv_img_set_zoom(scanning_box, 128);
    lv_obj_align(scanning_box, LV_ALIGN_CENTER, 0, -30);

    //生成二维码
    lv_obj_t *qr = lv_qrcode_create(cont);
    lv_qrcode_set_size(qr, 169);
    lv_qrcode_set_light_color(qr, lv_color_hex(0xFFFFFF));
    lv_qrcode_set_dark_color(qr, lv_color_hex(0x000000));
    const char *data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_align(qr, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_border_color(qr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(qr, 2, 0);
    lv_obj_set_style_radius(qr, 15, 0);

    //文字提示
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip_label = lv_label_create(cont);
    lv_label_set_text(tip_label, "请使用萤石云视频APP扫描二维码");
    lv_obj_set_style_text_opa(tip_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip_label, font_26, 0);
    lv_obj_set_style_text_color(tip_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip_label, LV_ALIGN_TOP_MID, 0, 320);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_startup_login_success(lv_obj_t *cont)
{
    //错误图案
    lv_obj_t *login_success = lv_img_create(cont);
    lv_img_set_src(login_success, "V:tk1/icon/success_2x.png");
    lv_img_set_zoom(login_success, 128);
    lv_obj_align(login_success, LV_ALIGN_TOP_MID, 0, -100);

    //文字提示
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "添加完成");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_90, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 252);

    if (NULL == font_24) font_24 = font_get_regular(24);
    lv_obj_t *tip2_label = lv_label_create(cont);
    // lv_obj_set_size(tip2_label, 266, 36);
    lv_obj_set_style_opa(tip2_label, LV_OPA_80, 0);
    lv_obj_set_style_text_line_space(tip2_label, 6, 0);
    lv_label_set_text(tip2_label, "即将进入探索之旅");
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_60, 0);
    lv_obj_set_style_text_font(tip2_label, font_24, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 297);

    //倒计时
    timer = lv_timer_create(login_success_timer_callback, 1000, tip2_label);
    lv_timer_set_repeat_count(timer, 6);
    lv_timer_set_auto_delete(timer, false);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void login_success_timer_callback(lv_timer_t *timer)
{
    lv_obj_t *label = lv_timer_get_user_data(timer);
    lv_label_set_text_fmt(label, "即将进入探索之旅（%ds）", tick_sec);

    if (tick_sec == 0)
    {
        tick_sec = 5;
        lv_obj_clean(startup_page);
        //TODO: 执行页面跳转
    }
    tick_sec--;
}

static void lv_startup_login_failed(lv_obj_t *cont)
{
    //错误图案
    lv_obj_t *net_error = lv_img_create(cont);
    lv_img_set_src(net_error, "V:tk1/icon/fail_2x.png");
    lv_img_set_zoom(net_error, 128);
    lv_obj_align(net_error, LV_ALIGN_TOP_MID, 0, -100);

    //文字提示
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "注册平台失败");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 183);

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
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 228);

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

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_startup_neterror(lv_obj_t *cont)
{
    //错误图案
    lv_obj_t *net_error = lv_img_create(cont);
    lv_img_set_src(net_error, "V:tk1/icon/fail_2x.png");
    lv_img_set_zoom(net_error, 128);
    lv_obj_align(net_error, LV_ALIGN_TOP_MID, 0, -100);
    // lv_obj_set_style_border_width(net_error, 2, 0);
    // lv_obj_set_style_border_color(net_error, lv_color_hex(0xFFFFFF), 0);

    //文字提示
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "设备网络异常");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 183);

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
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 228);

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

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_startup_first_user(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *click_face = lv_img_create(cont);
    lv_img_set_src(click_face, "V:tk1/icon/long_press_2x.png");
    lv_img_set_zoom(click_face, 128);
    lv_obj_align(click_face, LV_ALIGN_TOP_MID, -5, -65);

    //环形加载器
    arc = lv_arc_create(cont);
    lv_obj_set_size(arc, 226, 226);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_value(arc, 0);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_style(arc, &indc_style, LV_PART_INDICATOR);
    lv_obj_add_style(arc, &main_style, LV_PART_MAIN);
    lv_obj_align(arc, LV_ALIGN_TOP_MID, 0, 28);

    //设置隐形点击区域 LV_OPA_TRANSP
    lv_obj_t* click_area = lv_btn_create(cont);
    lv_obj_set_size(click_area, 180, 180);
    lv_obj_set_style_shadow_width(click_area, 0, 0);
    lv_obj_set_style_radius(click_area, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(click_area, true, 0);
    lv_obj_add_flag(click_area, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align_to(click_area, arc, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(click_area, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(click_area, lv_long_pressed_event, LV_EVENT_LONG_PRESSED, NULL);
    lv_obj_add_event_cb(click_area, lv_long_pressed_event, LV_EVENT_RELEASED, NULL);

    //文字说明
    if (NULL == font_34) font_34 = font_get_regular(34);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "长按一下");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_90, 0);
    lv_obj_set_style_text_font(tip1_label, font_34, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 289);

    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip2_label = lv_label_create(cont);
    lv_label_set_text(tip2_label, "召唤你的专属搭子");
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_80, 0);
    lv_obj_set_style_text_font(tip2_label, font_26, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 338);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_long_pressed_event(lv_event_t *e)
{
    static bool long_flag = false;
    static bool release_flag = false;
    lv_event_code_t code = lv_event_get_code(e);

    // printf("======================code: %d\n", code);

    if (code == LV_EVENT_LONG_PRESSED && long_flag == false)
    {
        long_flag = true;
        release_flag = false;

        lv_anim_init(&arc_anim);
        lv_anim_set_var(&arc_anim, arc);
        lv_anim_set_exec_cb(&arc_anim, set_arc_value);
        lv_anim_set_time(&arc_anim, 2000);
        lv_anim_set_repeat_count(&arc_anim, 1);
        lv_anim_set_values(&arc_anim, 0, 100);
        //设置动画结束后的回调函数
        lv_anim_set_ready_cb(&arc_anim, anim_finish_callback);
        lv_anim_start(&arc_anim);
    }

    if (code == LV_EVENT_RELEASED && release_flag == false)
    {
        long_flag = false;
        release_flag = true;

        // 获取当前进度值
        int32_t current_value = lv_arc_get_value(arc);

        lv_anim_pause(&arc_anim);
        lv_anim_set_values(&arc_anim, current_value, 0);//反向设置值
        lv_anim_set_time(&arc_anim, current_value * 20);//根据当前进度计算回退时间（按比例）
        lv_anim_set_ready_cb(&arc_anim, NULL);//移除原回调避免冲突
        lv_anim_set_playback_time(&arc_anim, 0);//清除可能的回放设置
        lv_anim_start(&arc_anim);
    }
}

static void set_arc_value(void* obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t*)obj, v);
}

static void anim_finish_callback(lv_anim_t *anim)
{
    printf("====长按完成\n");
    lv_anim_del_all();

    lv_obj_clean(startup_page);
    startup_page = NULL;
    //TODO: 触发召唤出智能体

}

#endif