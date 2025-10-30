#include "../lv_switch_interface.h"

#define CAMERA_ICON "V:tk1/realtime_shooting/photograph_icon_switchcamera.png"
#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"
#define PHOTOS_ICON_LUT "V:tk1/realtime_shooting/photograph_icon_lut.png"
#define PHOTOS_ICON_PARAMETER "V:tk1/realtime_shooting/photograph_icon_parameter.png"

lv_subject_t shooting_switch_video_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t up_area_style;
static lv_style_t down_area_style;
static lv_style_t camera_button_style;
static lv_style_t realtime_style;

static lv_obj_t *screen = NULL;
static lv_obj_t *time_area;
static lv_obj_t *led;
static lv_timer_t *zoom_timer = NULL;
static lv_timer_t *record_timer = NULL;
static lv_obj_t * zoom_label;
static lv_obj_t * right_panel = NULL;   // 右侧面板

static bool mutex_init_flag = false;
static lv_mutex_t timer_mutex;
static bool camera_state = false;
static uint32_t record_sec = 0;
static bool is_recording = false;
static bool left_panel_visible = false;
static bool right_panel_visible = false;
// 定义阈值
#define LEFT_EDGE_THRESHOLD 200
#define RIGHT_EDGE_THRESHOLD (LV_HOR_RES - LEFT_EDGE_THRESHOLD)

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void timer_callback_2(lv_timer_t *timer);
static void video_click_cb(lv_event_t * e);
//static void camera_click_cb(lv_event_t * e);
static void timer_cb(lv_timer_t * timer);
static void gesture_event_handler(lv_event_t * e);
static void multi_effect_filter_click_cb(lv_event_t * e);
static void parameter_adj_click_cb(lv_event_t * e);
static void back_click_cb(lv_event_t * e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_ADJ_PARAM,
    PAGE_SWITCH_MULTI_FILTER,
    PAGE_SWITCH_BACK
};

static lv_page_info_t shooting_switch_video_page = {
    .page_id = PAGE_FUNCTIONAL_SHOOTING_VIDEO,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_shooting_switch_video_get()
{
    return &shooting_switch_video_page;
}

static void lv_page_construct(void)
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
    shooting_switch_video_page.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_mutex_lock(&timer_mutex);
    if(zoom_timer) lv_timer_del(zoom_timer);
    zoom_timer = NULL;

    if(record_timer) lv_timer_del(record_timer);
    record_timer = NULL;
    shooting_switch_video_page.status = STATUS_EXITING;//准备离开
    lv_mutex_unlock(&timer_mutex);

    if(right_panel) 
    {
        printf("删除右侧面板\n");
        lv_obj_del(right_panel);
        right_panel = NULL;
        right_panel_visible = false;  // 更新状态标志
    }
    
    lv_obj_remove_event_cb(act_screen, gesture_event_handler);
    lv_style_reset(&screen_style);
    lv_style_reset(&up_area_style);
    lv_style_reset(&down_area_style);
    lv_style_reset(&camera_button_style);
    lv_style_reset(&realtime_style);
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

    //up_area_style
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[0].opa = LV_OPA_90;
    grad.stops[1].color = lv_color_hex(0x000000);
    grad.stops[1].opa = LV_OPA_TRANSP;
    grad.stops[0].frac = 0;
    grad.stops[1].frac = 255;
    lv_style_init(&up_area_style);
    lv_style_set_pad_all(&up_area_style, 0);
    lv_style_set_border_width(&up_area_style, 0);
    lv_style_set_radius(&up_area_style, 0);
    lv_style_set_bg_grad(&up_area_style, &grad);

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
    lv_style_set_border_opa(&down_area_style, LV_OPA_TRANSP);
    lv_style_set_radius(&down_area_style, 0);
    lv_style_set_bg_grad(&down_area_style, &down_grad);

    //camera_button_style
    lv_style_init(&camera_button_style);
    lv_style_set_bg_color(&camera_button_style, lv_color_hex(0xFFFFFF));
    lv_style_set_radius(&camera_button_style, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_opa(&camera_button_style, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&camera_button_style, LV_OPA_10);

    //realtime_style
    lv_style_init(&realtime_style);
    lv_style_set_bg_color(&realtime_style, lv_color_hex(0x1C1C1E));
    lv_style_set_bg_opa(&realtime_style, LV_OPA_COVER);
    lv_style_set_radius(&realtime_style, 20);
    lv_style_set_border_opa(&realtime_style, LV_OPA_TRANSP);
    lv_style_set_pad_all(&realtime_style, 0);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&shooting_switch_video_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&shooting_switch_video_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&shooting_switch_video_subject);
}
static void lv_page_load(lv_obj_t *cont)
{
    // 添加手势检测到实时取景背景
    lv_obj_add_event_cb(act_screen, gesture_event_handler, LV_EVENT_GESTURE, NULL);

    // 创建顶部矩形渐变框
    lv_obj_t *up_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(up_indicator_area, 502, 70);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);

    led = lv_obj_create(up_indicator_area);
    lv_obj_remove_style_all(led);
    lv_obj_set_style_bg_opa(led, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(led, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(led, lv_color_hex(0xFF5C5C), LV_PART_MAIN);
    lv_obj_align(led, LV_ALIGN_TOP_LEFT, 188, 36);
    lv_obj_set_size(led, 10, 10);
    lv_obj_add_flag(led, LV_OBJ_FLAG_HIDDEN); // 初始隐藏

    time_area = lv_obj_create(up_indicator_area);
    lv_obj_remove_style_all(time_area);
    lv_obj_set_size(time_area, 105, 41);
    lv_obj_set_style_bg_opa(time_area, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_align(time_area, LV_ALIGN_TOP_LEFT, 208, 20);
    lv_obj_add_flag(time_area, LV_OBJ_FLAG_HIDDEN); // 初始隐藏

    lv_obj_t *label = lv_label_create(time_area);
    lv_label_set_text(label, "00:00:00");
    lv_obj_set_style_text_font(label, font_get_regular(24), LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_center(label);

    // 创建底部矩形渐变框
    lv_obj_t *down_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(down_indicator_area, 502, 156);
    lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(down_indicator_area, &down_area_style, 0);

    //临时调试代码， 创建录像buton
    lv_obj_t *photo_btn = lv_btn_create(down_indicator_area);
    lv_obj_set_size(photo_btn, 70, 70);
    lv_obj_align(photo_btn, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_style_bg_color(photo_btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_radius(photo_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_opa(photo_btn, LV_OPA_COVER, 0);

    lv_obj_t *photo_label = lv_label_create(photo_btn);
    lv_label_set_text(photo_label, "录像");

    lv_obj_set_style_text_opa(photo_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(photo_label, font_get_regular(24), 0);
    lv_obj_set_style_text_color(photo_label, lv_color_white(), 0);
    lv_obj_align(photo_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(photo_btn, video_click_cb, LV_EVENT_CLICKED, NULL);

    // // 创建右下角摄像机图标
    lv_obj_t *camera_buton = lv_btn_create(down_indicator_area);
    lv_obj_set_size(camera_buton, 70, 70);
    lv_obj_align(camera_buton, LV_ALIGN_BOTTOM_RIGHT, -120, -30);
    lv_obj_add_style(camera_buton, &camera_button_style, LV_PART_MAIN);
    
    lv_obj_t * camera_icon = lv_img_create(camera_buton);
    lv_img_set_src(camera_icon, CAMERA_ICON);
    lv_obj_align(camera_icon, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_add_flag(camera_icon, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_add_event_cb(camera_icon, camera_click_cb, LV_EVENT_CLICKED, NULL);

    // 创建右侧焦距倍率显示
    lv_obj_t * zoom_container = lv_btn_create(down_indicator_area);
    lv_obj_set_size(zoom_container, 70, 70);
    lv_obj_align(zoom_container, LV_ALIGN_BOTTOM_RIGHT, -30, -30);
    lv_obj_add_style(zoom_container, &camera_button_style, LV_PART_MAIN);

    zoom_label = lv_label_create(zoom_container);
    lv_label_set_text(zoom_label, "1.0X");
    lv_obj_set_style_text_opa(zoom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(zoom_label, font_get_regular(16), 0);
    lv_obj_set_style_text_color(zoom_label, lv_color_white(), 0);
    lv_obj_align(zoom_label, LV_ALIGN_CENTER, 0, 0);

    // 创建定时器更新焦距倍率
    zoom_timer = lv_timer_create(timer_cb, 1000, NULL);
    lv_timer_set_auto_delete(zoom_timer, false);
    return;
}

// 隐藏右侧面板
static void hide_right_panel(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, right_panel);
    lv_anim_set_values(&a, LV_HOR_RES - 292, LV_HOR_RES);
    lv_anim_set_time(&a, 250);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_start(&a);
    
    right_panel_visible = false;
}

static void show_right_panel(lv_obj_t * parent)
{

   lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    // 创建右侧面板
    right_panel = lv_obj_create(parent);
    lv_obj_set_size(right_panel, 312, 410);//特意画宽20，覆盖右边的圆角
    lv_obj_set_style_bg_color(right_panel, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(right_panel, LV_OPA_90, 0);
    lv_obj_set_style_border_width(right_panel, 0, 0);
    lv_obj_set_style_radius(right_panel, 40, 0);
    lv_obj_set_style_clip_corner(right_panel, true, 0);
    lv_obj_align(right_panel, LV_ALIGN_TOP_LEFT, 210, 0);
    lv_obj_set_style_pad_all(right_panel, 0, LV_PART_MAIN);


    lv_obj_t * buttom_line = lv_obj_create(right_panel);
    lv_obj_set_size(buttom_line, 6, 50);
    lv_obj_set_style_radius(buttom_line, 4, 0);
    lv_obj_set_style_bg_color(buttom_line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(buttom_line, LV_ALIGN_TOP_LEFT, 10, 177); 
    lv_obj_set_style_opa(buttom_line, LV_OPA_COVER, 0);

    //创建返回按钮
    lv_obj_t * return_icon = lv_img_create(right_panel);
    lv_img_set_src(return_icon, ICON_BACK);
    lv_obj_set_size(return_icon, 50, 50);
    lv_obj_align(return_icon, LV_ALIGN_TOP_LEFT, 20, 20);

    lv_obj_add_flag(return_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(return_icon, back_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * title_label = lv_label_create(right_panel);
    lv_label_set_text(title_label, "画面调整");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_set_style_text_font(title_label, font_get_regular(28), 0);
    lv_obj_align_to(title_label, return_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);  // 图标右侧，垂直居中

    lv_obj_t * multi_effect_filter_obj = lv_obj_create(right_panel);
    lv_obj_set_size(multi_effect_filter_obj, 242, 110);
    lv_obj_align(multi_effect_filter_obj, LV_ALIGN_TOP_LEFT, 30, 84);
    lv_obj_add_style(multi_effect_filter_obj, &realtime_style, 0);

    lv_obj_add_event_cb(multi_effect_filter_obj, multi_effect_filter_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * photo_icon_lut = lv_img_create(multi_effect_filter_obj);
    lv_img_set_src(photo_icon_lut, PHOTOS_ICON_LUT);
    lv_obj_align(photo_icon_lut, LV_ALIGN_TOP_LEFT, 20, 20);

    lv_obj_t * multi_effect_filter_label = lv_label_create(right_panel);
    lv_label_set_text(multi_effect_filter_label, "百变滤镜");
    lv_obj_set_style_text_color(multi_effect_filter_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_set_style_text_font(multi_effect_filter_label, font_get_regular(28), 0);
    lv_obj_align_to(multi_effect_filter_label, photo_icon_lut, LV_ALIGN_OUT_RIGHT_MID, 0, 0);  // 图标右侧，垂直居中

    lv_obj_t * parameter_adj_obj = lv_obj_create(right_panel);
    lv_obj_set_size(parameter_adj_obj, 242, 110);
    lv_obj_align(parameter_adj_obj, LV_ALIGN_TOP_LEFT, 30, 214);
    lv_obj_add_style(parameter_adj_obj, &realtime_style, 0);
    lv_obj_add_event_cb(parameter_adj_obj, parameter_adj_click_cb, LV_EVENT_CLICKED, NULL);


    lv_obj_t * photo_icon_parameter = lv_img_create(parameter_adj_obj);
    lv_img_set_src(photo_icon_parameter, PHOTOS_ICON_PARAMETER);
    lv_obj_align(photo_icon_parameter, LV_ALIGN_TOP_LEFT, 20, 20);

    lv_obj_t * parameter_adj_label = lv_label_create(parameter_adj_obj);
    lv_label_set_text(parameter_adj_label, "参数调整");
    lv_obj_set_style_text_color(parameter_adj_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_set_style_text_font(parameter_adj_label, font_get_regular(28), 0);
    lv_obj_align_to(parameter_adj_label, photo_icon_parameter, LV_ALIGN_OUT_RIGHT_MID, 0, 0);  // 图标右侧，垂直居中

    lv_obj_add_event_cb(parameter_adj_label, parameter_adj_click_cb, LV_EVENT_CLICKED, NULL);

    // 动画显示右侧面板
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, right_panel);
    lv_anim_set_values(&a, LV_HOR_RES, LV_HOR_RES - 292);
    lv_anim_set_time(&a, 250);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_start(&a);
    
    right_panel_visible = true;
}

static void back_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_CLICKED) 
    {
        printf("hidden page\n");
        if(right_panel) 
        {
            printf("删除右侧面板\n");
            lv_obj_del(right_panel);
            right_panel = NULL;
            right_panel_visible = false;  // 更新状态标志
        }
    }
}

static void multi_effect_filter_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        printf("enter_multi_sffect\n");
        if(right_panel) 
        {
            printf("删除右侧面板\n");
            lv_obj_del(right_panel);
            right_panel = NULL;
            right_panel_visible = false;  // 更新状态标志
        }
        //todo:跳转到百变滤镜
        lv_subject_set_int(&shooting_switch_video_subject, PAGE_SWITCH_MULTI_FILTER);
    }
}

static void parameter_adj_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        printf("enter_parameter_adj\n");
        if(right_panel) 
        {
            printf("删除右侧面板\n");
            lv_obj_del(right_panel);
            right_panel = NULL;
            right_panel_visible = false;  // 更新状态标志
        }
        //todo:跳转到参数调整
        lv_subject_set_int(&shooting_switch_video_subject, PAGE_SWITCH_ADJ_PARAM);
    }
}

static void gesture_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_GESTURE) 
    { 
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

        // 获取触摸点的起始位置
        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t point;
        lv_indev_get_point(indev, &point);

        printf("手势方向: %d, 触摸点坐标: (%d, %d)\n", dir, point.x, point.y);

        // 1. 左边缘向右滑动 - 切换到别的界面
        if(dir == LV_DIR_RIGHT && point.x < LEFT_EDGE_THRESHOLD && !right_panel_visible) 
        {
            printf("左边缘向右滑动 - 切换到其他界面\n");
            // 切换到其他界面（根据您的实际界面管理方式调整）
            //lv_subject_set_int(&shooting_photo_subject, PAGE_SWITCH_ALBUM);
        }
        // 2. 右边缘向左滑动 - 显示右侧面板
        else if(dir == LV_DIR_LEFT && point.x > RIGHT_EDGE_THRESHOLD) 
        {
            printf("右边缘向左滑动 - 显示右侧面板\n");
            show_right_panel(lv_event_get_current_target(e));
        }
        // 3. 右边缘向右滑动 - 隐藏右侧面板
        else if(dir == LV_DIR_RIGHT && point.x > RIGHT_EDGE_THRESHOLD && right_panel_visible) 
        {
            printf("右边缘向右滑动 - 隐藏右侧面板\n");
            hide_right_panel();
        }
        else 
        {
            printf("其他手势或条件不满足\n");
        }
    }
}


// 屏幕点击事件回调函数
static void video_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_CLICKED) 
    {
        if(!is_recording) 
        {
            // 第一次点击：开始录像，显示时间和LED
            is_recording = true;
            
            // 显示LED和时间区域
            lv_obj_clear_flag(led, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(time_area, LV_OBJ_FLAG_HIDDEN);
            
            // 创建定时器更新录像时间
            record_timer = lv_timer_create(timer_callback_2, 1000, NULL);
            lv_timer_set_auto_delete(record_timer, false);
        }
        else 
        {
            // 第二次点击：停止录像，跳转到其他界面
            is_recording = false;
            record_sec = 0;
            
            // 隐藏LED和时间区域
            lv_obj_add_flag(led, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(time_area, LV_OBJ_FLAG_HIDDEN);

            lv_subject_set_int(&shooting_switch_video_subject, PAGE_SWITCH_NEXT);
        }
    }
}

static float get_zoom_level_from_hardware() 
{
    // 这里应该是从硬件获取的实际值
    static float zoom_level = 2.0f;
    return zoom_level;
}
static void timer_cb(lv_timer_t * timer) 
{
    lv_mutex_lock(&timer_mutex);
    if (shooting_switch_video_page.status == STATUS_EXITING)
    {
        lv_mutex_unlock(&timer_mutex);
        return;
    }
    float zoom_level = get_zoom_level_from_hardware();
    char zoom_str[16];
    snprintf(zoom_str, sizeof(zoom_str), "%.1fX", zoom_level);
    lv_label_set_text(zoom_label, zoom_str);
    lv_mutex_unlock(&timer_mutex);
}

// static void camera_click_cb(lv_event_t * e) 
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     if(code == LV_EVENT_CLICKED) 
//     {
//         camera_state = !camera_state;
//         // 这里实现切换物理镜头的逻辑
//         printf("切换物理镜头: %s\n", camera_state ? "后置" : "前置");
//     }
// }

static void timer_callback_2(lv_timer_t *timer)
{
    lv_mutex_lock(&timer_mutex);
    if (shooting_switch_video_page.status == STATUS_EXITING)
    {
        lv_mutex_unlock(&timer_mutex);
        return;
    }

    lv_obj_t *label = lv_obj_get_child(time_area, 0);
    lv_label_set_text_fmt(label, "%02d:%02d:%02d", record_sec/3600, record_sec / 60, record_sec % 60);
    record_sec++;
    lv_mutex_unlock(&timer_mutex);
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
    switch_page->old_page = &shooting_switch_video_page;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_shooting_photo_get();
            break;

        case PAGE_SWITCH_ADJ_PARAM:
            switch_page->new_page = lv_page_shooting_video_param_get();
            break;

        case PAGE_SWITCH_MULTI_FILTER:
            switch_page->new_page = lv_page_shooting_multi_filter_get();
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