#include "../lv_switch_interface.h"

lv_subject_t indvid_key_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static int indvid_key_select = 0;

typedef struct
{
    const char *icon_path;
    const char *name;
} indvid_key_info_t;
static indvid_key_info_t g_indvid_key_info[2] = {
    {"../lv_port_pc_vscode/assert/icon/photograph_icon_screenshot.png", "拍摄"},
    {"../lv_port_pc_vscode/assert/icon/photograph_icon_ai.png", "AI问答"},
};

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void indvid_key_iterm_create(lv_obj_t *cont_col, indvid_key_info_t *indvid_key, int i);
static void indvid_key_click_event_cb(lv_event_t *e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t indvid_key_page_info = {
    .page_id = PAGE_FUNCTIONAL_INDVID_KEY,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_indvid_key_info_get()
{
    return &indvid_key_page_info;
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
    indvid_key_page_info.page = screen;
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
    lv_subject_init_int(&indvid_key_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&indvid_key_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&indvid_key_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t *contain = lv_obj_create(cont);
    lv_obj_set_size(contain, lv_pct(100), 365);
    lv_obj_add_style(contain, &screen_style, 0);
    lv_obj_set_style_bg_opa(contain, LV_OPA_TRANSP, 0);
    // lv_obj_set_style_border_width(contain, 1, 0);
    // lv_obj_set_style_border_opa(contain, LV_OPA_COVER, 0);
    lv_obj_set_scroll_dir(contain, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(contain, LV_SCROLLBAR_MODE_OFF);
    lv_obj_align(contain, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *product_img = lv_img_create(contain);
    lv_obj_set_size(product_img, 412, 318);
    lv_img_set_src(product_img, "../lv_port_pc_vscode/assert/icon/key_yellow.png");
    lv_obj_align(product_img, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *descrip = lv_label_create(contain);
    lv_label_set_text(descrip, "当你在其他界面，可通过此按键");
    lv_obj_set_style_text_font(descrip, fzlthr_24, 0);
    lv_obj_set_style_text_opa(descrip, LV_OPA_90, 0);
    lv_obj_set_style_text_color(descrip, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(descrip, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(descrip, LV_ALIGN_TOP_LEFT, 40, 319);

    //滚动列表
    lv_obj_t *cont_col = lv_obj_create(contain);
    lv_obj_set_size(cont_col, lv_pct(100), 256);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_align(cont_col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(cont_col, LV_ALIGN_TOP_MID, 0, 350);

    for (uint8_t i = 0; i < 2; i++)
    {
        indvid_key_iterm_create(cont_col, &g_indvid_key_info[i], i);
    }

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
    lv_label_set_text(header, "自定义按键");
    lv_obj_set_style_text_font(header, fzlthb_30, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(header, back_btn, LV_ALIGN_OUT_RIGHT_MID, -8, 0);
    return;
}

static void indvid_key_iterm_create(lv_obj_t *cont_col, indvid_key_info_t *indvid_key, int index)
{
    lv_obj_t *btn = lv_obj_create(cont_col);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 422, 110);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x0A0B0D), 0);
    lv_obj_set_style_bg_grad_color(btn, lv_color_hex(0x202124), 0);
    lv_obj_set_style_bg_grad_dir(btn, LV_GRAD_DIR_HOR, 0);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(btn, indvid_key_click_event_cb, LV_EVENT_CLICKED, cont_col);

    lv_obj_t *select_ = lv_img_create(btn);
    lv_obj_align(select_, LV_ALIGN_RIGHT_MID, -34, 0);
    if (index == indvid_key_select)
    {
        lv_img_set_src(select_, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
    }
    else if (index != indvid_key_select)
    {
        lv_img_set_src(select_, "../lv_port_pc_vscode/assert/icon/photograph_icon_unselect.png");
    }

    lv_obj_t *icon = lv_img_create(btn);
    lv_obj_set_size(icon, 70, 70);
    lv_img_set_src(icon, indvid_key->icon_path);
    lv_obj_align_to(icon, btn, LV_ALIGN_LEFT_MID, 19, 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, indvid_key->name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, fzlthr_28, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 110, 0);

    return;
}

static void indvid_key_click_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *cont_col = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        for (uint8_t i = 0; i < lv_obj_get_child_cnt(cont_col); i++)
        {
            lv_obj_t *iterm = lv_obj_get_child(cont_col, i);
            lv_obj_t *img = lv_obj_get_child(iterm, 0);
            lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_unselect.png");
        }

        lv_obj_t *checkbox = lv_event_get_target(e);
        lv_obj_t *img = lv_obj_get_child(checkbox, 0);
        lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
        lv_obj_scroll_to_view(checkbox, LV_ANIM_ON);

        if (checkbox == lv_obj_get_child(cont_col, 0) && indvid_key_select)
        {
            indvid_key_select = 0;
            printf("拍摄\n");
        }
        else if (checkbox == lv_obj_get_child(cont_col, 1) && !indvid_key_select)
        {
            indvid_key_select = 1;
            printf("AI问答\n");
        }
    }
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&indvid_key_subject, PAGE_SWITCH_BACK);
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_WARN("page switch event:%d", page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发

    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &indvid_key_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_BACK:
            switch_page->new_page = lv_stack_pop();
            break;
        default:
            LV_LOG_WARN("page switch event:%d invaild", page_event);
            break;
    }

    if (NULL == switch_page->new_page) {
        lv_free(switch_page);
        return;
    }

    lv_subject_set_pointer(&switch_subject, switch_page);
}