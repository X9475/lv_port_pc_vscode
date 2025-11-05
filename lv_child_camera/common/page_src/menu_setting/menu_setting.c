#include "../lv_switch_interface.h"

#define SETTING_NUM     8

lv_subject_t menu_setting_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t btn_style;
static lv_style_t label_style;
static lv_style_t style_main;
static lv_style_t style_indicator;
static lv_style_t style_knob;
static lv_obj_t *auxiliary;
static lv_obj_t *password;
static lv_obj_t *single_record;
static lv_obj_t *audio;
static lv_obj_t *screensaver;
static lv_obj_t *more;
static lv_obj_t *bright_slider;
static lv_obj_t *volume_slider;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_event_handler_code(lv_event_cb_t *e);
static void lv_menu_setting_slider_event(lv_event_cb_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SINGLE_RECORD,      //单次录像时长
    PAGE_SWITCH_AUDIO_EFFECT,       //音频效果
    PAGE_SWITCH_SCREENSAVER,        //屏保样式
    PAGE_SWITCH_MORE,               //更多
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

static void lv_page_construct(void *this)
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
    lv_style_reset(&btn_style);
    lv_style_reset(&label_style);
    lv_style_reset(&screen_style);
    lv_style_reset(&style_main);
    lv_style_reset(&style_indicator);
    lv_style_reset(&style_knob);
    lv_page_subject_deinit();
}

static void lv_page_style_init()
{
    //screen_style
    lv_style_init(&screen_style);
    lv_style_set_radius(&screen_style, 0);
    lv_style_set_pad_all(&screen_style, 0);
    lv_style_set_border_width(&screen_style, 0);
    lv_style_set_bg_image_opa(&screen_style, LV_OPA_COVER);
    lv_style_set_bg_image_src(&screen_style, "../lv_port_pc_vscode/assert/icon/background.png");

    //btn_style
    lv_style_init(&btn_style);
    lv_style_set_radius(&btn_style, 0);
    lv_style_set_pad_all(&btn_style, 0);
    lv_style_set_border_width(&btn_style, 0);
    lv_style_set_bg_color(&btn_style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&btn_style, LV_OPA_40);

    //label_style
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, lv_color_white());
    lv_style_set_text_font(&label_style, fzlthr_26);
    lv_style_set_text_opa(&label_style, LV_OPA_COVER);
    lv_style_set_text_align(&label_style, LV_TEXT_ALIGN_CENTER);

    //style_main
    lv_style_init(&style_main);
    lv_style_set_bg_color(&style_main, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&style_main, LV_OPA_40);
    lv_style_set_radius(&style_main, 50);

    //style_indicator
    lv_style_init(&style_indicator);
    lv_style_set_radius(&style_indicator, 0);
    lv_style_set_bg_image_opa(&style_indicator, LV_OPA_COVER);
    lv_style_set_bg_image_src(&style_indicator, "../lv_port_pc_vscode/assert/icon/slider_bright.png");

    //style_knob
    lv_style_init(&style_knob);
    lv_style_set_bg_opa(&style_knob, LV_OPA_TRANSP);
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
    //辅助线
    auxiliary = lv_obj_create(cont);
    lv_obj_set_size(auxiliary, 100, 100);
    lv_obj_add_style(auxiliary, &btn_style, 0);
    lv_obj_set_style_radius(auxiliary, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(auxiliary, LV_ALIGN_TOP_LEFT, 35, 35);
    lv_obj_add_flag(auxiliary, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(auxiliary, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    lv_obj_t *auxiliary_img = lv_img_create(auxiliary);
    lv_obj_align(auxiliary_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_src(auxiliary_img, "../lv_port_pc_vscode/assert/icon/set_icon_auxiliary_lines.png");
    lv_img_set_zoom(auxiliary_img, 128);

    //密码
    password = lv_obj_create(cont);
    lv_obj_set_size(password, 100, 100);
    lv_obj_add_style(password, &btn_style, 0);
    lv_obj_set_style_radius(password, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(password, LV_ALIGN_TOP_LEFT, 145, 35);
    lv_obj_add_flag(password, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(password, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    lv_obj_t *password_img = lv_img_create(password);
    lv_obj_align(password_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_src(password_img, "../lv_port_pc_vscode/assert/icon/lock_on.png");
    lv_img_set_zoom(password_img, 128);

    //单次录像时长
    single_record = lv_obj_create(cont);
    lv_obj_set_size(single_record, 210, 100);
    lv_obj_add_style(single_record, &btn_style, 0);
    lv_obj_set_style_radius(single_record, 50, 0);
    lv_obj_align(single_record, LV_ALIGN_LEFT_MID, 35, 0);
    lv_obj_add_flag(single_record, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(single_record, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    lv_obj_t *single_record_txt = lv_label_create(single_record);
    lv_label_set_text(single_record_txt, "单次录像时长");
    lv_obj_add_style(single_record_txt, &label_style, 0);
    lv_obj_align(single_record_txt, LV_ALIGN_CENTER, 0, 0);

    //音频
    audio = lv_obj_create(cont);
    lv_obj_set_size(audio, 127, 100);
    lv_obj_add_style(audio, &btn_style, 0);
    lv_obj_set_style_radius(audio, 50, 0);
    lv_obj_align(audio, LV_ALIGN_BOTTOM_LEFT, 40, -35);
    lv_obj_add_flag(audio, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(audio, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    lv_obj_t *audio_txt = lv_label_create(audio);
    lv_label_set_text(audio_txt, "音频");
    lv_obj_add_style(audio_txt, &label_style, 0);
    lv_obj_align(audio_txt, LV_ALIGN_CENTER, 0, 0);

    //屏保
    screensaver = lv_obj_create(cont);
    lv_obj_set_size(screensaver, 128, 100);
    lv_obj_add_style(screensaver, &btn_style, 0);
    lv_obj_set_style_radius(screensaver, 50, 0);
    lv_obj_align(screensaver, LV_ALIGN_BOTTOM_MID, 0, -35);
    lv_obj_add_flag(screensaver, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(screensaver, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    lv_obj_t *screensaver_txt = lv_label_create(screensaver);
    lv_label_set_text(screensaver_txt, "屏保");
    lv_obj_add_style(screensaver_txt, &label_style, 0);
    lv_obj_align(screensaver_txt, LV_ALIGN_CENTER, 0, 0);

    //更多
    more = lv_obj_create(cont);
    lv_obj_set_size(more, 127, 100);
    lv_obj_add_style(more, &btn_style, 0);
    lv_obj_set_style_radius(more, 50, 0);
    lv_obj_align(more, LV_ALIGN_BOTTOM_RIGHT, -40, -35);
    lv_obj_add_flag(more, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(more, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    lv_obj_t *more_txt = lv_label_create(more);
    lv_label_set_text(more_txt, "更多");
    lv_obj_add_style(more_txt, &label_style, 0);
    lv_obj_align(more_txt, LV_ALIGN_CENTER, 0, 0);

    //亮度滑动条
    bright_slider = lv_slider_create(cont);
    lv_obj_set_size(bright_slider, 82, 228);
    lv_slider_set_range(bright_slider, 0, 100);
    lv_slider_set_value(bright_slider, 60, LV_ANIM_OFF);
    lv_obj_add_style(bright_slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(bright_slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(bright_slider, &style_knob, LV_PART_KNOB);
    lv_obj_align(bright_slider, LV_ALIGN_TOP_LEFT, 269, 35);
    lv_obj_add_event_cb(bright_slider, lv_menu_setting_slider_event, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *bright_icon = lv_img_create(bright_slider);
    lv_img_set_src(bright_icon, "../lv_port_pc_vscode/assert/icon/icon_bright.png");
    lv_img_set_zoom(bright_icon, 128);
    lv_obj_align(bright_icon, LV_ALIGN_BOTTOM_MID, 0, 10);

    //音量滑动条
    volume_slider = lv_slider_create(cont);
    lv_obj_set_size(volume_slider, 82, 228);
    lv_slider_set_range(volume_slider, 0, 100);
    lv_slider_set_value(volume_slider, 20, LV_ANIM_OFF);
    lv_obj_add_style(volume_slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(volume_slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(volume_slider, &style_knob, LV_PART_KNOB);
    lv_obj_align(volume_slider, LV_ALIGN_TOP_RIGHT, -38, 35);
    lv_obj_add_event_cb(volume_slider, lv_menu_setting_slider_event, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *volume_icon = lv_img_create(volume_slider);
    lv_img_set_src(volume_icon, "../lv_port_pc_vscode/assert/icon/icon_volume.png");
    lv_img_set_zoom(volume_icon, 128);
    lv_obj_align(volume_icon, LV_ALIGN_BOTTOM_MID, 0, 10);

    return;
}

static void lv_menu_setting_slider_event(lv_event_cb_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);

    if (LV_EVENT_VALUE_CHANGED == code)
    {
        int32_t value = lv_slider_get_value(slider);
        //发送到UI事件处理消息队列给dsp接口设置
        if (slider == volume_slider)
        {
            printf("===> volume_slider value: %d\n", value);
        }
        else if (slider == bright_slider)
        {
            printf("===> bright_slider value: %d\n", value);
        }
    }
}

static void lv_event_handler_code(lv_event_cb_t *e)
{
    static bool auxiliary_flag = false;
    static bool password_flag = false;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (LV_EVENT_CLICKED == code)
    {
        if (obj == auxiliary)
        {//辅助线
            if (!auxiliary_flag) {
                lv_obj_set_style_bg_color(auxiliary, lv_color_hex(0xCE94F8), 0);
                lv_obj_set_style_bg_opa(auxiliary, LV_OPA_COVER, 0);
                auxiliary_flag = true;
                //设置辅助线打开的标志，需要发送主题事件到拍摄界面回调
            }
            else
            {
                lv_obj_set_style_bg_color(auxiliary, lv_color_hex(0x000000), 0);
                lv_obj_set_style_bg_opa(auxiliary, LV_OPA_COVER, 0);
                auxiliary_flag = false;
            }
        }
        else if (obj == password)
        {//密码
            if (!password_flag) {
                lv_obj_set_style_bg_color(password, lv_color_hex(0xCE94F8), 0);
                lv_obj_set_style_bg_opa(password, LV_OPA_COVER, 0);
                password_flag = true;
                //设置AI语音助手打开的标志，需要发送主题事件到AI界面回调
            }
            else
            {
                lv_obj_set_style_bg_color(password, lv_color_hex(0x000000), 0);
                lv_obj_set_style_bg_opa(password, LV_OPA_COVER, 0);
                password_flag = false;
            }
        }
        else if (obj == single_record)
        {//单次录像时长
            lv_subject_set_int(&menu_setting_subject, PAGE_SWITCH_SINGLE_RECORD);
        }
        else if (obj == audio)
        {//音频
            lv_subject_set_int(&menu_setting_subject, PAGE_SWITCH_AUDIO_EFFECT);
        }
        else if (obj == screensaver)
        {//屏保
            lv_subject_set_int(&menu_setting_subject, PAGE_SWITCH_SCREENSAVER);
        }
        else if (obj == more)
        {//更多
            lv_subject_set_int(&menu_setting_subject, PAGE_SWITCH_MORE);
        }
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
    switch_page->old_page = &menu_setting_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_SINGLE_RECORD:
            lv_stack_push(&menu_setting_page_info);
            // lv_obj_add_flag(menu_setting_page_info.page, LV_OBJ_FLAG_HIDDEN);
            switch_page->new_page = lv_page_record_time_info_get();
            break;
        case PAGE_SWITCH_AUDIO_EFFECT:
            lv_stack_push(&menu_setting_page_info);
            // lv_obj_add_flag(menu_setting_page_info.page, LV_OBJ_FLAG_HIDDEN);
            switch_page->new_page = lv_page_audio_effect_info_get();
            break;
        case PAGE_SWITCH_SCREENSAVER:
            lv_stack_push(&menu_setting_page_info);
            // lv_obj_add_flag(menu_setting_page_info.page, LV_OBJ_FLAG_HIDDEN);
            switch_page->new_page = lv_page_screensaver_style_info_get();
            break;
        case PAGE_SWITCH_MORE:
            lv_stack_push(&menu_setting_page_info);
            // lv_obj_add_flag(menu_setting_page_info.page, LV_OBJ_FLAG_HIDDEN);
            switch_page->new_page = lv_page_settings_more_info_get();
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