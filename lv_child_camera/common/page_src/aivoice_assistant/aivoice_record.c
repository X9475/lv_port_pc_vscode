#include "../lv_switch_interface.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

/* ========== 布局参数 ========== */

#define AIRECORD_SCROLL_W           442
#define AIRECORD_SCROLL_H           340
#define AIRECORD_SCROLL_X           30
#define AIRECORD_SCROLL_Y           70
#define AIRECORD_MAX_BUBBLE_W       320
#define AIRECORD_BUBBLE_RADIUS      41
#define AIRECORD_BUBBLE_PAD_H       30
#define AIRECORD_BUBBLE_PAD_V       26

/* 智能体气泡颜色（左上角直角，其余圆角） */
#define AIRECORD_AGENT_COLOR        0x352A31
/* 问题气泡颜色（右上角直角，其余圆角） */
#define AIRECORD_QUESTION_COLOR     0xCE94F8

/* 语音条尺寸 */
#define AIRECORD_VOICE_W            206
#define AIRECORD_VOICE_H            94
/* 图片尺寸 */
#define AIRECORD_IMAGE_W            290
#define AIRECORD_IMAGE_H            180

/* 删除模式：消息右移距离 */
#define AIRECORD_DELETE_SHIFT       100
/* 勾选框尺寸 */
#define CHECKBOX_SIZE               45
/* 时间分隔高度 */
#define TIME_DIVIDER_H              30

/* 长按弹窗尺寸 */
#define POPUP_W                     188
#define POPUP_H                     107
#define POPUP_ARROW_H               12          /* 底部箭头高度 */
#define POPUP_RADIUS                20          /* 弹窗圆角 */

/* 超过5分钟(300秒)插入时间分隔 */
#define TIME_DIVIDER_THRESHOLD      300

/* ========== 数据结构 ========== */

/* 对话消息类型 */
typedef enum {
    AIRECORD_MSG_TYPE_TEXT = 0,     // 智能体回复（纯文字）
    AIRECORD_MSG_TYPE_VOICE,        // 问题-语音条
    AIRECORD_MSG_TYPE_IMAGE,        // 问题-图片
} airecord_msg_type_t;

/* 对话消息数据 */
typedef struct {
    airecord_msg_type_t type;
    bool is_question;               // true=问题(右侧), false=智能体回复(左侧)
    const char *text;               // 文字内容
    const char *voice_path;         // 语音文件路径
    int voice_duration;             // 语音时长(秒)
    const char *image_path;         // 图片路径
    uint64_t timestamp;             // 消息时间戳（秒），用于计算时间分隔
} airecord_msg_t;

/* 单条消息的内部包装结构 */
typedef struct {
    lv_obj_t *row;                  // 行容器（包含勾选框 + 气泡）
    lv_obj_t *checkbox;             // 勾选框
    lv_obj_t *bubble;               // 气泡对象
    airecord_msg_t msg_data;        // 消息数据副本
    bool checked;                   // 是否勾选
} airecord_item_t;

/* ========== 全局变量声明 ========== */
lv_subject_t airecord_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *airecord_scroll_cont = NULL;
static lv_obj_t *airecord_parent = NULL;        // 列表父容器
static airecord_item_t *airecord_items = NULL;  // 动态数组
static int airecord_item_count = 0;
static int airecord_item_capacity = 0;

/* 删除模式 */
static bool g_delete_mode = false;

/* 页面相关 */
static lv_obj_t *screen = NULL;
static lv_obj_t *back_btn = NULL;
static lv_obj_t *title_cont_one = NULL;
static lv_obj_t *title_cont_two = NULL;
static lv_obj_t *calender_page = NULL;
static lv_obj_t *airecord_page = NULL;
static lv_style_t screen_style;

/* 长按弹窗 */
static lv_obj_t *g_popup = NULL;
static lv_obj_t *g_longpress_target_row = NULL;
static lv_obj_t *g_longpress_target_bubble = NULL;

/* ========== 函数声明 ========== */
static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_single_init(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_page_load(lv_obj_t *cont);
static void page_back_event_cb(lv_event_t *e);
static void calender_button_click_cb(lv_event_t *e);
static void trash_button_click_cb(lv_event_t *e);
static void cancel_button_click_cb(lv_event_t *e);

static lv_obj_t *airecord_list_create(lv_obj_t *parent);
static lv_obj_t *calender_page_create();
static lv_obj_t *airecord_text_bubble_create(lv_obj_t *parent, const char *text, bool is_question);
static lv_obj_t *airecord_voice_bubble_create(lv_obj_t *parent, const char *voice_path, int duration, bool is_question);
static lv_obj_t *airecord_image_bubble_create(lv_obj_t *parent, const char *image_path, bool is_question);
static void airecord_add_time_divider_internal(uint64_t timestamp);
static void airecord_update_layout(void);
static void airecord_checkbox_click_cb(lv_event_t *e);
static void airecord_bubble_longpress_cb(lv_event_t *e);
static void airecord_enter_delete_mode_internal(lv_obj_t *target_row);
static void airecord_exit_delete_mode_internal(void);
static void airecord_popup_create(lv_obj_t *anchor_bubble);
static void airecord_popup_destroy(void);
static void airecord_popup_draw_event(lv_event_t *e);
static void airecord_popup_click_blank_cb(lv_event_t *e);
// static void airecord_is_empty();//空记录

typedef struct
{
    char week[16];    //星期
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
    // .back_btn = &back_btn,
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
    //单次初始化
    lv_page_single_init();

    screen = lv_obj_create(act_screen);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);
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

static void lv_page_single_init(void)
{
    static bool inited = false;

    if (inited != true)
    {
        inited = true;
    }
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
    lv_obj_add_flag(title_cont_one, LV_OBJ_FLAG_EVENT_BUBBLE);
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
    lv_obj_align(label_one, LV_ALIGN_LEFT_MID, 80, 10);
    lv_obj_add_flag(label_one, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *calender_btn = lv_btn_create(title_cont_one);
    lv_obj_set_size(calender_btn, 50, 50);
    lv_obj_set_style_shadow_width(calender_btn, 0, 0);
    lv_obj_set_style_bg_opa(calender_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(calender_btn, 0, 0);
    lv_obj_align(calender_btn, LV_ALIGN_TOP_RIGHT, -35, 20);
    lv_obj_add_event_cb(calender_btn, calender_button_click_cb, LV_EVENT_CLICKED, label_one);

    lv_obj_t *calender = lv_img_create(calender_btn);
    lv_obj_set_size(calender, 40, 40);
    lv_img_set_src(calender, "../lv_port_pc_vscode/assert/icon/calender_unselect.png");
    lv_obj_align(calender, LV_ALIGN_CENTER, 0, 0);

    /*************标题2*************/
    title_cont_two = lv_obj_create(cont);
    lv_obj_set_size(title_cont_two, lv_pct(100), 70);
    lv_obj_add_style(title_cont_two, &screen_style, 0);
    lv_obj_align(title_cont_two, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(title_cont_two, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(title_cont_two, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(title_cont_two, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *cancel_btn = lv_btn_create(title_cont_two);
    lv_obj_set_size(cancel_btn, 56, 37);
    lv_obj_set_style_shadow_width(cancel_btn, 0, 0);
    lv_obj_set_style_bg_opa(cancel_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(cancel_btn, 0, 0);
    lv_obj_align(cancel_btn, LV_ALIGN_BOTTOM_LEFT, 40, 0);
    lv_obj_add_event_cb(cancel_btn, cancel_button_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label_two = lv_label_create(cancel_btn);
    lv_obj_set_size(label_two, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_text(label_two, "取消");
    lv_obj_set_style_text_font(label_two, fzlthr_28, 0);
    lv_obj_set_style_text_opa(label_two, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label_two, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label_two, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_two, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *trash_btn = lv_btn_create(title_cont_two);
    lv_obj_set_size(trash_btn, 50, 50);
    lv_obj_set_style_shadow_width(trash_btn, 0, 0);
    lv_obj_set_style_bg_opa(trash_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(trash_btn, 0, 0);
    lv_obj_align(trash_btn, LV_ALIGN_TOP_RIGHT, -35, 20);
    lv_obj_add_event_cb(trash_btn, trash_button_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *trash = lv_img_create(trash_btn);
    lv_obj_set_size(trash, 40, 40);
    lv_img_set_src(trash, "../lv_port_pc_vscode/assert/icon/photograph_icon_trash_filled.png");
    lv_obj_align(trash, LV_ALIGN_CENTER, 0, 0);

    //创建记录列表
    airecord_page = airecord_list_create(cont);
    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    if (NULL != calender_page)
    {
        lv_obj_del(calender_page);
        calender_page = NULL;

        lv_obj_t *label_one = lv_obj_get_child(title_cont_one, 1);
        lv_obj_t *calender_btn = lv_obj_get_child(title_cont_one, 2);

        lv_obj_add_flag(label_one, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(calender_btn, LV_OBJ_FLAG_HIDDEN);

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
            lv_img_set_src(lv_obj_get_child(calender_btn, 0), "../lv_port_pc_vscode/assert/icon/calender_select.png");
        }
        else
        {
            lv_img_set_src(lv_obj_get_child(calender_btn, 0), "../lv_port_pc_vscode/assert/icon/calender_unselect.png");
        }

        //显示记录列表
        lv_obj_clear_flag(airecord_page, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_subject_set_int(&airecord_subject, PAGE_SWITCH_BACK);
    }
}

static void airecord_delete_selected(void)
{
    /* 从后往前删除，避免索引变化 */
    for (int i = airecord_item_count - 1; i >= 0; i--) {
        if (airecord_items[i].checked && airecord_items[i].row) {
            lv_obj_del(airecord_items[i].row);
            /* 移动后续元素 */
            for (int j = i; j < airecord_item_count - 1; j++) {
                airecord_items[j] = airecord_items[j + 1];
            }
            airecord_item_count--;
        }
    }

    airecord_exit_delete_mode_internal();
}

static void airecord_checkbox_set_checked(lv_obj_t *cb, bool checked)
{
    if (checked) {
        /* 隐藏圆圈背景和边框，只显示 icon */
        lv_obj_set_style_bg_opa(cb, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_opa(cb, LV_OPA_TRANSP, 0);

        /* 用 icon 图片覆盖 */
        lv_obj_t *check_mark = lv_obj_get_child(cb, 0);
        if (check_mark == NULL) {
            check_mark = lv_img_create(cb);
            lv_img_set_src(check_mark, "../lv_port_pc_vscode/assert/icon/airecord_select.png");
            lv_obj_set_size(check_mark, CHECKBOX_SIZE, CHECKBOX_SIZE);
            lv_obj_center(check_mark);
        }
        lv_obj_clear_flag(check_mark, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_style_bg_opa(cb, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_opa(cb, LV_OPA_50, 0);
        lv_obj_set_style_border_width(cb, 2, 0);
        lv_obj_set_style_border_color(cb, lv_color_hex(0xFFFFFF), 0);

        lv_obj_t *check_mark = lv_obj_get_child(cb, 0);
        if (check_mark) {
            lv_obj_add_flag(check_mark, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void airecord_enter_delete_mode_internal(lv_obj_t *target_row)
{
    g_delete_mode = true;

    /* 显示所有勾选框 */
    for (int i = 0; i < airecord_item_count; i++) {
        if (airecord_items[i].checkbox) {
            lv_obj_clear_flag(airecord_items[i].checkbox, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /* 如果指定了 target_row，自动勾选该行 */
    if (target_row) {
        for (int i = 0; i < airecord_item_count; i++) {
            if (airecord_items[i].row == target_row) {
                airecord_items[i].checked = true;
                airecord_checkbox_set_checked(airecord_items[i].checkbox, true);
                break;
            }
        }
    }

    airecord_update_layout();
}

static int airecord_get_selected_count(void)
{
    int count = 0;
    for (int i = 0; i < airecord_item_count; i++) {
        if (airecord_items[i].checked) count++;
    }
    return count;
}

static void cancel_button_click_cb(lv_event_t *e)
{
    /* 离开模式 */
    airecord_exit_delete_mode_internal();

    /* 更换标题栏内容 */
    if (title_cont_one) lv_obj_clear_flag(title_cont_one, LV_OBJ_FLAG_HIDDEN);
    if (title_cont_two) lv_obj_add_flag(title_cont_two, LV_OBJ_FLAG_HIDDEN);
}

static void trash_button_click_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_CLICKED == code)
    {
        if (g_delete_mode)
        {
            /* 删除模式下点击：如果有选中项则删除，否则退出删除模式 */
            if (airecord_get_selected_count() > 0)
            {
                airecord_delete_selected();
            }
            else
            {
                /* 离开删除模式 */
                airecord_exit_delete_mode_internal();
            }
        } else {
            /* 普通模式：进入删除模式 */
            airecord_enter_delete_mode_internal(NULL);
        }
    }
}

static void calender_button_click_cb(lv_event_t *e)
{
    lv_obj_t *label_one = lv_event_get_user_data(e);
    lv_obj_t *calender_btn = lv_event_get_target(e);

    lv_obj_clear_flag(label_one, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(calender_btn, LV_OBJ_FLAG_HIDDEN);

    //隐藏记录列表
    lv_obj_add_flag(airecord_page, LV_OBJ_FLAG_HIDDEN);

    //绘制日期选择
    calender_page = calender_page_create();
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

    //显示最近7天的数据（倒序：从当前往前推6天）
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

// static void airecord_is_empty()
// {
//     lv_obj_t *msg_obj = lv_obj_create(screen);
//     lv_obj_set_size(msg_obj, lv_pct(100), lv_pct(100));
//     lv_obj_add_style(msg_obj, &screen_style, 0);
//     lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_SCROLLABLE);
//     lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
//     lv_obj_move_background(msg_obj);
//     lv_obj_center(msg_obj);

//     lv_obj_t *image = lv_img_create(msg_obj);
//     lv_obj_set_size(image, 380, 210);
//     lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/empety_pic_photo.png");
//     lv_obj_align(image, LV_ALIGN_TOP_MID, 0, 70);

//     lv_obj_t *label = lv_label_create(msg_obj);
//     lv_obj_set_size(label, 422, 70);
//     lv_label_set_text(label, "没发现任何的问答记录哦~\n快去找PIKA聊聊天吧");
//     lv_obj_set_style_text_font(label, fzlthr_26, 0);
//     lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
//     lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
//     lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
//     lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -50);
// }

/**
 * 将时间戳格式化为 "6月34日 10:32" 格式
 */
static void format_timestamp(uint64_t ts, char *buf, size_t buf_size)
{
    time_t t = (time_t)ts;
    struct tm *tm_info = localtime(&t);
    if (tm_info) {
        snprintf(buf, buf_size, "%d月%d日 %02d:%02d",
                 tm_info->tm_mon + 1, tm_info->tm_mday,
                 tm_info->tm_hour, tm_info->tm_min);
    } else {
        snprintf(buf, buf_size, "%d月%d日 %02d:%02d", 1, 1, 0, 0);
    }
}

/**
 * 气泡绘制事件回调
 * 智能体回复：左上角直角，其余圆角41，颜色 #352A31
 * 问题：右上角直角，其余圆角41，颜色 #CE94F8
 */
static void airecord_bubble_draw_event(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_layer_t *layer = lv_event_get_layer(e);

    lv_area_t area;
    lv_obj_get_coords(obj, &area);

    bool is_question = (bool)(uintptr_t)lv_obj_get_user_data(obj);

    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);

    dsc.bg_opa = LV_OPA_COVER;
    dsc.radius = AIRECORD_BUBBLE_RADIUS;

    if (is_question) {
        dsc.bg_color = lv_color_hex(AIRECORD_QUESTION_COLOR);
    } else {
        dsc.bg_color = lv_color_hex(AIRECORD_AGENT_COLOR);
    }

    lv_draw_rect(layer, &dsc, &area);

    /* 再补一个矩形把指定角填平 */
    lv_draw_rect_dsc_t fill;
    lv_draw_rect_dsc_init(&fill);

    fill.bg_opa = LV_OPA_COVER;
    fill.bg_color = dsc.bg_color;
    fill.radius = 0;

    lv_area_t fix;

    if (is_question) {
        /* 右上角直角 */
        fix.x1 = area.x2 - AIRECORD_BUBBLE_RADIUS;
        fix.y1 = area.y1;
        fix.x2 = area.x2;
        fix.y2 = area.y1 + AIRECORD_BUBBLE_RADIUS;
    } else {
        /* 左上角直角 */
        fix.x1 = area.x1;
        fix.y1 = area.y1;
        fix.x2 = area.x1 + AIRECORD_BUBBLE_RADIUS;
        fix.y2 = area.y1 + AIRECORD_BUBBLE_RADIUS;
    }

    lv_draw_rect(layer, &fill, &fix);
}

/**
 * 纯文本的消息气泡创建
 */
static lv_obj_t *airecord_text_bubble_create(lv_obj_t *parent, const char *text, bool is_question)
{
    lv_obj_t *bubble = lv_obj_create(parent);
    lv_obj_remove_style_all(bubble);
    lv_obj_clear_flag(bubble, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_user_data(bubble, (void *)(uintptr_t)is_question);

    /* 气泡总宽度（含 padding）不超过 AIRECORD_MAX_BUBBLE_W */
    lv_obj_set_style_max_width(bubble, AIRECORD_MAX_BUBBLE_W, 0);
    lv_obj_set_height(bubble, LV_SIZE_CONTENT);

    /* 气泡自身的 padding */
    lv_obj_set_style_pad_left(bubble, AIRECORD_BUBBLE_PAD_H, 0);
    lv_obj_set_style_pad_right(bubble, AIRECORD_BUBBLE_PAD_H, 0);
    lv_obj_set_style_pad_top(bubble, AIRECORD_BUBBLE_PAD_V, 0);
    lv_obj_set_style_pad_bottom(bubble, AIRECORD_BUBBLE_PAD_V, 0);
    lv_obj_add_event_cb(bubble, airecord_bubble_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    lv_obj_t *label = lv_label_create(bubble);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    /* label 内容区最大宽度 = 气泡最大宽度 - 左右 padding */
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_style_max_width(label, AIRECORD_MAX_BUBBLE_W - AIRECORD_BUBBLE_PAD_H * 2, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, fzlthr_32, 0);
    lv_obj_set_style_pad_all(label, 0, 0);

    lv_obj_update_layout(label);

    lv_coord_t w = lv_obj_get_width(label);
    // lv_coord_t h = lv_obj_get_height(label);

    /* label 宽度受 max_width 限制，不会超过内容区上限 */
    w += AIRECORD_BUBBLE_PAD_H * 2;
    if (w > AIRECORD_MAX_BUBBLE_W) {
        w = AIRECORD_MAX_BUBBLE_W;
    }

    lv_obj_set_width(bubble, w);
    lv_obj_align(label, is_question ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID, 0, 0);

    return bubble;
}

/**
 * 带语音的消息气泡创建
 */
static lv_obj_t *airecord_voice_bubble_create(lv_obj_t *parent, const char *voice_path, int duration, bool is_question)
{
    LV_UNUSED(voice_path);

    lv_obj_t *bubble = lv_obj_create(parent);
    lv_obj_remove_style_all(bubble);
    lv_obj_clear_flag(bubble, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_user_data(bubble, (void *)(uintptr_t)is_question);
    lv_obj_set_size(bubble, AIRECORD_VOICE_W, AIRECORD_VOICE_H);
    lv_obj_add_event_cb(bubble, airecord_bubble_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    lv_obj_t *voice_icon = lv_img_create(bubble);
    lv_obj_set_size(voice_icon, 40, 40);
    lv_img_set_src(voice_icon, "../lv_port_pc_vscode/assert/icon/message_window_voice.png");
    lv_obj_align(voice_icon, LV_ALIGN_RIGHT_MID, -30, 0);

    lv_obj_t *duration_label = lv_label_create(bubble);
    lv_obj_set_size(duration_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(duration_label, "%d\' %d\"", duration / 60, duration % 60);
    lv_obj_set_style_text_color(duration_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(duration_label, fzlthr_30, 0);
    lv_obj_align_to(duration_label, voice_icon, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    return bubble;
}

/**
 * 图片的消息气泡创建
 */
static lv_obj_t *airecord_image_bubble_create(lv_obj_t *parent, const char *image_path, bool is_question)
{
    lv_obj_t *bubble = lv_obj_create(parent);
    lv_obj_remove_style_all(bubble);
    lv_obj_clear_flag(bubble, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(bubble, 20, 0);
    lv_obj_set_style_clip_corner(bubble, true, 0);
    lv_obj_set_user_data(bubble, (void *)(uintptr_t)is_question);
    lv_obj_set_size(bubble, AIRECORD_IMAGE_W, AIRECORD_IMAGE_H);
    // lv_obj_add_event_cb(bubble, airecord_bubble_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    lv_obj_t *img = lv_img_create(bubble); 
    lv_img_set_src(img, image_path);
    lv_obj_set_size(img, AIRECORD_IMAGE_W, AIRECORD_IMAGE_H); 
    lv_obj_center(img); // 在 bubble 内部居中

    return bubble;
}

/**
 * 添加时间分隔条，格式 "6月34日 10:32"
 */
static void airecord_add_time_divider_internal(uint64_t timestamp)
{
    if (airecord_scroll_cont == NULL) return;

    char time_str[64];
    format_timestamp(timestamp, time_str, sizeof(time_str));

    lv_obj_t *divider = lv_obj_create(airecord_scroll_cont);
    lv_obj_remove_style_all(divider);
    lv_obj_clear_flag(divider, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_width(divider, AIRECORD_SCROLL_W);
    lv_obj_set_height(divider, TIME_DIVIDER_H);
    lv_obj_set_style_bg_opa(divider, LV_OPA_TRANSP, 0);

    lv_obj_t *label = lv_label_create(divider);
    lv_label_set_text(label, time_str);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, fzlthr_20, 0);
    lv_obj_set_style_text_opa(label, LV_OPA_60, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(label);
}

/**
 * 勾选框创建，用于显示是否删除
 */
static lv_obj_t *airecord_checkbox_create(lv_obj_t *parent)
{
    lv_obj_t *cb = lv_obj_create(parent);
    lv_obj_remove_style_all(cb);
    lv_obj_set_size(cb, CHECKBOX_SIZE, CHECKBOX_SIZE);
    lv_obj_set_style_radius(cb, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(cb, 2, 0);
    lv_obj_set_style_border_color(cb, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(cb, LV_OPA_50, 0);
    lv_obj_set_style_bg_opa(cb, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(cb, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cb, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(cb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(cb, airecord_checkbox_click_cb, LV_EVENT_CLICKED, NULL);

    return cb;
}

/**
 * 弹窗自定义绘制事件
 * 绘制圆角矩形背景 + 底部凸出箭头（指向气泡中间）
 */
static void airecord_popup_draw_event(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_layer_t *layer = lv_event_get_layer(e);

    lv_area_t area;
    lv_obj_get_coords(obj, &area);

    lv_coord_t w = lv_area_get_width(&area);
    // lv_coord_t h = lv_area_get_height(&area);

    /* 主体矩形区域（不含箭头） */
    lv_area_t rect_area;
    rect_area.x1 = area.x1;
    rect_area.x2 = area.x2;
    rect_area.y1 = area.y1;
    rect_area.y2 = area.y2 - POPUP_ARROW_H;

    /* 1. 主体阴影 */
    lv_draw_box_shadow_dsc_t shadow_dsc;
    lv_draw_box_shadow_dsc_init(&shadow_dsc);
    shadow_dsc.color = lv_color_hex(0x000000);
    shadow_dsc.width = 20;
    shadow_dsc.opa = LV_OPA_60;
    shadow_dsc.radius = POPUP_RADIUS;
    shadow_dsc.bg_cover = 0;
    lv_draw_box_shadow(layer, &shadow_dsc, &rect_area);

    /* 2. 绘制圆角矩形主体 */
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.bg_color = lv_color_hex(0x2C2C2E);
    rect_dsc.radius = POPUP_RADIUS;
    rect_dsc.border_width = 0;
    lv_draw_rect(layer, &rect_dsc, &rect_area);

    /* 3. 绘制底部凸出箭头（三角形） */
    lv_coord_t arrow_center_x = area.x1 + w / 2;
    lv_coord_t arrow_top_y = area.y2 - POPUP_ARROW_H;

    lv_draw_triangle_dsc_t tri_dsc;
    lv_draw_triangle_dsc_init(&tri_dsc);
    tri_dsc.color = lv_color_hex(0x2C2C2E);
    tri_dsc.opa = LV_OPA_COVER;

    tri_dsc.p[0].x = arrow_center_x;
    tri_dsc.p[0].y = area.y2;
    tri_dsc.p[1].x = arrow_center_x - POPUP_ARROW_H;
    tri_dsc.p[1].y = arrow_top_y;
    tri_dsc.p[2].x = arrow_center_x + POPUP_ARROW_H;
    tri_dsc.p[2].y = arrow_top_y;

    lv_draw_triangle(layer, &tri_dsc);
}

static void airecord_popup_destroy(void)
{
    if (g_popup) {
        lv_obj_del(g_popup);
        g_popup = NULL;
    }
    g_longpress_target_row = NULL;
    g_longpress_target_bubble = NULL;

    /* 移除 scroll_cont 上的空白点击监听 */
    if (airecord_scroll_cont) {
        lv_obj_remove_event_cb(airecord_scroll_cont, airecord_popup_click_blank_cb);
    }
}

/**
 * 点击空白区域关闭弹窗
 * 判断逻辑：如果点击目标不是弹窗本身也不是弹窗的子对象，则关闭
 */
static void airecord_popup_click_blank_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED) return;
    if (g_popup == NULL) return;

    lv_obj_t *target = lv_event_get_target(e);

    /* 如果点击的是弹窗本身或弹窗的子对象，不处理 */
    lv_obj_t *parent = target;
    while (parent) {
        if (parent == g_popup) return;
        parent = lv_obj_get_parent(parent);
    }

    /* 点击的是弹窗外部区域，关闭弹窗 */
    airecord_popup_destroy();
}

/**
 * 删除按钮回调：直接删除该条消息
 */
static void airecord_popup_delete_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    lv_obj_t *target_row = g_longpress_target_row;
    airecord_popup_destroy();

    if (target_row) {
        /* 找到该行在数组中的索引 */
        for (int i = 0; i < airecord_item_count; i++) {
            if (airecord_items[i].row == target_row) {
                lv_obj_del(airecord_items[i].row);
                /* 移动后续元素 */
                for (int j = i; j < airecord_item_count - 1; j++) {
                    airecord_items[j] = airecord_items[j + 1];
                }
                airecord_item_count--;
                break;
            }
        }
    }

    /* 刷新布局以反映删除后的变化 */
    airecord_update_layout();
}

/**
 * 多选按钮回调：进入删除模式，显示勾选框
 */
static void airecord_popup_multi_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    airecord_popup_destroy();

    /* 进入删除模式但不自动勾选当前项 */
    airecord_enter_delete_mode_internal(NULL);

    /* 更换标题栏内容 */
    if (title_cont_one) lv_obj_add_flag(title_cont_one, LV_OBJ_FLAG_HIDDEN);
    if (title_cont_two) lv_obj_clear_flag(title_cont_two, LV_OBJ_FLAG_HIDDEN);
}

/**
 * 在气泡上方创建长按弹窗 188*107
 * 包含"删除"和"多选"两个选项，并列排列
 * 弹窗根据气泡的中间位置定位
 */
static void airecord_popup_create(lv_obj_t *anchor_bubble)
{
    if (airecord_scroll_cont == NULL) return;

    /* 弹窗创建在滚动容器内，随滚动移动 */
    g_popup = lv_obj_create(airecord_scroll_cont);
    lv_obj_remove_style_all(g_popup);
    lv_obj_set_size(g_popup, POPUP_W, POPUP_H + POPUP_ARROW_H);
    lv_obj_clear_flag(g_popup, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(g_popup, LV_OBJ_FLAG_IGNORE_LAYOUT);

    /* 自定义绘制：圆角矩形 + 底部箭头 */
    lv_obj_add_event_cb(g_popup, airecord_popup_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    /* ---- 两个按钮容器（并列，各占一半） ---- */
    lv_coord_t btn_w = POPUP_W / 2;

    /* "删除" 按钮（左侧） */
    lv_obj_t *del_btn = lv_obj_create(g_popup);
    lv_obj_remove_style_all(del_btn);
    lv_obj_set_size(del_btn, btn_w, POPUP_H);
    lv_obj_set_pos(del_btn, 0, 0);
    lv_obj_set_style_bg_opa(del_btn, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(del_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(del_btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *del_label = lv_label_create(del_btn);
    lv_label_set_text(del_label, "删除");
    lv_obj_set_style_text_color(del_label, lv_color_hex(0xFF5C5C), 0);
    lv_obj_set_style_text_font(del_label, fzlthr_30, 0);
    lv_obj_center(del_label);
    lv_obj_add_event_cb(del_btn, airecord_popup_delete_cb, LV_EVENT_CLICKED, NULL);

    /* 分隔线（垂直） */
    lv_obj_t *line = lv_obj_create(g_popup);
    lv_obj_remove_style_all(line);
    lv_obj_set_size(line, 1, POPUP_H - 20);
    lv_obj_set_pos(line, btn_w, 10);
    lv_obj_set_style_bg_color(line, lv_color_hex(0x404040), 0);
    lv_obj_set_style_bg_opa(line, LV_OPA_COVER, 0);
    lv_obj_clear_flag(line, LV_OBJ_FLAG_SCROLLABLE);

    /* "多选" 按钮（右侧） */
    lv_obj_t *multi_btn = lv_obj_create(g_popup);
    lv_obj_remove_style_all(multi_btn);
    lv_obj_set_size(multi_btn, btn_w, POPUP_H);
    lv_obj_set_pos(multi_btn, btn_w, 0);
    lv_obj_set_style_bg_opa(multi_btn, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(multi_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(multi_btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *multi_label = lv_label_create(multi_btn);
    lv_label_set_text(multi_label, "多选");
    lv_obj_set_style_text_color(multi_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(multi_label, fzlthr_30, 0);
    lv_obj_center(multi_label);
    lv_obj_add_event_cb(multi_btn, airecord_popup_multi_cb, LV_EVENT_CLICKED, NULL);

    /* ---- 根据气泡位置定位弹窗 ---- */
    lv_obj_update_layout(anchor_bubble);

    /* 获取气泡在 scroll_cont 内的坐标 */
    lv_obj_t *outer_row = lv_obj_get_parent(anchor_bubble);
    lv_coord_t row_y = lv_obj_get_y(outer_row);
    lv_coord_t bubble_x = lv_obj_get_x(anchor_bubble);
    lv_coord_t bubble_y = row_y + lv_obj_get_y(anchor_bubble);
    lv_coord_t bubble_w = lv_obj_get_width(anchor_bubble);

    /* 弹窗水平居中于气泡中间 */
    lv_coord_t popup_x = bubble_x + (bubble_w - POPUP_W) / 2;
    if (popup_x < 0) popup_x = 0;
    if (popup_x + POPUP_W > AIRECORD_SCROLL_W)
        popup_x = AIRECORD_SCROLL_W - POPUP_W;

    /* 弹窗在气泡上方 20px */
    lv_coord_t popup_y = bubble_y - POPUP_H - POPUP_ARROW_H - 20;
    if (popup_y < 0) popup_y = 5;

    lv_obj_set_pos(g_popup, popup_x, popup_y);
}

static void airecord_bubble_longpress_cb(lv_event_t *e)
{
    lv_obj_t *bubble = lv_event_get_target(e);
    lv_obj_t *outer_row = lv_obj_get_parent(bubble);

    /* 记录目标 */
    g_longpress_target_row = outer_row;
    g_longpress_target_bubble = bubble;

    /* 根据气泡位置弹出弹窗 */
    airecord_popup_create(bubble);

    /* 视角自动移到弹窗位置 */
    if (g_popup) {
        lv_obj_scroll_to_view(g_popup, LV_ANIM_ON);
    }

    /* 在 scroll_cont 上添加点击空白关闭弹窗的监听（弹窗创建在 scroll_cont 内，事件可正确冒泡） */
    lv_obj_add_event_cb(airecord_scroll_cont, airecord_popup_click_blank_cb, LV_EVENT_CLICKED, NULL);
}

static void airecord_exit_delete_mode_internal(void)
{
    g_delete_mode = false;

    /* 隐藏所有勾选框，清除选中状态 */
    for (int i = 0; i < airecord_item_count; i++) {
        if (airecord_items[i].checkbox) {
            lv_obj_add_flag(airecord_items[i].checkbox, LV_OBJ_FLAG_HIDDEN);
            airecord_items[i].checked = false;
            airecord_checkbox_set_checked(airecord_items[i].checkbox, false);
        }
    }

    airecord_update_layout();
}

static void airecord_checkbox_click_cb(lv_event_t *e)
{
    lv_obj_t *cb = lv_event_get_target(e);

    /* 找到对应的 item */
    for (int i = 0; i < airecord_item_count; i++) {
        if (airecord_items[i].checkbox == cb) {
            airecord_items[i].checked = !airecord_items[i].checked;
            airecord_checkbox_set_checked(cb, airecord_items[i].checked);
            break;
        }
    }
}

/**
 * 根据删除模式更新所有消息行的布局
 * 删除模式：消息右移100px，显示勾选框
 * 普通模式：消息正常位置
 */
static void airecord_update_layout(void)
{
    for (int i = 0; i < airecord_item_count; i++) {
        airecord_item_t *item = &airecord_items[i];
        if (item->row == NULL || item->bubble == NULL) continue;

        if (item->msg_data.is_question) {
            /* 问题靠右，贴着容器最右侧 */
            lv_obj_align(item->bubble, LV_ALIGN_TOP_RIGHT, 0, 0);
        } else {
            /* 智能体回复靠左，删除模式时距离勾选框 20px，否则贴左侧 */
            lv_coord_t shift = g_delete_mode ? (CHECKBOX_SIZE + 20 + 10) : 0;
            lv_obj_align(item->bubble, LV_ALIGN_TOP_LEFT, shift, 0);
        }

        /* 勾选框位置：始终在左侧 */
        if (item->checkbox) {
            lv_obj_align(item->checkbox, LV_ALIGN_LEFT_MID, 10, 0);
        }
    }
}

static void airecord_msg_row_click_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED && g_delete_mode) {
        /* 点击行容器，如果已选中则取消选中，否则选中 */
        lv_obj_t *row = lv_event_get_target(e);
        for (int i = 0; i < airecord_item_count; i++) {
            if (airecord_items[i].row == row) {
                airecord_items[i].checked = !airecord_items[i].checked;
                airecord_checkbox_set_checked(airecord_items[i].checkbox, airecord_items[i].checked);
                break;
            }
        }
    }
}

static void airecord_msg_add(airecord_msg_t *msg)
{
    if (airecord_scroll_cont == NULL) return;

    /* 检查是否需要插入时间分隔 */
    if (airecord_item_count > 0) {
        uint64_t prev_ts = airecord_items[airecord_item_count - 1].msg_data.timestamp;
        if (msg->timestamp > prev_ts &&
            (msg->timestamp - prev_ts) >= TIME_DIVIDER_THRESHOLD) {
            airecord_add_time_divider_internal(msg->timestamp);
        }
    }

    /* 扩展动态数组 */
    if (airecord_item_count >= airecord_item_capacity) {
        int new_cap = airecord_item_capacity == 0 ? 32 : airecord_item_capacity * 2;
        airecord_item_t *new_items = realloc(airecord_items, new_cap * sizeof(airecord_item_t));
        if (NULL == new_items) return;
        airecord_items = new_items;
        airecord_item_capacity = new_cap;
    }

    airecord_item_t *item = &airecord_items[airecord_item_count];
    memset(item, 0, sizeof(airecord_item_t));
    item->msg_data = *msg;
    item->checked = false;

    /* ---- 创建外层行容器（包含勾选框 + 气泡） ---- */
    lv_obj_t *outer_row = lv_obj_create(airecord_scroll_cont);
    lv_obj_remove_style_all(outer_row);
    lv_obj_clear_flag(outer_row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_width(outer_row, AIRECORD_SCROLL_W);
    lv_obj_set_style_bg_opa(outer_row, 0, 0);
    lv_obj_set_height(outer_row, LV_SIZE_CONTENT);
    lv_obj_add_flag(outer_row, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(outer_row, airecord_msg_row_click_cb, LV_EVENT_CLICKED, NULL);
    item->row = outer_row;

    /* ---- 创建勾选框 ---- */
    lv_obj_t *cb = airecord_checkbox_create(outer_row);
    lv_obj_align(cb, LV_ALIGN_LEFT_MID, 10, 0);
    item->checkbox = cb;

    /* ---- 创建气泡（直接挂在 outer_row 上） ---- */
    lv_obj_t *bubble = NULL;

    switch (msg->type) {
        case AIRECORD_MSG_TYPE_TEXT:
            bubble = airecord_text_bubble_create(outer_row, msg->text, msg->is_question);
            break;
        case AIRECORD_MSG_TYPE_VOICE:
            bubble = airecord_voice_bubble_create(outer_row, msg->voice_path, msg->voice_duration, msg->is_question);
            break;
        case AIRECORD_MSG_TYPE_IMAGE:
            bubble = airecord_image_bubble_create(outer_row, msg->image_path, msg->is_question);
            break;
    }

    if (bubble) {
        item->bubble = bubble;

        /* 添加长按事件 */
        lv_obj_add_flag(bubble, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(bubble, airecord_bubble_longpress_cb, LV_EVENT_LONG_PRESSED, NULL);

        lv_obj_update_layout(bubble);
        lv_coord_t h = lv_obj_get_height(bubble) + 16;
        lv_obj_set_height(outer_row, h);

        if (msg->is_question) {
            lv_obj_align(bubble, LV_ALIGN_TOP_RIGHT, 0, 0);
        } else {
            lv_obj_align(bubble, LV_ALIGN_TOP_LEFT, 0, 0);
        }
    }

    lv_obj_update_layout(airecord_scroll_cont);

    /* 滚动到底部 */
    lv_obj_scroll_to_view(outer_row, LV_ANIM_ON);

    airecord_item_count++;
}

static lv_obj_t *airecord_list_create(lv_obj_t *parent)
{
    airecord_parent = parent;

    /* 创建消息滚动容器 */
    airecord_scroll_cont = lv_obj_create(parent);
    lv_obj_set_size(airecord_scroll_cont, AIRECORD_SCROLL_W, AIRECORD_SCROLL_H);
    lv_obj_set_style_radius(airecord_scroll_cont, 0, 0);
    lv_obj_set_style_bg_opa(airecord_scroll_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(airecord_scroll_cont, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(airecord_scroll_cont, 0, 0);
    lv_obj_set_style_pad_all(airecord_scroll_cont, 0, 0);
    lv_obj_set_style_pad_row(airecord_scroll_cont, 8, 0);
    lv_obj_set_flex_flow(airecord_scroll_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(airecord_scroll_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(airecord_scroll_cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(airecord_scroll_cont, LV_ALIGN_TOP_MID, 0, 70);

    /* 添加mock数据示例 — 使用不同的时间戳以触发时间分隔 */
    uint64_t base_ts = 1770192000; // 2026-03-04 16:00:00 作为基准

    airecord_msg_t msgs[] = {
        {AIRECORD_MSG_TYPE_TEXT,   false, "你好！我是智能助手，有什么可以帮助你的吗？", NULL, 0, NULL, base_ts},
        {AIRECORD_MSG_TYPE_VOICE,  true,  NULL, "/home/voice/test.aac", 65, NULL, base_ts + 10},
        {AIRECORD_MSG_TYPE_TEXT,   false, "好的，我来帮你查找相关信息。", NULL, 0, NULL, base_ts + 20},
        {AIRECORD_MSG_TYPE_IMAGE,  true,  NULL, NULL, 0, "../lv_port_pc_vscode/assert/icon/cert_mask_bg.png", base_ts + 30},
        {AIRECORD_MSG_TYPE_TEXT,   false, "这是查找到的结果，请查看。如果还有其他问题，请随时告诉我。这是查找到的结果，请查看。如果还有其他问题，请随时告诉我.这是查找到的结果，请查看。如果还有其他问题，请随时告诉我.这是查找到的结果，请查看。如果还有其他问题，请随时告诉我", NULL, 0, NULL, base_ts + 400},
    };

    for (int i = 0; i < (int)(sizeof(msgs) / sizeof(msgs[0])); i++) {
        airecord_msg_add(&msgs[i]);
    }

    return airecord_scroll_cont;
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    // GUI_LOG(KEY_WARN,"page switch event:%d\n", page_event);
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
            LV_LOG_WARN("page switch event:%d invaild", page_event);
            break;
    }

    if (NULL == switch_page->new_page) {
        lv_free(switch_page);
        return;
    }

    lv_subject_set_pointer(&switch_subject, switch_page);
}
