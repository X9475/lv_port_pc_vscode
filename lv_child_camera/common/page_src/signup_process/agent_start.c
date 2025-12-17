#include "../lv_switch_interface.h"

lv_subject_t agent_start_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t style_main;
static lv_style_t style_indicator;
static lv_style_t style_knob;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_start_agent_slider_event(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t agent_start_page_info = {
    .page_id = PAGE_FUNCTIONAL_AGENT_START,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_agent_start_info_get()
{
    return &agent_start_page_info;
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

    agent_start_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
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
    lv_style_set_bg_color(&screen_style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&screen_style, LV_OPA_COVER);

    //style_main
    lv_style_init(&style_main);
    // lv_style_set_bg_image_src(&style_main, "../lv_port_pc_vscode/assert/icon/welcome_slider_bar.png");
    lv_style_set_bg_opa(&style_main, LV_OPA_TRANSP);
    lv_style_set_radius(&style_main, 50);

    //style_indicator
    lv_style_init(&style_indicator);
    lv_style_set_radius(&style_indicator, 0);
    lv_style_set_bg_opa(&style_indicator, LV_OPA_TRANSP);

    //style_knob
    lv_style_init(&style_knob);
    lv_style_set_bg_image_src(&style_knob, "../lv_port_pc_vscode/assert/icon/slider_button.png");
    lv_style_set_bg_opa(&style_knob, LV_OPA_TRANSP);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&agent_start_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&agent_start_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&agent_start_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *agent_bg = lv_img_create(cont);
    lv_img_set_src(agent_bg, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_img_set_zoom(agent_bg, 128);
    lv_obj_align(agent_bg, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *slider_bg = lv_img_create(cont);
    lv_img_set_src(slider_bg, "../lv_port_pc_vscode/assert/icon/welcome_slider_bar.png");
    lv_img_set_zoom(slider_bg, 128);
    lv_obj_align(slider_bg, LV_ALIGN_TOP_MID, 0, 240);

    //创建滑动条
    lv_obj_t *slider = lv_slider_create(cont);
    lv_obj_set_size(slider, 442, 100);
    lv_slider_set_range(slider, 0, 442);
    lv_slider_set_value(slider, 54, LV_ANIM_OFF);
    lv_obj_add_style(slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 290);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST);//仅旋钮模式
    lv_obj_add_event_cb(slider, lv_start_agent_slider_event, LV_EVENT_ALL, NULL);

    return;
}

static void lv_start_agent_slider_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);

    if (LV_EVENT_RELEASED == code)
    {
        lv_slider_set_value(slider, 54, LV_ANIM_ON);
        return;
    }

    if (LV_EVENT_VALUE_CHANGED == code)
    {
        int32_t value = lv_slider_get_value(slider);
        // printf("value = %d\n", value);

        //9~347,54~388
        if (value >= 388)
        {
            lv_slider_set_value(slider, 388, LV_ANIM_OFF);
            lv_dev_stage_set(LV_PAGE_STAGE_RUNNING);
            lv_subject_set_int(&agent_start_subject, PAGE_SWITCH_NEXT);
        }

        if (value <= 54)
        {
            lv_slider_set_value(slider, 54, LV_ANIM_OFF);
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
    switch_page->old_page = &agent_start_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_shooting_photo_get();
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