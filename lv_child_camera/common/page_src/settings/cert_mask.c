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
    .page_id = PAGE_FUNCTIONAL_FACTORY_RESTORE,
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
    lv_style_set_bg_image_opa(&screen_style, LV_OPA_COVER);
    lv_style_set_bg_image_src(&screen_style, "../lv_port_pc_vscode/assert/icon/background.png");
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
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *description = lv_obj_create(cont);
    lv_obj_set_size(description, 450, 180);
    lv_obj_set_style_pad_all(description, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(description, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(description, 18, LV_PART_MAIN);
    lv_obj_set_style_bg_color(description, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(description, LV_OPA_40, 0);
    lv_obj_clear_flag(description, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(description, LV_ALIGN_CENTER, 0, 0);

    const char *text = "CMIIT ID：XXXXXXXXXXXX\n版本：V100-1M88SF4G-Z\nTD-LTE无线数据终端\n中国制造";
    lv_obj_t *label = lv_label_create(description);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_22, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 30, 35);

    //环保标志
    lv_obj_t *image = lv_img_create(description);
    lv_obj_set_size(image, 100, 100);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/environment_protect_symbol_10.png");
    lv_img_set_zoom(image, 200);
    lv_obj_align(image, LV_ALIGN_BOTTOM_RIGHT, -20, -20);

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