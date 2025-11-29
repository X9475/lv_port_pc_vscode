#include "../lv_switch_interface.h"

#define SAVER_NUM       4

lv_subject_t screensaver_style_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_obj_t *line_cont;
static lv_timer_t *anim_timer = NULL;

static bool mutex_init_flag = false;
static lv_mutex_t timer_mutex;
static uint8_t exec_count = 0;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void scroll_saver_event_cb(lv_event_t *e);
static void screen_saver_timer_cb(lv_timer_t *timer);
static void *screen_saver_create(lv_obj_t *cont, const char *path);
static void screen_saver_click_event_cb(lv_event_t *e);
static void *line_container_create(lv_obj_t *cont);
static void circular_scroll_handle(lv_obj_t *cont, uint8_t dir);

static const char *saver_list[SAVER_NUM] = {
    "../lv_port_pc_vscode/assert/icon/screensaver.png",
    "../lv_port_pc_vscode/assert/icon/screensaver2.png",
    "../lv_port_pc_vscode/assert/icon/screensaver4.png",
    "../lv_port_pc_vscode/assert/icon/screensaver5.png",
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t screensaver_style_page_info = {
    .page_id = PAGE_FUNCTIONAL_SAVER_STYLE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_screensaver_style_info_get()
{
    return &screensaver_style_page_info;
}

static void lv_page_construct(void *this)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();
    //互斥锁初始化
    if (!mutex_init_flag) lv_mutex_init(&timer_mutex);
    mutex_init_flag = true;

    screen = lv_obj_create(act_screen);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    screensaver_style_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_mutex_lock(&timer_mutex);
    if (anim_timer) lv_timer_del(anim_timer);
    anim_timer = NULL;

    screensaver_style_page_info.status = STATUS_EXITING;//准备离开
    lv_mutex_unlock(&timer_mutex);

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
    lv_subject_init_int(&screensaver_style_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&screensaver_style_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&screensaver_style_subject);
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

    //右侧滚动条
    line_cont = line_container_create(cont);

    //创建屏保转盘
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_set_size(cont_col, 380, lv_pct(100));
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 80, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont_col, scroll_saver_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_flex_align(cont_col, 
                                LV_FLEX_ALIGN_CENTER,
                                LV_FLEX_ALIGN_CENTER,
                                LV_FLEX_ALIGN_CENTER);

    for (uint8_t i = 0; i < SAVER_NUM; i++)
    {
        lv_obj_t *saver = screen_saver_create(cont_col, saver_list[i]);
        lv_obj_add_event_cb(saver, screen_saver_click_event_cb, LV_EVENT_CLICKED, cont_col);
    }

    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, 0), LV_ANIM_OFF);
    lv_obj_send_event(cont_col, LV_EVENT_SCROLL, NULL);

    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&screensaver_style_subject, PAGE_SWITCH_BACK);
}

static void scroll_saver_event_cb(lv_event_t *e)
{
    lv_mutex_lock(&timer_mutex);
    if (STATUS_EXITING == screensaver_style_page_info.status)
    {
        lv_mutex_unlock(&timer_mutex);
        return;
    }

    lv_obj_t *cont = lv_event_get_target(e);
    lv_obj_t *first = lv_obj_get_child(cont, 0);
    lv_area_t first_a;
    lv_obj_get_coords(first, &first_a);

    //判断滚动方向
    static uint8_t dir = 0;//0 nul, 1 up, 2 down
    static int32_t last_y = 0xffffffff;

    if (last_y == 0xffffffff) last_y = first_a.y1;

    dir = first_a.y1 > last_y? 2 : 1;
    last_y = first_a.y1;

    //倾斜变换
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);

    static uint8_t last_idx = 0;
    static uint8_t cur_idx = 0;

    for (uint32_t i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        if (LV_ABS(child_a.y1 - 95) < 20) cur_idx = i;

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = child_y_center - cont_y_center;

        int32_t r = lv_obj_get_height(cont);
        uint32_t x_sqr = r * r - LV_ABS(diff_y) * LV_ABS(diff_y);

        lv_sqrt_res_t res;
        lv_sqrt(x_sqr, &res, 0x8000);
        int32_t x = r - res.i;

        //设置旋转中心为右侧边缘中间点
        lv_obj_set_style_transform_pivot_x(child, 610, 0);
        lv_obj_set_style_transform_pivot_y(child, 205, 0);

        // int32_t angle = -(diff_y) / 3;
        // angle = LV_ABS(diff_y) >= 60? angle : 0;
        lv_obj_set_style_translate_x(child, x, 0);
        // lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
    }

    if (cur_idx != last_idx)
    {
        exec_count = 0;
        last_idx = cur_idx;

        if (!anim_timer)
        {
            //创建定时器，每40ms执行滚动动态
            anim_timer = lv_timer_create(screen_saver_timer_cb, 40, &dir);
            lv_timer_set_auto_delete(anim_timer, false);
        }
        else
        {
            lv_timer_reset(anim_timer);
            if (lv_timer_get_paused(anim_timer))
            {
                lv_timer_resume(anim_timer);
            }
        }
    }

    lv_mutex_unlock(&timer_mutex);
}

static void screen_saver_timer_cb(lv_timer_t *timer)
{
    lv_mutex_lock(&timer_mutex);
    if (STATUS_EXITING == screensaver_style_page_info.status)
    {
        lv_mutex_unlock(&timer_mutex);
        return;
    }
    uint8_t *pdir = lv_timer_get_user_data(timer);
    circular_scroll_handle(line_cont, *pdir);
    if (++exec_count >= 30)
    {
        lv_timer_pause(anim_timer);
    }
    lv_mutex_unlock(&timer_mutex);
}

static void *screen_saver_create(lv_obj_t *cont, const char *path)
{
    lv_obj_t *saver = lv_obj_create(cont);
    lv_obj_remove_style_all(saver);
    lv_obj_set_size(saver, 280, 220);
    lv_obj_add_flag(saver, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(saver, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(saver, 20, 0);
    lv_obj_set_style_clip_corner(saver, true, 0);
    lv_obj_align(saver, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *style_image = lv_img_create(saver);
    lv_obj_set_size(style_image, 280, 220);
    lv_img_set_src(style_image, path);
    lv_img_set_zoom(style_image, 64);
    lv_obj_align(style_image, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *img = lv_img_create(saver);
    if (saver == lv_obj_get_child(cont, 0))
    {
        lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
    }
    else
    {
        lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_unselect.png");
    }
    lv_obj_align(img, LV_ALIGN_TOP_RIGHT, -20, 20);

    return saver;
}

static void screen_saver_click_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *cont_col = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        for (uint8_t i = 0; i < lv_obj_get_child_cnt(cont_col); i++)
        {
            lv_obj_t *iterm = lv_obj_get_child(cont_col, i);
            lv_obj_t *img = lv_obj_get_child(iterm, 1);
            lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_unselect.png");
        }

        lv_obj_t *checkbox = lv_event_get_target(e);
        lv_obj_t *img = lv_obj_get_child(checkbox, 1);
        lv_img_set_src(img, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
        lv_obj_scroll_to_view(checkbox, LV_ANIM_ON);
    }
}

static void *line_container_create(lv_obj_t *cont)
{
    lv_obj_t *line_cont = lv_obj_create(cont);
    lv_obj_add_style(line_cont, &screen_style, 0);
    lv_obj_set_size(line_cont, 20, 150);
    lv_obj_align(line_cont, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_obj_set_flex_flow(line_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(line_cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(line_cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(line_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(line_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_row(line_cont, 8, 0);

    for (uint8_t i = 0; i < 25; i++)
    {
        lv_obj_t *line = lv_obj_create(line_cont);
        lv_obj_remove_style_all(line);
        lv_obj_set_size(line, 30, 2);
        lv_obj_set_style_bg_opa(line, LV_OPA_20, 0);
        lv_obj_set_style_bg_color(line, lv_color_hex(0xFFFFFF), 0);
    }

    //初始位置设置为中间的子对象
    lv_obj_scroll_to_view(lv_obj_get_child(line_cont, 12), LV_ANIM_OFF);
    circular_scroll_handle(line_cont, 0);

    return line_cont;
}

static void circular_scroll_handle(lv_obj_t *cont, uint8_t dir)
{
    lv_coord_t child_cnt = lv_obj_get_child_cnt(cont);
    if(child_cnt < 2) return;

    //获取当前中心坐标
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    //针对移动后的位置绘制曲线
    for (uint32_t i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff_y = child_y_center - cont_y_center;

        // int32_t x = LV_ABS(diff_y * 1 / 4);
        int32_t x = 14.252f - 18.252f * expf(-0.085f * LV_ABS(diff_y));
        lv_obj_set_style_translate_x(child, x, 0);

        if (LV_ABS(diff_y) < 20) {//局部透明度
            lv_obj_set_style_bg_opa(child, LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(child, LV_OPA_20, 0);
        }
    }

    if (dir == 1)
        lv_obj_scroll_by(cont, 0, -1, LV_ANIM_OFF);

    if (dir == 2)
        lv_obj_scroll_by(cont, 0, 1, LV_ANIM_OFF);
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
    switch_page->old_page = &screensaver_style_page_info;

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