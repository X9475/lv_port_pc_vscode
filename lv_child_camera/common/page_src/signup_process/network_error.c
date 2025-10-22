#include "../lv_switch_interface.h"

lv_subject_t network_error_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_obj_t *slider_agent;
static lv_obj_t *label_agent;
static int32_t label_opa = 0;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void retry_btn_click_event_cb(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_RETRY,
    PAGE_SWITCH_BACK
};

static lv_page_info_t network_error_page_info = {
    .page_id = PAGE_FUNCTIONAL_NETWORK_4G,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_network_error_info_get()
{
    return &network_error_page_info;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();

    screen = lv_obj_create(act_screen);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    network_error_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_page_subject_deinit();
}

static void lv_page_style_init()
{
    //screen_style
    lv_style_init(&screen_style);
    lv_style_set_radius(&screen_style, 0);
    lv_style_set_pad_all(&screen_style, 0);
    lv_style_set_border_width(&screen_style, 0);
    lv_style_set_bg_color(&screen_style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&screen_style, LV_OPA_COVER);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&network_error_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&network_error_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&network_error_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //错误图案
    lv_obj_t *net_error = lv_img_create(cont);
    lv_img_set_src(net_error, "../lv_port_pc_vscode/assert/icon/fail_2x.png");
    lv_img_set_zoom(net_error, 128);
    lv_obj_align(net_error, LV_ALIGN_TOP_MID, 0, -100);

    //文字提示
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "设备网络异常");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, fzlthr_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 183);

    lv_obj_t *tip2_label = lv_label_create(cont);
    lv_obj_set_size(tip2_label, 375, 72);
    lv_obj_set_style_opa(tip2_label, LV_OPA_80, 0);
    lv_obj_set_style_text_line_space(tip2_label, 6, 0);
    lv_label_set_text(tip2_label, "1、请确保设备所摆放位置信号良好2、请确保流量卡中有可用流量");
    lv_label_set_long_mode(tip2_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_60, 0);
    lv_obj_set_style_text_font(tip2_label, fzlthr_24, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 228);

    //重试
    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 148, 70);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 320);
    lv_obj_set_style_radius(btn, 51, 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_add_event_cb(btn, retry_btn_click_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *tip3_label = lv_label_create(btn);
    lv_label_set_text(tip3_label, "重试");
    lv_obj_set_style_text_opa(tip3_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip3_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(tip3_label, lv_color_hex(0x1C1C1E), 0);
    lv_obj_set_style_text_align(tip3_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip3_label, LV_ALIGN_CENTER, 0, 0);

    return;
}

static void retry_btn_click_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&network_error_subject, PAGE_SWITCH_RETRY);
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_INFO("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发

    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &network_error_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_RETRY:
            // switch_page->new_page = lv_page_agent_start_info_get();
            break;
        case PAGE_SWITCH_BACK:
            switch_page->new_page = lv_stack_pop();
            break;
        default:
            LV_LOG_WARN("[%s:%d] -- page switch event:%d invaild", __FILE__, __LINE__, page_event);
            break;
    }

    if (NULL == switch_page->new_page) {
        lv_free(switch_page);
        return;
    }

}