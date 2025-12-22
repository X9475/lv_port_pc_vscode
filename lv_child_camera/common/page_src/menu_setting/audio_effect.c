#include "../lv_switch_interface.h"

#define AUDIO_NUM       7

lv_subject_t audio_effect_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void audio_iterm_click_event_cb(lv_event_cb_t *e);
static void *lv_audio_effect_iterm_create(lv_obj_t *cont, const char *name);

static const char *audio_list[AUDIO_NUM] = {
    "拍照音", "呼叫铃声", "接听铃声", "闹钟铃声", "消息提示音", "按键音", "超出区域"
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_PHOTO_SOUND,    //拍照音
    PAGE_SWITCH_RING_BELL,      //呼叫铃声
    PAGE_SWITCH_ANSWER_BELL,    //接听铃声
    PAGE_SWITCH_ALARM_CLOCK,    //闹钟铃声
    PAGE_SWITCH_NOTIFY_SOUND,   //消息提示音
    PAGE_SWITCH_KEYPAD_TONE,    //按键音
    PAGE_SWITCH_BEYOND_AREA,    //超出区域
    PAGE_SWITCH_BACK
};

static lv_page_info_t audio_effect_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_EFFECT,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_audio_effect_info_get()
{
    return &audio_effect_page_info;
}

static void lv_page_construct(void *this)
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
    lv_page_type_set(TYPE_MENU_SETTING_TWO);

    audio_effect_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&screen_style);
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
    lv_subject_init_int(&audio_effect_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&audio_effect_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&audio_effect_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 70, 70);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(back_btn, cont, LV_ALIGN_TOP_LEFT, 20, 10);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 3, 0);

    //滚动列表
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 340);
    lv_obj_add_style(cont_col, &screen_style, 0);
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

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_BACK);
}

static void audio_iterm_click_event_cb(lv_event_cb_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    const char *name = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        if (lv_strcmp(name, "拍照音") == 0)
        {
            lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_PHOTO_SOUND);
        }
        else if (lv_strcmp(name, "呼叫铃声") == 0)
        {
            lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_RING_BELL);
        }
        else if (lv_strcmp(name, "接听铃声") == 0)
        {
            lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_ANSWER_BELL);
        }
        else if (lv_strcmp(name, "闹钟铃声") == 0)
        {
            lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_ALARM_CLOCK);
        }
        else if (lv_strcmp(name, "消息提示音") == 0)
        {
            lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_NOTIFY_SOUND);
        }
        else if (lv_strcmp(name, "按键音") == 0)
        {
            lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_KEYPAD_TONE);
        }
        else if (lv_strcmp(name, "超出区域") == 0)
        {
            lv_subject_set_int(&audio_effect_subject, PAGE_SWITCH_BEYOND_AREA);
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
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label,fzlthb_28, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 30, 0);

    //进入指示
    lv_obj_t *image = lv_img_create(btn);
    lv_obj_set_size(image, 7, 13);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_img_set_angle(image, 1800);
    lv_img_set_zoom(image, 160);
    lv_obj_align_to(image, btn, LV_ALIGN_RIGHT_MID, -31, 0);

    return btn;
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
    switch_page->old_page = &audio_effect_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_PHOTO_SOUND:
            lv_stack_push(&audio_effect_page_info);
            switch_page->new_page = lv_page_audio_photo_info_get();
            break;
        case PAGE_SWITCH_RING_BELL:
            lv_stack_push(&audio_effect_page_info);
            switch_page->new_page = lv_page_audio_ring_bell_info_get();
            break;
        case PAGE_SWITCH_ANSWER_BELL:
            lv_stack_push(&audio_effect_page_info);
            switch_page->new_page = lv_page_audio_answer_bell_info_get();
            break;
        case PAGE_SWITCH_ALARM_CLOCK:
            lv_stack_push(&audio_effect_page_info);
            switch_page->new_page = lv_page_audio_alarm_clock_info_get();
            break;
        case PAGE_SWITCH_NOTIFY_SOUND:
            lv_stack_push(&audio_effect_page_info);
            switch_page->new_page = lv_page_audio_notify_sound_info_get();
            break;
        case PAGE_SWITCH_KEYPAD_TONE:
            lv_stack_push(&audio_effect_page_info);
            switch_page->new_page = lv_page_audio_keypad_tone_info_get();
            break;
        case PAGE_SWITCH_BEYOND_AREA:
            lv_stack_push(&audio_effect_page_info);
            switch_page->new_page = lv_page_audio_beyond_area_info_get();
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