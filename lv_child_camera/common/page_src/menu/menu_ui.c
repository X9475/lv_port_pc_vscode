#include "../lv_switch_interface.h"

#define APP_NUM     7

lv_subject_t menu_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t style_mask;

typedef struct
{
    const char *name;
    const char *color_icon;
    const char *grey_icon;
} lv_menu_dev_t;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_page_reserve_del(void);
static void scroll_app_item_event_cb(lv_event_t * e);
static void app_icon_event_cb(lv_event_t * e);
static void *lv_app_create(int i, lv_obj_t *cont, const char *name, const char *path);
static void set_gray_app_style(lv_obj_t *obj, lv_menu_dev_t *iterm_ptr);
static void set_color_app_style(int i, lv_obj_t *obj, lv_menu_dev_t *iterm_ptr);
static void set_indicator_light(int i);

static lv_menu_dev_t menu_app_list[APP_NUM] = {
    {"拍摄", "../lv_port_pc_vscode/assert/icon/photograph_icon_screenshot_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_screenshot.png"},
    {"AI问答", "../lv_port_pc_vscode/assert/icon/photograph_icon_ai_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_ai.png"},
    {"视频通话", "../lv_port_pc_vscode/assert/icon/photograph_icon_videocall_filled_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_videocall_filled.png"},
    {"留言板", "../lv_port_pc_vscode/assert/icon/photograph_icon_message_board_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_message_board.png"},
    {"闹钟提醒", "../lv_port_pc_vscode/assert/icon/photograph_icon_alarm_clock_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_alarm_clock.png"},
    {"相册", "../lv_port_pc_vscode/assert/icon/photograph_icon_album_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_album.png"},
    {"消息中心", "../lv_port_pc_vscode/assert/icon/photograph_icon_ring_filled_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_ring_filled.png"}
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SHOOT,          //拍摄
    PAGE_SWITCH_AI_ANSWER,      //AI问答
    PAGE_SWITCH_VIDEO_CALL,     //视频通话
    PAGE_SWITCH_MESSAGE_BOARD,  //留言板
    PAGE_SWITCH_ALARM_CLOCK,    //闹钟提醒
    PAGE_SWITCH_ALBUM,          //相册
    PAGE_SWITCH_MESSAGE_CENTER, //消息中心
    PAGE_SWITCH_BACK            //返回
};

static lv_page_info_t menu_page_info = {
    .page_id = PAGE_FUNCTIONAL_MENU,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_menu_info_get()
{
    return &menu_page_info;
}

static void lv_page_construct(void *this)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();
    //加入栈表
    // lv_stack_push(&menu_page_info);

    screen = lv_obj_create(act_screen);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    if (lv_page_type_get() != TYPE_NONE)
    {
        lv_page_type_set(TYPE_MENU);
    }

    menu_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&screen_style);
    lv_style_reset(&style_mask);
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

    //图层蒙板
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 3;
    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[0].opa = LV_OPA_90;
    grad.stops[1].color = lv_color_hex(0x000000);
    grad.stops[1].opa = LV_OPA_TRANSP;
    grad.stops[2].color = lv_color_hex(0x000000);
    grad.stops[2].opa = LV_OPA_90;
    grad.stops[0].frac = 0;
    grad.stops[1].frac = 127;
    grad.stops[2].frac = 255;
    lv_style_init(&style_mask);
    lv_style_copy(&style_mask, &screen_style);
    lv_style_set_bg_grad(&style_mask, &grad);
    lv_style_set_bg_grad_dir(&style_mask, LV_GRAD_DIR_VER);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&menu_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&menu_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&menu_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, 365, lv_pct(100));
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 80, 0);
    lv_obj_set_style_clip_corner(cont_col, true, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont_col, scroll_app_item_event_cb, LV_EVENT_SCROLL, NULL);

    lv_obj_t *image = NULL;
    for (uint8_t i = 0; i < APP_NUM; i++)
    {
        //创建子功能按钮
        lv_obj_t *btn = lv_app_create(i, cont_col, menu_app_list[i].name, menu_app_list[i].grey_icon);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_event_cb(btn, app_icon_event_cb, LV_EVENT_CLICKED, &menu_app_list[i]);
    }

    //绘制刻度圆盘
    lv_obj_t *scale = lv_img_create(cont);
    lv_img_set_src(scale, "../lv_port_pc_vscode/assert/icon/menu_knob_2x.png");
    lv_img_set_zoom(scale, 128);
    lv_obj_align_to(scale, cont, LV_ALIGN_LEFT_MID, 267, 0);

    //图层蒙板
    lv_obj_t *mask = lv_obj_create(cont);
    lv_obj_set_size(mask, 55, 330);
    lv_obj_align(mask, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(mask, &style_mask, 0);

    lv_obj_send_event(cont_col, LV_EVENT_SCROLL, NULL);
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, 0), LV_ANIM_OFF);

    return;
}

static void app_icon_event_cb(lv_event_t * e)
{
    lv_obj_t *app_obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_menu_dev_t *iterm_data = lv_event_get_user_data(e);

    lv_obj_scroll_to_view(app_obj, LV_ANIM_OFF);

    if (code == LV_EVENT_CLICKED) 
    {
        printf("===>点击: %s, 进入子菜单\n", iterm_data->name);
        if (lv_strcmp(iterm_data->name, "相册") == 0)
        {
            lv_subject_set_int(&menu_subject, PAGE_SWITCH_ALBUM);
        }
        else if(lv_strcmp(iterm_data->name, "拍摄") == 0)
        {
            lv_subject_set_int(&menu_subject, PAGE_SWITCH_SHOOT);
        }
        else if(lv_strcmp(iterm_data->name, "AI问答") == 0)
        {
            lv_subject_set_int(&menu_subject, PAGE_SWITCH_AI_ANSWER);
        }
    }
}

static void *lv_app_create(int i, lv_obj_t *cont, const char *name, const char *path)
{
    //背景
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 300, 120);
    lv_obj_set_style_radius(btn, 65, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2A3534), 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    // //圆形
    // lv_obj_t *image1 = lv_img_create(btn);
    // lv_obj_set_size(image1, 80, 80);
    // if ((i % 2) == 0) {
    //     lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/purple_circle.png");
    // } else {
    //     lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/green_circle.png");
    // }
    // lv_obj_align_to(image1, btn, LV_ALIGN_LEFT_MID, 20, 0);
    // //叠加图标
    // lv_obj_t *image2 = lv_img_create(btn);
    // lv_obj_set_size(image2, 50, 50);
    // lv_img_set_src(image2, path);
    // lv_obj_align_to(image2, btn, LV_ALIGN_LEFT_MID, 35, 0);
    //文字
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, fzlthb_30, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 116, 0);

    return btn;
}

static void set_gray_app_style(lv_obj_t *obj, lv_menu_dev_t *iterm_ptr)
{
    //背景
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2A3534), 0);
    //圆形
    lv_obj_t *image1 = lv_obj_get_child(obj, 0);
    lv_obj_set_style_image_opa(image1, LV_OPA_30, 0);
    //叠加图标
    lv_obj_t *image2 = lv_obj_get_child(obj, 1);
    lv_img_set_src(image2, iterm_ptr->grey_icon);
    lv_obj_set_style_image_opa(image2, LV_OPA_30, 0);
    //文字
    lv_obj_t *label = lv_obj_get_child(obj, 2);
    lv_obj_set_style_text_opa(label, LV_OPA_60, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
}

static void set_color_app_style(int i, lv_obj_t *obj, lv_menu_dev_t *iterm_ptr)
{
    //背景
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    if ((i % 2) == 0) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xCE94F8), 0);
    } else {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xAFF99C), 0);
    }
    //圆形
    lv_obj_t *image1 = lv_obj_get_child(obj, 0);
    lv_obj_set_style_image_opa(image1, LV_OPA_COVER, 0);
    //叠加图标
    lv_obj_t *image2 = lv_obj_get_child(obj, 1);
    lv_img_set_src(image2, iterm_ptr->color_icon);
    lv_obj_set_style_image_opa(image2, LV_OPA_COVER, 0);
    //文字
    lv_obj_t *label = lv_obj_get_child(obj, 2);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
}

static void set_indicator_light(int i)
{
    //指示灯
    lv_obj_t *image = lv_img_create(screen);
    // lv_obj_set_size(image, 80, 80);
    if ((i % 2) == 0) {
        lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_purple.png");
    } else {
        lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_green.png");
    }
    lv_obj_align_to(image, screen, LV_ALIGN_RIGHT_MID, -36, -2);
}

static void scroll_app_item_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    uint32_t child_cnt = lv_obj_get_child_count(cont);
    for (uint32_t i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = child_y_center - cont_y_center;
        // printf("i = %d, y = %d, %d\n", i, LV_ABS(diff_y), diff_y);

        int32_t r = lv_obj_get_height(cont);
        uint32_t x_sqr = r * r - LV_ABS(diff_y) * LV_ABS(diff_y);

        lv_sqrt_res_t res;
        lv_sqrt(x_sqr, &res, 0x8000);
        int32_t x = r - res.i;

        // 设置旋转中心为右侧边缘中间点
        lv_obj_set_style_transform_pivot_x(child, 502, 0);
        lv_obj_set_style_transform_pivot_y(child, 205, 0);
        if (LV_ABS(diff_y) >= 50)//阈值范围可调
        {
            // set_gray_app_style(child, &menu_app_list[i]);
            int32_t angle = -(diff_y) / 2;
            if (diff_y < 0)
            {
                lv_obj_set_style_translate_x(child, x, 0);
                lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
            }
            else
            {
                lv_obj_set_style_translate_x(child, x + 25, 0);
                lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
            }
        }
        else
        {
            set_indicator_light(i);
            // set_color_app_style(i, child, &menu_app_list[i]);
            lv_obj_set_style_translate_x(child, x, 0);
            lv_obj_set_style_transform_rotation(child, 0, LV_PART_MAIN);
        }
    }
}

static void lv_page_reserve_del(void)
{
    if (menu_page_info.reserved != NULL)
    {
        lv_page_info_pt reserved = (lv_page_info_pt)menu_page_info.reserved;
        printf("[%s:%d] -- delete page id: %d\n", __FILE__, __LINE__, reserved->page_id);
        reserved->destruct_cb();
        if (reserved->page) lv_obj_del(reserved->page);
        menu_page_info.reserved = NULL;
        // lv_stack_pop();//移除栈顶元素
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
    switch_page->old_page = &menu_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_ALBUM:
            lv_page_reserve_del();
            switch_page->new_page = lv_page_album_get();
            break;
        case PAGE_SWITCH_SHOOT:
            lv_page_reserve_del();
            switch_page->new_page = lv_page_shooting_photo_get();
            break;
        case PAGE_SWITCH_AI_ANSWER:
            lv_page_reserve_del();
            lv_stack_push(&menu_page_info);
            switch_page->new_page = lv_page_aidialog_info_get();
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