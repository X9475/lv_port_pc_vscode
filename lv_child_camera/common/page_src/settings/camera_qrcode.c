#include "../lv_switch_interface.h"

lv_subject_t settingQr_subject;
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
    PAGE_SWITCH_BACK
};

static lv_page_info_t settingQr_page_info = {
    .page_id = PAGE_FUNCTIONAL_SETTING_QRCODE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_settingQr_info_get()
{
    return &settingQr_page_info;
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

    settingQr_page_info.page = screen;
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
    lv_subject_init_int(&settingQr_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&settingQr_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&settingQr_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t *bg_img = lv_img_create(cont);
    lv_obj_set_size(bg_img, lv_pct(100), lv_pct(100));
    lv_img_set_src(bg_img, "../lv_port_pc_vscode/assert/icon/videocall_add_contact.png");
    lv_img_set_zoom(bg_img, 128);
    lv_obj_align(bg_img, LV_ALIGN_CENTER, 0, 0);

    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    //生成二维码
    lv_obj_t *qr = lv_qrcode_create(cont);
    lv_qrcode_set_size(qr, 220);
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
    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&settingQr_subject, PAGE_SWITCH_BACK);
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
    switch_page->old_page = &settingQr_page_info;

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