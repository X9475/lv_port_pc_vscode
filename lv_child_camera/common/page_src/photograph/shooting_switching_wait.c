#include "../lv_switch_interface.h"

lv_subject_t  shooting_switch_wait_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;

static lv_obj_t *screen = NULL;
static lv_timer_t *switch_timer = NULL;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void timer_callback(lv_timer_t * timer);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t shooting_switch_wait_page = {
    .page_id = PAGE_FUNCTIONAL_SHOOTING_WAIT,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_shooting_switch_wait_get()
{
    return &shooting_switch_wait_page;
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
    shooting_switch_wait_page.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    //删除定时器,会崩溃,所以注释掉
    if(switch_timer) 
    {
        lv_timer_del(switch_timer);
        switch_timer = NULL;
    }

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
    lv_subject_init_int(&shooting_switch_wait_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&shooting_switch_wait_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&shooting_switch_wait_subject);
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&shooting_switch_wait_subject, PAGE_SWITCH_BACK);
}

static void lv_page_load(lv_obj_t *cont)
{
    // lv_obj_t * animimg0 = lv_animimg_create(cont);
    // lv_obj_align(animimg0, LV_ALIGN_TOP_LEFT, 98, 43);

    //todo:等待后续的动态图像
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "切换中");

    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(30), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 320);

    // 创建定时器，2秒后执行跳转
    switch_timer = lv_timer_create(timer_callback, 2000, NULL);
    lv_timer_set_repeat_count(switch_timer, 1);  // 只执行一次

    return;
}

// 定时器回调函数
static void timer_callback(lv_timer_t * timer)
{
    //删除定时器
    if(switch_timer) 
    {
        lv_timer_del(switch_timer);
        switch_timer = NULL;
    }

    // 跳转到其他页面的代码
    lv_subject_set_int(&shooting_switch_wait_subject, PAGE_SWITCH_NEXT);
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
    switch_page->old_page = &shooting_switch_wait_page;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_shooting_switch_video_get();
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