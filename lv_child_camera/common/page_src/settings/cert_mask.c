#include "../lv_switch_interface.h"

lv_subject_t cert_mask_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
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
    lv_style_reset(&style_mask);
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

    //图层蒙板
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[0].opa = LV_OPA_TRANSP;
    grad.stops[1].color = lv_color_hex(0x000000);
    grad.stops[1].opa = LV_OPA_COVER;
    grad.stops[0].frac = 0;
    grad.stops[1].frac = 255;
    lv_style_init(&style_mask);
    lv_style_copy(&style_mask, &screen_style);
    lv_style_set_bg_grad(&style_mask, &grad);
    lv_style_set_bg_grad_dir(&style_mask, LV_GRAD_DIR_VER);
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

    //背景图
    lv_obj_t *image = lv_img_create(cont);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/setting_icon_legal_notice.png");
    lv_obj_set_size(image, 140, 140);
    lv_obj_align_to(image, cont, LV_ALIGN_TOP_MID, 0, 20);

    //文字提示：认证标识
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "认证标识");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_40, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 147);

    //创建滚动容器作为文本的父容器
    lv_obj_t *scroll_cont = lv_obj_create(cont);
    lv_obj_set_size(scroll_cont, 422, 201);
    lv_obj_add_style(scroll_cont, &screen_style, LV_PART_MAIN);
    lv_obj_align(scroll_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(scroll_cont, LV_FLEX_FLOW_COLUMN);

    //创建长文本标签
    lv_obj_t *description = lv_label_create(scroll_cont);
    lv_label_set_long_mode(description, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(description, lv_pct(100));
    lv_obj_set_style_text_font(description, fzlthr_28, 0);
    lv_obj_set_style_text_opa(description, LV_OPA_60, 0);
    lv_obj_set_style_text_color(description, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(description, LV_TEXT_ALIGN_CENTER, 0);

    //设置长文本内容
    const char *long_text = "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?"
                            "Are you sure to exit the level calibration process?";
    lv_label_set_text(description, long_text);

    // 确保内容可以滚动
    lv_obj_set_scroll_dir(scroll_cont, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scroll_cont, LV_SCROLLBAR_MODE_AUTO);

    //文字蒙层
    lv_obj_t *mask = lv_obj_create(cont);
    lv_obj_add_style(mask, &style_mask, 0);
    lv_obj_set_size(mask, lv_pct(100), 100);
    lv_obj_align(mask, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_clear_flag(mask, LV_OBJ_FLAG_CLICKABLE);//禁止拦截点击事件

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