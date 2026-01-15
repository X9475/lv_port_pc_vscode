#include <string.h>
#include <time.h>
#include "../lv_switch_interface.h"

#define ITERM_NUM       8

lv_subject_t message_center_subject;

static lv_switch_page_pt switch_page;
static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_obj_t *delete_icon = NULL;
static lv_obj_t *list_cont = NULL;
static lv_obj_t *mask_layer = NULL;
static lv_timer_t *refresh_timer = NULL;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void create_message_center_item(lv_obj_t *list_cont, int idx);
static void lv_iterm_dalete_click_cb(lv_event_t *e);
static void lv_message_dalete_click_cb(lv_event_t *e);
static void lv_message_all_is_cleaned();

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_QRCODERUN,
    PAGE_SWITCH_SHOOT_PHOTO,
    PAGE_SWITCH_SHOOT_VIDEO,
    PAGE_SWITCH_BACK
};

static lv_page_info_t message_center_page_info = {
    .page_id = PAGE_FUNCTION_MESSAGE_CENTER,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_message_center_get()
{
    return &message_center_page_info;
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
    message_center_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    // 清理定时器
    if (refresh_timer) lv_timer_del(refresh_timer);
    refresh_timer = NULL;
    
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
    lv_style_set_shadow_width(&screen_style, 0);
    lv_style_set_bg_opa(&screen_style, LV_OPA_COVER);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&message_center_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&message_center_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&message_center_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //返回按钮
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

    //创建标题
    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "消息中心");
    lv_obj_set_style_text_font(header, fzlthb_30, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align_to(header, back_btn, LV_ALIGN_OUT_RIGHT_MID, -5, 0);

    //删除图标
    delete_icon = lv_img_create(cont);
    lv_obj_set_size(delete_icon, 40, 40);
    lv_img_set_src(delete_icon, "../lv_port_pc_vscode/assert/icon/trash.png");
    lv_obj_align(delete_icon, LV_ALIGN_TOP_RIGHT, -30, 25);
    lv_obj_add_flag(delete_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(delete_icon, lv_message_dalete_click_cb, LV_EVENT_CLICKED, NULL);

    //创建未接来电列表容器
    list_cont = lv_obj_create(cont);
    lv_obj_set_size(list_cont, lv_pct(100), 330);
    lv_obj_add_style(list_cont, &screen_style, 0);
    lv_obj_set_style_bg_opa(list_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(list_cont, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_flex_align(list_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list_cont, LV_ALIGN_TOP_MID, 0, 80);

    for (int i = 0; i < ITERM_NUM; i++)
    {
        create_message_center_item(list_cont, i);
    }

    return;
}

static void lv_message_all_is_cleaned()
{
    lv_obj_t *empty_msg = lv_img_create(screen);
    lv_img_set_src(empty_msg, "../lv_port_pc_vscode/assert/icon/empety_pic_photo.png");
    // lv_img_set_zoom(empty_msg, 128);
    lv_obj_set_size(empty_msg, 380, 210);
    lv_obj_align(empty_msg, LV_ALIGN_TOP_MID, 0, 80);

    lv_obj_t *msglabel = lv_label_create(screen);
    lv_label_set_text(msglabel, "暂时没有消息哦~");
    lv_obj_set_style_text_opa(msglabel, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(msglabel, fzlthr_26, 0);
    lv_obj_set_style_text_color(msglabel, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(msglabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(msglabel, LV_ALIGN_TOP_MID, 0, 300);

    //隐藏删除图标
    lv_obj_add_flag(delete_icon, LV_OBJ_FLAG_HIDDEN);

    return;
}

static void lv_message_dalete_cancel_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_user_data(e);
    if (lv_obj_is_valid(obj))
    {
        lv_obj_del(obj);
        obj = NULL;
    }
}

static void lv_message_dalete_confirm_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_user_data(e);
    if (lv_obj_is_valid(obj))
    {
        lv_obj_del(obj);
        obj = NULL;
    }

    if (lv_obj_is_valid(list_cont))
    {
        lv_obj_clean(list_cont);
        //显示无内容
        lv_message_all_is_cleaned();
    }
}

static void lv_message_dalete_click_cb(lv_event_t *e)
{
    mask_layer = lv_obj_create(screen);
    lv_obj_set_size(mask_layer, lv_pct(100), lv_pct(100));
    lv_obj_add_style(mask_layer, &screen_style, 0);
    lv_obj_set_style_bg_color(mask_layer, lv_color_hex(0x000000), 0); 
    lv_obj_set_style_bg_opa(mask_layer, LV_OPA_80, 0);
    lv_obj_clear_flag(mask_layer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(mask_layer);

    //文案提示
    lv_obj_t *label = lv_label_create(mask_layer);
    lv_obj_set_size(label, 390, 80);
    lv_label_set_text(label, "清空后将无法找回所有联系人留言信息，确定清空吗？");
    lv_obj_set_style_text_font(label, fzlthr_30, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_90, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 139);

    //取消
    lv_obj_t *cancel = lv_btn_create(mask_layer);
    lv_obj_set_size(cancel, 148, 70);
    lv_obj_align(cancel, LV_ALIGN_BOTTOM_LEFT, 78, -30);
    lv_obj_set_style_radius(cancel, 51, 0);
    lv_obj_set_style_bg_opa(cancel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cancel, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_shadow_opa(cancel, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(cancel, lv_message_dalete_cancel_cb, LV_EVENT_CLICKED, mask_layer);

    lv_obj_t *img_cancel = lv_img_create(cancel);
    lv_img_set_src(img_cancel, "../lv_port_pc_vscode/assert/icon/common_icon_cancel_button.png");
    lv_obj_set_size(img_cancel, 50, 50);
    lv_obj_align_to(img_cancel, cancel, LV_ALIGN_CENTER, 0, 0);

    //确认
    lv_obj_t *confirm = lv_btn_create(mask_layer);
    lv_obj_set_size(confirm, 148, 70);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_RIGHT, -78, -30);
    lv_obj_set_style_radius(confirm, 51, 0);
    lv_obj_set_style_bg_opa(confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(confirm, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_shadow_opa(confirm, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(confirm, lv_message_dalete_confirm_cb, LV_EVENT_CLICKED, mask_layer);

    lv_obj_t *img_confirm = lv_img_create(confirm);
    lv_img_set_src(img_confirm, "../lv_port_pc_vscode/assert/icon/common_icon_ok_button.png");
    lv_obj_set_size(img_confirm, 50, 50);
    lv_obj_align_to(img_confirm, confirm, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_image_recolor_opa(img_confirm, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img_confirm, lv_color_hex(0x0A0B0D), 0);
}

static void create_message_center_item(lv_obj_t *list_cont, int idx)
{
    //创建水平滑动的容器
    lv_obj_t *hor_cont = lv_obj_create(list_cont);
    lv_obj_set_size(hor_cont, 462, 110);
    lv_obj_add_style(hor_cont, &screen_style, 0);
    lv_obj_set_scroll_dir(hor_cont, LV_DIR_HOR);
    lv_obj_set_flex_flow(hor_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_scroll_snap_x(hor_cont, LV_SCROLL_SNAP_END);
    lv_obj_set_scrollbar_mode(hor_cont, LV_SCROLLBAR_MODE_OFF);

    //显示未接电话详情
    lv_obj_t *item = lv_obj_create(hor_cont);
    lv_obj_set_size(item, 462, 110);
    lv_obj_add_style(item, &screen_style, 0);
    lv_obj_set_style_radius(item, 50, 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_60, 0);
    if ((idx % 3) == 0) {
        lv_obj_set_style_bg_color(item, lv_color_hex(0x2A3534), 0);
    } else if ((idx % 3) == 1) {
        lv_obj_set_style_bg_color(item, lv_color_hex(0x352A31), 0);
    } else if ((idx % 3) == 2){
        lv_obj_set_style_bg_color(item, lv_color_hex(0x352E2A), 0);
    }
    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);

    //创建头像容器
    lv_obj_t *avatar = lv_obj_create(item);
    lv_obj_set_size(avatar, 80, 80);
    lv_obj_add_style(avatar, &screen_style, 0);
    lv_obj_set_style_radius(avatar, 40, 0);
    lv_obj_set_style_clip_corner(avatar, true, 0);
    lv_obj_set_style_bg_color(avatar, lv_color_hex(0xFFFFFF), 0); 
    lv_obj_set_style_bg_opa(avatar, LV_OPA_COVER, 0);
    lv_obj_clear_flag(avatar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(avatar, item, LV_ALIGN_LEFT_MID, 15, 0);

    //设置头像
    lv_obj_t *img = lv_img_create(avatar);
    lv_obj_set_size(img, 80, 80);
    lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/head_photo.png");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    //设置头像框
    lv_obj_t *head_frame = lv_img_create(avatar);
    lv_img_set_src(head_frame, "../lv_port_pc_vscode/assert/icon/head_frame.png");
    lv_obj_align(head_frame, LV_ALIGN_CENTER, 0, 0);

    //创建角色标识
    lv_obj_t *role_label = lv_label_create(item);
    lv_label_set_text(role_label, "未接来电");
    lv_obj_set_style_text_font(role_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(role_label, lv_color_hex(0xFF5C5C), 0);
    lv_obj_set_style_text_opa(role_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_align(role_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(role_label, item, LV_ALIGN_TOP_LEFT, 115, 20);

    //创建联系人名称标签
    lv_obj_t *name_label = lv_label_create(item);
    lv_label_set_text_fmt(name_label, "来源于V-%d", idx);
    lv_obj_set_style_text_font(name_label, fzlthr_24, 0);
    lv_obj_set_style_text_opa(name_label, LV_OPA_60, 0);
    lv_obj_set_style_text_color(name_label, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(name_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(name_label, role_label, LV_ALIGN_BOTTOM_LEFT, 0, 30);

    //创建通话间标签（放在右侧）
    lv_obj_t *time_label = lv_label_create(item);
    lv_label_set_text(time_label, "11:56");
    lv_obj_set_style_text_font(time_label, fzlthr_16, 0);
    lv_obj_set_style_text_opa(time_label, LV_OPA_60, 0);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xEBEBF5), 0);
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(time_label, item, LV_ALIGN_TOP_LEFT, 377, 20);

    //删除选项
    lv_obj_t *del_obj = lv_obj_create(hor_cont);
    lv_obj_set_size(del_obj, 95, 110);
    lv_obj_add_style(del_obj, &screen_style, 0);
    lv_obj_set_style_radius(del_obj, 47, 0);
    lv_obj_set_style_bg_opa(del_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(del_obj, lv_color_hex(0xFF5C5C), 0);
    lv_obj_add_flag(del_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(del_obj, lv_iterm_dalete_click_cb, LV_EVENT_CLICKED, hor_cont);

    lv_obj_t *del_right = lv_obj_create(del_obj);
    lv_obj_set_size(del_right, 45, 110);
    lv_obj_add_style(del_right, &screen_style, 0);
    lv_obj_set_style_bg_opa(del_right, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(del_right, lv_color_hex(0xFF5C5C), 0); 
    lv_obj_align(del_right, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_flag(del_right, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(del_right, lv_iterm_dalete_click_cb, LV_EVENT_CLICKED, hor_cont);

    lv_obj_t *del_label = lv_label_create(del_obj);
    lv_label_set_text(del_label, "删除");
    lv_obj_set_style_text_font(del_label, fzlthr_24, 0);
    lv_obj_set_style_text_color(del_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(del_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_align(del_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(del_label, LV_ALIGN_LEFT_MID, 20, 0);

    return;
}

static void lv_iterm_dalete_click_cb(lv_event_t *e)
{
    lv_obj_t *horcont = lv_event_get_user_data(e);
    lv_obj_del(horcont);
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&message_center_subject, PAGE_SWITCH_BACK);
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_WARN("Page switch event: %d", page_event);
    
    if (page_event == PAGE_SWITCH_NONE) return;
    
    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    switch_page->old_page = &message_center_page_info;
    
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
