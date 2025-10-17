#include "../lv_switch_interface.h"

#define SETTING_NUM     8

lv_subject_t settings_more_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void *lv_more_setting_iterm_create(lv_obj_t *cont, const char *name);
static void setting_iterm_click_event_cb(lv_event_cb_t *e);

static const char *setting_list[SETTING_NUM] = {
    "单次录像时长","熄屏时间","时间展示形式","存储管理", \
    "振动幅度","关于相机","恢复出厂设置","认证标志"
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SINGLE_RECORD_DURATION, //单次录像时长设置
    PAGE_SWITCH_SCREEN_OFF_TIME,        //熄屏时间
    PAGE_SWITCH_TIME_DISPLAY_FORMAT,    //时间展示形式
    PAGE_SWITCH_VIBRATION_AMPLITUDE,    //振动幅度
    PAGE_SWITCH_CAMERA_ABOUT,           //关于相机
    PAGE_SWITCH_FACTORY_RESTORE,        //恢复出厂设置
    PAGE_SWITCH_STORAGE_MANAGER,        //存储管理
    PAGE_SWITCH_CERTIFICATION_MARK,     //认证标志
    PAGE_SWITCH_BACK
};

static lv_page_info_t settings_more_page_info = {
    .page_id = PAGE_FUNCTIONAL_MORE_SETTINGS,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_settings_more_info_get()
{
    return &settings_more_page_info;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();
    //加入栈表
    // lv_stack_push(&settings_more_page_info);

    screen = lv_obj_create(NULL);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    settings_more_page_info.page = screen;
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
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&settings_more_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&settings_more_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&settings_more_subject);
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

    //滚动列表
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 340);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_align_to(cont_col, cont, LV_ALIGN_TOP_MID, 20, 70);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);

    for (uint8_t i = 0; i < SETTING_NUM; i++)
    {
        lv_obj_t *btn = lv_more_setting_iterm_create(cont_col, setting_list[i]);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(btn, setting_iterm_click_event_cb, LV_EVENT_CLICKED, setting_list[i]);
    }

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_BACK);
}

static void *lv_more_setting_iterm_create(lv_obj_t *cont, const char *name)
{
    //背景
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 462, 98);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x121212), 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    //文字
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, fzlthb_28, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 30, 0);

    //进入指示
    lv_obj_t *image = lv_img_create(btn);
    lv_obj_set_size(image, 7, 13);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_img_set_angle(image, 1800);
    lv_img_set_zoom(image, 160);
    lv_obj_align_to(image, btn, LV_ALIGN_RIGHT_MID, -31, 0);

    return btn;
}

static void setting_iterm_click_event_cb(lv_event_cb_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    const char *name = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        printf("Click %s\n", name);
        if (lv_strcmp(name, "单次录像时长") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "熄屏时间") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "时间展示形式") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "存储管理") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "振动幅度") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "关于相机") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "恢复出厂设置") == 0)
        {
            //TODO: 页面跳转
        }
        else if (lv_strcmp(name, "认证标志") == 0)
        {
            //TODO: 页面跳转
        }
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
    switch_page->old_page = &settings_more_page_info;

    switch (page_event)
    {
        // case PAGE_SWITCH_SINGLE_RECORD_DURATION:
        //     switch_page->new_page = lv_page_signup_success_info_get();
        //     // switch_page->new_page = lv_page_menu_info_get();
        //     break;
        // case PAGE_SWITCH_SCREEN_OFF_TIME:
        //     // switch_page->new_page = lv_page_signup_failed_info_get();
        //     break;
        // case PAGE_SWITCH_TIME_DISPLAY_FORMAT:
        //     // switch_page->new_page = lv_page_signup_failed_info_get();
        //     break;
        // case PAGE_SWITCH_VIBRATION_AMPLITUDE:
        //     // switch_page->new_page = lv_page_signup_failed_info_get();
        //     break;
        // case PAGE_SWITCH_CAMERA_ABOUT:
        //     // switch_page->new_page = lv_page_signup_failed_info_get();
        //     break;
        // case PAGE_SWITCH_FACTORY_RESTORE:
        //     // switch_page->new_page = lv_page_signup_failed_info_get();
        //     break;
        // case PAGE_SWITCH_STORAGE_MANAGER:
        //     // switch_page->new_page = lv_page_signup_failed_info_get();
        //     break;
        // case PAGE_SWITCH_CERTIFICATION_MARK:
        //     // switch_page->new_page = lv_page_signup_failed_info_get();
        //     break;
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