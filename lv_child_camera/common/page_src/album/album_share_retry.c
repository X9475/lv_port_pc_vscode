#include "../lv_switch_interface.h"

#define PHOTOGRAPH_PIC_FAIL "V:tk1/realtime_shooting/photograph_pic_fail.png"

lv_subject_t  album_share_retry_subject;
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

static void cancel_buton_click_event(lv_event_t * e);
static void retry_buton_click_event(lv_event_t * e);

//待跳转的页面种类
//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_CANCEL,
    PAGE_SWITCH_SHARE_SUC,
    PAGE_SWITCH_BACK
};

static lv_page_info_t album_page_share_retry = {
    .page_id = PAGE_FUNCTIONAL_ALBUM_SHARE_RETRY,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_album_share_retry_get()
{
    return &album_page_share_retry;
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
    album_page_share_retry.page = screen;
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
    lv_subject_init_int(&album_share_retry_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_share_retry_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_share_retry_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_add_style(cont, &screen_style, 0);

    lv_obj_t * photo_pic_fail_icon = lv_img_create(cont);
    lv_img_set_src(photo_pic_fail_icon, PHOTOGRAPH_PIC_FAIL);
    lv_obj_set_size(photo_pic_fail_icon, 258, 258);
    lv_obj_align(photo_pic_fail_icon, LV_ALIGN_TOP_LEFT, 115, 32);

    lv_obj_t *btn_obj = lv_obj_create(cont);
    lv_obj_set_size(btn_obj, 148, 70);
    lv_obj_set_style_radius(btn_obj, 51, 0);
    lv_obj_set_style_opa(btn_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn_obj, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_border_width(btn_obj, 0, 0);
    lv_obj_align(btn_obj, LV_ALIGN_TOP_LEFT, 78, 310);
    lv_obj_clear_flag(btn_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(btn_obj);
    lv_label_set_text(label, "取消");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(30), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(btn_obj, cancel_buton_click_event, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *btn1_obj = lv_obj_create(cont);
    lv_obj_set_size(btn1_obj, 148, 70);
    lv_obj_set_style_radius(btn1_obj, 51, 0);
    lv_obj_set_style_opa(btn1_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn1_obj, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_border_width(btn1_obj, 0, 0);
    lv_obj_align(btn1_obj, LV_ALIGN_TOP_LEFT, 276, 310);
    lv_obj_clear_flag(btn1_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label1 = lv_label_create(btn1_obj);
    lv_label_set_text(label1, "重试");
    lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label1, font_get_regular(30), 0);
    lv_obj_set_style_text_color(label1, lv_color_hex(0X000000), 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(btn1_obj, retry_buton_click_event, LV_EVENT_CLICKED, NULL);

    return;
}

static void cancel_buton_click_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_CLICKED == code)
    {
        // 返回录像界面，暂不删除文件
        LV_LOG_USER("cancel icon clicked!");
        lv_subject_set_int(&album_share_retry_subject, PAGE_SWITCH_CANCEL);

    }
}

static void retry_buton_click_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_CLICKED == code)
    {
        // 返回录像界面，暂不删除文件
        LV_LOG_USER("retry icon clicked!");
        lv_subject_set_int(&album_share_retry_subject, PAGE_SWITCH_SHARE_SUC);

    }
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_WARN("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发
    
    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &album_page_share_retry;

    switch (page_event)
    {
        case PAGE_SWITCH_CANCEL:
            switch_page->new_page = lv_page_album_get();
            break;

        case PAGE_SWITCH_SHARE_SUC:
            switch_page->new_page = lv_page_album_share_suc_get();
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