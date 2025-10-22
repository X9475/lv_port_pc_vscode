#include "../lv_switch_interface.h"

#define SETTING_NUM     8

lv_subject_t menu_setting_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_obj_t *label1;
static lv_obj_t *label2;
static lv_obj_t *label3;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_event_handler_code(lv_event_cb_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_AUDIO_EFFECT,       //音频效果
    PAGE_SWITCH_SAVER_STYLE,        //屏保样式
    PAGE_SWITCH_SETTING_MORE,       //更多设置
    PAGE_SWITCH_BACK
};

static lv_page_info_t menu_setting_page_info = {
    .page_id = PAGE_FUNCTIONAL_MENU_SETTING,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_menu_setting_info_get()
{
    return &menu_setting_page_info;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();

    screen = lv_obj_create(top_screen);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    menu_setting_page_info.page = screen;
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
    lv_subject_init_int(&menu_setting_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&menu_setting_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&menu_setting_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    label1 = lv_label_create(cont);
    lv_label_set_text(label1, "音频效果");
    lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label1, fzlthr_26, 0);
    lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_flag(label1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label1, LV_ALIGN_LEFT_MID, 30, 0);
    lv_obj_add_event_cb(label1, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    label2 = lv_label_create(cont);
    lv_label_set_text(label2, "屏保样式");
    lv_obj_set_style_text_opa(label2, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label2, fzlthr_26, 0);
    lv_obj_set_style_text_color(label2, lv_color_white(), 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_flag(label2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(label2, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    label3 = lv_label_create(cont);
    lv_label_set_text(label3, "更多设置");
    lv_obj_set_style_text_opa(label3, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label3, fzlthr_26, 0);
    lv_obj_set_style_text_color(label3, lv_color_white(), 0);
    lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_flag(label3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label3, LV_ALIGN_RIGHT_MID, -30, 0);
    lv_obj_add_event_cb(label3, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    return;
}

static void lv_event_handler_code(lv_event_cb_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);

    if (obj == label1)
    {
        lv_subject_set_int(&menu_setting_subject, PAGE_SWITCH_AUDIO_EFFECT);
    }
    else if (obj == label2)
    {
        lv_subject_set_int(&menu_setting_subject, PAGE_SWITCH_SAVER_STYLE);
    }
    else if (obj == label3)
    {
        lv_subject_set_int(&menu_setting_subject, PAGE_SWITCH_SETTING_MORE);
    }
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
    // switch_page->old_page = &menu_setting_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_AUDIO_EFFECT:
            // lv_stack_push(&menu_setting_page_info);
            lv_obj_add_flag(menu_setting_page_info.page, LV_OBJ_FLAG_HIDDEN);
            switch_page->new_page = lv_page_audio_effect_info_get();
            break;
        case PAGE_SWITCH_SAVER_STYLE:
            // lv_stack_push(&menu_setting_page_info);
            lv_obj_add_flag(menu_setting_page_info.page, LV_OBJ_FLAG_HIDDEN);
            switch_page->new_page = lv_page_screensaver_style_info_get();
            break;
        case PAGE_SWITCH_SETTING_MORE:
            // lv_stack_push(&menu_setting_page_info);
            lv_obj_add_flag(menu_setting_page_info.page, LV_OBJ_FLAG_HIDDEN);
            switch_page->new_page = lv_page_settings_more_info_get();
            break;
        // case PAGE_SWITCH_BACK:
        //     switch_page->new_page = lv_stack_pop();
        //     break;
        default:
            LV_LOG_WARN("[%s:%d] -- page switch event:%d invaild", __FILE__, __LINE__, page_event);
            break;
    }

    if (NULL == switch_page->new_page) {
        lv_free(switch_page);
        return;
    }

}