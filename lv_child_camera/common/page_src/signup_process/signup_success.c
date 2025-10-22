#include "../lv_switch_interface.h"

lv_subject_t signup_success_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_timer_t *timer;
static uint32_t tick_sec = 5;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void signup_success_countdown_timer_cb(lv_timer_t *timer);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t signup_success_page_info = {
    .page_id = PAGE_FUNCTIONAL_SIGNUP_SUCCESS,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt *lv_page_signup_success_info_get()
{
    return &signup_success_page_info;
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

    signup_success_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    if (NULL != timer) lv_timer_del(timer);
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
    lv_subject_init_int(&signup_success_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&signup_success_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&signup_success_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //错误图案
    lv_obj_t *login_success = lv_img_create(cont);
    lv_img_set_src(login_success, "../lv_port_pc_vscode/assert/icon/success_2x.png");
    lv_img_set_zoom(login_success, 128);
    lv_obj_align(login_success, LV_ALIGN_TOP_MID, 0, -100);

    //文字提示
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "添加完成");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_90, 0);
    lv_obj_set_style_text_font(tip1_label, fzlthr_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 252);

    lv_obj_t *tip2_label = lv_label_create(cont);
    lv_obj_set_style_opa(tip2_label, LV_OPA_80, 0);
    lv_obj_set_style_text_line_space(tip2_label, 6, 0);
    lv_label_set_text(tip2_label, "即将进入探索之旅");
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_60, 0);
    lv_obj_set_style_text_font(tip2_label, fzlthr_24, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 297);

    //倒计时
    timer = lv_timer_create(signup_success_countdown_timer_cb, 1000, tip2_label);
    lv_timer_set_repeat_count(timer, 6);
    lv_timer_set_auto_delete(timer, false);

    return;
}

static void signup_success_countdown_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *label = lv_timer_get_user_data(timer);
    lv_label_set_text_fmt(label, "即将进入探索之旅（%ds）", tick_sec);

    if (tick_sec == 0) {
        tick_sec = 5;
        lv_subject_set_int(&signup_success_subject, PAGE_SWITCH_NEXT);
    }
    tick_sec--;
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
    switch_page->old_page = &signup_success_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_inital_startup_info_get();
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

    lv_subject_set_pointer(&switch_subject, switch_page);
}

