#include "../lv_switch_interface.h"

#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"

static const char * photograph_left_options = "倒计时\n画面比例\n分辨率";
static const char * ratio_options = "1:1\n4:3\n16:9";
static const char * timer_options = "5s\nOFF\n3s";
static const char * resolution_options ="4K\n1080P";

lv_subject_t shooting_adj_param_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t up_area_roller_style;
static lv_style_t separator_line_style;
static lv_style_t left_main_style;
static lv_style_t left_select_style;
static lv_style_t right_main_style;
static lv_style_t right_select_style;

static lv_obj_t *screen = NULL;
static lv_obj_t * right_roller;
static lv_obj_t * left_roller;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void back_click_cb(lv_event_t * e);
static void left_roller_event_cb(lv_event_t * e);
static void right_roller_event_cb(lv_event_t * e);
static void create_separator_line(lv_obj_t * cont);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t shooting_adj_param_page = {
    .page_id = PAGE_FUNCTIONAL_SHOOTING_ADJ_PARAM,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_shooting_adj_param_get()
{
    return &shooting_adj_param_page;
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
    shooting_adj_param_page.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&screen_style);
    lv_style_reset(&up_area_roller_style);
    lv_style_reset(&separator_line_style);
    lv_style_reset(&left_main_style);
    lv_style_reset(&left_select_style);
    lv_style_reset(&right_main_style);
    lv_style_reset(&right_select_style);
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

    //up_area_roller_style
    lv_style_init(&up_area_roller_style);
    lv_style_set_border_opa(&up_area_roller_style, LV_OPA_TRANSP);
    lv_style_set_bg_color(&up_area_roller_style, lv_color_hex(0x000000));
    lv_style_set_bg_grad_color(&up_area_roller_style, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&up_area_roller_style, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&up_area_roller_style, LV_OPA_60);
    lv_style_set_bg_grad_opa(&up_area_roller_style, LV_OPA_0);

    ////separator_line_style
    static lv_grad_dsc_t separator_line_grad;
    separator_line_grad.dir = LV_GRAD_DIR_VER;
    separator_line_grad.stops_count = 5;
    separator_line_grad.stops[0].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[0].opa = LV_OPA_TRANSP;
    separator_line_grad.stops[0].frac = 0;   // 起始位置（0%）

    separator_line_grad.stops[1].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[1].opa = LV_OPA_80;
    separator_line_grad.stops[1].frac = 64;  // 约25%位置（64/255）

    separator_line_grad.stops[2].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[2].opa = LV_OPA_100;
    separator_line_grad.stops[2].frac = 128; // 中间位置（50%）

    separator_line_grad.stops[3].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[3].opa = LV_OPA_80;
    separator_line_grad.stops[3].frac = 192; // 约75%位置（192/255

    separator_line_grad.stops[4].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[4].opa = LV_OPA_0;
    separator_line_grad.stops[4].frac = 255; // 结束位置（100%)
    lv_style_init(&separator_line_style);
    lv_style_set_bg_grad(&separator_line_style, &separator_line_grad);
    lv_style_set_bg_opa(&separator_line_style, LV_OPA_COVER);

    //left_main_style
    lv_style_init(&left_main_style);
    lv_style_copy(&left_main_style, &screen_style);
    lv_style_set_text_opa(&left_main_style, LV_OPA_COVER);
    lv_style_set_text_color(&left_main_style, lv_color_hex(0x38383A));
    lv_style_set_text_font(&left_main_style, font_get_regular(48));
    lv_style_set_text_line_space(&left_main_style, 50);
    lv_style_set_text_align(&left_main_style, LV_TEXT_ALIGN_LEFT);
    
    //left_select_style
    lv_style_init(&left_select_style);
    lv_style_copy(&left_select_style, &screen_style);
    lv_style_set_text_color(&left_select_style, lv_color_hex(0XAFF99C));
    lv_style_set_text_align(&left_select_style, LV_TEXT_ALIGN_LEFT);

    //right_main_style
    lv_style_init(&right_main_style);
    lv_style_copy(&right_main_style, &left_main_style);
    lv_style_set_text_line_space(&right_main_style, 35);
    lv_style_set_text_font(&right_main_style, oswaldr_48);

    //right_select_style
    lv_style_init(&right_select_style);
    lv_style_copy(&right_select_style, &left_select_style);
    lv_style_set_text_font(&right_select_style, oswaldr_48);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&shooting_adj_param_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&shooting_adj_param_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&shooting_adj_param_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
   //创建返回按钮
    lv_obj_t * back_icon = lv_img_create(cont);
    lv_img_set_src(back_icon, ICON_BACK);
    lv_obj_set_size(back_icon, 50, 50);
    lv_obj_align(back_icon, LV_ALIGN_TOP_LEFT, 30, 20);

    lv_obj_add_flag(back_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back_icon, back_click_cb, LV_EVENT_CLICKED, NULL);

    // 创建顶部矩形渐变框
    lv_obj_t *up_roller_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(up_roller_indicator_area, 502, 50);
    lv_obj_align(up_roller_indicator_area, LV_ALIGN_TOP_LEFT, 0, 100);
    lv_obj_add_style(up_roller_indicator_area, &up_area_roller_style, 0);

    /* 创建左边滚轮 */
    left_roller = lv_roller_create(cont);
    lv_roller_set_options(left_roller, photograph_left_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(left_roller, 1, LV_ANIM_OFF); // 默认选择画面比例
    lv_roller_set_visible_row_count(left_roller, 3);
    lv_obj_set_width(left_roller, 192);
    lv_obj_set_height(left_roller, 272);
    lv_obj_align(left_roller, LV_ALIGN_TOP_LEFT, 60, 88);
    // lv_obj_set_style_text_font(left_roller, font_get_regular(34), 0);
    lv_obj_add_event_cb(left_roller, left_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 创建渐变分隔线 */
    create_separator_line(cont);

    /* 创建右边滚轮 */
    right_roller = lv_roller_create(cont);
    lv_roller_set_options(right_roller, ratio_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择画面比例
    lv_roller_set_visible_row_count(right_roller, 3);
    lv_obj_set_width(right_roller, 150);
    lv_obj_set_height(right_roller, 272);
    lv_obj_align(right_roller, LV_ALIGN_TOP_LEFT, 352, 88);
    // lv_obj_set_style_text_font(right_roller,font_get_regular(34), 0);
    lv_obj_add_event_cb(right_roller, right_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    lv_obj_add_style(left_roller, &left_main_style, LV_PART_MAIN);
    lv_obj_add_style(right_roller, &right_main_style, LV_PART_MAIN);

    lv_obj_add_style(left_roller, &left_select_style, LV_PART_SELECTED);
    lv_obj_add_style(right_roller, &right_select_style, LV_PART_SELECTED);

    return;
}

/* 创建渐变分隔线 */
static void create_separator_line(lv_obj_t * parent)
{
    lv_obj_t *separator_line = lv_obj_create(parent);
    lv_obj_remove_style_all(separator_line); // 移除所有默认样式
    
    // 设置分隔线尺寸和位置
    lv_obj_set_size(separator_line, 1, 272); 
    lv_obj_align(separator_line, LV_ALIGN_TOP_LEFT, 301, 88);
    //创建垂直渐变效果
    lv_obj_add_style(separator_line, &separator_line_style, 0);
}

static void right_roller_event_cb(lv_event_t * e)
{
    static int32_t last_index = -1;
    lv_obj_t *right_roller = lv_event_get_target(e);
    uint16_t selected = lv_roller_get_selected(right_roller);

    if (last_index != lv_roller_get_selected(right_roller))
    {//选项发生变化
        last_index = lv_roller_get_selected(right_roller);

        // char selected_text[32] = {0};
        // lv_roller_get_selected_str(right_roller, selected_text, sizeof(selected_text));
        // LV_LOG_USER("Roller changed: Index=%d, Text=%s", selected, selected_text);
        uint16_t left_selected = lv_roller_get_selected(left_roller);

        /* 根据左边滚轮的选择更新右边滚轮内容 */
        switch(selected) 
        {
            case 0:
                if(left_selected == 0)
                {
                    LV_LOG_WARN("This is a timer event message, time:5s");
                }
                else if(left_selected == 1)
                {
                    LV_LOG_WARN("This is a photo event message, photo:1:1");
                }
                else if(left_selected == 2)
                {
                    LV_LOG_WARN("This is a resolition event message, resolution:4K");
                }
                break;

            case 1: 
                if(left_selected == 0)
                {
                    LV_LOG_WARN("This is a timer event message, time:OFF");
                }
                else if(left_selected == 1)
                {
                    LV_LOG_WARN("This is a photo event message, photo:4:3");
                }
                else if(left_selected == 2)
                {
                    LV_LOG_WARN("This is a resolition event message, resolution:1080P");
                }
                break;

            case 2: 
                if(left_selected == 0)
                {
                    LV_LOG_WARN("This is a timer event message, time:3s");
                }
                else if(left_selected == 1)
                {
                    LV_LOG_WARN("This is a photo event message, photo:16:9");
                }
                else if(left_selected == 2)
                {
                    LV_LOG_WARN("This is a resolition event message, resolution:none");
                }
                break;
        }
    }
}

static void left_roller_event_cb(lv_event_t * e)
{
    static int32_t last_index = -1;
    lv_obj_t *left_roller = lv_event_get_target(e);
    uint16_t selected = lv_roller_get_selected(left_roller);

    if (last_index != lv_roller_get_selected(left_roller))
    {//选项发生变化
        last_index = lv_roller_get_selected(left_roller);
        /* 根据左边滚轮的选择更新右边滚轮内容 */
        switch(selected) 
        {
            case 0: 
                lv_roller_set_options(right_roller, timer_options, LV_ROLLER_MODE_NORMAL);
                lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择OFF
                break;

            case 1: 
                lv_roller_set_options(right_roller, ratio_options, LV_ROLLER_MODE_NORMAL);
                lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择4:3
                break;

            case 2: 
                lv_roller_set_options(right_roller, resolution_options, LV_ROLLER_MODE_NORMAL);
                lv_roller_set_selected(right_roller, 0, LV_ANIM_OFF); // 默认选择4K
                break;
        }
    }
}


static void back_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_CLICKED) 
    {
        printf("back photograph\n");
        lv_subject_set_int(&shooting_adj_param_subject, PAGE_SWITCH_NEXT);
    }
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_WARN("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发
    
    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &shooting_adj_param_page;

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