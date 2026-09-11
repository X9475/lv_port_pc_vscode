#include "../lv_switch_interface.h"

lv_subject_t ptr_pre_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void border_btn_click_cb(lv_event_t *e);
static void rotate_btn_click_cb(lv_event_t *e);
static void printer_btn_click_cb(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t ptr_pre_page_info = {
    .page_id = PAGE_FUNCTIONAL_PTR_PREVIEW,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_printer_preview_get()
{
    return &ptr_pre_page_info;
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
    ptr_pre_page_info.page = screen;
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
    lv_subject_init_int(&ptr_pre_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&ptr_pre_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&ptr_pre_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 90, 80);
    lv_obj_set_style_radius(back_btn, 0, 0);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(back_btn, cont, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 15, 0);

    //标题
    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "打印预览");
    lv_obj_set_style_text_font(header, fzlthb_30, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(header, back_btn, LV_ALIGN_OUT_RIGHT_MID, -8, 0);

    //白色背景
    lv_obj_t *backdrop = lv_obj_create(cont);
    lv_obj_set_size(backdrop, 232, 312);
    lv_obj_add_style(backdrop, &screen_style, 0);
    lv_obj_set_style_bg_color(backdrop, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(backdrop, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(backdrop, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align(backdrop, LV_ALIGN_BOTTOM_MID, 0, -20);

    //边框显示按钮
    lv_obj_t *border_btn = lv_btn_create(cont);
    lv_obj_set_size(border_btn, 60, 60);
    lv_obj_set_style_radius(border_btn, 0, 0);
    lv_obj_set_style_shadow_width(border_btn, 0, 0);
    lv_obj_set_style_bg_opa(border_btn, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(border_btn, border_btn_click_cb, LV_EVENT_CLICKED, backdrop);
    lv_obj_align(border_btn, LV_ALIGN_TOP_RIGHT, -130, 10);

    lv_obj_t *border_icon = lv_img_create(border_btn);
    lv_obj_set_size(border_icon, 50, 50);
    lv_img_set_src(border_icon, "../lv_port_pc_vscode/assert/icon/printer_border.png");
    lv_img_set_zoom(border_icon, 128);
    lv_obj_align(border_icon, LV_ALIGN_CENTER, 0, 0);

    //旋转按钮
    lv_obj_t *rotate_btn = lv_btn_create(cont);
    lv_obj_set_size(rotate_btn, 60, 60);
    lv_obj_set_style_radius(rotate_btn, 0, 0);
    lv_obj_set_style_shadow_width(rotate_btn, 0, 0);
    lv_obj_set_style_bg_opa(rotate_btn, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(rotate_btn, rotate_btn_click_cb, LV_EVENT_CLICKED, backdrop);
    lv_obj_align(rotate_btn, LV_ALIGN_TOP_RIGHT, -60, 10);

    lv_obj_t *rotate_icon = lv_img_create(rotate_btn);
    lv_obj_set_size(rotate_icon, 50, 50);
    lv_img_set_src(rotate_icon, "../lv_port_pc_vscode/assert/icon/printer_rotate.png");
    lv_img_set_zoom(rotate_icon, 128);
    lv_obj_align(rotate_icon, LV_ALIGN_CENTER, 0, 0);

    //打印机按钮
    lv_obj_t *printer_btn = lv_btn_create(cont);
    lv_obj_set_size(printer_btn, 80, 80);
    lv_obj_set_style_radius(printer_btn, 40, 0);
    lv_obj_set_style_shadow_width(printer_btn, 0, 0);
    lv_obj_set_style_bg_opa(printer_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(printer_btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_add_event_cb(printer_btn, printer_btn_click_cb, LV_EVENT_CLICKED, backdrop);
    lv_obj_align(printer_btn, LV_ALIGN_BOTTOM_RIGHT, -25, -25);

    lv_obj_t *printer_icon = lv_img_create(printer_btn);
    lv_obj_set_size(printer_icon, 40, 40);
    lv_img_set_src(printer_icon, "../lv_port_pc_vscode/assert/icon/printer_icon.png");
    lv_img_set_zoom(printer_icon, 128);
    lv_obj_align(printer_icon, LV_ALIGN_CENTER, 0, 0);
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&ptr_pre_subject, PAGE_SWITCH_BACK);
}

static void border_btn_click_cb(lv_event_t *e)
{
    lv_obj_t *backdrop = lv_event_get_user_data(e);
    lv_obj_t *back_btn = lv_event_get_target(e);

    if (NULL == backdrop || !lv_obj_is_valid(backdrop)) return;

    if (lv_obj_has_flag(backdrop, LV_OBJ_FLAG_HIDDEN))
    {
        lv_obj_clear_flag(backdrop, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_background(backdrop);//移置底层
        lv_img_set_src(lv_obj_get_child(back_btn, 0), "../lv_port_pc_vscode/assert/icon/printer_border.png");
    }
    else
    {
        lv_obj_add_flag(backdrop, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(lv_obj_get_child(back_btn, 0), "../lv_port_pc_vscode/assert/icon/printer_no_border.png");
    }
}

static void rotate_btn_click_cb(lv_event_t *e)
{
    //预览效果的图片
}

static void printer_btn_click_cb(lv_event_t *e)
{
    //上传图片
    lv_subject_set_int(&anormal_subject, PAGE_ABNORMAL_PRINTER_UPLOAD);
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
    switch_page->old_page = &ptr_pre_page_info;

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
