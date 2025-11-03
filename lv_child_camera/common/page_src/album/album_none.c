#include "../lv_switch_interface.h"

#define EMPTY_PIC_PHOTO "V:tk1/realtime_shooting/empety_pic_photo.png"
#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"

lv_subject_t  album_none_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;

static lv_obj_t *screen = NULL;

static void lv_page_construct(void *this);
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
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t album_page_none = {
    .page_id = PAGE_FUNCTIONAL_ALBUM_NONE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_album_none_get()
{
    return &album_page_none;
}

static void lv_page_construct(void *this)
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
    album_page_none.page = screen;
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
    lv_subject_init_int(&album_none_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_none_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_none_subject);
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&album_none_subject, PAGE_SWITCH_BACK);
}
static void lv_page_load(lv_obj_t *cont)
{
    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, ICON_BACK);
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * empty_photo_icon = lv_img_create(cont);
    lv_img_set_src(empty_photo_icon, EMPTY_PIC_PHOTO);
    lv_obj_set_size(empty_photo_icon, 380, 210);
    lv_obj_align(empty_photo_icon, LV_ALIGN_TOP_LEFT, 61, 60);

    //设置文字到主菜单
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "暂无内容");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(26), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 290);

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
    switch_page->old_page = &album_page_none;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
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