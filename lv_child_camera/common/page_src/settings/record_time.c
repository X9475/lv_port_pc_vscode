#include "../lv_switch_interface.h"

lv_subject_t record_time_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t style_roller;
static lv_style_t style_select_roller;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void *setting_single_roller_iterm_create(lv_obj_t *cont, const char *opts);
static void setting_single_roller_event_cb(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t record_time_page_info = {
    .page_id = PAGE_FUNCTIONAL_RECORD_TIME,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_record_time_info_get()
{
    return &record_time_page_info;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();

    screen = lv_obj_create(NULL);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    record_time_page_info.page = screen;
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

    //style_roller
    lv_style_init(&style_roller);
    lv_style_copy(&style_roller, &screen_style);
    lv_style_set_border_opa(&style_roller, LV_OPA_COVER);
    lv_style_set_border_width(&style_roller, 1);
    lv_style_set_border_color(&style_roller, lv_color_hex(0x38383A));
    lv_style_set_border_side(&style_roller, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_text_align(&style_roller, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&style_roller, font_get_regular(60));//字体大小60px
    lv_style_set_text_line_space(&style_roller, 30);//行间距

    //style_select_roller
    lv_style_init(&style_select_roller);
    lv_style_copy(&style_select_roller, &screen_style);
    lv_style_set_text_color(&style_select_roller, lv_color_hex(0XAFF99C));
    lv_style_set_text_align(&style_select_roller, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&style_select_roller, font_get_regular(70));//字体大小70px
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&record_time_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&record_time_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&record_time_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    //文字
    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "单次录像时间");
    lv_obj_set_style_text_font(header, fzlthb_30, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align_to(header, cont, LV_ALIGN_TOP_LEFT, 80, 26);

    lv_obj_t *roller = setting_single_roller_iterm_create(cont, "15s\n30s\n1min");
    lv_obj_add_event_cb(roller, setting_single_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&record_time_subject, PAGE_SWITCH_BACK);
}

static void *setting_single_roller_iterm_create(lv_obj_t *cont, const char *opts)
{
    lv_obj_t *roller = lv_roller_create(cont);
    lv_obj_add_style(roller, &screen_style, 0);

    lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(roller, 1, LV_ANIM_OFF);
    lv_roller_set_visible_row_count(roller, 3);

    lv_obj_add_style(roller, &style_roller, LV_PART_MAIN);
    lv_obj_add_style(roller, &style_select_roller, LV_PART_SELECTED);

    lv_obj_set_size(roller, lv_pct(100), 240);
    lv_obj_align(roller, LV_ALIGN_TOP_MID, 0, 105);

    return roller;
}

static void setting_single_roller_event_cb(lv_event_t *e)
{
    static int32_t last_index = -1;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        if (last_index != lv_roller_get_selected(obj))
        {//选择项发生变化
            last_index = lv_roller_get_selected(obj);
            //TODO: 通知业务同步处理
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
    switch_page->old_page = &record_time_page_info;

    switch (page_event)
    {
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