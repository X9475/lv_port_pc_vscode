#include "../lv_switch_interface.h"

lv_subject_t device_volume_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t main_style;
static lv_style_t knob_style;
static lv_style_t indicator_style;
static lv_style_t number_style;
static lv_subject_t volume_subject;

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

static lv_page_info_t device_volume_page_info = {
    .page_id = PAGE_FUNCTIONAL_DEVICE_VOLUME,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_device_volume_info_get()
{
    return &device_volume_page_info;
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
    device_volume_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&main_style);
    lv_style_reset(&knob_style);
    lv_style_reset(&indicator_style);
    lv_style_reset(&number_style);
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

    //main_style
    lv_style_init(&main_style);
    lv_style_set_radius(&main_style, 3);
    lv_style_set_bg_color(&main_style, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&main_style, LV_OPA_20);

    //knob_style
    lv_style_init(&knob_style);
    lv_style_set_radius(&knob_style, 2);
    lv_style_set_bg_color(&knob_style, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&knob_style, LV_OPA_COVER);
    lv_style_set_pad_all(&knob_style, 0);
    lv_style_set_pad_top(&knob_style, 6);
    lv_style_set_pad_bottom(&knob_style, 6);

    //indicator_style
    lv_style_init(&indicator_style);
    lv_style_set_bg_color(&indicator_style, lv_color_hex(0xAFF99C));
    lv_style_set_bg_opa(&indicator_style, LV_OPA_COVER);

    //number_style
    lv_style_init(&number_style);
    lv_style_set_text_font(&number_style, oswaldr_20);
    lv_style_set_text_color(&number_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_opa(&number_style, LV_OPA_COVER);
    lv_style_set_text_align(&number_style, LV_TEXT_ALIGN_CENTER);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&volume_subject, 50);
    lv_subject_init_int(&device_volume_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&device_volume_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&volume_subject);
    lv_subject_deinit(&device_volume_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 70, 70);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(back_btn, cont, LV_ALIGN_TOP_LEFT, 20, 10);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 3, 0);

    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "设备音量");
    lv_obj_set_style_text_font(header, fzlthb_30, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(header, back_btn, LV_ALIGN_OUT_RIGHT_MID, -8, 0);

    lv_obj_t *contain = lv_obj_create(cont);
    lv_obj_remove_style_all(contain);
    lv_obj_set_size(contain, 422, 131);
    lv_obj_set_style_radius(contain, 14, 0);
    lv_obj_set_style_bg_opa(contain, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(contain, lv_color_hex(0x1F1F1F), 0);
    lv_obj_clear_flag(contain, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(contain, LV_ALIGN_TOP_MID, 0, 140);

    lv_obj_t *label1 = lv_label_create(contain);
    lv_label_set_text(label1, "设备音量");
    lv_obj_set_style_text_font(label1, fzlthr_20, 0);
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 20, 26);

    lv_obj_t *slider = lv_slider_create(contain);
    lv_obj_remove_style_all(slider);
    lv_obj_set_size(slider, 318, 8);
    lv_obj_add_style(slider, &main_style, LV_PART_MAIN);
    lv_obj_add_style(slider, &knob_style, LV_PART_KNOB);
    lv_obj_add_style(slider, &indicator_style, LV_PART_INDICATOR);
    lv_obj_align(slider, LV_ALIGN_TOP_LEFT, 42, 88);
    lv_slider_bind_value(slider, &volume_subject);

    lv_obj_t *volume = lv_label_create(contain);
    lv_obj_add_style(volume, &number_style, 0);
    lv_obj_align(volume, LV_ALIGN_TOP_LEFT, 381, 27);
    lv_label_bind_text(volume, &volume_subject, "%d");

    lv_obj_t *lvolume = lv_label_create(contain);
    lv_obj_add_style(lvolume, &number_style, 0);
    lv_label_set_text(lvolume, "0");
    lv_obj_align(lvolume, LV_ALIGN_BOTTOM_LEFT, 19, -28);

    lv_obj_t *rvolume = lv_label_create(contain);
    lv_obj_add_style(rvolume, &number_style, 0);
    lv_label_set_text(rvolume, "100");
    lv_obj_align(rvolume, LV_ALIGN_BOTTOM_RIGHT, -21, -28);

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&device_volume_subject, PAGE_SWITCH_BACK);
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
    switch_page->old_page = &device_volume_page_info;

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