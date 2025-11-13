#include "../lv_switch_interface.h"

lv_subject_t toast_subject;
static lv_style_t screen_style;
static lv_style_t page_style_bg;
static lv_anim_t anim;

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
static void lv_abnorml_storage_exhausted_toast(lv_obj_t *cont);
static void lv_setting_no_sdcard_toast(lv_obj_t *cont);
static void lv_setting_sdcard_abnormal_toast(lv_obj_t *cont);
static void lv_setting_dev_offline_toast(lv_obj_t *cont);
static void lv_setting_call_failed_toast(lv_obj_t *cont);
static void lv_setting_add_success_toast(lv_obj_t *cont);
static void lv_setting_add_failed_toast(lv_obj_t *cont);
static void lv_setting_max_valume_toast(lv_obj_t *cont);
static void lv_setting_min_valume_toast(lv_obj_t *cont);
static void lv_setting_network_instability_toast(lv_obj_t *cont);
static void lv_setting_message_failed_toast(lv_obj_t *cont);
static void lv_setting_send_failed_toast(lv_obj_t *cont);
static void lv_menu_setting_down_slide_toast(lv_obj_t *cont);
static void lv_menu_up_slide_toast(lv_obj_t *cont);
static void anim_set_hand_pos(void *obj, int32_t v);
static void page_click_event_cb(lv_event_t *e);

static void lv_page_style_init()
{
    lv_style_init(&screen_style);
    lv_style_set_radius(&screen_style, 0);
    lv_style_set_pad_all(&screen_style, 0);
    lv_style_set_border_width(&screen_style, 0);
    lv_style_set_bg_opa(&screen_style, LV_OPA_TRANSP);

    lv_style_init(&page_style_bg);
    lv_style_copy(&page_style_bg, &screen_style);
    lv_style_set_bg_color(&page_style_bg, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&page_style_bg, LV_OPA_80);
}

static void lv_page_style_deinit()
{
    lv_style_reset(&screen_style);
    lv_style_reset(&page_style_bg);
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

    static lv_obj_t *cont;
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_INFO("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_TOAST_NONE) return;//注意首次触发

    lv_page_style_init();
    if (page_event != PAGE_TOAST_CONFIRM)
    {
        if (lv_obj_is_valid(top_screen))
        {
            cont = lv_obj_create(top_screen);
            lv_obj_add_style(cont, &screen_style, 0);
            lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES);
            lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_center(cont);

            lv_current_page_info_set(page_event, NULL);
        }
    }

    switch (page_event)
    {
        case PAGE_TOAST_FORMAT_DOING:
            lv_setting_format_doing_toast(cont);
            break;
        case PAGE_TOAST_FORMAT_SUCCESS:
            lv_setting_format_success_toast(cont);
            break;
        case PAGE_TOAST_FORMAT_ERROR:
            lv_setting_format_failed_toast(cont);        
            break;
        case PAGE_TOAST_SWICTH_DOING:
            lv_setting_switch_doing_toast(cont);        
            break;
        case PAGE_TOAST_SWICTH_SUCCESS:
            lv_setting_switch_success_toast(cont);        
            break;
        case PAGE_TOAST_SWICTH_ERROR:
            lv_setting_switch_failed_toast(cont);        
            break;
        case PAGE_TOAST_FACTORY_RESTORE_DOING:
            lv_setting_recovery_doing_toast(cont);        
            break;
        case PAGE_TOAST_FACTORY_RESTORE_SUCCESS:
            lv_setting_recovery_success_toast(cont);
            break;
        case PAGE_TOAST_FACTORY_RESTORE_ERROR:
            lv_setting_recovery_failed_toast(cont);
            break;
        case PAGE_TOAST_STORAGE_EXHAUSTED:
            lv_abnorml_storage_exhausted_toast(cont);
            break;
        case PAGE_TOAST_NO_SDCARD:
            lv_setting_no_sdcard_toast(cont);
            break;
        case PAGE_TOAST_SDCARD_ABNORMAL:
            lv_setting_sdcard_abnormal_toast(cont);
            break;
        case PAGE_TOAST_DEV_OFFLINE:
            lv_setting_dev_offline_toast(cont);
            break;
        case PAGE_TOAST_CALL_FAILED_NET_BAD:
            lv_setting_call_failed_toast(cont);
            break;
        case PAGE_TOAST_ADD_SUCCESS:
            lv_setting_add_success_toast(cont);
            break;
        case PAGE_TOAST_ADD_FAILED_RETRY:
            lv_setting_add_failed_toast(cont);
            break;
        case PAGE_TOAST_MAX_VALUME:
            lv_setting_max_valume_toast(cont);
            break;
        case PAGE_TOAST_MIN_VALUME:
            lv_setting_min_valume_toast(cont);
            break;
        case PAGE_TOAST_NETWORK_INSTABILITY:
            lv_setting_network_instability_toast(cont);
            break;
        case PAGE_TOAST_MESSAGE_FAILED:
            lv_setting_message_failed_toast(cont);
            break;
        case PAGE_TOAST_SEND_FAILED:
            lv_setting_send_failed_toast(cont);
            break;
        case PAGE_TOAST_SETTING_MENU_SLIDE://滑动出设置菜单提示
            lv_menu_setting_down_slide_toast(cont);
            break;
        case PAGE_TOAST_MENU_SLIDE://滑动出菜单提示
            lv_menu_up_slide_toast(cont);
            break;
        case PAGE_TOAST_CONFIRM:
            lv_page_style_deinit();
            if (lv_obj_is_valid(top_screen)) lv_obj_clean(top_screen);
            lv_subject_set_int(&toast_subject, PAGE_TOAST_NONE);//重置为初始状态
            break;
        default:
            LV_LOG_WARN("[%s:%d] -- page switch event:%d invaild", __FILE__, __LINE__, page_event);
            break;
    }

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

    //文字提示：格式化中…
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "格式化中…");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -32, -30);

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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -32, -30);

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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

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

    //文字提示：切换中…
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "切换中…");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -32, -30);

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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -32, -30);

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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -32, -30);

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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -32, -30);

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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_abnorml_storage_exhausted_toast(lv_obj_t *cont)
{
    //文字提示：存储容量耗尽无法拍摄
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
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    //背景图
    lv_obj_t *toast = lv_img_create(cont);
    lv_img_set_src(toast, "../lv_port_pc_vscode/assert/icon/toast_fail.png");
    lv_img_set_zoom(toast, 128);
    lv_obj_set_size(toast, 120, 80);
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_no_sdcard_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 400, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：无SD卡无法拍摄和查看
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "无SD卡无法拍摄和查看");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_sdcard_abnormal_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 400, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：SD卡异常无法拍摄和查看
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "SD卡异常无法拍摄和查看");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_dev_offline_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 368, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：设备离线，无法使用
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "设备离线，无法使用");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_call_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 368, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：呼叫失败，网络异常
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "呼叫失败，网络异常");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_add_success_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 230, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：添加成功
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "添加成功");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -32, -30);

    return;
}

static void lv_setting_add_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 336, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：添加失败，请重试
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "添加失败，请重试");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_max_valume_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 304, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：已达到最大音量
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "已达到最大音量");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_min_valume_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 304, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：已达到最小音量
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "已达到最小音量");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_network_instability_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 304, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：当前网络不稳定
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "当前网络不稳定");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_message_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 368, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：留言失败，网络异常
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "留言失败，网络异常");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_setting_send_failed_toast(lv_obj_t *cont)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 208, 92);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(obj, 16, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);

    //文字提示：发送失败
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, "发送失败");
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
    lv_obj_align_to(toast, obj, LV_ALIGN_BOTTOM_LEFT, -20, -52);

    return;
}

static void lv_menu_setting_down_slide_toast(lv_obj_t *cont)
{
    lv_obj_t *background = lv_obj_create(cont);
    lv_obj_remove_style_all(background);
    lv_obj_set_size(background, lv_pct(100), lv_pct(100));
    lv_obj_add_style(background, &page_style_bg, 0);
    lv_obj_align(background, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 600, 600);
    lv_obj_set_style_radius(obj, 300, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, -350);

    lv_obj_t *slide = lv_obj_create(cont);
    lv_obj_remove_style_all(slide);
    lv_obj_set_size(slide, 13, 70);
    lv_obj_set_style_radius(slide, 5, 0);
    lv_obj_set_style_bg_color(slide, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_bg_opa(slide, LV_OPA_COVER, 0);
    lv_obj_align_to(slide, cont, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *label = lv_label_create(cont);
    lv_obj_set_size(label, 402, 74);
    lv_label_set_text(label, "向下滑动\n进入全局设置页");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_28, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, cont, LV_ALIGN_TOP_MID, 0, 158);

    //移动手势
    lv_obj_t *hand = lv_img_create(cont);
    lv_img_set_src(hand, "../lv_port_pc_vscode/assert/icon/common_icon_hand_up.png");
    lv_obj_align_to(hand, cont, LV_ALIGN_TOP_MID, 68, 0);
    lv_obj_set_user_data(hand, slide);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, hand);
    lv_anim_set_exec_cb(&anim, anim_set_hand_pos);
    lv_anim_set_time(&anim, 1500);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_values(&anim, -60, 0);
    lv_anim_start(&anim);

    lv_obj_add_flag(obj, LV_EVENT_CLICKED);
    lv_obj_add_event_cb(obj, page_click_event_cb, LV_EVENT_CLICKED, &anim);
    return;
}

static void lv_menu_up_slide_toast(lv_obj_t *cont)
{
    lv_obj_t *background = lv_obj_create(cont);
    lv_obj_remove_style_all(background);
    lv_obj_set_size(background, lv_pct(100), lv_pct(100));
    lv_obj_add_style(background, &page_style_bg, 0);
    lv_obj_align(background, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 600, 600);
    lv_obj_set_style_radius(obj, 300, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 350);

    lv_obj_t *slide = lv_obj_create(cont);
    lv_obj_remove_style_all(slide);
    lv_obj_set_size(slide, 13, 70);
    lv_obj_set_style_radius(slide, 5, 0);
    lv_obj_set_style_bg_color(slide, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_bg_opa(slide, LV_OPA_COVER, 0);
    lv_obj_align_to(slide, cont, LV_ALIGN_BOTTOM_MID, 0, -80);

    //盖板
    lv_obj_t *mask = lv_obj_create(cont);
    lv_obj_remove_style_all(mask);
    lv_obj_set_size(mask, 15, 100);
    lv_obj_set_style_bg_color(mask, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(mask, LV_OPA_COVER, 0);
    lv_obj_align(mask, LV_ALIGN_BOTTOM_MID, 0, -40);

    lv_obj_t *label = lv_label_create(cont);
    lv_obj_set_size(label, 402, 74);
    lv_label_set_text(label, "向上滑动\n进入功能菜单页");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_28, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, cont, LV_ALIGN_BOTTOM_MID, 0, -40);

    //移动手势
    lv_obj_t *hand = lv_img_create(cont);
    lv_img_set_src(hand, "../lv_port_pc_vscode/assert/icon/common_icon_hand_up.png");
    lv_obj_align_to(hand, cont, LV_ALIGN_BOTTOM_MID, 68, -30);
    lv_obj_set_user_data(hand, slide);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, hand);
    lv_anim_set_exec_cb(&anim, anim_set_hand_pos);
    lv_anim_set_time(&anim, 1500);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_values(&anim, 0, -60);
    lv_anim_start(&anim);

    lv_obj_add_flag(obj, LV_EVENT_CLICKED);
    lv_obj_add_event_cb(obj, page_click_event_cb, LV_EVENT_CLICKED, &anim);
    return;
}

static void anim_set_hand_pos(void *obj, int32_t v)
{
    lv_obj_set_style_translate_y((lv_obj_t *)obj, v, 0);

    lv_obj_t *slide = lv_obj_get_user_data((lv_obj_t *)obj);
    lv_obj_set_style_translate_y(slide, v, 0);
}

static void page_click_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *anim = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED)
    {
        lv_anim_delete(anim, NULL);
        lv_subject_set_int(&toast_subject, PAGE_TOAST_CONFIRM);
    }
}