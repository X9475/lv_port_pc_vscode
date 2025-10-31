#include "../lv_switch_interface.h"

#define PHOTOGRAPH_ICON_GUIDE_GREEN "V:tk1/realtime_shooting/photograph_icon_guide_green.png"

lv_subject_t  shooting_adj_focus_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t down_area_style;
static lv_style_t indicator_style;
static lv_style_t minor_ticks_style;
static lv_style_t main_line_style;
static lv_style_t section_label_style;

static lv_obj_t *screen = NULL;
static lv_scale_section_t * section;

// 全局变量声明
static lv_timer_t *auto_switch_timer = NULL;

// 全局变量记录旋转状态
static int16_t initial_rotation = 210;  // 初始角度
static int16_t current_rotation = 0;    // 当前相对于初始角度的偏移
static int16_t max_left_rotation = -120; // 最大向左旋转角度
static int16_t left_rotation_amount = 0; // 已向左旋转的角度

static float current_focus_value = 0;  // 当前焦距值

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void btn_left_event_cb(lv_event_t *e);
static void btn_right_event_cb(lv_event_t *e);

static void auto_switch_cb(lv_timer_t * timer);
static void reset_auto_switch_timer(lv_event_t * e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t shooting_adj_focus_page = {
    .page_id = PAGE_FUNCTIONAL_SHOOTING_ADJ_FOCUS,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_shooting_adj_focus_get()
{
    return &shooting_adj_focus_page;
}

static void lv_page_construct(void)
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
    shooting_adj_focus_page.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    if (auto_switch_timer) lv_timer_del(auto_switch_timer);
    auto_switch_timer = NULL;

    lv_style_reset(&screen_style);
    lv_style_reset(&down_area_style);
    lv_style_reset(&indicator_style);
    lv_style_reset(&minor_ticks_style);
    lv_style_reset(&main_line_style);
    lv_style_reset(&section_label_style);
    lv_page_subject_deinit();
}

static void lv_page_style_init()
{
    //screen_style
    lv_style_init(&screen_style);
    lv_style_set_radius(&screen_style, 0);
    lv_style_set_pad_all(&screen_style, 0);
    lv_style_set_border_width(&screen_style, 0);
    //lv_style_set_bg_color(&screen_style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&screen_style, LV_OPA_TRANSP);

    //down_area_style
    static lv_grad_dsc_t down_grad;
    down_grad.dir = LV_GRAD_DIR_VER;
    down_grad.stops_count = 2;
    down_grad.stops[0].color = lv_color_hex(0x000000);
    down_grad.stops[0].opa = LV_OPA_TRANSP;
    down_grad.stops[1].color = lv_color_hex(0x000000);
    down_grad.stops[1].opa = LV_OPA_90;
    down_grad.stops[0].frac = 0;
    down_grad.stops[1].frac = 255;
    lv_style_init(&down_area_style);
    lv_style_set_pad_all(&down_area_style, 0);
    lv_style_set_border_width(&down_area_style, 0);
    lv_style_set_radius(&down_area_style, 0);
    lv_style_set_bg_grad(&down_area_style, &down_grad);

    //indicator_style
    lv_style_init(&indicator_style);
    lv_style_set_text_font(&indicator_style, font_get_regular(18));
    lv_style_set_text_color(&indicator_style, lv_color_hex(0XFFFFFF));
    lv_style_set_line_color(&indicator_style, lv_color_hex(0XFFFFFF));
    lv_style_set_length(&indicator_style, 12);
    lv_style_set_line_width(&indicator_style, 2);

    //minor_ticks_style
    lv_style_init(&minor_ticks_style);
    lv_style_set_line_color(&minor_ticks_style, lv_color_hex(0X979797));
    lv_style_set_length(&minor_ticks_style, 10);
    lv_style_set_line_width(&minor_ticks_style, 2);

    //main_line_style
    lv_style_init(&main_line_style);
    lv_style_set_arc_color(&main_line_style, lv_color_black());
    lv_style_set_arc_width(&main_line_style, 5);

    //section_label_style
    lv_style_init(&section_label_style);
    lv_style_set_text_font(&section_label_style, font_get_regular(24));
    lv_style_set_text_color(&section_label_style, lv_color_hex(0XAFF99C));
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&shooting_adj_focus_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&shooting_adj_focus_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&shooting_adj_focus_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    // 创建底部矩形渐变框
    lv_obj_t *down_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(down_indicator_area, 502, 230);
    lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(down_indicator_area, &down_area_style, 0);
    lv_obj_clear_flag(down_indicator_area, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *scale = lv_scale_create(down_indicator_area);
    // 设置刻度盘的尺寸为150x150像素
    lv_obj_set_size(scale, 200, 200);
    // 设置刻度盘模式为圆环内侧刻度
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    // 设置背景不透明度为60%
    lv_obj_set_style_bg_opa(scale, LV_OPA_COVER, 0);
    // 设置背景颜色为黑色
    lv_obj_set_style_bg_color(scale, lv_color_black(), 0);
    // 设置圆角为圆形（完全圆角）
    lv_obj_set_style_radius(scale, LV_RADIUS_CIRCLE, 0);
    // 启用裁剪圆角（让内容也按照圆角裁剪）
    lv_obj_set_style_clip_corner(scale, true, 0);
    // 将刻度盘居中显示
    lv_obj_align(scale, LV_ALIGN_BOTTOM_RIGHT, 95, 35);

    // 显示刻度标签
    lv_scale_set_label_show(scale, true);

    // 设置总刻度数量为11个（0-10）
    lv_scale_set_total_tick_count(scale, 11);
    // 每10个刻度显示一个主刻度
    lv_scale_set_major_tick_every(scale, 5);

    // 定义小时刻度标签文本
    static const char * hour_ticks[] = {"1", "1.5", "2", NULL};
    // 设置刻度标签文本源
    lv_scale_set_text_src(scale, hour_ticks);
    lv_obj_add_style(scale, &indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(scale, &minor_ticks_style, LV_PART_ITEMS);
    lv_obj_add_style(scale, &main_line_style, LV_PART_MAIN);

    lv_scale_set_range(scale, 0, 10);
    lv_scale_set_angle_range(scale, 120);
    lv_scale_set_rotation(scale, initial_rotation);

    /* 配置特殊区间,只应用标签样式*/
    section = lv_scale_add_section(scale);
    lv_scale_section_set_range(section, 0, 1);
    lv_scale_section_set_style(section, LV_PART_INDICATOR, &section_label_style);  // 只修改标签颜色

    //创建指针图标
    lv_obj_t * guide_green_icon = lv_img_create(down_indicator_area);
    lv_img_set_src(guide_green_icon, PHOTOGRAPH_ICON_GUIDE_GREEN);
    //lv_obj_set_size(guide_green_icon, 40, 40);
    lv_obj_align(guide_green_icon, LV_ALIGN_TOP_LEFT, 395, 90);

    lv_obj_set_style_transform_angle(guide_green_icon, 300, 0);

    // 创建向左旋转按钮
    lv_obj_t *btn_left = lv_btn_create(down_indicator_area);
    lv_obj_set_size(btn_left, 60, 40);
    lv_obj_set_style_radius(btn_left, 30, 0);
    lv_obj_align(btn_left, LV_ALIGN_BOTTOM_LEFT, 100, -40);

    lv_obj_t *label = lv_label_create(btn_left);
    lv_label_set_text(label, "调大");

    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // 创建向右旋转按钮
    lv_obj_t *btn_right = lv_btn_create(down_indicator_area);
    lv_obj_set_size(btn_right, 60, 40);
    lv_obj_set_style_radius(btn_right, 30, 0);
    lv_obj_align(btn_right, LV_ALIGN_BOTTOM_LEFT, 200, -40);

    lv_obj_t *label1 = lv_label_create(btn_right);
    lv_label_set_text(label1, "调小");

    lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label1, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

    // 按钮事件回调
    lv_obj_add_event_cb(btn_left, btn_left_event_cb, LV_EVENT_CLICKED, scale);
    lv_obj_add_event_cb(btn_right, btn_right_event_cb, LV_EVENT_CLICKED, scale);

    // 创建2秒后自动跳转的定时器
    auto_switch_timer = lv_timer_create(auto_switch_cb, 2000, NULL);

    return;
}

// 添加获取焦距值的函数
float get_current_focus_value(void)
{
    return current_focus_value;
}

// 自动跳转回调函数
static void auto_switch_cb(lv_timer_t * timer)
{
    lv_subject_set_int(&shooting_adj_focus_subject, PAGE_SWITCH_BACK);
}

// 取消自动跳转回调函数
static void reset_auto_switch_timer(lv_event_t * e)
{
    //创建2秒定时器
    if (NULL == auto_switch_timer)
    {
        auto_switch_timer = lv_timer_create(auto_switch_cb, 2000, NULL);
        lv_timer_set_auto_delete(auto_switch_timer, false);
    }
    else
    {
        lv_timer_reset(auto_switch_timer);
        printf("自动跳转定时器已重置\n");
    }
}

static void rotate_scale(lv_obj_t *scale, int16_t angle_change) 
{
    printf("angle_change=%d, current_rotation=%d, left_rotation_amount=%d\n", 
           angle_change, current_rotation, left_rotation_amount);

    // 限制向右转动：只有先向左转动过才能向右转动
    if (angle_change > 0 && left_rotation_amount <= 0) 
    {
        printf("Cannot rotate right without first rotating left\n");
        return;
    }

    // 计算新的旋转角度
    int16_t new_rotation = current_rotation + angle_change;
    
    // 限制旋转范围：不能超过向左120度的限制
    if (new_rotation < max_left_rotation) 
    {
        new_rotation = max_left_rotation;
        printf("Reached maximum left rotation limit\n");
    }
    
    // 限制向右转动：不能超过初始位置（0度偏移）
    if (new_rotation > 0) 
    {
        new_rotation = 0;
        printf("Reached initial position, cannot rotate further right\n");
    }
    
    // 更新向左旋转的总量
    if (angle_change < 0) 
    {
        left_rotation_amount += abs(angle_change);
    } 
    else if (angle_change > 0) 
    {
        left_rotation_amount -= angle_change;
        if (left_rotation_amount < 0) left_rotation_amount = 0;
    }
    
    current_rotation = new_rotation;
    
    // 设置刻度盘的旋转角度（初始角度 + 当前偏移）
    lv_scale_set_rotation(scale, initial_rotation + current_rotation);

    if (current_rotation == 0) 
    {
        // 初始角度：选定(0, 1)范围
        lv_scale_section_set_range(section, 0, 1);
        current_focus_value = 1;  // 对应1.0倍焦距
        printf("Section range set to (0, 1) - Initial position\n");
    }
    else if (current_rotation == -60) 
    {
        // 向左转动60度：选定(5, 6)范围
        lv_scale_section_set_range(section, 5, 6);
        current_focus_value = 1.5;  // 对应10倍焦距
        printf("Section range set to (5, 6) - 60° left rotation\n");
    }
    else if (current_rotation == -120) 
    {
        // 向左转动120度：选定(9, 10)范围
        lv_scale_section_set_range(section, 9, 10);
        current_focus_value = 2.0;  // 对应20倍焦距
        printf("Section range set to (9, 10) - 120° left rotation\n");
    }
    else
    {
        // 非特殊角度：取消选中效果
        lv_scale_section_set_range(section, -1, -1);

        // 线性计算焦距值：从1倍到20倍，对应0到-120度
        float ratio = (float)abs(current_rotation) / 120.0f;
        current_focus_value = 1 + ratio;
    }
    printf("No selection at %d°, focus value: %.2f\n", current_rotation, current_focus_value);

    // 重置自动跳转定时器
    reset_auto_switch_timer(NULL);
    
    printf("New rotation: %d (absolute: %d)\n", current_rotation, initial_rotation + current_rotation);
}

static void btn_left_event_cb(lv_event_t *e) 
{
    lv_obj_t *scale = lv_event_get_user_data(e);
    rotate_scale(scale, -12); // 向左旋转6度
}

// 向右旋转按钮事件回调
static void btn_right_event_cb(lv_event_t *e) 
{
    lv_obj_t *scale = lv_event_get_user_data(e);
    rotate_scale(scale, 12); // 向右旋转6度
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
    switch_page->old_page = &shooting_adj_focus_page;

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

    if (NULL == switch_page->new_page) 
     {
         lv_free(switch_page);
         return;
     }

    lv_subject_set_pointer(&switch_subject, switch_page);
}