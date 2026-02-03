#include "../lv_switch_interface.h"

lv_subject_t voice_control_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t main_style;
static lv_style_t indicator_style;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void sw_event_handler(lv_event_t *e);
static void page_back_event_cb(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t voice_control_page_info = {
    .page_id = PAGE_FUNCTIONAL_VOICE_CONTROL,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_voice_control_info_get()
{
    return &voice_control_page_info;
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
    voice_control_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&main_style);
    lv_style_reset(&indicator_style);
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
    lv_style_set_bg_color(&main_style, lv_color_hex(0x454545));
    lv_style_set_bg_opa(&main_style, LV_OPA_COVER);

    //indicator_style
    lv_style_init(&indicator_style);
    lv_style_set_bg_color(&indicator_style, lv_color_hex(0xAFF99C));
    lv_style_set_bg_opa(&indicator_style, LV_OPA_COVER);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&voice_control_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&voice_control_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&voice_control_subject);
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
    lv_label_set_text(header, "语音控制开关");
    lv_obj_set_style_text_font(header, fzlthb_30, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(header, back_btn, LV_ALIGN_OUT_RIGHT_MID, -8, 0);

    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 422, 110);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x0A0B0D), 0);
    lv_obj_set_style_bg_grad_color(btn, lv_color_hex(0x202124), 0);
    lv_obj_set_style_bg_grad_dir(btn, LV_GRAD_DIR_HOR, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 80);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "语音控制");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_28, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 45, 0);

    lv_obj_t *sw = lv_switch_create(btn);
    lv_obj_set_size(sw, 67, 40);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_style(sw, &main_style, LV_PART_MAIN);
    lv_obj_add_style(sw, &indicator_style, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_switch_set_orientation(sw, LV_SWITCH_ORIENTATION_HORIZONTAL);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -25, 0);
    lv_obj_add_event_cb(sw, sw_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *tips = lv_label_create(cont);
    lv_obj_set_size(tips, 362, 93);
    lv_label_set_text(tips, "开启后设备会响应皮卡拍照、皮卡录像、给“xxx”打一个视频电话等语音指令");
    lv_obj_set_style_text_opa(tips, LV_OPA_60, 0);
    lv_obj_set_style_text_font(tips, fzlthr_24, 0);
    lv_obj_set_style_text_color(tips, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(tips, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tips, LV_ALIGN_BOTTOM_MID, 0, -25);

    return;
}

static void sw_event_handler(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    if (lv_obj_has_state(obj, LV_STATE_CHECKED))
    {
        printf("开启\n");
    }
    else
    {
        printf("关闭\n");
    }
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&voice_control_subject, PAGE_SWITCH_BACK);
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
    switch_page->old_page = &voice_control_page_info;

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