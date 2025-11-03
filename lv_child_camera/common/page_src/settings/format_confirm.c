#include "../lv_switch_interface.h"

lv_subject_t format_confirm_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_obj_t *confirm;
static lv_obj_t *cancel;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_cancel_and_confirm_click_event(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_CONFIRM,
    PAGE_SWITCH_BACK
};

static lv_page_info_t format_confirm_page_info = {
    .page_id = PAGE_FUNCTIONAL_FACTORY_RESTORE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_format_confirm_info_get()
{
    return &format_confirm_page_info;
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

    format_confirm_page_info.page = screen;
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
    lv_subject_init_int(&format_confirm_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&format_confirm_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&format_confirm_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *storage = lv_img_create(cont);
    lv_img_set_src(storage, "../lv_port_pc_vscode/assert/icon/format_sd.png");
    lv_img_set_zoom(storage, 128);
    lv_obj_set_size(storage, 380, 210);
    lv_obj_align(storage, LV_ALIGN_TOP_MID, 0, 25);

    //文字提示：确定将SD卡格式化吗？
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "确定将SD卡格式化吗？");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, fzlthr_26, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 245);

    //取消
    cancel = lv_btn_create(cont);
    lv_obj_set_size(cancel, 148, 70);
    lv_obj_align(cancel, LV_ALIGN_BOTTOM_LEFT, 78, -30);
    lv_obj_set_style_radius(cancel, 51, 0);
    lv_obj_set_style_bg_opa(cancel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cancel, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_shadow_opa(cancel, LV_OPA_TRANSP, 0);
    lv_obj_t *img_cancel = lv_img_create(cancel);
    lv_img_set_src(img_cancel, "../lv_port_pc_vscode/assert/icon/common_icon_cancel_button.png");
    lv_obj_set_size(img_cancel, 50, 50);
    lv_obj_align_to(img_cancel, cancel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(cancel, lv_cancel_and_confirm_click_event, LV_EVENT_CLICKED, cancel);

    //确认
    confirm = lv_btn_create(cont);
    lv_obj_set_size(confirm, 148, 70);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_RIGHT, -78, -30);
    lv_obj_set_style_radius(confirm, 51, 0);
    lv_obj_set_style_bg_opa(confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(confirm, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(confirm, LV_OPA_TRANSP, 0);

    lv_obj_t *img_confirm = lv_img_create(confirm);
    lv_img_set_src(img_confirm, "../lv_port_pc_vscode/assert/icon/common_icon_ok_button.png");
    lv_obj_set_size(img_confirm, 50, 50);
    lv_obj_align_to(img_confirm, confirm, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_image_recolor_opa(img_confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img_confirm, lv_color_hex(0x0A0B0D), 0);
    lv_obj_add_event_cb(confirm, lv_cancel_and_confirm_click_event, LV_EVENT_CLICKED, confirm);

    return;
}

static void lv_cancel_and_confirm_click_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_user_data(e);

    if (btn == confirm)
    {
        lv_subject_set_int(&format_confirm_subject, PAGE_SWITCH_CONFIRM);
    }
    else if (btn == cancel)
    {
        lv_subject_set_int(&format_confirm_subject, PAGE_SWITCH_BACK);
    }
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
    switch_page->old_page = &format_confirm_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_CONFIRM:
            // switch_page->new_page = lv_page_agent_start_info_get();
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