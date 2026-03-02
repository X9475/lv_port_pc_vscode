#include "../lv_switch_interface.h"
#include <stdio.h>

#define ITEM_NUM     6

lv_subject_t menu_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_obj_t *indicator = NULL;
static lv_obj_t *rotate_scale = NULL;
static lv_style_t screen_style;
static lv_style_t bottom_style;
static lv_style_t left_style;
static lv_style_t right_style;
static lv_style_t main_style;
static lv_style_t iterms_style;
static lv_style_t indicator_style;

typedef struct  
{
    int32_t angle;      //转动角度
    uint8_t rotate_dir; //转动方向, 1 left, 2 right
    uint32_t last_xcoord;//上一次的X坐标
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
static int32_t x_start = 0;
static int32_t x_end = 0;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void scroll_item_event_cb(lv_event_t *e);
static void scroll_item_create(lv_obj_t *cont, const char *path, int index);
static void scroll_item_click_cb(lv_event_t *e);
static lv_obj_t *rotate_disc_draw(lv_obj_t *cont);

//test旋钮转动菜单
typedef struct {
    int command;
} MenuCommand;
static MenuCommand g_cmd;

static lv_mutex_t mutex;
static void *input_thread(void* arg);
static void delete_mask_page(lv_timer_t *timer);
static void async_rotate_cb(void *cmd);
static int last_tabindex = 0;

static const char *menu_item[ITEM_NUM] = {
    "../lv_port_pc_vscode/assert/icon/menu_shoot.png",
    "../lv_port_pc_vscode/assert/icon/menu_ai.png",
    "../lv_port_pc_vscode/assert/icon/menu_video.png",
    "../lv_port_pc_vscode/assert/icon/menu_album.png",
    "../lv_port_pc_vscode/assert/icon/menu_msg.png",
    "../lv_port_pc_vscode/assert/icon/menu_setting.png",
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SHOOT,          //拍摄
    PAGE_SWITCH_AI_ANSWER,      //AI问答
    PAGE_SWITCH_VIDEO_CALL,     //视频通话
    // PAGE_SWITCH_MESSAGE_BOARD,  //留言板
    // PAGE_SWITCH_ALARM_CLOCK,    //闹钟提醒
    PAGE_SWITCH_ALBUM,          //相册
    PAGE_SWITCH_MESSAGE_CENTER, //消息中心
    PAGE_SWITCH_SETTINGS,       //设置
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
    g_rotate_ctrl.last_xcoord = 0xffffffff;

    //绘制当前页面
    lv_page_load(screen);
    lv_page_type_set(TYPE_MENU);

    menu_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&main_style);
    lv_style_reset(&iterms_style);
    lv_style_reset(&indicator_style);
    lv_style_reset(&screen_style);
    lv_style_reset(&bottom_style);
    lv_style_reset(&left_style);
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

    static lv_grad_dsc_t grad0;
    grad0.dir = LV_GRAD_DIR_VER;
    grad0.stops_count = 2;
    grad0.stops[0].color = lv_color_hex(0x000000);
    grad0.stops[0].opa = LV_OPA_TRANSP;
    grad0.stops[1].color = lv_color_hex(0x000000);
    grad0.stops[1].opa = LV_OPA_COVER;
    grad0.stops[0].frac = 0;
    grad0.stops[1].frac = 255;
    lv_style_init(&bottom_style);
    lv_style_copy(&bottom_style, &screen_style);
    lv_style_set_bg_grad(&bottom_style, &grad0);

    static lv_grad_dsc_t grad1;
    grad1.dir = LV_GRAD_DIR_HOR;
    grad1.stops_count = 2;
    grad1.stops[0].color = lv_color_hex(0x000000);
    grad1.stops[0].opa = LV_OPA_COVER;
    grad1.stops[1].color = lv_color_hex(0x000000);
    grad1.stops[1].opa = LV_OPA_TRANSP;
    grad1.stops[0].frac = 0;
    grad1.stops[1].frac = 255;
    lv_style_init(&left_style);
    lv_style_copy(&left_style, &screen_style);
    lv_style_set_bg_grad(&left_style, &grad1);

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
    //静态背景
    lv_obj_t *backimg = lv_img_create(cont);
    lv_img_set_src(backimg, "../lv_port_pc_vscode/assert/icon/menu_backimg.png");
    lv_obj_align(backimg, LV_ALIGN_CENTER, 0, 0);

    //滚动区域
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 325);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_HOR);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_ROW);
    // lv_obj_set_style_border_color(cont_col, lv_color_white(), 0);
    // lv_obj_set_style_border_width(cont_col, 1, 0);
    lv_obj_set_style_bg_opa(cont_col, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_x(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_align(cont_col, LV_ALIGN_TOP_MID, 0, 55);
    lv_obj_add_event_cb(cont_col, scroll_item_event_cb, LV_EVENT_SCROLL, NULL);

    //圆盘刻度
    rotate_scale = rotate_disc_draw(cont);
    //指示灯
    indicator = lv_img_create(cont);
    lv_img_set_src(indicator, "../lv_port_pc_vscode/assert/icon/photograph_icon_guide_blue.png");
    lv_image_set_rotation(indicator, -900);
    lv_obj_align_to(indicator, cont, LV_ALIGN_BOTTOM_MID, 0, -28);

    for (uint8_t i = 0; i < ITEM_NUM; i++)
    {
        scroll_item_create(cont_col, menu_item[i], i);
    }
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, last_tabindex), LV_ANIM_OFF);

    //底部渐变
    lv_obj_t *bottom_mask = lv_obj_create(cont);
    lv_obj_set_size(bottom_mask, lv_pct(100), 30);
    lv_obj_add_style(bottom_mask, &bottom_style, 0);
    lv_obj_align(bottom_mask, LV_ALIGN_BOTTOM_MID, 0, 0);
    //左边渐变
    lv_obj_t *left_mask = lv_obj_create(cont);
    lv_obj_set_size(left_mask, 40, lv_pct(100));
    lv_obj_add_style(left_mask, &left_style, 0);
    lv_obj_align(left_mask, LV_ALIGN_LEFT_MID, 0, 0);
    //右边渐变
    lv_obj_t *right_mask = lv_obj_create(cont);
    lv_obj_set_size(right_mask, 40, lv_pct(100));
    lv_obj_add_style(right_mask, &right_style, 0);
    lv_obj_align(right_mask, LV_ALIGN_RIGHT_MID, 0, 0);

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
                    for (int i = 0; i < lv_obj_get_child_cnt(cont_col); i++)
                    {
                        lv_obj_t *child = lv_obj_get_child(cont_col, i);
                        lv_area_t child_a;
                        lv_obj_get_coords(child, &child_a);
                        int32_t child_x_center = child_a.x1 + lv_area_get_width(&child_a) / 2;
                        if (LV_ABS(child_x_center - 251) < 5)
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

static void rotate_anim_ready_cb(lv_anim_t *anim)
{
    lv_anim_del_all();
}

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
    if (current_index >= (ITEM_NUM - 1))
    {
        current_index = ITEM_NUM - 1;
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

static void anim_ready_cb(lv_anim_t *a)
{
    if (x_start != 0 && LV_ABS(x_end-x_start) > 5)
    {
        x_start = 0;
        return;
    }

    int index = (int)(intptr_t)(a->user_data);
    if(index == 0) {
        printf("进入拍摄\n");
        last_tabindex = 0;
        lv_subject_set_int(&menu_subject, PAGE_SWITCH_SHOOT);
    }
    else if(index == 1) {
        printf("进入AI问答\n");
        last_tabindex = 1;
        lv_subject_set_int(&menu_subject, PAGE_SWITCH_AI_ANSWER);
    }
    else if (index == 3) {
        printf("进入相册\n");
        last_tabindex = 3;
        lv_subject_set_int(&menu_subject, PAGE_SWITCH_ALBUM);
    }
    else if (index == 4) {
        printf("进入消息中心\n");
        last_tabindex = 4;
        lv_subject_set_int(&menu_subject, PAGE_SWITCH_MESSAGE_CENTER);
    }
    else if (index == 5) {
        printf("进入设置\n");
        last_tabindex = 5;
        lv_subject_set_int(&menu_subject, PAGE_SWITCH_SETTINGS);
    }
}

static void scroll_item_create(lv_obj_t *cont, const char *path, int index)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_set_size(obj, 216, 255);
    lv_obj_add_style(obj, &screen_style, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *image = lv_img_create(obj);
    lv_img_set_src(image, path);
    lv_obj_align(image, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_user_data(obj, (void *)(intptr_t)index);
    lv_obj_add_event_cb(obj, scroll_item_click_cb, LV_EVENT_ALL, image);
    return;
}

static void scroll_item_click_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *image = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED)
    {
        //按下时自动滚动到居中
        lv_obj_scroll_to_view(obj, LV_ANIM_ON);

        //按下效果：快速缩小
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, image);
        lv_anim_set_values(&a, 256, 230);
        lv_anim_set_time(&a, 80);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
        lv_anim_start(&a);

        lv_point_t point;
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        x_start = point.x;
    }
    else if(code == LV_EVENT_RELEASED)
    {
        int index = (int)(intptr_t)lv_obj_get_user_data(obj);

        lv_point_t point;
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        x_end = point.x;

        //释放效果：弹性恢复
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, image);
        lv_anim_set_values(&a, 230, 256);
        lv_anim_set_time(&a, 120);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
        lv_anim_set_ready_cb(&a, anim_ready_cb);
        lv_anim_set_user_data(&a, (void *)(intptr_t)index);
        lv_anim_start(&a);
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
    lv_obj_align_to(scale, cont, LV_ALIGN_BOTTOM_MID, 0, 292);

    return scale;
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

    if (g_rotate_ctrl.last_xcoord == 0xffffffff)
    {
        g_rotate_ctrl.last_xcoord = first_a.x1;
    }
    g_rotate_ctrl.rotate_dir = first_a.x1 > g_rotate_ctrl.last_xcoord? 2 : 1;
    g_rotate_ctrl.last_xcoord = first_a.x1;

    //控制转盘转动
    if (!g_rotate_ctl_s.knob_rotate)
    {
        if (g_rotate_ctrl.rotate_dir == 2)
            g_rotate_ctrl.angle = g_rotate_ctrl.angle - 2;
        else if (g_rotate_ctrl.rotate_dir == 1)
            g_rotate_ctrl.angle = g_rotate_ctrl.angle + 2;

        if (LV_ABS(g_rotate_ctrl.angle) != 2) {//排除首次角度变动
            lv_scale_set_rotation(rotate_scale, g_rotate_ctrl.angle);
        }
    }

    //滚动容器旋转、偏移、缩放
    for (uint32_t i = 0; i < lv_obj_get_child_count(cont); i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t x_center = lv_disp_get_hor_res(NULL) / 2;
        int32_t child_x_center = child_a.x1 + lv_area_get_width(&child_a) / 2;
        int32_t diff_x = child_x_center - x_center;

        //计算距离中心的相对位置（-1到1）
        float relative_pos = (float)diff_x / x_center;
        //设置旋转中心为对象中心
        int32_t pivot_x = lv_area_get_width(&child_a) / 2;
        int32_t pivot_y = lv_area_get_height(&child_a) / 2;
        lv_obj_set_style_transform_pivot_x(child, pivot_x, 0);
        lv_obj_set_style_transform_pivot_y(child, pivot_y, 0);

        if (LV_ABS(diff_x) >= 5)
        {
            //平滑的旋转角度（基于距离的非线性变化）
            int32_t angle = relative_pos * 150;
            //平滑的缩放效果（中心最大，边缘最小）- 修改为256-170范围
            int32_t scale = 256 - (int32_t)(LV_ABS(relative_pos) * 86);
            //添加Y轴偏移增加3D效果
            int32_t translate_y = (int32_t)(LV_ABS(relative_pos) * 100);

            lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
            lv_obj_set_style_transform_scale(child, scale, LV_PART_MAIN);
            lv_obj_set_style_translate_y(child, translate_y, 0);
        }
        else
        {
            //中心位置：无旋转，正常大小
            lv_obj_set_style_translate_y(child, 0, 0);
            lv_obj_set_style_transform_rotation(child, 0, LV_PART_MAIN);
            lv_obj_set_style_transform_scale(child, 256, LV_PART_MAIN);
            set_indicator_light(i);
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
    switch_page->old_page = &menu_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_MESSAGE_CENTER:
            lv_stack_push(&menu_page_info);
            switch_page->new_page = lv_page_message_center_get();
            break;
        case PAGE_SWITCH_ALBUM:
            lv_stack_push(&menu_page_info);
            switch_page->new_page = lv_page_album_get();
            break;
        case PAGE_SWITCH_SHOOT:
            lv_stack_push(&menu_page_info);
            switch_page->new_page = lv_page_shooting_photo_get();
            break;
        case PAGE_SWITCH_AI_ANSWER:
            lv_stack_push(&menu_page_info);
            switch_page->new_page = lv_page_aidialog_info_get();
            break;
        case PAGE_SWITCH_SETTINGS:
            lv_stack_push(&menu_page_info);
            switch_page->new_page = lv_page_settings_more_info_get();
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