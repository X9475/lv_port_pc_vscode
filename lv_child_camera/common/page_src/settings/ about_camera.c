#include "../lv_switch_interface.h"

lv_subject_t about_camera_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t label1_style;
static lv_style_t label2_style;
static lv_style_t label3_style;
static lv_style_t style_mask;

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

static lv_page_info_t about_camera_page_info = {
    .page_id = PAGE_FUNCTIONAL_ABOUT_CAMERA,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_about_camera_info_get()
{
    return &about_camera_page_info;
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
    about_camera_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&label1_style);
    lv_style_reset(&label2_style);
    lv_style_reset(&label3_style);
    lv_style_reset(&style_mask);
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

    //label1_style
    lv_style_init(&label1_style);
    lv_style_set_text_opa(&label1_style, LV_OPA_COVER);
    lv_style_set_text_color(&label1_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&label1_style, fzlthb_50);
    lv_style_set_text_align(&label1_style, LV_TEXT_ALIGN_CENTER);

    //label2_style
    lv_style_init(&label2_style);
    lv_style_set_text_opa(&label2_style, LV_OPA_COVER);
    lv_style_set_text_color(&label2_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&label2_style, fzlthr_26);
    lv_style_set_text_align(&label2_style, LV_TEXT_ALIGN_LEFT);

    //label3_style
    lv_style_init(&label3_style);
    lv_style_set_text_opa(&label3_style, LV_OPA_40);
    lv_style_set_text_color(&label3_style, lv_color_hex(0xEBEBF5));
    lv_style_set_text_font(&label3_style, oswaldr_20);
    lv_style_set_text_align(&label3_style, LV_TEXT_ALIGN_LEFT);

    //图层蒙板
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_HOR;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[0].opa = LV_OPA_TRANSP;
    grad.stops[1].color = lv_color_hex(0x000000);
    grad.stops[1].opa = LV_OPA_90;
    grad.stops[0].frac = 0;
    grad.stops[1].frac = 255;
    lv_style_init(&style_mask);
    lv_style_copy(&style_mask, &screen_style);
    lv_style_set_bg_grad(&style_mask, &grad);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&about_camera_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&about_camera_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&about_camera_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //返回按钮
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

    lv_obj_t *label1 = lv_label_create(cont);
    lv_obj_set_size(label1, 222, 61);
    lv_label_set_text(label1, "P I K A");
    lv_obj_add_style(label1, &label1_style, 0);
    lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 28, 82);

    lv_obj_t *label2 = lv_label_create(cont);
    lv_obj_set_size(label2, 158, 37);
    lv_label_set_text(label2, "设备版本号");
    lv_obj_add_style(label2, &label2_style, 0);
    lv_obj_align(label2, LV_ALIGN_TOP_LEFT, 65, 171);

    lv_obj_t *label3 = lv_label_create(cont);
    lv_obj_set_size(label3, 134, 37);
    lv_label_set_text(label3, "设备型号");
    lv_obj_add_style(label3, &label2_style, 0);
    lv_obj_align(label3, LV_ALIGN_TOP_LEFT, 65, 256);

    lv_obj_t *label4 = lv_label_create(cont);
    lv_obj_set_size(label4, 184, 30);
    lv_label_set_long_mode(label4, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(label4, "V2.0 build 11111111111");
    lv_obj_add_style(label4, &label3_style, 0);
    lv_obj_align(label4, LV_ALIGN_TOP_LEFT, 65, 208);

    lv_obj_t *label5 = lv_label_create(cont);
    lv_obj_set_size(label5, 184, 30);
    lv_label_set_long_mode(label5, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(label5, "V2.0 build 11111111111");
    lv_obj_add_style(label5, &label3_style, 0);
    lv_obj_align(label5, LV_ALIGN_TOP_LEFT, 65, 293);

    //产品图
    lv_obj_t *image = lv_img_create(cont);
    lv_obj_set_size(image, 240, 350);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/product_red.png");
    lv_obj_align(image, LV_ALIGN_TOP_LEFT, 262, 40);

    lv_obj_t *mask = lv_obj_create(cont);
    lv_obj_remove_style_all(mask);
    lv_obj_add_style(mask, &style_mask, 0);
    lv_obj_set_size(mask, 150, lv_pct(100));
    lv_obj_align(mask, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 130, 58);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_radius(btn, 29, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);

    lv_obj_t *label6 = lv_label_create(btn);
    lv_label_set_text(label6, "新版本");
    lv_obj_set_style_text_opa(label6, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label6, fzlthr_26, 0);
    lv_obj_set_style_text_color(label6, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_align(label6, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label6, LV_ALIGN_CENTER, 0, 0);

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&about_camera_subject, PAGE_SWITCH_BACK);
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
    switch_page->old_page = &about_camera_page_info;

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