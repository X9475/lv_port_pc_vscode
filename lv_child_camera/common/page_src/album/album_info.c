#include "../lv_switch_interface.h"

#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"

lv_subject_t  album_info_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t style_file_info;

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

static lv_page_info_t album_page_info = {
    .page_id = PAGE_FUNCTIONAL_ALBUM_INFO,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_album_info_get()
{
    return &album_page_info;
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
    album_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&screen_style);
    lv_style_reset(&style_file_info);
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

    // album_info_style
    lv_style_init(&style_file_info);
    lv_style_set_bg_color(&style_file_info, lv_color_hex(0x121212));
    lv_style_set_bg_opa(&style_file_info, LV_OPA_COVER);
    lv_style_set_radius(&style_file_info, 20);
    lv_style_set_border_width(&style_file_info, 0);
    lv_style_set_pad_all(&style_file_info, 0);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&album_info_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_info_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_info_subject);
}

static void album_info_back_click_cb(lv_event_t *e)
{
    lv_subject_set_int(&album_info_subject, PAGE_SWITCH_BACK);
}

static void lv_page_load(lv_obj_t *cont)
{
    //创建返回按钮
    lv_obj_t * back_icon = lv_img_create(cont);
    lv_img_set_src(back_icon, ICON_BACK);
    lv_obj_set_size(back_icon, 50, 50);
    lv_obj_align(back_icon, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back_icon, album_info_back_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *file_name_obj = lv_obj_create(cont);
    lv_obj_set_size(file_name_obj, 462, 98);
    lv_obj_add_style(file_name_obj, &style_file_info, LV_PART_MAIN);
    lv_obj_align(file_name_obj, LV_ALIGN_TOP_MID, 0, 72);

    lv_obj_t *file_name_label = lv_label_create(file_name_obj);
    lv_label_set_text(file_name_label, "文件名");

    lv_obj_set_style_text_opa(file_name_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(file_name_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(file_name_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(file_name_label, LV_ALIGN_LEFT_MID, 30, 0);

    lv_obj_t *filename_label = lv_label_create(file_name_obj);
    lv_label_set_text(filename_label, "343454656566.JPG"); //todo:后续添加具体文件名

    lv_obj_set_style_text_opa(filename_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(filename_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(filename_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(filename_label, LV_ALIGN_RIGHT_MID, -30, 0);

    lv_obj_t *shooting_time_obj = lv_obj_create(cont);
    lv_obj_set_size(shooting_time_obj, 462, 98);
    lv_obj_add_style(shooting_time_obj, &style_file_info, LV_PART_MAIN);
    lv_obj_align(shooting_time_obj, LV_ALIGN_TOP_MID, 0, 184);

    lv_obj_t *shooting_time_label = lv_label_create(shooting_time_obj);
    lv_label_set_text(shooting_time_label, "拍摄时间");
    lv_obj_set_style_text_opa(shooting_time_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(shooting_time_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(shooting_time_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(shooting_time_label, LV_ALIGN_LEFT_MID, 30, 0);
    
    lv_obj_t *time_label = lv_label_create(shooting_time_obj);
    lv_label_set_text(time_label, "2025/08/01 13:46:23"); //todo:后续添加获取到的文件名
    lv_obj_set_style_text_opa(time_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(time_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(time_label, LV_ALIGN_RIGHT_MID, -30, 0);

    lv_obj_t *file_change_obj = lv_obj_create(cont);
    lv_obj_set_size(file_change_obj, 462, 98);
    lv_obj_add_style(file_change_obj, &style_file_info, LV_PART_MAIN);
    lv_obj_align(file_change_obj, LV_ALIGN_TOP_MID, 0, 296);

    lv_obj_t *file_size_label = lv_label_create(file_change_obj);
    lv_label_set_text(file_size_label, "文件大小");
    lv_obj_set_style_text_opa(file_size_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(file_size_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(file_size_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(file_size_label, LV_ALIGN_LEFT_MID, 30, 0);

    lv_obj_t *filesize_label = lv_label_create(file_change_obj);
    lv_label_set_text(filesize_label, "4.00MB"); //todo:后续展示实际获取到的文件大小
    lv_obj_set_style_text_opa(filesize_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(filesize_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(filesize_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(filesize_label, LV_ALIGN_RIGHT_MID, -30, 0);

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
    switch_page->old_page = &album_page_info;

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