#include "../lv_switch_interface.h"

lv_subject_t storage_manage_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t style_storage_num;
static lv_style_t style_storage_text;
static lv_style_t style_line;
static float use_storage = 89.0;//GB
static float total_storage = 128.0;//GB

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void storage_manage_event_cb(lv_event_t *e);
static void storage_manage_format_click_event_cb(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_FORMAT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t storage_manage_page_info = {
    .page_id = PAGE_FUNCTIONAL_STORAGE_MGR,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_storage_manage_info_get()
{
    return &storage_manage_page_info;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();

    screen = lv_obj_create(NULL);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    storage_manage_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
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

    //style_storage_num
    lv_style_init(&style_storage_num);
    lv_style_set_text_opa(&style_storage_num, LV_OPA_COVER);
    lv_style_set_text_color(&style_storage_num, lv_color_hex(0XFFFFFF));
    lv_style_set_text_align(&style_storage_num, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&style_storage_num, font_get_regular(28));

    //style_storage_text
    lv_style_init(&style_storage_text);
    lv_style_set_text_opa(&style_storage_text, LV_OPA_COVER);
    lv_style_set_text_color(&style_storage_text, lv_color_hex(0XFFFFFF));
    lv_style_set_text_align(&style_storage_text, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&style_storage_text, font_get_regular(26));

    //style_line
    static lv_grad_dsc_t grad1;
    grad1.dir = LV_GRAD_DIR_VER;
    grad1.stops_count = 2;
    grad1.stops[0].color = lv_color_hex(0xFFFFFF);
    grad1.stops[0].opa = LV_OPA_TRANSP;
    grad1.stops[1].color = lv_color_hex(0xFFFFFF);
    grad1.stops[1].opa = LV_OPA_COVER;
    grad1.stops[0].frac = 0;
    grad1.stops[1].frac = 255;
    lv_style_init(&style_line);
    lv_style_copy(&style_line, &screen_style);
    lv_style_set_bg_grad(&style_line, &grad1);
    lv_style_set_bg_grad_dir(&style_line, LV_GRAD_DIR_VER);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&storage_manage_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&storage_manage_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&storage_manage_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    static lv_style_t style_main;
    lv_style_init(&style_main);
    lv_style_set_bg_image_src(&style_main, "../lv_port_pc_vscode/assert/icon/setting_pic_progress_bar_none.png");
    lv_style_set_bg_opa(&style_main, LV_OPA_TRANSP);
    lv_style_set_radius(&style_main, 100);

    static lv_style_t style_indicator;
    lv_style_init(&style_indicator);
    lv_style_set_radius(&style_indicator, 0);
    lv_style_set_bg_image_src(&style_indicator, "../lv_port_pc_vscode/assert/icon/setting_pic_progress_bar_have.png");
    lv_style_set_bg_opa(&style_indicator, LV_OPA_TRANSP);

    static lv_style_t style_knob;
    lv_style_init(&style_knob);
    lv_style_set_bg_opa(&style_knob, LV_OPA_TRANSP);

    //渐变背景
    lv_obj_t *image = lv_img_create(cont);
    lv_obj_set_size(image, 442, LV_SIZE_CONTENT);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/setting_pic_dazzle_light.png");
    lv_obj_set_style_opa(image, LV_OPA_COVER, 0);
    lv_obj_align(image, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *background = lv_obj_create(cont);
    lv_obj_remove_style_all(background);
    lv_obj_add_style(background, &screen_style, 0);
    lv_obj_set_size(background, 442, 203);
    lv_obj_align(background, LV_ALIGN_BOTTOM_RIGHT, -30, -91);

    //滑动条
    lv_obj_t *slider = lv_slider_create(cont);
    lv_obj_set_size(slider, 442, 50);
    lv_slider_set_mode(slider, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -91);
    lv_obj_add_style(slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
    lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
    lv_slider_set_value(slider, (use_storage / total_storage) * 100, LV_ANIM_OFF);
    lv_obj_clear_flag(slider, LV_OBJ_FLAG_CLICKABLE);//不能被点击

    lv_obj_t *line = lv_obj_create(cont);
    lv_obj_remove_style_all(line);
    lv_obj_set_size(line, 2, 203);
    lv_obj_add_style(line, &style_line, 0);
    lv_obj_align(line, LV_ALIGN_BOTTOM_LEFT, 30, -91);
    lv_obj_set_user_data(background, line);

    lv_obj_add_event_cb(slider, storage_manage_event_cb, LV_EVENT_VALUE_CHANGED, background);
    lv_obj_send_event(slider, LV_EVENT_VALUE_CHANGED, NULL);

    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    //格式化
    lv_obj_t *format = lv_btn_create(cont);
    lv_obj_set_size(format, 126, 60);
    lv_obj_set_style_shadow_width(format, 0, 0);
    lv_obj_set_style_radius(format, 34, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(format, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(format, lv_color_hex(0x2C2C2E), LV_PART_MAIN);
    lv_obj_align(format, LV_ALIGN_TOP_RIGHT, -15, 15);
    lv_obj_add_event_cb(format, storage_manage_format_click_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(format);
    lv_label_set_text(label, "格式化");
    lv_obj_set_style_text_font(label, fzlthr_22, 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    //滑动条显示文本
    lv_obj_t *bar_label_1 = lv_label_create(cont);
    lv_label_set_text(bar_label_1, "89");
    lv_obj_set_style_text_font(bar_label_1, fzlthr_170, 0);
    lv_obj_set_style_text_opa(bar_label_1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(bar_label_1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(bar_label_1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(bar_label_1, LV_ALIGN_TOP_LEFT, 28, 80);

    lv_obj_t *bar_label_2 = lv_label_create(cont);
    lv_label_set_text(bar_label_2, "GB / 128GB");
    lv_obj_set_style_text_font(bar_label_2, fzlthr_34, 0);
    lv_obj_set_style_text_opa(bar_label_2, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(bar_label_2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(bar_label_2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(bar_label_2, bar_label_1, LV_ALIGN_OUT_RIGHT_TOP, 8, 40);

    //底部显示文本
    lv_obj_t *label_1 = lv_label_create(cont);
    lv_label_set_text(label_1, "36");
    lv_obj_add_style(label_1, &style_storage_num, 0);
    lv_obj_align(label_1, LV_ALIGN_BOTTOM_LEFT, 30, -34);

    lv_obj_t *label_2 = lv_label_create(cont);
    lv_label_set_text(label_2, "照片");
    lv_obj_add_style(label_2, &style_storage_text, 0);
    lv_obj_align_to(label_2, label_1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t *label_3 = lv_label_create(cont);
    lv_label_set_text(label_3, "36");
    lv_obj_add_style(label_3, &style_storage_num, 0);
    lv_obj_align_to(label_3, label_2, LV_ALIGN_OUT_RIGHT_MID, 34, 0);

    lv_obj_t *label_4 = lv_label_create(cont);
    lv_label_set_text(label_4, "录像");
    lv_obj_add_style(label_4, &style_storage_text, 0);
    lv_obj_align_to(label_4, label_3, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t *label_5 = lv_label_create(cont);
    lv_label_set_text(label_5, "4");
    lv_obj_add_style(label_5, &style_storage_num, 0);
    lv_obj_align_to(label_5, label_4, LV_ALIGN_OUT_RIGHT_MID, 51, 0);

    lv_obj_t *label_6 = lv_label_create(cont);
    lv_label_set_text(label_6, "延时摄影段数");
    lv_obj_add_style(label_6, &style_storage_text, 0);
    lv_obj_align_to(label_6, label_5, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&storage_manage_subject, PAGE_SWITCH_BACK);
}

static void storage_manage_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *background = lv_event_get_user_data(e);
    lv_obj_t *line = lv_obj_get_user_data(background);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        int value = lv_slider_get_value(slider);
        lv_obj_set_size(background, 442 - (442 * value / 100), 203);
        lv_obj_set_x(line, (442 * value / 100) + 30);
    }
}

static void storage_manage_format_click_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&storage_manage_subject, PAGE_SWITCH_FORMAT);
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
    switch_page->old_page = &storage_manage_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_FORMAT:
            lv_stack_push(&storage_manage_page_info);
            switch_page->new_page = lv_page_format_confirm_info_get();
            break;
        case PAGE_SWITCH_BACK:
            switch_page->new_page = lv_stack_pop();
            break;
        default:
            LV_LOG_WARN("[%s:%d] -- page switch event:%d invaild", __FILE__, __LINE__, page_event);
            break;
    }

    if (NULL == switch_page->new_page) return;
    lv_subject_set_pointer(&switch_subject, switch_page);
}