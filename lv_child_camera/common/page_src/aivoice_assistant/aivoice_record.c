#include "../lv_switch_interface.h"

lv_subject_t airecord_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_obj_t *title_cont_one = NULL;
static lv_obj_t *title_cont_two = NULL;
static lv_obj_t *calender_page = NULL;
static lv_obj_t *airecord_page = NULL;
static lv_style_t screen_style;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_page_load(lv_obj_t *cont);
static void page_back_event_cb(lv_event_t *e);
static  void calender_button_click_cb(lv_event_t *e);

static lv_obj_t *calender_page_create();//日历选择
static lv_obj_t *airecord_list_create();//记录列表
static void airecord_is_empty();//空记录

typedef struct
{
    char week[32];    //星期
    uint32_t date;    //日期
    bool is_exsit;    //是否存在
    bool is_select;   //是否选择
} calender_info_t;

static const char *week_str[] = { "周天", "周一", "周二", "周三", "周四", "周五", "周六"};
static calender_info_t g_calender_info[] = {
    {"周一", 21, false, false},
    {"周二", 22, true, false},
    {"周三", 23, false, false},
    {"周四", 24, true, false},
    {"周五", 25, false, false},
    {"周六", 26, false, false},
    {"周天", 27, true, true},
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t airecord_page_info = {
    .page_id = PAGE_FUNCTIONAL_AIRECORD,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_airecord_get()
{
    return &airecord_page_info;
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
    airecord_page_info.page = screen;
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
    lv_subject_init_int(&airecord_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&airecord_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&airecord_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    /*************标题1*************/
    title_cont_one = lv_obj_create(cont);
    lv_obj_set_size(title_cont_one, lv_pct(100), 70);
    lv_obj_add_style(title_cont_one, &screen_style, 0);
    lv_obj_align(title_cont_one, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(title_cont_one, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *back_btn = lv_btn_create(title_cont_one);
    lv_obj_set_size(back_btn, 70, 70);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, back_btn);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 20, 10);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 3, 0);

    lv_obj_t *label_one = lv_label_create(title_cont_one);
    lv_obj_set_size(label_one, 112, 37);
    lv_label_set_text(label_one, "选择时间");
    lv_obj_set_style_text_font(label_one, fzlthr_28, 0);
    lv_obj_set_style_text_opa(label_one, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label_one, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label_one, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_one, LV_ALIGN_LEFT_MID, 80, 15);
    lv_obj_add_flag(label_one, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *calender = lv_img_create(title_cont_one);
    lv_obj_set_size(calender, 40, 40);
    lv_img_set_src(calender, "../lv_port_pc_vscode/assert/icon/calender_unselect.png");
    lv_obj_align(calender, LV_ALIGN_TOP_RIGHT, -30, 25);
    lv_obj_add_flag(calender, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(calender, calender_button_click_cb, LV_EVENT_CLICKED, back_btn);

    /*************标题2*************/
    title_cont_two = lv_obj_create(cont);
    lv_obj_set_size(title_cont_two, lv_pct(100), 70);
    lv_obj_add_style(title_cont_two, &screen_style, 0);
    lv_obj_align(title_cont_two, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(title_cont_two, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(title_cont_two, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *label_two = lv_label_create(title_cont_two);
    lv_obj_set_size(label_two, 56, 37);
    lv_label_set_text(label_two, "取消");
    lv_obj_set_style_text_font(label_two, fzlthr_28, 0);
    lv_obj_set_style_text_opa(label_two, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label_two, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label_two, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_two, LV_ALIGN_BOTTOM_LEFT, 40, 0);

    lv_obj_t *trash = lv_img_create(title_cont_two);
    lv_obj_set_size(trash, 40, 40);
    lv_img_set_src(trash, "../lv_port_pc_vscode/assert/icon/photograph_icon_trash_filled.png");
    lv_obj_align(trash, LV_ALIGN_TOP_RIGHT, -40, 25);

    //创建记录列表
    airecord_page = airecord_list_create();

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    if (NULL != calender_page)
    {
        lv_obj_del(calender_page);
        calender_page = NULL;

        lv_obj_t *label_one = lv_obj_get_child(title_cont_one, 1);
        lv_obj_t *calender = lv_obj_get_child(title_cont_one, 2);

        lv_obj_add_flag(label_one, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(calender, LV_OBJ_FLAG_HIDDEN);
        
        bool is_select = false;
        for (int i = 0; i < sizeof(g_calender_info)/sizeof(g_calender_info[0]); i++)
        {
            if (g_calender_info[i].is_select)
            {
                is_select = true;
                break;
            }
        }

        if (is_select)
        {
            lv_img_set_src(calender, "../lv_port_pc_vscode/assert/icon/calender_select.png");
        }
        else
        {
            lv_img_set_src(calender, "../lv_port_pc_vscode/assert/icon/calender_unselect.png");
        }

        //显示记录列表
        lv_obj_clear_flag(airecord_page, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_subject_set_int(&airecord_subject, PAGE_SWITCH_BACK);
    }
}

static  void calender_button_click_cb(lv_event_t *e)
{
    // lv_obj_t *calender = lv_event_get_target(e);
    lv_obj_t *label_one = lv_obj_get_child(title_cont_one, 1);
    lv_obj_t *calender = lv_obj_get_child(title_cont_one, 2);

    lv_obj_clear_flag(label_one, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(calender, LV_OBJ_FLAG_HIDDEN);

    //隐藏记录列表
    lv_obj_add_flag(airecord_page, LV_OBJ_FLAG_HIDDEN);
    //绘制日期选择
    calender_page = calender_page_create();
}

static lv_obj_t *airecord_list_create()
{
    //绘制记录页面
    lv_obj_t *cont = lv_obj_create(screen);
    lv_obj_set_size(cont, 442, 340);
    lv_obj_add_style(cont, &screen_style, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 20, 0);
    lv_obj_set_style_pad_column(cont, 20, 0);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_move_background(cont);

    for (int i = 0; i < 7; i++)
    {
        lv_obj_t *obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 442, 40);
        lv_obj_add_style(obj, &screen_style, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x1F2221), 0);
        lv_obj_set_style_radius(obj, 20, 0);
        lv_obj_set_style_border_width(obj, 0, 0);
        lv_obj_set_style_border_color(obj, lv_color_hex(0xAFF99C), 0);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    return cont;
}

static void date_button_click_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    lv_obj_t *cont = lv_event_get_user_data(e);
    lv_obj_t *date_btn = lv_event_get_target(e);

    if (LV_EVENT_CLICKED == code)
    {
        for (int i = 0; i < lv_obj_get_child_count(cont); i++)
        {
            lv_obj_t *obj = lv_obj_get_child(cont, i);
            if (obj == date_btn)
            {
                if (!g_calender_info[i].is_select)
                {
                    g_calender_info[i].is_select = true;
                    lv_obj_set_style_border_width(obj, 3, 0);
                }
                else
                {
                    g_calender_info[i].is_select = false;
                    lv_obj_set_style_border_width(obj, 0, 0);
                }
            }
            else
            {
                g_calender_info[i].is_select = false;
                lv_obj_set_style_border_width(obj, 0, 0);
            }
        }
    }
}

static void single_calender_create(lv_obj_t *cont, int index)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_set_size(obj, 94, 130);
    lv_obj_add_style(obj, &screen_style, 0);
    lv_obj_set_style_radius(obj, 30, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1F2221), 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xAFF99C), 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(obj, date_button_click_cb, LV_EVENT_CLICKED, cont);

    if (g_calender_info[index].is_select)
    {
        lv_obj_set_style_border_width(obj, 3, 0);
    }

    lv_obj_t *week = lv_label_create(obj);
    lv_obj_set_size(week, 49, 29);
    lv_label_set_text(week, g_calender_info[index].week);
    lv_obj_set_style_text_font(week, fzlthr_22, 0);
    lv_obj_set_style_text_opa(week, LV_OPA_60, 0);
    lv_obj_set_style_text_color(week, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(week, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(week, LV_ALIGN_TOP_MID, 0, 16);

    if (index == (sizeof(g_calender_info)/sizeof(g_calender_info[0])-1))
    {
        lv_obj_t *date = lv_label_create(obj);
        lv_obj_set_size(date, 64, 42);
        lv_label_set_text(date, "今天");
        lv_obj_set_style_text_font(date, fzlthr_32, 0);
        lv_obj_set_style_text_opa(date, LV_OPA_COVER, 0);
        lv_obj_set_style_text_color(date, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_align(date, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(date, LV_ALIGN_TOP_MID, 0, 48);
    }
    else
    {
        char date_str[16] = {0};
        snprintf(date_str, sizeof(date_str), "%d", g_calender_info[index].date);
        lv_obj_t *date = lv_label_create(obj);
        lv_obj_set_size(date, 49, 29);
        lv_label_set_text(date, date_str);
        lv_obj_set_style_text_font(date, fzlthr_32, 0);
        lv_obj_set_style_text_opa(date, LV_OPA_COVER, 0);
        lv_obj_set_style_text_color(date, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_align(date, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(date, LV_ALIGN_TOP_MID, 0, 48);
    }

    if (g_calender_info[index].is_exsit)
    {
        lv_obj_t *led  = lv_led_create(obj);
        lv_obj_set_size(led, 10, 10);
        lv_led_set_color(led, lv_color_hex(0xAFF99C));
        lv_led_set_brightness(led, LV_LED_BRIGHT_MAX);
        lv_obj_set_style_radius(led, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_shadow_width(led, 0, 0);
        lv_obj_set_style_shadow_spread(led, 0, 0);
        lv_obj_align(led, LV_ALIGN_BOTTOM_MID, 0, -20);
    }
}

static void update_actual_date()
{
    time_t now;
    time(&now);
    struct tm *tm_now = localtime(&now);
    
    if (tm_now == NULL) return;
    int current_weekday = tm_now->tm_wday;

    // 显示最近7天的数据（倒序：从当前往前推6天）
    for (int i = 0; i < sizeof(g_calender_info)/sizeof(g_calender_info[0]); i++)
    {
        //倒序：i=0显示6天前，i=6显示今天
        int days_ago = 6 - i;
        g_calender_info[i].date = tm_now->tm_mday - days_ago;
        
        //计算对应的星期
        int week_index = current_weekday - days_ago;
        if (week_index < 0)
        {
            week_index += 7;
        }

        lv_strcpy(g_calender_info[i].week, week_str[week_index]);
    }
}

static lv_obj_t *calender_page_create()
{
    //更新实际的日期
    update_actual_date();

    //绘制日期选择页面
    lv_obj_t *cont = lv_obj_create(screen);
    lv_obj_set_size(cont, 442, 340);
    lv_obj_add_style(cont, &screen_style, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 20, 0);
    lv_obj_set_style_pad_column(cont, 20, 0);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_move_background(cont);

    for (int i = 0; i < 7; i++)
    {
        single_calender_create(cont, i);
    }

    return cont;
}

static void airecord_is_empty()
{
    lv_obj_t *msg_obj = lv_obj_create(screen);
    lv_obj_set_size(msg_obj, lv_pct(100), lv_pct(100));
    lv_obj_add_style(msg_obj, &screen_style, 0);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_move_background(msg_obj);
    lv_obj_center(msg_obj);

    lv_obj_t *image = lv_img_create(msg_obj);
    lv_obj_set_size(image, 380, 210);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/empety_pic_photo.png");
    lv_obj_align(image, LV_ALIGN_TOP_MID, 0, 70);

    lv_obj_t *label = lv_label_create(msg_obj);
    lv_obj_set_size(label, 422, 70);
    lv_label_set_text(label, "没发现任何的问答记录哦~\n快去找PIKA聊聊天吧");
    lv_obj_set_style_text_font(label, fzlthr_26, 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -50);
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
    switch_page->old_page = &airecord_page_info;

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
