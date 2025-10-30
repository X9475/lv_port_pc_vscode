#include "../lv_switch_interface.h"

#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"
#define AI_ADD_ICON "V:tk1/realtime_shooting/ai_add.png"
#define AI_switch_ICON "V:tk1/realtime_shooting/ai_switch.png"


lv_subject_t  album_ai_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;

static lv_obj_t *screen = NULL;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

//待跳转的页面种类
//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t album_page_ai = {
    .page_id = PAGE_FUNCTIONAL_ALBUM_AI,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_album_ai_get()
{
    return &album_page_ai;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();
    //加入栈表
    // lv_stack_push(&agent_start_page_info);

    screen = lv_obj_create(act_screen);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);
    album_page_ai.page = screen;
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
    lv_subject_init_int(&album_ai_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_ai_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_ai_subject);
}

static void album_info_back_click_cb(lv_event_t *e)
{
    lv_subject_set_int(&album_ai_subject, PAGE_SWITCH_BACK);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t * live_view = lv_image_create(cont);
    lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
    //todo:获取当前最新的录像流视频和图片信息，并将图片信息展示在界面上
    lv_image_set_src(live_view, "V:png/img_camera_backup.png");
    lv_obj_center(live_view);

    //创建返回按钮
    lv_obj_t * back_icon = lv_img_create(live_view);
    lv_img_set_src(back_icon, ICON_BACK);
    lv_obj_set_size(back_icon, 40, 40);
    lv_obj_align_to(back_icon, live_view, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back_icon, album_info_back_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *ai_label = lv_label_create(live_view);
    lv_label_set_text(ai_label, "AI对话");

    lv_obj_set_style_text_opa(ai_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(ai_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(ai_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align_to(ai_label, back_icon, LV_ALIGN_TOP_LEFT, 40, 5);

    //switch icon
    lv_obj_t * switch_icon = lv_img_create(live_view);
    lv_img_set_src(switch_icon, AI_switch_ICON);
    lv_obj_set_size(switch_icon, 40, 40);
    lv_obj_align(switch_icon, LV_ALIGN_TOP_LEFT, 432, 20);

    lv_obj_t *base_obj = lv_obj_create(live_view);
    lv_obj_set_size(base_obj, 442, 90);
    lv_obj_align(base_obj, LV_ALIGN_TOP_LEFT, 30, 300);
    lv_obj_set_style_radius(base_obj, 78, 0);
    lv_obj_set_style_opa(base_obj, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(base_obj, lv_color_hex(0XFFFFFF), 0);
    lv_obj_set_style_border_width(base_obj, 0, 0);
    lv_obj_set_style_border_color(base_obj, lv_color_hex(0XFFFFFF), 0);

    //photo icon
    lv_obj_t * photo_icon = lv_img_create(base_obj);
    lv_img_set_src(photo_icon, AI_ADD_ICON);
    lv_obj_set_size(photo_icon, 70, 70);
    lv_obj_align(photo_icon, LV_ALIGN_LEFT_MID, 10, 0);

    //add icon
    lv_obj_t * add_icon = lv_img_create(base_obj);
    lv_img_set_src(add_icon, AI_ADD_ICON);
    lv_obj_set_size(add_icon, 70, 70);
    lv_obj_align(add_icon, LV_ALIGN_RIGHT_MID, -10, 0);

    lv_obj_t *chat_label = lv_label_create(base_obj);
    lv_label_set_text(chat_label, "按住说话");
    lv_obj_set_style_text_opa(chat_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(chat_label, font_get_regular(30), 0);
    lv_obj_set_style_text_color(chat_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(chat_label, LV_ALIGN_CENTER, 0, 0);

    return;
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
    switch_page->old_page = &album_page_ai;

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