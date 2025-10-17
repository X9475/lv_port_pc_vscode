#include "lv_signup_process.h"

lv_subject_t agent_start_subject;
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

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();
    //加入栈表
    // lv_stack_push(&agent_start_page_info);

    screen = lv_obj_create(NULL);
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
    lv_img_set_src(agent_bg, "V:tk1/icon/start_agent_2x.png");
    lv_img_set_zoom(agent_bg, 128);
    lv_obj_align(agent_bg, LV_ALIGN_CENTER, 0, 0);

    //创建滑动条
    slider_agent = lv_slider_create(cont);
    lv_obj_set_size(slider_agent, 442, 100);
    lv_slider_set_range(slider_agent, 0, 442);
    lv_obj_set_style_bg_opa(slider_agent, LV_OPA_80, 0);
    lv_obj_set_style_bg_color(slider_agent, lv_color_hex(0x9FE5FF), 0);
    lv_obj_set_style_bg_grad_color(slider_agent, lv_color_hex(0x494949), 0);
    lv_obj_set_style_bg_grad_dir(slider_agent, LV_GRAD_DIR_HOR, 0);
    lv_obj_align(slider_agent, LV_ALIGN_TOP_MID, 0, 290);
    lv_obj_remove_style(slider_agent, NULL, LV_PART_KNOB);
    lv_obj_remove_style(slider_agent, NULL, LV_PART_INDICATOR);

    lv_obj_t *label = lv_label_create(cont);
    lv_obj_set_size(label, 86, 86);
    lv_obj_set_pos(label, 41, 297);
    lv_obj_set_style_text_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(label, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(label, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_bg_grad_color(label, lv_color_hex(0x79FFF5), 0);
    lv_obj_set_style_bg_grad_dir(label, LV_GRAD_DIR_HOR, 0);

    lv_obj_t *door = lv_img_create(label);
    lv_img_set_src(door, "V:tk1/icon/door_icon_two.png");
    lv_obj_align(door, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(slider_agent, lv_start_agent_slider_event, LV_EVENT_VALUE_CHANGED, label);

    //文字
    label_opa = LV_OPA_90;
    label_agent = lv_label_create(slider_agent);
    lv_label_set_text(label_agent, "开启探索世界");
    lv_obj_set_style_text_opa(label_agent, label_opa, 0);
    lv_obj_set_style_text_font(label_agent, fzlthr_30, 0);
    lv_obj_set_style_text_color(label_agent, lv_color_white(), 0);
    lv_obj_set_style_text_align(label_agent, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label_agent, slider_agent, LV_ALIGN_CENTER, 0, 0);

    return;
}

static void lv_start_agent_slider_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (LV_EVENT_VALUE_CHANGED == code)
    {
        int32_t value = lv_slider_get_value(slider_agent);
        // printf("==== value: %d\n", value);

        if (value < 41) lv_slider_set_value(slider_agent, 41, LV_ANIM_ON);//最小值限制
        if (value > 378) lv_slider_set_value(slider_agent, 378, LV_ANIM_ON);//最大值限制

        if (value < 378 && value > 41) {
            uint16_t new_opa = (label_opa -= 4) <= 0? 0 : label_opa;
            lv_obj_set_x(label, value);//设置标签坐标
            lv_obj_set_style_text_opa(label_agent, new_opa, 0);
        }

        if (value == 378) {
            lv_subject_set_int(&agent_start_subject, PAGE_SWITCH_NEXT);
        }
    }
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    extern lv_page_info_pt lv_page_menu_info_get();

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
            switch_page->new_page = lv_page_menu_info_get();
            break;
        case PAGE_SWITCH_BACK:
            switch_page->new_page = lv_stack_pop();
            break;
        default:
            LV_LOG_WARN("[%s:%d] -- page switch event:%d invaild", __FILE__, __LINE__, page_event);
            break;
    }

    if (NULL == switch_page->new_page) return;
    lv_subject_set_pointer(&switch_subject, switch_page);
}