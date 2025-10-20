#include "../lv_switch_interface.h"

lv_subject_t toast_subject;

static lv_obj_t *top_screen = NULL;
static lv_style_t screen_style;

static void lv_page_style_init();
static void lv_page_style_deinit();
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_setting_format_doing_toast(lv_obj_t *cont);
static void lv_setting_format_success_toast(lv_obj_t *cont);
static void lv_setting_format_failed_toast(lv_obj_t *cont);
static void lv_setting_switch_doing_toast(lv_obj_t *cont);
static void lv_setting_switch_success_toast(lv_obj_t *cont);
static void lv_setting_switch_failed_toast(lv_obj_t *cont);
static void lv_setting_recovery_doing_toast(lv_obj_t *cont);
static void lv_setting_recovery_success_toast(lv_obj_t *cont);
static void lv_setting_recovery_failed_toast(lv_obj_t *cont);

static void lv_page_style_init()
{
    lv_style_init(&screen_style);
    lv_style_set_radius(&screen_style, 0);
    lv_style_set_pad_all(&screen_style, 0);
    lv_style_set_border_width(&screen_style, 0);
    lv_style_set_bg_opa(&screen_style, LV_OPA_TRANSP);
}

static void lv_page_style_deinit()
{
    lv_style_reset(&screen_style);
}

void lv_toast_page_subject_init()
{
    lv_subject_init_int(&toast_subject, PAGE_TOAST_NONE);
    lv_subject_add_observer(&toast_subject, lv_switch_observer_cb, NULL);

    return;
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_INFO("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_TOAST_NONE) return;//注意首次触发

    lv_page_style_init();
    if (page_event != PAGE_TOAST_CONCELL && NULL == top_screen) {
        top_screen = lv_layer_top();
        lv_obj_add_style(top_screen, &screen_style, 0);
    }

    if (NULL != top_screen)
    {
        switch (page_event)
        {
            case PAGE_TOAST_FORMAT_DOING:
                lv_setting_format_doing_toast(top_screen);
                break;
            case PAGE_TOAST_FORMAT_SUCCESS:
                lv_setting_format_success_toast(top_screen);
                break;
            case PAGE_TOAST_FORMAT_ERROR:
                lv_setting_format_failed_toast(top_screen);        
                break;
            case PAGE_TOAST_SWICTH_DOING:
                lv_setting_switch_doing_toast(top_screen);        
                break;
            case PAGE_TOAST_SWICTH_SUCCESS:
                lv_setting_switch_success_toast(top_screen);        
                break;
            case PAGE_TOAST_SWICTH_ERROR:
                lv_setting_switch_failed_toast(top_screen);        
                break;
            case PAGE_TOAST_FACTORY_RESTORE_DOING:
                lv_setting_recovery_doing_toast(top_screen);        
                break;
            case PAGE_TOAST_FACTORY_RESTORE_SUCCESS:
                lv_setting_recovery_success_toast(top_screen);
                break;
            case PAGE_TOAST_FACTORY_RESTORE_ERROR:
                lv_setting_recovery_failed_toast(top_screen);
                break;
            case PAGE_TOAST_CONCELL:
                lv_obj_clean(top_screen); top_screen = NULL;
                break;
            default:
                LV_LOG_WARN("[%s:%d] -- page switch event:%d invaild", __FILE__, __LINE__, page_event);
                break;
        }
    }

    lv_page_style_deinit();
    return;
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
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "格式化中...");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_success.png");
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
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "格式化成功");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_success.png");
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
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "格式化失败");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_fail.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 108, 115);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_switch_doing_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 225, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：切换中...
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "切换中...");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 105, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_switch_success_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 230, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：切换成功
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "切换成功");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_success.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 105, 140);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_setting_switch_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 208, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：切换失败
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "切换失败");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_fail.png");
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
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "恢复出厂设置中…");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_success.png");
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
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "恢复成功");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_RIGHT_MID, -30, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_success.png");
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
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "恢复失败");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_fail.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align(toast, LV_ALIGN_TOP_LEFT, 124, 115);

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}