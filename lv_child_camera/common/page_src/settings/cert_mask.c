#include "../lv_switch_interface.h"

lv_subject_t cert_mask_subject;
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

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_CONFIRM,
    PAGE_SWITCH_BACK
};

static lv_page_info_t cert_mask_page_info = {
    .page_id = PAGE_FUNCTIONAL_CERT_MASK,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_cert_mask_info_get()
{
    return &cert_mask_page_info;
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
    lv_page_type_set(TYPE_MENU_SETTING_THREE);

    cert_mask_page_info.page = screen;
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
    lv_subject_init_int(&cert_mask_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&cert_mask_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&cert_mask_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //背景
    lv_obj_t *cart_bg = lv_img_create(cont);
    lv_img_set_src(cart_bg, "../lv_port_pc_vscode/assert/icon/cert_mask_bg.png");
    lv_obj_align(cart_bg, LV_ALIGN_CENTER, 0, 0);

    //返回按钮
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 70, 70);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(back_btn, cont, LV_ALIGN_TOP_RIGHT, -35, 15);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 40, 40);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_cancel_button.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *cerk = lv_label_create(cont);
    lv_obj_set_size(cerk, 157, 42);
    lv_label_set_text(cerk, "认证标志");
    lv_obj_set_style_text_opa(cerk, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(cerk, fzlthr_32, 0);
    lv_obj_set_style_text_color(cerk, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(cerk, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_letter_space(cerk, 8, 0);
    lv_obj_align(cerk, LV_ALIGN_TOP_LEFT, 46, 154);

    lv_obj_t *descrip = lv_label_create(cont);
    lv_obj_set_size(descrip, 410, 140);
    lv_label_set_text(descrip, "CMIIT ID：XXXXXXXXXXXX\n版本：V100-1M88SF4G-Z\nTD-LTE无线数据终端\n中国制造");
    lv_obj_set_style_text_opa(descrip, LV_OPA_60, 0);
    lv_obj_set_style_text_font(descrip, fzlthr_26, 0);
    lv_obj_set_style_text_color(descrip, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(descrip, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_line_space(descrip, 5, 0);
    lv_obj_align(descrip, LV_ALIGN_TOP_LEFT, 46, 212);

    //环保标志
    lv_obj_t *image = lv_img_create(cont);
    lv_obj_set_size(image, 90, 90);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/environment_protect_symbol_10.png");
    lv_obj_align(image, LV_ALIGN_TOP_LEFT, 46, 40);

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&cert_mask_subject, PAGE_SWITCH_BACK);
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
    switch_page->old_page = &cert_mask_page_info;

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