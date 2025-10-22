#include "../lv_switch_interface.h"

#define CAMERA_ICON "V:tk1/realtime_shooting/photograph_icon_switchcamera.png"

lv_subject_t shooting_switch_video_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t up_area_style;
static lv_style_t down_area_style;
static lv_style_t camera_button_style;

static lv_obj_t *screen = NULL;
static lv_obj_t *time_area;
static lv_obj_t *led;
static lv_timer_t *timer = NULL;
static lv_timer_t *timer1 = NULL;
static lv_obj_t * zoom_label;

static bool camera_state = false;
static uint32_t record_sec = 0;
static bool is_recording = false;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void timer_callback_2(lv_timer_t *timer);
static void screen_click_cb(lv_event_t * e);
//static void camera_click_cb(lv_event_t * e);
static void timer_cb(lv_timer_t * timer);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
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
     // 清理定时器
    if(timer) 
    {
        lv_timer_del(timer);
        timer = NULL;
    }

    if(timer1) 
    {
        lv_timer_del(timer1);
        timer1 = NULL;
    }

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

    lv_style_init(&camera_button_style);
    lv_style_set_bg_color(&camera_button_style, lv_color_hex(0xFFFFFF));
    lv_style_set_radius(&camera_button_style, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_opa(&camera_button_style, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&camera_button_style, LV_OPA_10);
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
    lv_obj_add_style(cont, &screen_style, 0);

    // 创建实时取景背景（假设是图像或视频组件）
    lv_obj_t * live_view = lv_image_create(cont);

    // 为整个屏幕添加点击事件
    lv_obj_add_event_cb(cont, screen_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
    lv_image_set_src(live_view, "V:png/img_camera_backup.png");
    lv_img_set_zoom(live_view, 128);
    lv_obj_center(live_view);

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
    timer = lv_timer_create(timer_cb, 1000, NULL);

    return;
}

// 屏幕点击事件回调函数
static void screen_click_cb(lv_event_t * e) 
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
            if(timer1 == NULL) 
            {
                timer1 = lv_timer_create(timer_callback_2, 1000, NULL);
            }   
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
    float zoom_level = get_zoom_level_from_hardware();
    char zoom_str[16];
    snprintf(zoom_str, sizeof(zoom_str), "%.1fX", zoom_level);
    lv_label_set_text(zoom_label, zoom_str);
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
    lv_obj_t *label = lv_obj_get_child(time_area, 0);

    lv_label_set_text_fmt(label, "%02d:%02d:%02d", record_sec/3600, record_sec / 60, record_sec % 60);
    record_sec++ ;
    return;
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
    switch_page->old_page = &shooting_switch_video_page;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_shooting_photo_get();
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