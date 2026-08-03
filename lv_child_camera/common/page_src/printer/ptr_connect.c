#include "../lv_switch_interface.h"

#define BT_SEARCH_TIME    6 //搜索倒计时时长
#define BT_REFRESH_TIME   10 //刷新倒计时时长
#define BT_CONNECT_TIME   10 //连接超时时时长
    
typedef void (* handle_cb)(void);
typedef struct {
    lv_obj_t *msgbox;
    handle_cb event_cb;
} msg_box_info_t, *msg_box_info_pt;
static msg_box_info_t msg_box_info[2] = {0};

lv_subject_t ptr_conn_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_obj_t *major_label = NULL;
static lv_obj_t *minor_label = NULL;
static lv_obj_t *cancel = NULL;
static lv_obj_t *refresh = NULL;
static lv_style_t screen_style;

static int search_sec = BT_SEARCH_TIME;
static int refresh_sec = BT_REFRESH_TIME;
static int connct_sec = BT_CONNECT_TIME;
static int search_num = 3;//发现设备的数量

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void countdown_timer_cb(lv_timer_t *timer);
static void page_back_event_cb(lv_event_t *e);

//展示搜索到的设备列表
lv_obj_t *create_device_discovery_page(lv_obj_t *parent);
static void confirm_connect_with_printer(const char *device_name);
static void search_printer_failed();
static void printer_is_connecting();
static void printer_connect_failed();

static enum
{
    BT_SEARCH_FAILED = 0,   //搜索失败
    BT_CONNECT_FAILED,      //连接失败
    BT_CONNECT_CONFIRM,     //建立连接确认
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t ptr_conn_page_info = {
    .page_id = PAGE_FUNCTIONAL_PTR_CONNECT,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_printer_conn_get()
{
    return &ptr_conn_page_info;
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
    ptr_conn_page_info.page = screen;
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
    lv_subject_init_int(&ptr_conn_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&ptr_conn_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&ptr_conn_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 70, 70);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, back_btn);
    lv_obj_align_to(back_btn, cont, LV_ALIGN_TOP_LEFT, 20, 10);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 3, 0);

    major_label = lv_label_create(cont);
    lv_obj_set_style_text_opa(major_label, LV_OPA_90, 0);
    lv_obj_set_style_text_font(major_label, fzlthr_26, 0);
    lv_obj_set_style_text_color(major_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(major_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(major_label, LV_ALIGN_TOP_MID, 0, 263);

    minor_label = lv_label_create(cont);
    lv_label_set_text(minor_label, "请开启打印机，并靠近本设备");
    lv_obj_set_style_text_opa(minor_label, LV_OPA_60, 0);
    lv_obj_set_style_text_font(minor_label, fzlthr_24, 0);
    lv_obj_set_style_text_color(minor_label, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(minor_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(minor_label, major_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_add_flag(minor_label, LV_OBJ_FLAG_HIDDEN);

    // search_printer_failed();
    // printer_connect_failed();
    // confirm_connect_with_printer("CPP-25680");
    // printer_is_connecting();

    //通知重新扫描，每1.5秒扫描一次，共10次
    search_sec = BT_SEARCH_TIME;
    lv_timer_t *count_timer = lv_timer_create(countdown_timer_cb, 1000, NULL);
    lv_timer_ready(count_timer);
    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&ptr_conn_subject, PAGE_SWITCH_BACK);
}

static void countdown_timer_cb(lv_timer_t *timer)
{
    if (NULL == major_label || !lv_obj_is_valid(major_label)) return;
    if (NULL == minor_label || !lv_obj_is_valid(minor_label)) return;

    lv_label_set_text_fmt(major_label, "正在搜索附近的打印机（%ds）...", search_sec--);

    if (search_sec == (BT_SEARCH_TIME - 5))
    {
        lv_obj_clear_flag(minor_label, LV_OBJ_FLAG_HIDDEN);
    }
    else if (search_sec < 0)
    {
        search_sec = BT_SEARCH_TIME;
        lv_timer_pause(timer);
        //检查是否有设备被发现
        // search_printer_failed();

        if (NULL != lv_obj_get_parent(major_label))
        {
            // create_device_discovery_page(lv_obj_get_parent(major_label));
        }
    }
}

static void scan_device_item_click_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    int index = (int)(intptr_t)lv_obj_get_user_data(obj);

    printf("index = %d\n", index);

    //提示是否建立连接

}

static void scan_device_item_create(lv_obj_t *cont, int32_t index)
{
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 462, 110);
    lv_obj_set_style_radius(btn, 51, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x352E2A), 0);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_user_data(btn, (void *)(intptr_t)index);
    lv_obj_add_event_cb(btn, scan_device_item_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "设备名称");
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, fzlthr_30, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 115, 0);

    lv_obj_t *save = lv_label_create(btn);
    lv_label_set_text(save, "已保存设备");
    lv_obj_set_size(save, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_opa(save, LV_OPA_60, 0);
    lv_obj_set_style_text_color(save, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_font(save, fzlthr_16, 0);
    lv_obj_set_style_text_align(save, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(save, LV_ALIGN_RIGHT_MID, -35, 0);

    lv_obj_t *image = lv_img_create(btn);
    lv_obj_set_size(image, 80, 80);
    lv_img_set_zoom(image, 128);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/printer.png");
    lv_obj_align(image, LV_ALIGN_LEFT_MID, 15, 0);
}

static void refresh_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *label = lv_timer_get_user_data(timer);

    if (NULL == label || !lv_obj_is_valid(label)) return;
    if (NULL == cancel || !lv_obj_is_valid(cancel)) return;
    if (NULL == refresh || !lv_obj_is_valid(refresh)) return;

    lv_label_set_text_fmt(label, "正在持续发现附近设备（%ds）...", refresh_sec--);

    //刷新搜索10秒结束
    if (refresh_sec < 0)
    {
        refresh_sec = BT_REFRESH_TIME;
        lv_timer_del(timer);
        lv_obj_add_flag(cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(refresh, LV_OBJ_FLAG_HIDDEN);
        lv_obj_del(label);
    }
}

static void cancel_btn_click_cb(lv_event_t *e)
{
    lv_obj_t *button = lv_event_get_user_data(e);

    if (NULL == cancel || !lv_obj_is_valid(cancel)) return;
    if (NULL == refresh || !lv_obj_is_valid(refresh)) return;

    if (!lv_obj_has_flag(cancel, LV_OBJ_FLAG_HIDDEN))
    {
        lv_subject_set_int(&ptr_conn_subject, PAGE_SWITCH_BACK);
    }
    else
    {
        lv_obj_clear_flag(cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(refresh, LV_OBJ_FLAG_HIDDEN);
        
        lv_obj_t *label = lv_label_create(lv_obj_get_parent(button));
        lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_text_opa(label, LV_OPA_60, 0);
        lv_obj_set_style_text_font(label, fzlthr_22, 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0xEBEBF5), 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 35, 63);
        
        //通知重新扫描，每1.5秒扫描一次，共7次
        refresh_sec = BT_REFRESH_TIME;
        lv_timer_t *refresh_timer = lv_timer_create(refresh_timer_cb, 1000, label);
        lv_timer_ready(refresh_timer);
    }
}

lv_obj_t *create_device_discovery_page(lv_obj_t *parent)
{
    /* 创建主页面容器 */
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_add_style(cont, &screen_style, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_move_foreground(cont);//移动到最上层
    lv_obj_center(cont);

    int number = 3;
    lv_obj_t *header = lv_label_create(cont);
    lv_obj_set_size(header, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(header, "所有设备（%d）", number);
    lv_obj_set_style_text_font(header, fzlthr_28, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(header, LV_ALIGN_TOP_LEFT, 35, 21);

    lv_obj_t *button = lv_btn_create(cont);
    lv_obj_set_size(button, 70, 70);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(button, cancel_btn_click_cb, LV_EVENT_CLICKED, button);
    lv_obj_align_to(button, cont, LV_ALIGN_TOP_RIGHT, -35, 15);

    //取消叉号
    cancel = lv_img_create(button);
    lv_obj_set_size(cancel, 40, 40);
    lv_img_set_src(cancel, "../lv_port_pc_vscode/assert/icon/common_icon_cancel_button.png");
    lv_obj_align(cancel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(cancel, LV_OBJ_FLAG_HIDDEN);

    //刷新按钮(共用返回触发)
    refresh = lv_img_create(button);
    lv_obj_set_size(refresh, 40, 40);
    lv_img_set_src(refresh, "../lv_port_pc_vscode/assert/icon/refresh.png");
    lv_img_set_zoom(refresh, 128);
    lv_obj_align(refresh, LV_ALIGN_CENTER, 0, 0);

    //创建已发现设备列表
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 310);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_row(cont_col, 15, 0);
    lv_obj_align_to(cont_col, cont, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_flex_align(cont_col, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (uint8_t i = 0; i < search_num; i++)
    {
        scan_device_item_create(cont_col, i);
    }
}

static void msgbox_button_click_event(lv_event_t *e)
{
    lv_obj_t *msg_obj = lv_event_get_user_data(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (NULL == msg_obj || !lv_obj_is_valid(msg_obj)) return;

    int index = (int)(intptr_t)lv_obj_get_user_data(msg_obj);
    switch (index)
    {
        case BT_SEARCH_FAILED://重试
            printf("retry\n");
            break;
        case BT_CONNECT_FAILED://重试
            printf("retry\n");
            break;
        case BT_CONNECT_CONFIRM://连接确认
            if (lv_obj_get_child(msg_obj, 0) == obj)
            {//取消
                printf("cancel\n");
            }
            else if (lv_obj_get_child(msg_obj, 1) == obj)
            {//确定
                printf("confirm\n");
            }
            break;
        default:
            break;
    }

    lv_obj_del(msg_obj);
}

//未搜索到打印机
static void search_printer_failed()
{
    lv_obj_t *msg_obj = lv_obj_create(screen);
    lv_obj_set_size(msg_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(msg_obj, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(msg_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(msg_obj, 0, 0);
    lv_obj_set_style_border_width(msg_obj, 0, 0);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_move_foreground(msg_obj);
    lv_obj_center(msg_obj);

    int32_t index = BT_SEARCH_FAILED;
    lv_obj_set_user_data(msg_obj, (void *)(intptr_t)index);

    lv_obj_t *msg_img = lv_img_create(msg_obj);
    lv_obj_set_size(msg_img, 120, 120);
    lv_img_set_src(msg_img, "../lv_port_pc_vscode/assert/icon/fail_2x.png");
    lv_img_set_zoom(msg_img, 100);
    lv_obj_align(msg_img, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *msg_label1 = lv_label_create(msg_obj);
    lv_label_set_text(msg_label1, "未搜索到打印机");
    lv_obj_set_style_text_opa(msg_label1, LV_OPA_90, 0);
    lv_obj_set_style_text_font(msg_label1, fzlthr_26, 0);
    lv_obj_set_style_text_color(msg_label1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(msg_label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(msg_label1, LV_ALIGN_TOP_MID, 0, 147);

    lv_obj_t *msg_label2 = lv_label_create(msg_obj);
    lv_label_set_text(msg_label2, "1.请确保打印机有电并已开机\n2.请确保打印机在TK1五米之内\n3.确保没有其他手机正在连接打印机");
    lv_obj_set_style_text_opa(msg_label2, LV_OPA_60, 0);
    lv_obj_set_style_text_font(msg_label2, fzlthr_24, 0);
    lv_obj_set_style_text_color(msg_label2, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(msg_label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(msg_label2, LV_ALIGN_TOP_MID, 0, 192);

    lv_obj_t *msg_btn = lv_obj_create(msg_obj);
    lv_obj_set_size(msg_btn, 148, 70);
    lv_obj_set_style_radius(msg_btn, 51, 0);
    lv_obj_set_style_bg_opa(msg_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(msg_btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_border_width(msg_btn, 0, 0);
    lv_obj_align(msg_btn, LV_ALIGN_TOP_MID, 0, 320);
    lv_obj_clear_flag(msg_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(msg_btn, msgbox_button_click_event, LV_EVENT_CLICKED, msg_obj);

    lv_obj_t *btn_label = lv_label_create(msg_btn);
    lv_label_set_text(btn_label, "重试");
    lv_obj_set_style_text_opa(btn_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(btn_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(btn_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_align(btn_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(btn_label, LV_ALIGN_CENTER, 0, 0);
    return;
}

//确定是否建立连接
static void confirm_connect_with_printer(const char *device_name)
{
    lv_obj_t *msg_obj = lv_obj_create(screen);
    lv_obj_set_size(msg_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(msg_obj, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(msg_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(msg_obj, 0, 0);
    lv_obj_set_style_border_width(msg_obj, 0, 0);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_move_foreground(msg_obj);
    lv_obj_center(msg_obj);

    int32_t index = BT_CONNECT_CONFIRM;
    lv_obj_set_user_data(msg_obj, (void *)(intptr_t)index);

    lv_obj_t *cancel_btn = lv_obj_create(msg_obj);
    lv_obj_set_size(cancel_btn, 148, 70);
    lv_obj_set_style_radius(cancel_btn, 51, 0);
    lv_obj_set_style_bg_opa(cancel_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cancel_btn, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_border_width(cancel_btn, 0, 0);
    lv_obj_clear_flag(cancel_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cancel_btn, LV_ALIGN_TOP_LEFT, 78, 310);
    lv_obj_add_event_cb(cancel_btn, msgbox_button_click_event, LV_EVENT_CLICKED, msg_obj);

    lv_obj_t *cancel_label = lv_label_create(cancel_btn);
    lv_label_set_text(cancel_label, "取消");
    lv_obj_set_style_text_opa(cancel_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(cancel_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(cancel_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(cancel_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(cancel_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *confirm_btn = lv_obj_create(msg_obj);
    lv_obj_set_size(confirm_btn, 148, 70);
    lv_obj_set_style_radius(confirm_btn, 51, 0);
    lv_obj_set_style_opa(confirm_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(confirm_btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_border_width(confirm_btn, 0, 0);
    lv_obj_clear_flag(confirm_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(confirm_btn, cancel_btn, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
    lv_obj_add_event_cb(confirm_btn, msgbox_button_click_event, LV_EVENT_CLICKED, msg_obj);

    lv_obj_t *confirm_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_label, "确定");
    lv_obj_set_style_text_opa(confirm_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(confirm_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(confirm_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_align(confirm_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(confirm_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *msg_img = lv_img_create(msg_obj);
    lv_obj_set_size(msg_img, 300, 173);
    lv_img_set_src(msg_img, "../lv_port_pc_vscode/assert/icon/connect_printer.png");
    lv_img_set_zoom(msg_img, 128);
    lv_obj_align(msg_img, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *msg_label1 = lv_label_create(msg_obj);
    lv_label_set_text(msg_label1, device_name);
    lv_obj_set_style_text_opa(msg_label1, LV_OPA_90, 0);
    lv_obj_set_style_text_font(msg_label1, fzlthr_26, 0);
    lv_obj_set_style_text_color(msg_label1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(msg_label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(msg_label1, LV_ALIGN_TOP_MID, 0, 197);

    lv_obj_t *msg_label2 = lv_label_create(msg_obj);
    lv_label_set_text(msg_label2, "确认要与该设备建立连接吗？");
    lv_obj_set_style_text_opa(msg_label2, LV_OPA_60, 0);
    lv_obj_set_style_text_font(msg_label2, fzlthr_24, 0);
    lv_obj_set_style_text_color(msg_label2, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(msg_label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(msg_label2, LV_ALIGN_TOP_MID, 0, 241);
}

static void connecting_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *msg_obj = lv_timer_get_user_data(timer);
    lv_obj_t *msg_label = lv_obj_get_child(msg_obj, 0);

    if (NULL == msg_label || !lv_obj_is_valid(msg_label)) return;
    lv_label_set_text_fmt(msg_label, "连接中（%ds）...", connct_sec--);

    if (connct_sec < 0)
    {
        connct_sec = BT_CONNECT_TIME;
        lv_timer_pause(timer);
        lv_obj_del(msg_obj);
    }
}

//建立连接中
static void printer_is_connecting()
{
    lv_obj_t *msg_obj = lv_obj_create(screen);
    lv_obj_set_size(msg_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(msg_obj, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(msg_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(msg_obj, 0, 0);
    lv_obj_set_style_border_width(msg_obj, 0, 0);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_move_foreground(msg_obj);
    lv_obj_center(msg_obj);

    lv_obj_t *msg_label = lv_label_create(msg_obj);
    lv_obj_set_size(msg_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_opa(msg_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(msg_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(msg_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(msg_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(msg_label, LV_ALIGN_TOP_MID, 0, 280);

    lv_obj_t *msg_img = lv_img_create(msg_obj);
    lv_obj_set_size(msg_img, 310, 190);
    lv_img_set_src(msg_img, "../lv_port_pc_vscode/assert/icon/printer_connecting.png");
    lv_obj_align(msg_img, LV_ALIGN_TOP_MID, 0, 60);

    connct_sec = BT_CONNECT_TIME;
    lv_timer_t *conn_timer = lv_timer_create(connecting_timer_cb, 1000, msg_obj);
    lv_timer_ready(conn_timer);
}

//连接失败
static void printer_connect_failed()
{
    lv_obj_t *msg_obj = lv_obj_create(screen);
    lv_obj_set_size(msg_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(msg_obj, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(msg_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(msg_obj, 0, 0);
    lv_obj_set_style_border_width(msg_obj, 0, 0);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(msg_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_move_foreground(msg_obj);
    lv_obj_center(msg_obj);

    int32_t index = BT_CONNECT_FAILED;
    lv_obj_set_user_data(msg_obj, (void *)(intptr_t)index);

    lv_obj_t *msg_img = lv_img_create(msg_obj);
    lv_obj_set_size(msg_img, 200, 200);
    lv_img_set_src(msg_img, "../lv_port_pc_vscode/assert/icon/fail_2x.png");
    lv_img_set_zoom(msg_img, 128);
    lv_obj_align(msg_img, LV_ALIGN_TOP_MID, 0, 45);

    lv_obj_t *msg_label = lv_label_create(msg_obj);
    lv_label_set_text(msg_label, "连接失败");
    lv_obj_set_style_text_opa(msg_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(msg_label, fzlthr_26, 0);
    lv_obj_set_style_text_color(msg_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(msg_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(msg_label, LV_ALIGN_TOP_MID, 0, 257);

    lv_obj_t *msg_btn = lv_obj_create(msg_obj);
    lv_obj_set_size(msg_btn, 148, 70);
    lv_obj_set_style_radius(msg_btn, 51, 0);
    lv_obj_set_style_bg_opa(msg_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(msg_btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_border_width(msg_btn, 0, 0);
    lv_obj_align(msg_btn, LV_ALIGN_TOP_MID, 0, 320);
    lv_obj_clear_flag(msg_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(msg_btn, msgbox_button_click_event, LV_EVENT_CLICKED, msg_obj);

    lv_obj_t *btn_label = lv_label_create(msg_btn);
    lv_label_set_text(btn_label, "重试");
    lv_obj_set_style_text_opa(btn_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(btn_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(btn_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_align(btn_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(btn_label, LV_ALIGN_CENTER, 0, 0);
    return;
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
    switch_page->old_page = &ptr_conn_page_info;

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
