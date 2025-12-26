#include "../lv_switch_interface.h"
#include <stdio.h>

#define APP_NUM     5

lv_subject_t menu_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_obj_t *indicator = NULL;
static lv_obj_t *rotate_scale = NULL;
static lv_style_t screen_style;
static lv_style_t style_mask;
static lv_style_t main_line_style;
static lv_style_t iterms_style;
static lv_style_t indicator_style;

typedef struct  
{
    int32_t angle;      //转动角度
    uint8_t rotate_dir; //转动方向, 1 up, 2 down
    uint32_t last_ycoord;//上一次的Y坐标
} rotate_ctrl_t;
static rotate_ctrl_t g_rotate_ctrl = {0};

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
// static void lv_page_reserve_del(void);
static void scroll_app_item_event_cb(lv_event_t * e);
static void app_icon_event_cb(lv_event_t * e);
static void *lv_app_create(int i, lv_obj_t *cont, const char *name, const char *path);
static void set_gray_app_style(lv_obj_t *obj, lv_menu_dev_t *iterm_ptr);
static void set_color_app_style(int i, lv_obj_t *obj, lv_menu_dev_t *iterm_ptr);
static void set_indicator_light(int i);
static lv_obj_t *rotate_disc_draw(lv_obj_t *cont);

//test旋钮转动菜单
typedef struct {
    int command;
} MenuCommand;
static MenuCommand g_cmd;

typedef struct
{
    bool init_flag;
    lv_timer_t *timer;
    lv_obj_t *mask;
    int index;
    bool knob_rotate;
} rotate_ctl_s;
static rotate_ctl_s g_rotate_ctl_s = {false, NULL, NULL, 0};

static lv_mutex_t mutex;
static void *input_thread(void* arg);
static void delete_mask_page(lv_timer_t *timer);
static void async_rotate_cb(void *cmd);
static int last_tabindex = 0;

static lv_menu_dev_t menu_app_list[APP_NUM] = {
    {"拍摄", "../lv_port_pc_vscode/assert/icon/photograph_icon_screenshot_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_screenshot.png"},
    {"AI问答", "../lv_port_pc_vscode/assert/icon/photograph_icon_ai_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_ai.png"},
    {"视频通话", "../lv_port_pc_vscode/assert/icon/photograph_icon_videocall_filled_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_videocall_filled.png"},
    /*{"留言板", "../lv_port_pc_vscode/assert/icon/photograph_icon_message_board_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_message_board.png"},
    {"闹钟提醒", "../lv_port_pc_vscode/assert/icon/photograph_icon_alarm_clock_black.png", "../lv_port_pc_vscode/assert/icon/photograph_icon_alarm_clock.png"},*/
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

    g_rotate_ctl_s.init_flag = false;
    g_rotate_ctl_s.timer = NULL;
    g_rotate_ctl_s.mask = NULL;
    g_rotate_ctl_s.index = 0;
    g_rotate_ctl_s.knob_rotate = false;

    g_rotate_ctrl.rotate_dir = 0;
    g_rotate_ctrl.angle = 0;
    g_rotate_ctrl.last_ycoord = 0xffffffff;

    //绘制当前页面
    lv_page_load(screen);
    lv_page_type_set(TYPE_MENU);

    menu_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&main_line_style);
    lv_style_reset(&iterms_style);
    lv_style_reset(&indicator_style);
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

    //main_line_style
    lv_style_init(&main_line_style);
    lv_style_set_arc_opa(&main_line_style, LV_OPA_TRANSP);

    //iterms_style
    lv_style_init(&iterms_style);
    lv_style_set_line_color(&iterms_style, lv_color_white());
    lv_style_set_line_width(&iterms_style, 1);

    //indicator_style
    lv_style_init(&indicator_style);
    lv_style_set_line_color(&indicator_style, lv_color_white());
    lv_style_set_line_width(&indicator_style, 1);
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

    //绘制转动圆盘
    rotate_scale = rotate_disc_draw(cont);
    //指示灯
    indicator = lv_img_create(cont);
    lv_img_set_src(indicator, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_blue.png");
    lv_obj_align_to(indicator, cont, LV_ALIGN_RIGHT_MID, -36, 0);

    lv_obj_t *image = NULL;
    for (uint8_t i = 0; i < APP_NUM; i++)
    {
        //创建子功能按钮
        lv_obj_t *btn = lv_app_create(i, cont_col, menu_app_list[i].name, menu_app_list[i].grey_icon);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_event_cb(btn, app_icon_event_cb, LV_EVENT_CLICKED, &menu_app_list[i]);
    }
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, last_tabindex), LV_ANIM_OFF);

    //绘制刻度圆盘
    // lv_obj_t *scale = lv_img_create(cont);
    // lv_img_set_src(scale, "../lv_port_pc_vscode/assert/icon/menu_knob_2x.png");
    // lv_img_set_zoom(scale, 128);
    // lv_obj_align_to(scale, cont, LV_ALIGN_LEFT_MID, 267, 0);

    //图层蒙板
    lv_obj_t *mask = lv_obj_create(cont);
    lv_obj_set_size(mask, 55, 330);
    lv_obj_align(mask, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(mask, &style_mask, 0);

    //顶部滑动触发区域
    lv_obj_t *gesture_area = lv_obj_create(cont);
    lv_obj_set_size(gesture_area, lv_pct(100), 30);
    lv_obj_align(gesture_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(gesture_area, &screen_style, 0);
    lv_obj_set_style_bg_opa(gesture_area, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(gesture_area, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(gesture_area, LV_OBJ_FLAG_SCROLLABLE);

    //起线程获取指令 test
    pthread_t tid;
    lv_mutex_init(&mutex);
    pthread_create(&tid, NULL, input_thread, NULL);
    return;
}

/***********************************************test start************************************************************/
static void* input_thread(void* arg)
{
    while(1)
    {
        char buf[64];
        if(fgets(buf, sizeof(buf), stdin))
        {
            sscanf(buf, "%d", &g_cmd.command);
            printf("input cmd: %d\n", g_cmd.command);

            lv_page_info_current_pt current_page = lv_current_page_info_get();
            if (current_page->page_id == PAGE_FUNCTIONAL_MENU)
            {
                if (!g_rotate_ctl_s.init_flag)
                {
                    //创建透明屏幕禁止屏幕响应
                    g_rotate_ctl_s.mask = lv_obj_create(menu_page_info.page);
                    lv_obj_remove_style_all(g_rotate_ctl_s.mask);
                    lv_obj_set_size(g_rotate_ctl_s.mask, lv_pct(100), lv_pct(100));
                    lv_obj_set_style_bg_opa(g_rotate_ctl_s.mask, LV_OPA_TRANSP, 0);
                    lv_obj_align(g_rotate_ctl_s.mask, LV_ALIGN_CENTER, 0, 0);
                    lv_obj_clear_flag(menu_page_info.page, LV_OBJ_FLAG_GESTURE_BUBBLE);
                    g_rotate_ctl_s.init_flag = true;
                    g_rotate_ctl_s.knob_rotate = true;

                    if (NULL == g_rotate_ctl_s.timer)
                    {
                        g_rotate_ctl_s.timer = lv_timer_create(delete_mask_page, 2000, NULL);
                    }

                    //获取当前中间项目索引
                    lv_obj_t *cont_col = lv_obj_get_child(menu_page_info.page, 0);
                    const int child_count = lv_obj_get_child_cnt(cont_col);
                    for (int i = 0; i < child_count; i++)
                    {
                        lv_obj_t *child = lv_obj_get_child(cont_col, i);
                        lv_area_t child_a;
                        lv_obj_get_coords(child, &child_a);
                        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
                        if (LV_ABS(child_y_center - 205) < 5) 
                        {
                            g_rotate_ctl_s.index = i;
                            break;
                        }
                    }
                }

                lv_mutex_lock(&mutex);
                lv_async_call(async_rotate_cb, &g_cmd.command);
                lv_mutex_unlock(&mutex);

                lv_timer_reset(g_rotate_ctl_s.timer);
            }
        }
    }

    return NULL;
}

static void rotate_anim_ready_cb(lv_anim_t *anim);
static void async_rotate_cb(void *cmd)
{
    lv_mutex_lock(&mutex);
    static int current_index = 0;
    current_index = g_rotate_ctl_s.index;
    static int offset = 0; //1:正向，-1:反向

    //更新索引
    offset = *(int*)cmd > 1 ? 1 : -1;
    current_index += offset;
    bool rotate_flag = false;

    //边界检查
    if (current_index >= (APP_NUM - 1))
    {
        current_index = APP_NUM - 1;
    } 
    else if (current_index <= 0)
    {
        current_index = 0;
    }

    if (g_rotate_ctl_s.index != current_index)
    {
        rotate_flag = true;
        g_rotate_ctl_s.index = current_index;
    }

    lv_obj_t *cont_col = lv_obj_get_child(menu_page_info.page, 0);
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, current_index), LV_ANIM_OFF);

    if (rotate_flag)
    {
        if (*(int*)cmd > 1)
        {
            //逆时针旋转动画
            lv_anim_t a_rotate;
            lv_anim_init(&a_rotate);
            lv_anim_set_var(&a_rotate, rotate_scale);
            lv_anim_set_exec_cb(&a_rotate, (lv_anim_exec_xcb_t)lv_scale_set_rotation);
            lv_anim_set_values(&a_rotate, g_rotate_ctrl.angle, g_rotate_ctrl.angle - 10);
            lv_anim_set_time(&a_rotate, 500);
            lv_anim_set_path_cb(&a_rotate, lv_anim_path_ease_out);
            lv_anim_set_ready_cb(&a_rotate, rotate_anim_ready_cb);
            lv_anim_start(&a_rotate);
            g_rotate_ctrl.angle -= 10;
        }
        else
        {
            //顺时针旋转动画
            lv_anim_t a_rotate;
            lv_anim_init(&a_rotate);
            lv_anim_set_var(&a_rotate, rotate_scale);
            lv_anim_set_exec_cb(&a_rotate, (lv_anim_exec_xcb_t)lv_scale_set_rotation);
            lv_anim_set_values(&a_rotate, g_rotate_ctrl.angle, g_rotate_ctrl.angle + 10);
            lv_anim_set_time(&a_rotate, 500);
            lv_anim_set_path_cb(&a_rotate, lv_anim_path_ease_out);
            lv_anim_set_ready_cb(&a_rotate, rotate_anim_ready_cb);
            lv_anim_start(&a_rotate);
            g_rotate_ctrl.angle += 10;
        }
    }
    lv_mutex_unlock(&mutex);
    return;
}

static void rotate_anim_ready_cb(lv_anim_t *anim)
{
    lv_anim_del_all();
}

static void delete_mask_page(lv_timer_t *timer)
{
    printf("delete_mask_page\n");
    lv_obj_del(g_rotate_ctl_s.mask);
    lv_timer_del(g_rotate_ctl_s.timer);
    g_rotate_ctl_s.timer = NULL;
    g_rotate_ctl_s.init_flag = false;
    g_rotate_ctl_s.knob_rotate = false;
    lv_obj_add_flag(menu_page_info.page, LV_OBJ_FLAG_GESTURE_BUBBLE);

    return;
}
/***********************************************test end************************************************************/

static void app_icon_event_cb(lv_event_t * e)
{
    lv_obj_t *app_obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_menu_dev_t *iterm_data = lv_event_get_user_data(e);

    lv_obj_scroll_to_view(app_obj, LV_ANIM_OFF);

    if (code == LV_EVENT_CLICKED) 
    {
        printf("===>点击: %s, 进入子菜单\n", iterm_data->name);
        if(lv_strcmp(iterm_data->name, "拍摄") == 0)
        {
            last_tabindex = 0;
            lv_subject_set_int(&menu_subject, PAGE_SWITCH_SHOOT);
        }
        else if(lv_strcmp(iterm_data->name, "AI问答") == 0)
        {
            last_tabindex = 1;
            lv_subject_set_int(&menu_subject, PAGE_SWITCH_AI_ANSWER);
        }
        else if (lv_strcmp(iterm_data->name, "相册") == 0)
        {
            last_tabindex = 3;
            lv_subject_set_int(&menu_subject, PAGE_SWITCH_ALBUM);
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

    //圆形
    lv_obj_t *image1 = lv_img_create(btn);
    lv_obj_set_size(image1, 80, 80);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/purple_circle.png");
    lv_obj_align_to(image1, btn, LV_ALIGN_LEFT_MID, 20, 0);

    //叠加图标
    lv_obj_t *image2 = lv_img_create(btn);
    lv_obj_set_size(image2, 50, 50);
    lv_img_set_src(image2, path);
    lv_obj_align_to(image2, btn, LV_ALIGN_LEFT_MID, 35, 0);

    //文字
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, fzlthb_30, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 116, 0);

    return btn;
}

static lv_obj_t *rotate_disc_draw(lv_obj_t *cont)
{
    lv_obj_t *scale = lv_scale_create(cont);
    lv_obj_set_size(scale, 330, 330);

    lv_scale_set_label_show(scale, false);
    lv_scale_set_total_tick_count(scale, 60);
    lv_obj_set_style_length(scale, 8, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 8, LV_PART_INDICATOR);
    lv_scale_set_range(scale, 0, 60);
    lv_scale_set_angle_range(scale, 360);
    lv_scale_set_rotation(scale, 180);

    lv_scale_set_label_show(scale, true);

    lv_obj_add_style(scale, &iterms_style, LV_PART_ITEMS);
    lv_obj_add_style(scale, &main_line_style, LV_PART_MAIN);
    lv_obj_add_style(scale, &indicator_style, LV_PART_INDICATOR);

    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(scale, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scale, lv_color_black(), 0);
    lv_obj_set_style_radius(scale, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(scale, true, 0);
    lv_obj_align_to(scale, cont, LV_ALIGN_LEFT_MID, 445, 0);

    return scale;
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
    if ((i % 3) == 0) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x94AEF8), 0);
    } else if ((i % 3) == 1) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xAFF99C), 0);
    } else if ((i % 3) == 2){
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xCE94F8), 0);
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
    if ((i % 3) == 0) {
        lv_img_set_src(indicator, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_blue.png");
    } else if ((i % 3) == 1) {
        lv_img_set_src(indicator, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_green.png");
    } else if ((i % 3) == 2) {
        lv_img_set_src(indicator, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_purple.png");
    }
}

static void scroll_app_item_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_obj_t *first = lv_obj_get_child(cont, 0);
    lv_area_t first_a;
    lv_obj_get_coords(first, &first_a);

    if (g_rotate_ctrl.last_ycoord == 0xffffffff)
    {
        g_rotate_ctrl.last_ycoord = first_a.y1;
    }
    g_rotate_ctrl.rotate_dir = first_a.y1 > g_rotate_ctrl.last_ycoord? 2 : 1;
    g_rotate_ctrl.last_ycoord = first_a.y1;

    //控制转盘转动
    if (!g_rotate_ctl_s.knob_rotate)
    {
        if (g_rotate_ctrl.rotate_dir == 2)
            g_rotate_ctrl.angle = g_rotate_ctrl.angle - 2;
        else if (g_rotate_ctrl.rotate_dir == 1)
            g_rotate_ctrl.angle = g_rotate_ctrl.angle + 2;

        lv_scale_set_rotation(rotate_scale, g_rotate_ctrl.angle);
    }

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
            set_gray_app_style(child, &menu_app_list[i]);
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
            set_color_app_style(i, child, &menu_app_list[i]);
            lv_obj_set_style_translate_x(child, x, 0);
            lv_obj_set_style_transform_rotation(child, 0, LV_PART_MAIN);
        }
    }
}

// static void lv_page_reserve_del(void)
// {
//     if (menu_page_info.reserved != NULL)
//     {
//         lv_page_info_pt reserved = (lv_page_info_pt)menu_page_info.reserved;
//         printf("[%s:%d] -- delete page id: %d\n", __FILE__, __LINE__, reserved->page_id);
//         reserved->destruct_cb();
//         if (reserved->page) lv_obj_del(reserved->page);
//         menu_page_info.reserved = NULL;
//         // lv_stack_pop();//移除栈顶元素
//     }
// }

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
            // lv_page_reserve_del();
            switch_page->new_page = lv_page_album_get();
            break;
        case PAGE_SWITCH_SHOOT:
            // lv_page_reserve_del();
            switch_page->new_page = lv_page_shooting_photo_get();
            break;
        case PAGE_SWITCH_AI_ANSWER:
            // lv_page_reserve_del();
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