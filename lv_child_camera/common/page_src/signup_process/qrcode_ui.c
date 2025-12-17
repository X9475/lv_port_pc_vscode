#include "../lv_switch_interface.h"

lv_subject_t qrcode_subject;
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
static void lv_event_handler_code(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SUCCESS,
    PAGE_SWITCH_FAILED,
    PAGE_SWITCH_BACK
};

static lv_page_info_t qrcode_page_info = {
    .page_id = PAGE_FUNCTIONAL_QRCODE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_qrcode_info_get()
{
    return &qrcode_page_info;
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
    lv_dev_stage_set(LV_PAGE_STAGE_ADDING);

    qrcode_page_info.page = screen;
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
    lv_subject_init_int(&qrcode_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&qrcode_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&qrcode_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //识别框
    lv_obj_t *scanning_box = lv_img_create(cont);
    lv_img_set_src(scanning_box, "../lv_port_pc_vscode/assert/icon/scanning_box_2x.png");
    lv_img_set_zoom(scanning_box, 128);
    lv_obj_align(scanning_box, LV_ALIGN_CENTER, 0, -30);

    //生成二维码
    lv_obj_t *qr = lv_qrcode_create(cont);
    lv_qrcode_set_size(qr, 169);
    lv_qrcode_set_light_color(qr, lv_color_hex(0xFFFFFF));
    lv_qrcode_set_dark_color(qr, lv_color_hex(0x000000));

    const char *data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_align(qr, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_border_color(qr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(qr, 2, 0);
    lv_obj_set_style_radius(qr, 15, 0);

    //文字提示
    lv_obj_t *tip_label = lv_label_create(cont);
    lv_label_set_text(tip_label, "请使用萤石云视频APP扫描二维码");
    lv_obj_set_style_text_opa(tip_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip_label, fzlthr_26, 0);
    lv_obj_set_style_text_color(tip_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip_label, LV_ALIGN_TOP_MID, 0, 320);

    //跳过
    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 80, 30);
    lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, -30, 20);
    lv_obj_set_style_radius(btn, 10, 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_add_event_cb(btn, lv_event_handler_code, LV_EVENT_CLICKED, NULL);

    lv_obj_t *tip1_label = lv_label_create(btn);
    lv_label_set_text(tip1_label, "跳过");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, fzlthr_22, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_CENTER, 0, 0);

    return;
}

static void lv_event_handler_code(lv_event_t *e)
{
    lv_subject_set_int(&qrcode_subject, PAGE_SWITCH_SUCCESS);
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
    switch_page->old_page = &qrcode_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_SUCCESS:
            switch_page->new_page = lv_page_signup_success_info_get();
            break;
        case PAGE_SWITCH_FAILED:
            switch_page->new_page = lv_page_signup_failed_info_get();
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