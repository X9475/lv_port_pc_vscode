/**
 * @file lv_setting_dev.c
 * @author your name (you@domain.com)
 * @brief UI设备设置源文件
 * @version 0.1
 * @date 2025-09-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include <math.h>
#include "lv_setting_dev.h"

#if (LV_CHILD_CAMERA != 0) && (LV_CHILD_CAMERA_SETTING != 0)

/*********************
 *      DEFINES
 *********************/
#define SETTING_NUM     8
#define AUDIO_NUM       5
#define SAVER_NUM       5

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    lv_obj_t *option; //选择对象
    uint8_t select_flag; //选择标志，0：未选择；1：已选择
} saver_option_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_init();
static void lv_page_open();
static void lv_page_close();
static void page_back_cb(lv_event_t *e);
static void lv_setting_more_settings(lv_obj_t *cont);
static void *lv_more_setting_iterm_create(lv_obj_t *cont, const char *name);
static void setting_iterm_click_event_cb(lv_event_cb_t *e);
static void lv_setting_audio_effect(lv_obj_t *cont);
static void *lv_audio_effect_iterm_create(lv_obj_t *cont, const char *name);
static void audio_iterm_click_event_cb(lv_event_cb_t *e);
static void lv_setting_single_record_duration(lv_obj_t *cont);
static void lv_setting_screen_off_time(lv_obj_t *cont);
static void lv_setting_time_display_format(lv_obj_t *cont);
static void lv_setting_vibration_amplitude(lv_obj_t *cont);
static void *lv_roller_iterm_create(lv_obj_t *cont, const char *opts);
static void roller_event_handler(lv_event_t *e);
static void lv_setting_recovery_factory_confirmation(lv_obj_t *cont);
static void lv_setting_certification(lv_obj_t *cont);
static void lv_setting_format_confirmation(lv_obj_t *cont);
static void lv_setting_screen_saver_style(lv_obj_t *cont);
static void scroll_saver_event_cb(lv_event_t *e);
static void screen_saver_timer_cb(lv_timer_t *timer);
static void *screen_saver_create(lv_obj_t *cont, const char *path);
static void screen_saver_click_event_cb(lv_event_t *e);
static void *line_container_create(lv_obj_t *cont);
static void circular_scroll_handle(lv_obj_t *cont, uint8_t dir);
static void lv_cancel_and_confirm_click_event(lv_event_t *e);
static void lv_setting_format_doing_toast(lv_obj_t *cont);
static void lv_setting_format_success_toast(lv_obj_t *cont);
static void lv_setting_format_failed_toast(lv_obj_t *cont);
static void lv_setting_change_doing_toast(lv_obj_t *cont);
static void lv_setting_change_success_toast(lv_obj_t *cont);
static void lv_setting_change_failed_toast(lv_obj_t *cont);
static void lv_setting_recovery_doing_toast(lv_obj_t *cont);
static void lv_setting_recovery_success_toast(lv_obj_t *cont);
static void lv_setting_recovery_failed_toast(lv_obj_t *cont);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *setting_page;
static lv_style_t style;
static lv_style_t style_roller;
static lv_style_t style_select_roller;
static lv_style_t style_mask;
static const lv_font_t *font_24;
static const lv_font_t *font_26;
static const lv_font_t *font_28;
static const lv_font_t *font_30;
static const lv_font_t *font_30B;
static const lv_font_t *font_32;
static const lv_font_t *font_34;
static const lv_font_t *font_40;
static lv_obj_t *confirm;
static lv_obj_t *cancel;
static lv_obj_t *sd_confirm;
static lv_obj_t *sd_cancel;
static lv_obj_t *line_cont;

static lv_timer_t *anim_timer = NULL;
static uint8_t exec_count = 0;

static saver_option_t saver_option_list[SAVER_NUM];
static const char *setting_list[SETTING_NUM] = {
    "单次录像时长","熄屏时间","时间展示形式","存储管理", \
    "振动幅度","关于相机","恢复出厂设置","认证标志"
};
static const char *audio_list[AUDIO_NUM] = {
    "拍照音","呼叫铃声","接听铃声","消息提示音", "按键音"
};
static const char *saver_list[SAVER_NUM] = {
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png",
    "V:tk1/icon/screen_saver.png"
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

    //style_roller
    lv_style_init(&style_roller);
    lv_style_copy(&style_roller, &style);
    lv_style_set_border_opa(&style_roller, LV_OPA_COVER);
    lv_style_set_border_width(&style_roller, 1);
    lv_style_set_border_color(&style_roller, lv_color_hex(0x38383A));
    lv_style_set_border_side(&style_roller, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_text_align(&style_roller, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&style_roller, font_get_regular(60));//字体大小60px
    lv_style_set_text_line_space(&style_roller, 30);//行间距

    //style_select_roller
    lv_style_init(&style_select_roller);
    lv_style_copy(&style_select_roller, &style);
    lv_style_set_text_color(&style_select_roller, lv_color_hex(0XAFF99C));
    lv_style_set_text_align(&style_select_roller, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&style_select_roller, font_get_regular(70));//字体大小70px

    //图层蒙板
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[0].opa = LV_OPA_TRANSP;
    grad.stops[1].color = lv_color_hex(0x000000);
    grad.stops[1].opa = LV_OPA_COVER;
    grad.stops[0].frac = 0;
    grad.stops[1].frac = 255;
    lv_style_init(&style_mask);
    lv_style_copy(&style_mask, &style);
    lv_style_set_bg_grad(&style_mask, &grad);
    lv_style_set_bg_grad_dir(&style_mask, LV_GRAD_DIR_VER);

}

static void lv_page_open()
{
    style_init();

    //屏幕对象
    setting_page = lv_obj_create(NULL);
    lv_obj_remove_style_all(setting_page);
    lv_obj_clear_flag(setting_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(setting_page, &style, 0);
    lv_obj_center(setting_page);

    // TODO: 根据业务区分调用
    {
        //更多设置
        // lv_setting_more_settings(setting_page);
        //音频效果
        // lv_setting_audio_effect(setting_page);
        //单次录像时间
        // lv_setting_single_record_duration(setting_page);
        //熄屏时间
        // lv_setting_screen_off_time(setting_page);
        //时间展示形式
        // lv_setting_time_display_format(setting_page);
        //存储管理
        // lv_setting_storage_manage(setting_page);
        //振动幅度
        // lv_setting_vibration_amplitude(setting_page);
        //关于相机
        // lv_setting_camera_about(setting_page);
        //恢复出厂设置确认
        // lv_setting_recovery_factory_confirmation(setting_page);
        //认证标志
        // lv_setting_certification(setting_page);
        //格式确认
        // lv_setting_format_confirmation(setting_page);
        //屏保样式
        lv_setting_screen_saver_style(setting_page);
        //格式化中toast
        // lv_setting_format_doing_toast(setting_page);
        //格式化成功
        // lv_setting_format_success_toast(setting_page);
        //格式化失败
        // lv_setting_format_failed_toast(setting_page);
        //切换中toast
        // lv_setting_change_doing_toast(setting_page);
        //切换成功
        // lv_setting_change_success_toast(setting_page);
        //切换失败
        // lv_setting_change_failed_toast(setting_page);
        //恢复出厂中
        // lv_setting_recovery_doing_toast(setting_page);
        //恢复出厂成功
        // lv_setting_recovery_success_toast(setting_page);
        //恢复出厂失败
        // lv_setting_recovery_failed_toast(setting_page);
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
    lv_font_manager_del_font(font_34);
    lv_font_manager_del_font(font_40);
    lv_font_deinit();
}

static void lv_setting_more_settings(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //滚动列表
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 340);
    lv_obj_add_style(cont_col, &style, 0);
    lv_obj_align_to(cont_col, cont, LV_ALIGN_TOP_MID, 20, 70);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);

    for (uint8_t i = 0; i < SETTING_NUM; i++)
    {
        //创建子功能按钮
        lv_obj_t *btn = lv_more_setting_iterm_create(cont_col, setting_list[i]);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(btn, setting_iterm_click_event_cb, LV_EVENT_CLICKED, setting_list[i]);
    }

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void *lv_more_setting_iterm_create(lv_obj_t *cont, const char *name)
{
    //背景
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 462, 98);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x121212), 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    //文字
    if (NULL == font_28) font_28 = font_get_regular(28);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, font_28, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 30, 0);

    //进入指示
    lv_obj_t *image = lv_img_create(btn);
    lv_obj_set_size(image, 7, 13);
    lv_img_set_src(image, "V:tk1/icon/common_icon_back.png");
    lv_img_set_angle(image, 1800);
    lv_img_set_zoom(image, 160);
    lv_obj_align_to(image, btn, LV_ALIGN_RIGHT_MID, -31, 0);

    return btn;
}

static void setting_iterm_click_event_cb(lv_event_cb_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    const char *name = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        printf("Click %s\n", name);
        if (lv_strcmp(name, "单次录像时长") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "熄屏时间") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "时间展示形式") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "存储管理") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "振动幅度") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "关于相机") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "恢复出厂设置") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "认证标志") == 0)
        {
            //TODO: 页面跳转
        }
    }
}

static void lv_setting_audio_effect(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //滚动列表
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 340);
    lv_obj_add_style(cont_col, &style, 0);
    lv_obj_align_to(cont_col, cont, LV_ALIGN_TOP_MID, 20, 70);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);

    for (uint8_t i = 0; i < AUDIO_NUM; i++)
    {
        //创建子功能按钮
        lv_obj_t *btn = lv_audio_effect_iterm_create(cont_col, audio_list[i]);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(btn, audio_iterm_click_event_cb, LV_EVENT_CLICKED, audio_list[i]);
    }

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void page_back_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_CLICKED == code)
    {
        lv_obj_clean(setting_page);
        setting_page = NULL;
        //TODO: 回到上个页面

    }
}

static void audio_iterm_click_event_cb(lv_event_cb_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    const char *name = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        printf("Click %s\n", name);
        if (lv_strcmp(name, "拍照音") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "呼叫铃声") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "接听铃声") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "消息提示音") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "按键音") == 0)
        {
            //TODO: 页面跳转
        }
    }
}

static void *lv_audio_effect_iterm_create(lv_obj_t *cont, const char *name)
{
    //背景
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 462, 98);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x121212), 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    //文字
    if (NULL == font_28) font_28 = font_get_regular(28);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, font_28, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 30, 0);

    //进入指示
    lv_obj_t *image = lv_img_create(btn);
    lv_obj_set_size(image, 7, 13);
    lv_img_set_src(image, "V:tk1/icon/common_icon_back.png");
    lv_img_set_angle(image, 1800);
    lv_img_set_zoom(image, 160);
    lv_obj_align_to(image, btn, LV_ALIGN_RIGHT_MID, -31, 0);

    return btn;
}

static void lv_setting_single_record_duration(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //文字
    if (NULL == font_30B) font_30B = font_get_bold(30);
    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "单次录像时间");
    lv_obj_set_style_text_font(header, font_30B, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align_to(header, cont, LV_ALIGN_TOP_LEFT, 80, 26);

    lv_obj_t *roller = lv_roller_iterm_create(cont, "15s\n30s\n1min");
    lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void *lv_roller_iterm_create(lv_obj_t *cont, const char *opts)
{
    lv_obj_t *roller = lv_roller_create(cont);
    lv_obj_add_style(roller, &style, 0);

    lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(roller, 1, LV_ANIM_OFF);
    lv_roller_set_visible_row_count(roller, 3);

    lv_obj_add_style(roller, &style_roller, LV_PART_MAIN);
    lv_obj_add_style(roller, &style_select_roller, LV_PART_SELECTED);

    lv_obj_set_size(roller, lv_pct(100), 240);
    lv_obj_align(roller, LV_ALIGN_TOP_MID, 0, 105);

    return roller;
}

static void roller_event_handler(lv_event_t *e)
{
    static int32_t last_index = -1;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    char buf[32];
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        if (last_index != lv_roller_get_selected(obj))
        {//选择项发生变化
            last_index = lv_roller_get_selected(obj);
            lv_roller_get_selected_str(obj, buf, sizeof(buf));

            if (lv_strcmp("15s\n30s\n1min", lv_roller_get_options(obj)) == 0)
            {//单次录像时长
                printf("1. roller selected value: %s\n", buf);
                //TODO: 通知业务同步处理
            }
            else if (lv_strcmp("5s\n10s\n30s", lv_roller_get_options(obj)) == 0)
            {//熄屏时间
                printf("2. roller selected value: %s\n", buf);
                //TODO: 通知业务同步处理
            }
            else if (lv_strcmp("12小时制\n24小时制", lv_roller_get_options(obj)) == 0)
            {//时间展示形式
                printf("3. roller selected value: %s\n", buf);
                //TODO: 通知业务同步处理
            }
            else if (lv_strcmp("低\n中\n高", lv_roller_get_options(obj)) == 0)
            {//振动幅度
                printf("4. roller selected value: %s\n", buf);
                //TODO: 通知业务同步处理
            }
        }
    }
}

static void lv_setting_screen_off_time(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //文字
    if (NULL == font_30B) font_30B = font_get_bold(30);
    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "熄屏时间");
    lv_obj_set_style_text_font(header, font_30B, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align_to(header, cont, LV_ALIGN_TOP_LEFT, 80, 26);

    lv_obj_t *roller = lv_roller_iterm_create(cont, "5s\n10s\n30s");
    lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_time_display_format(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //文字
    if (NULL == font_30B) font_30B = font_get_bold(30);
    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "时间展示形式");
    lv_obj_set_style_text_font(header, font_30B, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align_to(header, cont, LV_ALIGN_TOP_LEFT, 80, 26);

    lv_obj_t *roller = lv_roller_iterm_create(cont, "12小时制\n24小时制");
    lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_vibration_amplitude(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //文字
    if (NULL == font_30B) font_30B = font_get_bold(30);
    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "振动幅度");
    lv_obj_set_style_text_font(header, font_30B, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align_to(header, cont, LV_ALIGN_TOP_LEFT, 80, 26);

    lv_obj_t *roller = lv_roller_iterm_create(cont, "低\n中\n高");
    lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_recovery_factory_confirmation(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *storage = lv_img_create(cont);
    lv_img_set_src(storage, "V:tk1/icon/storage.png");
    lv_img_set_zoom(storage, 128);
    lv_obj_set_size(storage, 380, 210);
    lv_obj_align(storage, LV_ALIGN_TOP_MID, 0, 25);

    //文字提示：确定将设备恢复出厂设置吗？
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "确定将设备恢复出厂设置吗？");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 245);

    //取消
    cancel = lv_btn_create(cont);
    lv_obj_set_size(cancel, 148, 70);
    lv_obj_align(cancel, LV_ALIGN_BOTTOM_LEFT, 78, -30);
    lv_obj_set_style_radius(cancel, 51, 0);
    lv_obj_set_style_bg_opa(cancel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cancel, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_shadow_opa(cancel, LV_OPA_TRANSP, 0);
    lv_obj_t *img_cancel = lv_img_create(cancel);
    lv_img_set_src(img_cancel, "V:tk1/icon/common_icon_cancel_button.png");
    lv_obj_set_size(img_cancel, 50, 50);
    lv_obj_align_to(img_cancel, cancel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(cancel, lv_cancel_and_confirm_click_event, LV_EVENT_CLICKED, cancel);

    //确认
    confirm = lv_btn_create(cont);
    lv_obj_set_size(confirm, 148, 70);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_RIGHT, -78, -30);
    lv_obj_set_style_radius(confirm, 51, 0);
    lv_obj_set_style_bg_opa(confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(confirm, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(confirm, LV_OPA_TRANSP, 0);
    lv_obj_t *img_confirm = lv_img_create(confirm);
    lv_img_set_src(img_confirm, "V:tk1/icon/common_icon_ok_button.png");
    lv_obj_set_size(img_confirm, 50, 50);
    lv_obj_align_to(img_confirm, confirm, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_image_recolor_opa(img_confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img_confirm, lv_color_hex(0x0A0B0D), 0);
    lv_obj_add_event_cb(confirm, lv_cancel_and_confirm_click_event, LV_EVENT_CLICKED, confirm);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_cancel_and_confirm_click_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED)
    {
        if (btn == confirm)
        {//确认恢复出厂设置
            printf("confirm click\n");
            //TODO: 通知业务同步处理
        }
        else if (btn == cancel)
        {//取消恢复出厂设置
            printf("cancel click\n");
            //TODO: 通知业务同步处理
        }
        else if (btn == sd_cancel)
        {//取消SD卡格式化
            printf("sd_cancel click\n");
            //TODO: 通知业务同步处理
        }
        else if (btn == sd_confirm)
        {//格式化SD卡
            printf("sd_confirm click\n");
            //TODO: 通知业务同步处理
        }
        lv_obj_clean(setting_page);
        setting_page = NULL;
        //TODO: 回到上个页面
    }
}

static void lv_setting_certification(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //背景图
    lv_obj_t *image = lv_img_create(cont);
    lv_img_set_src(image, "V:tk1/icon/setting_icon_legal_notice.png");
    lv_obj_set_size(image, 140, 140);
    lv_obj_align_to(image, cont, LV_ALIGN_TOP_MID, 0, 20);

    //文字提示：认证标识
    if (NULL == font_40) font_40 = font_get_regular(40);
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "认证标识");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_40, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 147);

    //创建滚动容器作为文本的父容器
    lv_obj_t *scroll_cont = lv_obj_create(cont);
    lv_obj_set_size(scroll_cont, 422, 201);
    lv_obj_add_style(scroll_cont, &style, LV_PART_MAIN);
    lv_obj_align(scroll_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(scroll_cont, LV_FLEX_FLOW_COLUMN);

    //创建长文本标签
    if (NULL == font_28) font_28 = font_get_regular(28);
    lv_obj_t *description = lv_label_create(scroll_cont);
    lv_label_set_long_mode(description, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(description, lv_pct(100));
    lv_obj_set_style_text_font(description, font_28, 0);
    lv_obj_set_style_text_opa(description, LV_OPA_60, 0);
    lv_obj_set_style_text_color(description, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(description, LV_TEXT_ALIGN_CENTER, 0);

    //设置长文本内容
    const char *long_text = "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?";
    lv_label_set_text(description, long_text);

    // 确保内容可以滚动
    lv_obj_set_scroll_dir(scroll_cont, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scroll_cont, LV_SCROLLBAR_MODE_AUTO);

    //文字蒙层
    lv_obj_t *mask = lv_obj_create(cont);
    lv_obj_add_style(mask, &style_mask, 0);
    lv_obj_set_size(mask, lv_pct(100), 100);
    lv_obj_align(mask, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_clear_flag(mask, LV_OBJ_FLAG_CLICKABLE);//禁止拦截点击事件

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_format_confirmation(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *storage = lv_img_create(cont);
    lv_img_set_src(storage, "V:tk1/icon/format_sd.png");
    lv_img_set_zoom(storage, 128);
    lv_obj_set_size(storage, 380, 210);
    lv_obj_align(storage, LV_ALIGN_TOP_MID, 0, 25);

    //文字提示：确定将SD卡格式化吗？
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "确定将SD卡格式化吗？");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, font_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 245);

    //取消
    sd_cancel = lv_btn_create(cont);
    lv_obj_set_size(sd_cancel, 148, 70);
    lv_obj_align(sd_cancel, LV_ALIGN_BOTTOM_LEFT, 78, -30);
    lv_obj_set_style_radius(sd_cancel, 51, 0);
    lv_obj_set_style_bg_opa(sd_cancel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(sd_cancel, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_shadow_opa(sd_cancel, LV_OPA_TRANSP, 0);
    lv_obj_t *img_cancel = lv_img_create(sd_cancel);
    lv_img_set_src(img_cancel, "V:tk1/icon/common_icon_cancel_button.png");
    lv_obj_set_size(img_cancel, 50, 50);
    lv_obj_align_to(img_cancel, sd_cancel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(sd_cancel, lv_cancel_and_confirm_click_event, LV_EVENT_CLICKED, sd_cancel);

    //确认
    sd_confirm = lv_btn_create(cont);
    lv_obj_set_size(sd_confirm, 148, 70);
    lv_obj_align(sd_confirm, LV_ALIGN_BOTTOM_RIGHT, -78, -30);
    lv_obj_set_style_radius(sd_confirm, 51, 0);
    lv_obj_set_style_bg_opa(sd_confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(sd_confirm, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(sd_confirm, LV_OPA_TRANSP, 0);
    lv_obj_t *img_confirm = lv_img_create(sd_confirm);
    lv_img_set_src(img_confirm, "V:tk1/icon/common_icon_ok_button.png");
    lv_obj_set_size(img_confirm, 50, 50);
    lv_obj_align_to(img_confirm, sd_confirm, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_image_recolor_opa(img_confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img_confirm, lv_color_hex(0x0A0B0D), 0);
    lv_obj_add_event_cb(sd_confirm, lv_cancel_and_confirm_click_event, LV_EVENT_CLICKED, sd_confirm);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_screen_saver_style(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "V:tk1/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_cb, LV_EVENT_CLICKED, NULL);

    //右侧滚动条
    line_cont = line_container_create(cont);

    //创建屏保转盘
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_add_style(cont_col, &style, 0);
    // lv_obj_set_style_border_color(cont_col, lv_color_hex(0xFFFFFF), 0);
    // lv_obj_set_style_border_width(cont_col, 1, 0);
    lv_obj_set_size(cont_col, 380, lv_pct(100));
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 80, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont_col, scroll_saver_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_flex_align(cont_col, 
                                LV_FLEX_ALIGN_CENTER,
                                LV_FLEX_ALIGN_CENTER,
                                LV_FLEX_ALIGN_CENTER);

    for (uint8_t i = 0; i < SAVER_NUM; i++)
    {
        lv_obj_t *saver = screen_saver_create(cont_col, saver_list[i]);
        saver_option_list[i].option = lv_obj_get_child(saver, 1);
        if (i == 0) {
            saver_option_list[i].select_flag = 1;
            lv_img_set_src(saver_option_list[i].option, "V:tk1/icon/photograph_icon_select_green.png");
            continue;
        }
        saver_option_list[i].select_flag = 0;
    }

    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, 0), LV_ANIM_OFF);
    lv_obj_send_event(cont_col, LV_EVENT_SCROLL, NULL);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void scroll_saver_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);

    lv_obj_t *first = lv_obj_get_child(cont, 0);
    lv_area_t first_a;
    lv_obj_get_coords(first, &first_a);

    //判断滚动方向
    static uint8_t dir = 0;//0 nul, 1 up, 2 down
    static int32_t last_y = 0xffffffff;

    if (last_y == 0xffffffff) last_y = first_a.y1;

    dir = first_a.y1 > last_y? 2 : 1;
    last_y = first_a.y1;

    //倾斜变换
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);

    static uint8_t last_idx = 0;
    static uint8_t cur_idx = 0;

    for (uint32_t i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        if (LV_ABS(child_a.y1 - 95) < 20) cur_idx = i;

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = child_y_center - cont_y_center;

        int32_t r = lv_obj_get_height(cont);
        uint32_t x_sqr = r * r - LV_ABS(diff_y) * LV_ABS(diff_y);

        lv_sqrt_res_t res;
        lv_sqrt(x_sqr, &res, 0x8000);
        int32_t x = r - res.i;

        //设置旋转中心为右侧边缘中间点
        lv_obj_set_style_transform_pivot_x(child, 610, 0);
        lv_obj_set_style_transform_pivot_y(child, 205, 0);

        int32_t angle = -(diff_y) / 3;
        angle = LV_ABS(diff_y) >= 60? angle : 0;
        lv_obj_set_style_translate_x(child, x - 20, 0);
        lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
    }

    if (cur_idx != last_idx) {
        last_idx = cur_idx;
        if (!anim_timer) {
            //创建定时器，每40ms执行滚动动态
            anim_timer = lv_timer_create(screen_saver_timer_cb, 40, &dir);
            exec_count = 0;
        }
    }
}

static void screen_saver_timer_cb(lv_timer_t *timer)
{
    uint8_t *pdir = lv_timer_get_user_data(timer);

    circular_scroll_handle(line_cont, *pdir);
    //次数控制
    if(++exec_count >= 15) {
        lv_timer_del(anim_timer);
        anim_timer = NULL;
    }
}

static void *screen_saver_create(lv_obj_t *cont, const char *path)
{
    lv_obj_t *saver = lv_obj_create(cont);
    lv_obj_remove_style_all(saver);
    lv_obj_set_size(saver, 280, 220);
    lv_obj_set_style_radius(saver, 20, 0);
    lv_obj_set_style_clip_corner(saver, true, 0);
    lv_obj_align(saver, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *image = lv_img_create(saver);
    lv_obj_set_size(image, 280, 220);
    lv_img_set_src(image, path);
    lv_obj_align(image, LV_ALIGN_CENTER, 0, 0);

    //未选择
    lv_obj_t *option = lv_img_create(saver);
    lv_img_set_src(option, "V:tk1/icon/photograph_icon_unselect.png");
    lv_obj_align(option, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_add_flag(option, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_add_event_cb(saver, screen_saver_click_event_cb, LV_EVENT_CLICKED, NULL);

    return saver;
}

static void screen_saver_click_event_cb(lv_event_t *e)
{
    lv_obj_t *saver = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_CLICKED == code)
    {
        lv_obj_t *option = lv_obj_get_child(saver, 1);
        for (uint8_t i = 0; i < SAVER_NUM; i++)
        {
            saver_option_t *saver_option = &saver_option_list[i];
            if (option == saver_option->option) {
                if (saver_option->select_flag == 0) {
                    saver_option->select_flag = 1;
                    lv_img_set_src(saver_option->option, "V:tk1/icon/photograph_icon_select_green.png");
                }
                else {
                    saver_option->select_flag = 0;
                    lv_img_set_src(saver_option->option, "V:tk1/icon/photograph_icon_unselect.png");
                }
                break;
            }
        }
    }
}

static void *line_container_create(lv_obj_t *cont)
{
    lv_obj_t *line_cont = lv_obj_create(cont);
    lv_obj_add_style(line_cont, &style, 0);
    lv_obj_set_size(line_cont, 20, 150);
    lv_obj_align(line_cont, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_obj_set_flex_flow(line_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(line_cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(line_cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(line_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(line_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_row(line_cont, 8, 0);

    for (uint8_t i = 0; i < 25; i++)
    {
        lv_obj_t *line = lv_obj_create(line_cont);
        lv_obj_remove_style_all(line);
        lv_obj_set_size(line, 30, 2);
        lv_obj_set_style_bg_opa(line, LV_OPA_20, 0);
        lv_obj_set_style_bg_color(line, lv_color_hex(0xFFFFFF), 0);
    }

    //初始位置设置为中间的子对象
    lv_obj_scroll_to_view(lv_obj_get_child(line_cont, 12), LV_ANIM_OFF);
    circular_scroll_handle(line_cont, 0);

    return line_cont;
}

static void circular_scroll_handle(lv_obj_t *cont, uint8_t dir)
{
    lv_coord_t child_cnt = lv_obj_get_child_cnt(cont);
    if(child_cnt < 2) return;

    //获取当前中心坐标
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    //针对移动后的位置绘制曲线
    for (uint32_t i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = child_y_center - cont_y_center;

        // int32_t x = LV_ABS(diff_y * 1 / 4);
        int32_t x = 14.252f - 18.252f * expf(-0.085f * LV_ABS(diff_y));
        lv_obj_set_style_translate_x(child, x, 0);

        if (LV_ABS(diff_y) < 20) {//局部透明度
            lv_obj_set_style_bg_opa(child, LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(child, LV_OPA_20, 0);
        }
    }

    if (dir == 1)
        lv_obj_scroll_by(cont, 0, -1, LV_ANIM_OFF);

    if (dir == 2)
        lv_obj_scroll_by(cont, 0, 1, LV_ANIM_OFF);
}

static void lv_setting_format_doing_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 262, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：格式化中...
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "格式化中...");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "V:tk1/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 90, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_format_success_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 262, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：格式化成功
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "格式化成功");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "V:tk1/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 90, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_format_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 240, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：格式化失败
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "格式化失败");
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
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 108, 115);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_change_doing_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 225, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：切换中...
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "切换中...");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "V:tk1/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 105, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_change_success_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 230, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：切换成功
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "切换成功");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "V:tk1/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 105, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_change_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 208, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：切换失败
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "切换失败");
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
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 124, 115);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_recovery_doing_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 353, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：恢复出厂设置中…
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "恢复出厂设置中…");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "V:tk1/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 35, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_recovery_success_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 230, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：恢复成功
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "恢复成功");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "V:tk1/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 105, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_recovery_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 208, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：恢复失败
    if (NULL == font_32) font_32 = font_get_regular(32);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "恢复失败");
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
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 124, 115);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

#endif