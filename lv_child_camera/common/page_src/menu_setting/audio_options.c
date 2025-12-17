#include "../lv_switch_interface.h"

lv_subject_t audio_option_subject;
static lv_switch_page_pt switch_page;
static lv_page_info_pt cur_page = NULL;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont, lv_page_info_pt page_info);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void lv_page_audio_photo_load(lv_obj_t *cont);
static void lv_page_audio_ring_bell_load(lv_obj_t *cont);
static void lv_page_audio_answer_bell_load(lv_obj_t *cont);
static void lv_page_audio_alarm_clock_load(lv_obj_t *cont);
static void lv_page_audio_notify_sound_load(lv_obj_t *cont);
static void lv_page_audio_keypad_tone_load(lv_obj_t *cont);
static void lv_page_audio_beyond_area_load(lv_obj_t *cont);
static void lv_page_audio_list_create(lv_obj_t *cont, const char *name, const char *label_list[]);
static void *lv_autio_option_iterm_create(lv_obj_t *cont, const char *name);
static void autio_option_iterm_click_event_cb(lv_event_cb_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

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

    cur_page = (lv_page_info_t *)this;
    lv_page_load(screen, cur_page);
    lv_page_type_set(TYPE_MENU_SETTING_THREE);

    cur_page->page = screen;
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
    lv_subject_init_int(&audio_option_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&audio_option_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&audio_option_subject);
}

static void lv_page_load(lv_obj_t *cont, lv_page_info_pt page_info)
{
    //根据选择的页面类型加载页面
    switch (page_info->page_id)
    {
        case PAGE_FUNCTIONAL_AUDIO_PHOTO:
            lv_page_audio_photo_load(cont);
            break;
        case PAGE_FUNCTIONAL_AUDIO_RING_BELL:
            lv_page_audio_ring_bell_load(cont);
            break;
        case PAGE_FUNCTIONAL_AUDIO_ANSWER_BELL:
            lv_page_audio_answer_bell_load(cont);
            break;
        case PAGE_FUNCTIONAL_AUDIO_ALARM_CLOCK:
            lv_page_audio_alarm_clock_load(cont);
            break;
        case PAGE_FUNCTIONAL_AUDIO_NOTIFY_SOUND:
            lv_page_audio_notify_sound_load(cont);
            break;
        case PAGE_FUNCTIONAL_AUDIO_KEYPAD_TONE:
            lv_page_audio_keypad_tone_load(cont);
            break;
        case PAGE_FUNCTIONAL_AUDIO_BEYOND_AREA:
            lv_page_audio_beyond_area_load(cont);
            break;
        default:
            LV_LOG_WARN("page id error");
            break;
    }

    return;
}

/// @brief 拍照音
static lv_page_info_t audio_photo_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_PHOTO,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};
static const char *audio_photo[] = {"仅震动", "铃声1", "铃声2", NULL};
lv_page_info_pt lv_page_audio_photo_info_get() { return &audio_photo_page_info; }
static void lv_page_audio_photo_load(lv_obj_t *cont)
{
    lv_page_audio_list_create(cont, "拍照音", audio_photo);
}

/// @brief 呼叫铃声
static lv_page_info_t audio_ring_bell_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_RING_BELL,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};
static const char *ring_bell[] = {"仅震动", "铃声1", "铃声2", NULL};
lv_page_info_pt lv_page_audio_ring_bell_info_get() { return &audio_ring_bell_page_info; }
static void lv_page_audio_ring_bell_load(lv_obj_t *cont)
{
    lv_page_audio_list_create(cont, "呼叫铃声", ring_bell);
}

/// @brief 接听铃声
static lv_page_info_t audio_answer_bell_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_ANSWER_BELL,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};
static const char *answer_bell[] = {"仅震动", "铃声1", "铃声2", NULL};
lv_page_info_pt lv_page_audio_answer_bell_info_get() { return &audio_answer_bell_page_info; }
static void lv_page_audio_answer_bell_load(lv_obj_t *cont)
{
    lv_page_audio_list_create(cont, "接听铃声", answer_bell);
}

/// @brief 闹钟铃声
static lv_page_info_t audio_alarm_clock_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_ALARM_CLOCK,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};
static const char *alarm_clock[] = {"仅震动", "铃声1", "铃声2", NULL};
lv_page_info_pt lv_page_audio_alarm_clock_info_get() { return &audio_alarm_clock_page_info; }
static void lv_page_audio_alarm_clock_load(lv_obj_t *cont)
{
    lv_page_audio_list_create(cont, "闹钟铃声", alarm_clock);
}

/// @brief 消息提示音
static lv_page_info_t audio_notify_sound_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_NOTIFY_SOUND,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};
static const char *notify_sound[] = {"仅震动", NULL};
lv_page_info_pt lv_page_audio_notify_sound_info_get() { return &audio_notify_sound_page_info; }
static void lv_page_audio_notify_sound_load(lv_obj_t *cont)
{
    lv_page_audio_list_create(cont, "消息提示音", notify_sound);
}

/// @brief 按键音
static lv_page_info_t audio_keypad_tone_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_KEYPAD_TONE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};
static const char *keypad_tone[] = {"仅震动", NULL};
lv_page_info_pt lv_page_audio_keypad_tone_info_get() { return &audio_keypad_tone_page_info; }
static void lv_page_audio_keypad_tone_load(lv_obj_t *cont)
{
    lv_page_audio_list_create(cont, "按键音", keypad_tone);
}

/// @brief 超出区域
static lv_page_info_t audio_beyond_area_page_info = {
    .page_id = PAGE_FUNCTIONAL_AUDIO_BEYOND_AREA,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};
static const char *beyond_area[] = {"仅震动", NULL};
lv_page_info_pt lv_page_audio_beyond_area_info_get() { return &audio_beyond_area_page_info; }
static void lv_page_audio_beyond_area_load(lv_obj_t *cont)
{
    lv_page_audio_list_create(cont, "超出区域", beyond_area);
}

static void lv_page_audio_list_create(lv_obj_t *cont, const char *name, const char *label_list[])
{
    lv_obj_t *back_btn = lv_img_create(cont);
    lv_img_set_src(back_btn, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, fzlthb_30, 0);
    lv_obj_align_to(label, back_btn, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    //滚动列表
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 332);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_align_to(cont_col, cont, LV_ALIGN_TOP_MID, 40, 78);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);

    for (uint8_t i = 0; label_list[i] != NULL; i++)
    {
        lv_obj_t *btn = lv_autio_option_iterm_create(cont_col, label_list[i]);
        lv_obj_add_event_cb(btn, autio_option_iterm_click_event_cb, LV_EVENT_CLICKED, cont_col);
    }

    return;
}

static void *lv_autio_option_iterm_create(lv_obj_t *cont, const char *name)
{
    lv_obj_t *checkbox = lv_obj_create(cont);
    lv_obj_remove_style_all(checkbox);
    lv_obj_set_size(checkbox, 422, 110);
    lv_obj_add_flag(checkbox, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(checkbox, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(checkbox, 1, 0);
    lv_obj_set_style_border_color(checkbox, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(checkbox, 20, 0);
    lv_obj_set_style_bg_opa(checkbox, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(checkbox, lv_color_hex(0x0A0B0D), 0);
    lv_obj_set_style_bg_grad_color(checkbox, lv_color_hex(0x202124), 0);
    lv_obj_set_style_bg_grad_dir(checkbox, LV_GRAD_DIR_HOR, 0);
    lv_obj_align(checkbox, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(checkbox);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_28, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, checkbox, LV_ALIGN_LEFT_MID, 45, 0);

    lv_obj_t *img = lv_img_create(checkbox);
    if (checkbox == lv_obj_get_child(cont, 0))
    {
        lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
    }
    else
    {
        lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_unselect.png");
    }
    lv_obj_align_to(img, checkbox, LV_ALIGN_RIGHT_MID, -34, 0);

    return checkbox;
}

static void autio_option_iterm_click_event_cb(lv_event_cb_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *cont_col = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        for (uint8_t i = 0; i < lv_obj_get_child_cnt(cont_col); i++)
        {
            lv_obj_t *iterm = lv_obj_get_child(cont_col, i);
            lv_obj_t *img = lv_obj_get_child(iterm, 1);
            lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_unselect.png");
        }

        lv_obj_t *checkbox = lv_event_get_target(e);
        lv_obj_t *img = lv_obj_get_child(checkbox, 1);
        lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
        lv_obj_scroll_to_view(checkbox, LV_ANIM_ON);
    }
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&audio_option_subject, PAGE_SWITCH_BACK);
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
    switch_page->old_page = cur_page;

    switch (page_event)
    {
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