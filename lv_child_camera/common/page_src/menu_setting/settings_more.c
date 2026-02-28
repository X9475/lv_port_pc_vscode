#include "../lv_switch_interface.h"
#include <stdio.h>

#define SETTINS_APP 3

lv_subject_t settings_more_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_obj_t *indicator = NULL;
static lv_obj_t *rotate_scale = NULL;
static lv_style_t screen_style;
static lv_style_t right_style;
static lv_style_t main_style;
static lv_style_t iterms_style;
static lv_style_t indicator_style;

typedef struct
{
    const char *name;
    const char *color_icon;
    const char *grey_icon;
} list_info_t;

typedef struct  
{
    int32_t angle;      //转动角度
    uint8_t rotate_dir; //转动方向, 1 up, 2 down
    uint32_t last_ycoord;//上一次的y坐标
} rotate_ctrl_t;
static rotate_ctrl_t g_rotate_ctrl = {0};

typedef struct
{
    bool init_flag;
    lv_timer_t *timer;
    lv_obj_t *mask;
    int index;
    bool knob_rotate;
} rotate_ctl_s;
static rotate_ctl_s g_rotate_ctl_s = {false, NULL, NULL, 0};
static int32_t y_start = 0;
static int32_t y_end = 0;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void scroll_item_create(lv_obj_t *cont, list_info_t *layout_info, int index);
static void set_color_app_style(int i, lv_obj_t *obj, list_info_t *iterm_ptr);
static void set_gray_app_style(lv_obj_t *obj, list_info_t *iterm_ptr);
static void scroll_item_event_cb(lv_event_t *e);
static void scroll_item_click_cb(lv_event_t *e);
static void page_back_event_cb(lv_event_t *e);
static lv_obj_t *rotate_disc_draw(lv_obj_t *cont);

//test旋钮转动菜单
typedef struct {
    int command;
} MenuCommand;
static MenuCommand g_cmd;

static void *input_thread(void* arg);
static void delete_mask_page(lv_timer_t *timer);
static void async_rotate_cb(void *cmd);
static int last_tabindex = 0;

static list_info_t settings_list[SETTINS_APP] = {
    {"拍摄设置", "../lv_port_pc_vscode/assert/icon/set_shooting_select.png", "../lv_port_pc_vscode/assert/icon/set_shooting_no_select.png"},
    {"个性化设置", "../lv_port_pc_vscode/assert/icon/set_personaliz_select.png", "../lv_port_pc_vscode/assert/icon/set_personaliz_no_select.png"},
    {"设备信息", "../lv_port_pc_vscode/assert/icon/set_equipment_select.png", "../lv_port_pc_vscode/assert/icon/set_equipment_no_select.png"}
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SHOOT,  //拍摄
    PAGE_SWITCH_INDVID, //个性化
    PAGE_SWITCH_INFO,   //设备信息
    PAGE_SWITCH_BACK    //返回
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
    settings_more_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&main_style);
    lv_style_reset(&iterms_style);
    lv_style_reset(&indicator_style);
    lv_style_reset(&screen_style);
    lv_style_reset(&right_style);
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

    static lv_grad_dsc_t grad2;
    grad2.dir = LV_GRAD_DIR_HOR;
    grad2.stops_count = 2;
    grad2.stops[0].color = lv_color_hex(0x000000);
    grad2.stops[0].opa = LV_OPA_TRANSP;
    grad2.stops[1].color = lv_color_hex(0x000000);
    grad2.stops[1].opa = LV_OPA_COVER;
    grad2.stops[0].frac = 0;
    grad2.stops[1].frac = 255;
    lv_style_init(&right_style);
    lv_style_copy(&right_style, &screen_style);
    lv_style_set_bg_grad(&right_style, &grad2);

    //main_style
    lv_style_init(&main_style);
    lv_style_set_arc_opa(&main_style, LV_OPA_TRANSP);

    //iterms_style
    lv_style_init(&iterms_style);
    lv_style_set_line_color(&iterms_style, lv_color_white());
    lv_style_set_line_opa(&iterms_style, LV_OPA_80);
    lv_style_set_line_width(&iterms_style, 2);
    lv_style_set_length(&iterms_style, 12);

    //indicator_style
    lv_style_init(&indicator_style);
    lv_style_set_line_color(&indicator_style, lv_color_white());
    lv_style_set_line_opa(&indicator_style, LV_OPA_80);
    lv_style_set_line_width(&indicator_style, 2);
    lv_style_set_length(&indicator_style, 12);
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

    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, 365, lv_pct(100));
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_style_border_color(cont_col, lv_color_white(), 0);
    // lv_obj_set_style_border_width(cont_col, 1, 0);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 80, 0);
    lv_obj_add_event_cb(cont_col, scroll_item_event_cb, LV_EVENT_SCROLL, NULL);

    //圆盘刻度
    rotate_scale = rotate_disc_draw(cont);
    //指示灯
    indicator = lv_img_create(cont);
    lv_img_set_src(indicator, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_blue.png");
    lv_obj_align_to(indicator, cont, LV_ALIGN_RIGHT_MID, -36, 0);

    for (uint8_t i = 0; i < SETTINS_APP; i++)
    {
        scroll_item_create(cont_col, &settings_list[i], i);
    }
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, last_tabindex), LV_ANIM_OFF);

    lv_obj_t *header = lv_label_create(cont);
    lv_label_set_text(header, "设置");
    lv_obj_set_style_text_font(header, fzlthb_30, 0);
    lv_obj_set_style_text_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align_to(header, back_btn, LV_ALIGN_OUT_RIGHT_MID, -8, 0);

    //右边渐变
    lv_obj_t *right_mask = lv_obj_create(cont);
    lv_obj_set_size(right_mask, 40, lv_pct(100));
    lv_obj_add_style(right_mask, &right_style, 0);
    lv_obj_align(right_mask, LV_ALIGN_RIGHT_MID, 0, 0);

    //起线程获取指令 test
    pthread_t tid;
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
            if (current_page->page_id == PAGE_FUNCTIONAL_MORE_SETTINGS)
            {
                if (!g_rotate_ctl_s.init_flag)
                {
                    //创建透明屏幕禁止屏幕响应
                    g_rotate_ctl_s.mask = lv_obj_create(settings_more_page_info.page);
                    lv_obj_remove_style_all(g_rotate_ctl_s.mask);
                    lv_obj_set_size(g_rotate_ctl_s.mask, lv_pct(100), lv_pct(100));
                    lv_obj_set_style_bg_opa(g_rotate_ctl_s.mask, LV_OPA_TRANSP, 0);
                    lv_obj_align(g_rotate_ctl_s.mask, LV_ALIGN_CENTER, 0, 0);
                    lv_obj_clear_flag(settings_more_page_info.page, LV_OBJ_FLAG_GESTURE_BUBBLE);
                    g_rotate_ctl_s.init_flag = true;
                    g_rotate_ctl_s.knob_rotate = true;

                    if (NULL == g_rotate_ctl_s.timer)
                    {
                        g_rotate_ctl_s.timer = lv_timer_create(delete_mask_page, 1000, NULL);
                    }

                    //获取当前中间项目索引
                    lv_obj_t *cont_col = lv_obj_get_child(settings_more_page_info.page, 0);
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

                lv_async_call(async_rotate_cb, &g_cmd.command);
                lv_timer_reset(g_rotate_ctl_s.timer);
            }
        }
    }

    return NULL;
}

static void rotate_anim_ready_cb(lv_anim_t *anim)
{
    lv_anim_del_all();
}

static void async_rotate_cb(void *cmd)
{
    static int current_index = 0;
    current_index = g_rotate_ctl_s.index;
    static int offset = 0; //1:正向，-1:反向

    //更新索引
    offset = *(int*)cmd > 1 ? 1 : -1;
    current_index += offset;
    bool rotate_flag = false;

    //边界检查
    if (current_index >= (SETTINS_APP - 1))
    {
        current_index = SETTINS_APP - 1;
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

    lv_obj_t *cont_col = lv_obj_get_child(settings_more_page_info.page, 0);
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
}

static void delete_mask_page(lv_timer_t *timer)
{
    printf("delete_mask_page\n");
    lv_obj_del(g_rotate_ctl_s.mask);
    lv_timer_del(g_rotate_ctl_s.timer);
    g_rotate_ctl_s.timer = NULL;
    g_rotate_ctl_s.init_flag = false;
    g_rotate_ctl_s.knob_rotate = false;
    lv_obj_add_flag(settings_more_page_info.page, LV_OBJ_FLAG_GESTURE_BUBBLE);

    return;
}
/***********************************************test end************************************************************/
static void scroll_item_create(lv_obj_t *cont, list_info_t *layout_info, int index)
{
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 300, 120);
    lv_obj_set_style_radius(btn, 65, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2A3534), 0);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_user_data(btn, (void *)(intptr_t)index);
    lv_obj_add_event_cb(btn, scroll_item_click_cb, LV_EVENT_ALL, NULL);

    lv_obj_t *circle = lv_img_create(btn);
    lv_obj_set_size(circle, 80, 80);
    lv_img_set_src(circle, "../lv_port_pc_vscode/assert/icon/purple_circle.png");
    lv_obj_align_to(circle, btn, LV_ALIGN_LEFT_MID, 20, 0);

    lv_obj_t *icon = lv_img_create(btn);
    lv_obj_set_size(icon, 50, 50);
    lv_img_set_src(icon, layout_info->grey_icon);
    lv_obj_align_to(icon, btn, LV_ALIGN_LEFT_MID, 35, 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, layout_info->name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, fzlthb_30, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 116, 0);
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_BACK);
}

static void scroll_item_click_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED)
    {
        //按下时自动滚动到居中
        lv_obj_scroll_to_view(obj, LV_ANIM_ON);

        lv_point_t point;
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        y_start = point.y;
    }
    else if(code == LV_EVENT_RELEASED)
    {
        int index = (int)(intptr_t)lv_obj_get_user_data(obj);

        lv_point_t point;
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        y_end = point.y;

        if (y_start != 0 && LV_ABS(y_end-y_start) > 5) {
            y_start = 0;
            return;
        }

        if (index == 0) {
            printf("进入拍摄设置\n");
            last_tabindex = 0;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_SHOOT);
        }
        else if (index == 1) {
            printf("进入个性化设置\n");
            last_tabindex = 1;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_INDVID);
        }
        else if (index == 2) {
            printf("进入设备信息\n");
            last_tabindex = 2;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_INFO);
        }
    }
}

static lv_obj_t *rotate_disc_draw(lv_obj_t *cont)
{
    lv_obj_t *scale = lv_scale_create(cont);
    lv_obj_set_size(scale, 330, 330);

    lv_scale_set_range(scale, 0, 40);
    lv_scale_set_rotation(scale, -90);
    lv_scale_set_label_show(scale, false);
    lv_scale_set_angle_range(scale, 360);
    lv_scale_set_total_tick_count(scale, 41);

    lv_obj_add_style(scale, &main_style, LV_PART_MAIN);
    lv_obj_add_style(scale, &iterms_style, LV_PART_ITEMS);
    lv_obj_add_style(scale, &indicator_style, LV_PART_INDICATOR);

    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_obj_align_to(scale, cont, LV_ALIGN_LEFT_MID, 455, 0);

    return scale;
}

static void set_color_app_style(int i, lv_obj_t *obj, list_info_t *iterm_ptr)
{
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    if ((i % 3) == 0) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x94AEF8), 0);
    } else if ((i % 3) == 1) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xAFF99C), 0);
    } else if ((i % 3) == 2){
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xCE94F8), 0);
    }

    lv_obj_t *circle = lv_obj_get_child(obj, 0);
    lv_obj_set_style_image_opa(circle, LV_OPA_COVER, 0);

    lv_obj_t *icon = lv_obj_get_child(obj, 1);
    lv_img_set_src(icon, iterm_ptr->color_icon);
    lv_obj_set_style_image_opa(icon, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_obj_get_child(obj, 2);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
}


static void set_gray_app_style(lv_obj_t *obj, list_info_t *iterm_ptr)
{
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2A3534), 0);

    lv_obj_t *circle = lv_obj_get_child(obj, 0);
    lv_obj_set_style_image_opa(circle, LV_OPA_30, 0);

    lv_obj_t *icon = lv_obj_get_child(obj, 1);
    lv_img_set_src(icon, iterm_ptr->grey_icon);
    lv_obj_set_style_image_opa(icon, LV_OPA_30, 0);

    lv_obj_t *label = lv_obj_get_child(obj, 2);
    lv_obj_set_style_text_opa(label, LV_OPA_60, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
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

static void scroll_item_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);

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

    for (uint32_t i = 0; i < lv_obj_get_child_count(cont); i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);
        
        int32_t y_center = lv_disp_get_ver_res(NULL) / 2;
        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = child_y_center - y_center;

        int32_t r_height = lv_obj_get_height(cont);
        uint32_t x_sqr = r_height * r_height - LV_ABS(diff_y) * LV_ABS(diff_y);
        lv_sqrt_res_t res;
        lv_sqrt(x_sqr, &res, 0x8000);
        int32_t translate_x = r_height - res.i;

        //设置旋转中心为右侧边缘中心点
        lv_obj_set_style_transform_pivot_x(child, 502, 0);
        lv_obj_set_style_transform_pivot_y(child, 205, 0);

        if (LV_ABS(diff_y) >= 50)
        {
            set_gray_app_style(child, &settings_list[i]);
            int32_t angle = -(diff_y) / 2;
            if (diff_y < 0)
            {
                lv_obj_set_style_translate_x(child, translate_x, 0);
                lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
            }
            else
            {
                lv_obj_set_style_translate_x(child, translate_x + 25, 0);
                lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
            }
        }
        else
        {
            //中心位置：无旋转，正常大小
            lv_obj_set_style_translate_x(child, 0, 0);
            lv_obj_set_style_transform_rotation(child, 0, LV_PART_MAIN);
            set_color_app_style(i, child, &settings_list[i]);
            set_indicator_light(i);
        }
    }
}

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_INFO("page switch event:%d", page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发
    
    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &settings_more_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_SHOOT:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_shoot_setting_info_get();
            break;
        case PAGE_SWITCH_INDVID:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_indvid_setting_info_get();
            break;
        case PAGE_SWITCH_INFO:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_equip_setting_info_get();
            break;
        case PAGE_SWITCH_BACK:
            switch_page->new_page = lv_stack_pop();
            if (switch_page->new_page->page_id == PAGE_FUNCTIONAL_MENU_SETTING)
            {
                switch_page->new_page = lv_stack_pop();
                lv_page_type_set(TYPE_FUNCTIONAL);
            }
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