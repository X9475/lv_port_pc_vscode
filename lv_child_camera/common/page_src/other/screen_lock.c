#include "../lv_switch_interface.h"

#define SCREENLOCK_2 "../lv_port_pc_vscode/assert/icon/screensaver2.png"
#define SCREENLOCK_4 "../lv_port_pc_vscode/assert/icon/screensaver4.png"
#define SCREENLOCK_5 "../lv_port_pc_vscode/assert/icon/screensaver5.png"

lv_subject_t screenlock_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;

lv_obj_t *capacity;
lv_obj_t *percent;
lv_obj_t *battery;
lv_obj_t *week;
lv_obj_t *date;
lv_obj_t *times;
struct tm *time_info;

int screenlock_style = 4;//选择样式

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_async_time_calcula();
static void click_event_handler(lv_event_t *e);
static void gesture_event_handler(lv_event_t *e);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
};

static lv_page_info_t screenlock_page_info = {
    .page_id = PAGE_FUNCTIONAL_SCREENLOCK,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_screenlock_info_get()
{
    return &screenlock_page_info;
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
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);
    lv_obj_add_event_cb(screen, click_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(act_screen, gesture_event_handler, LV_EVENT_GESTURE, NULL);

    //异步获取时间
    lv_async_call(lv_async_time_calcula, NULL);

    screenlock_page_info.page = screen;
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
    lv_style_set_bg_opa(&screen_style, LV_OPA_TRANSP);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&screenlock_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&screenlock_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&screenlock_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t *img_bg = lv_img_create(cont);
    // if (screenlock_style == 1)
    // {
    //     lv_img_set_src(img_bg, SCREENLOCK_1);
    // }
    if (screenlock_style == 2)
    {
        lv_img_set_src(img_bg, SCREENLOCK_2);
    }
    if (screenlock_style == 4)
    {
        lv_img_set_src(img_bg, SCREENLOCK_4);
    }
    if (screenlock_style == 5)
    {
        lv_img_set_src(img_bg, SCREENLOCK_5);
    }
    lv_obj_align(img_bg, LV_ALIGN_CENTER, 0, 0);

    //状态栏
    lv_obj_t *status_bar = lv_obj_create(cont);
    lv_obj_set_size(status_bar, lv_pct(100), 120);
    lv_obj_add_style(status_bar, &screen_style, 0);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

    //电池图标
    battery = lv_img_create(status_bar);
    lv_obj_set_size(battery, 40, 40);
    lv_img_set_src(battery, "../lv_port_pc_vscode/assert/icon/battery_80.png");
    lv_obj_align(battery, LV_ALIGN_TOP_LEFT, 50,20);

    percent = lv_label_create(status_bar);
    lv_label_set_text(percent, "10%");
    lv_obj_set_style_text_font(percent, fzlthr_24, 0);
    lv_obj_set_style_text_opa(percent, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(percent, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(percent, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(percent, battery, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    //存储卡
    lv_obj_t *sdcard = lv_img_create(status_bar);
    lv_obj_set_size(sdcard, 40, 40);
    lv_img_set_src(sdcard, "../lv_port_pc_vscode/assert/icon/memory_card.png");
    lv_img_set_zoom(sdcard, 128);
    lv_obj_align_to(sdcard, percent, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    capacity = lv_label_create(status_bar);
    lv_label_set_text(capacity, "32GB");
    lv_obj_set_style_text_font(capacity, fzlthr_24, 0);
    lv_obj_set_style_text_opa(capacity, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(capacity, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(capacity, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(capacity, sdcard, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    week = lv_label_create(status_bar);
    lv_label_set_text(week, "周日");
    lv_obj_set_style_text_font(week, fzlthr_22, 0);
    lv_obj_set_style_text_opa(week, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(week, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(week, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(week, LV_ALIGN_TOP_RIGHT, -50, 35);

    date = lv_label_create(status_bar);
    lv_label_set_text(date, "01|01");
    lv_obj_set_style_text_font(date, fzlthb_36, 0);
    lv_obj_set_style_text_opa(date, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(date, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(date, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(date, LV_ALIGN_TOP_RIGHT, -50, 69);

    times = lv_label_create(cont);
    lv_obj_set_style_text_opa(times, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(times, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(times, LV_TEXT_ALIGN_CENTER, 0);
    if (screenlock_style == 1)
    {
        lv_obj_set_style_text_font(times, fzlthb_56, 0);
        lv_obj_align(times, LV_ALIGN_BOTTOM_LEFT, 160, -92);
    }
    if (screenlock_style == 2)
    {
        lv_obj_set_style_text_font(times, fzlthb_118, 0);
        lv_obj_align(times, LV_ALIGN_CENTER, 0, 0);
    }
    if (screenlock_style == 4)
    {
        lv_obj_set_style_text_font(times, fzlthb_118, 0);
        lv_obj_align(times, LV_ALIGN_BOTTOM_LEFT, 58, -20);
    }
    if (screenlock_style == 5)
    {
        lv_obj_set_style_text_font(times, fzlthb_118, 0);
        lv_obj_align(times, LV_ALIGN_TOP_LEFT, 20, 100);
    }

    return;
}

static void lv_async_time_calcula()
{
    time_t raw_time;
    time(&raw_time);
    time_info = localtime(&raw_time);

    // 解析时间数据
    int hour = time_info->tm_hour;     // 24小时制 (0-23)
    int minute = time_info->tm_min;    // 范围是 0-59
    int month = time_info->tm_mon + 1;
    int day = time_info->tm_mday;

    const char *week_text[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
    int week_index = time_info->tm_wday;//tm_wday 范围是 0-6（0=周日）
    lv_label_set_text(week, week_text[week_index]);

    char date_text[10];
    snprintf(date_text, sizeof(date_text), "%02d|%02d", month, day);
    lv_label_set_text(date, date_text);

    int use_12_hour_format = 0;
    char time_text[20];
    if (use_12_hour_format)
    {
        //12小时制（例如:2:30）
        int display_hour = hour % 12;
        if (display_hour == 0) display_hour = 12;  // 0点显示为12
        if (screenlock_style == 1)
        {
            snprintf(time_text, sizeof(time_text), "%02d\n%02d", display_hour, minute);
        }
        if (screenlock_style == 2)
        {
            snprintf(time_text, sizeof(time_text), "%02d:%02d", display_hour, minute);
        }
        if (screenlock_style == 4)
        {
            snprintf(time_text, sizeof(time_text), "%02d\n%02d", display_hour, minute);
        }
        if (screenlock_style == 5)
        {
            snprintf(time_text, sizeof(time_text), "%02d:%02d", display_hour, minute);
        }
    }
    else
    {
        //24小时制（例如:14:30）
        if (screenlock_style == 1)
        {
            snprintf(time_text, sizeof(time_text), "%02d\n%02d", hour, minute);
        }
        if (screenlock_style == 2)
        {
            snprintf(time_text, sizeof(time_text), "%02d:%02d", hour, minute);
        }
        if (screenlock_style == 4)
        {
            snprintf(time_text, sizeof(time_text), "%02d\n%02d", hour, minute);
        }
        if (screenlock_style == 5)
        {
            snprintf(time_text, sizeof(time_text), "%02d:%02d", hour, minute);
        }
    }
    lv_label_set_text(times, time_text);

    return;
}

static void click_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    printf("[%s:%d] -- click event:%d\n", __FILE__, __LINE__, code);
}

static void gesture_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    printf("[%s:%d] -- gesture event:%d\n", __FILE__, __LINE__, code);
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
    switch_page->old_page = &screenlock_page_info;

    switch (page_event)
    {
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