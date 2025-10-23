#include "../lv_switch_interface.h"

#define FLASH_ICON_1 "V:tk1/realtime_shooting/photograph_icon_flash_lamp_off.png"
#define FLASH_ICON_2 "V:tk1/realtime_shooting/photograph_icon_flash_lamp_on.png"
#define RECORD_ICON "V:tk1/realtime_shooting/photograph_icon_record.png"
#define CAMERA_ICON "V:tk1/realtime_shooting/photograph_icon_switchcamera.png"
#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"
#define VIDEO_BACKGROUND "V:tk1/realtime_shooting/video_background.png"
#define PHOTOGRAPH_ICON_GUIDE_GREEN "V:tk1/realtime_shooting/photograph_icon_guide_green.png"

lv_subject_t  shooting_photo_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t up_area_style;
static lv_style_t down_area_style;
static lv_style_t btn_style;
static lv_style_t camera_button_style;

static lv_obj_t *screen = NULL;
static lv_obj_t * left_panel = NULL;    // 左侧面板
static lv_obj_t * right_panel = NULL;   // 右侧面板
static lv_obj_t * flash_img;
static lv_obj_t * zoom_label;
static lv_scale_section_t * section;
static lv_timer_t *zoom_timer = NULL;

static bool left_panel_visible = false;
static bool right_panel_visible = false;
static bool flash_state = false;
static bool camera_state = false;
static int initial_rotation = 210;  // 初始角度

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void timer_cb(lv_timer_t * timer);
static void camera_click_cb(lv_event_t * e);
static void record_click_cb(lv_event_t * e);
static void flash_click_cb(lv_event_t * e);
static void gesture_event_handler(lv_event_t * e);
static void zoom_btn_long_press_handler(lv_event_t * e);

static void btn_left_event_cb(lv_event_t *e);
static void btn_right_event_cb(lv_event_t *e);

//待跳转的页面种类
//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SHOOTING_ADJ_PARAM,     //参数调整页面
    PAGE_SWITCH_SHOOTING_FOCUS,         //对焦
    PAGE_SWITCH_SHOOTING_MODE,          //录像录制模式
    PAGE_SWITCH_SHOOTING_MULTI_FILTER,  //百变滤镜
    PAGE_SWITCH_SHOOTING_SWITCH_PARAM,  //参数切换界面
    PAGE_SWITCH_SHOOTING_SWITCH_VIDEO,  //预览相册切换
    PAGE_SWITCH_SHOOTING_MODE_SWITCH,   //模式切换中
    PAGE_SWITCH_BACK
};

static lv_page_info_t shooting_photo_page = {
    .page_id = PAGE_FUNCTIONAL_SHOOTING_PHOTO,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_shooting_photo_get()
{
    return &shooting_photo_page;
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
    shooting_photo_page.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    if (zoom_timer) 
    {
        lv_timer_del(zoom_timer);
        zoom_timer = NULL;
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

    lv_style_init(&btn_style);
    lv_style_set_bg_color(&btn_style, lv_color_white());
    lv_style_set_radius(&btn_style, 70);
    lv_style_set_shadow_opa(&btn_style, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&btn_style, LV_OPA_TRANSP); // 设置背景透明度

    lv_style_init(&camera_button_style);
    lv_style_set_bg_color(&camera_button_style, lv_color_hex(0xFFFFFF));
    lv_style_set_radius(&camera_button_style, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_opa(&camera_button_style, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&camera_button_style, LV_OPA_10);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&shooting_photo_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&shooting_photo_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&shooting_photo_subject);
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&shooting_photo_subject, PAGE_SWITCH_BACK);
}
static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_add_style(cont, &screen_style, 0);

    // 创建实时取景背景（假设是图像或视频组件）
    lv_obj_t * live_view = lv_image_create(cont);
    lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
    lv_image_set_src(live_view, "V:png/img_camera_backup.png");
    lv_img_set_zoom(live_view, 128);
    lv_obj_center(live_view);

    // 添加手势检测到实时取景背景
    lv_obj_add_event_cb(cont, gesture_event_handler, LV_EVENT_GESTURE, NULL);

    // 创建顶部矩形渐变框
    lv_obj_t *up_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(up_indicator_area, 502, 70);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);

    // 创建顶部闪电图标
    flash_img = lv_img_create(up_indicator_area);
    lv_img_set_src(flash_img, FLASH_ICON_1);
    lv_obj_align(flash_img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(flash_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(flash_img, flash_click_cb, LV_EVENT_CLICKED, NULL);

    // 创建底部矩形渐变框
    lv_obj_t *down_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(down_indicator_area, 502, 156);
    lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(down_indicator_area, &down_area_style, 0);

    // 创建左下角录像buton区域
    lv_obj_t *rec_btn = lv_btn_create(down_indicator_area);
    lv_obj_set_size(rec_btn, 140, 70);
    lv_obj_align(rec_btn, LV_ALIGN_BOTTOM_LEFT, 30, -30);
    lv_obj_add_style(rec_btn, &btn_style, LV_PART_MAIN);

    lv_obj_t * record_background = lv_img_create(down_indicator_area);
    lv_img_set_src(record_background, VIDEO_BACKGROUND);
    lv_obj_align(record_background, LV_ALIGN_BOTTOM_LEFT, 30, -30);

    // 录像图标
    lv_obj_t * record_icon = lv_img_create(rec_btn);
    lv_img_set_src(record_icon, RECORD_ICON);
    lv_obj_align(record_icon, LV_ALIGN_CENTER, -30, 0);

    //录像文本
    lv_obj_t *label = lv_label_create(rec_btn);
    lv_label_set_text(label, "录像");

    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 22, 0);
    lv_obj_add_event_cb(rec_btn, record_click_cb, LV_EVENT_CLICKED, NULL);

    // // 创建右下角摄像机图标
    lv_obj_t *camera_buton = lv_btn_create(down_indicator_area);
    lv_obj_set_size(camera_buton, 70, 70);
    lv_obj_align(camera_buton, LV_ALIGN_BOTTOM_RIGHT, -120, -30);
    lv_obj_add_style(camera_buton, &camera_button_style, LV_PART_MAIN);
    
    lv_obj_t * camera_icon = lv_img_create(camera_buton);
    lv_img_set_src(camera_icon, CAMERA_ICON);
    lv_obj_align(camera_icon, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(camera_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_icon, camera_click_cb, LV_EVENT_CLICKED, NULL);

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

    // 为zoom_container添加事件处理
    lv_obj_add_event_cb(zoom_container, zoom_btn_long_press_handler, LV_EVENT_ALL, NULL);

    // 创建定时器更新焦距倍率
    zoom_timer = lv_timer_create(timer_cb, 500, NULL); // 每500ms更新一次

    return;
}


// 假设的底层接口获取焦距倍率
static float get_zoom_level_from_hardware() 
{
    // 这里应该是从硬件获取的实际值
    static float zoom_level = 1.0f;
    return zoom_level;
}
// 定时器回调，更新焦距倍率
static void timer_cb(lv_timer_t * timer) 
{
    // 检查zoom_label是否有效
    if (zoom_label == NULL || !lv_obj_is_valid(zoom_label)) 
    {
        return;
    }

    // 检查zoom_label是否在当前屏幕中
    lv_obj_t *screen = lv_obj_get_screen(zoom_label);
    if (screen != lv_scr_act()) 
    {
        return;
    }
       
    float zoom_level = get_zoom_level_from_hardware();
    char zoom_str[16];
    snprintf(zoom_str, sizeof(zoom_str), "%.1fX", zoom_level);
    lv_label_set_text(zoom_label, zoom_str);
}

static void zoom_btn_long_press_handler(lv_event_t * e) 
{
    static bool is_long_pressed = false;  // 全局标志位，用于标记是否已处理长按
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_LONG_PRESSED) 
    {
        is_long_pressed = true;  // 标记已处理长按
        printf("长按出现调整倍率\n");

        // 暂停定时器，防止更新冲突
        if (zoom_timer) 
        {
            lv_timer_pause(zoom_timer);
        }

        lv_obj_t *screen = lv_obj_create(act_screen);
        lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);

        // 创建底部矩形渐变框
        lv_obj_t *down_indicator_area = lv_obj_create(screen);
        lv_obj_set_size(down_indicator_area, 502, 230);
        lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_add_style(down_indicator_area, &down_area_style, 0);
        //lv_obj_clear_flag(down_indicator_area, LV_OBJ_FLAG_SCROLLABLE);

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

        // 设置总刻度数量为21个（0-20）
        lv_scale_set_total_tick_count(scale, 21);
        // 每10个刻度显示一个主刻度
        lv_scale_set_major_tick_every(scale, 10);

        // 定义小时刻度标签文本（模拟时钟的12小时制）
        static const char * hour_ticks[] = {"1", "2", "3", NULL};
        // 设置刻度标签文本源
        lv_scale_set_text_src(scale, hour_ticks);

        static lv_style_t indicator_style;
        lv_style_init(&indicator_style);

        /* Label style properties */
        lv_style_set_text_font(&indicator_style, font_get_regular(18));
        lv_style_set_text_color(&indicator_style, lv_color_hex(0XFFFFFF));

        /* Major tick properties */
        lv_style_set_line_color(&indicator_style, lv_color_hex(0XFFFFFF));
        lv_style_set_length(&indicator_style, 12); /* tick length */
        lv_style_set_line_width(&indicator_style, 2); /* tick width */
        lv_obj_add_style(scale, &indicator_style, LV_PART_INDICATOR);

        /* Minor tick properties */
        static lv_style_t minor_ticks_style;
        lv_style_init(&minor_ticks_style);
        lv_style_set_line_color(&minor_ticks_style, lv_color_hex(0X979797));
        lv_style_set_length(&minor_ticks_style, 10); /* tick length */
        lv_style_set_line_width(&minor_ticks_style, 2); /* tick width */
        lv_obj_add_style(scale, &minor_ticks_style, LV_PART_ITEMS);

        /* Main line properties */
        static lv_style_t main_line_style;
        lv_style_init(&main_line_style);
        lv_style_set_arc_color(&main_line_style, lv_color_black());
        lv_style_set_arc_width(&main_line_style, 5);
        lv_obj_add_style(scale, &main_line_style, LV_PART_MAIN);

        lv_scale_set_range(scale, 0, 20);
        lv_scale_set_angle_range(scale, 120);
        lv_scale_set_rotation(scale, initial_rotation);

        static lv_style_t section_label_style;
        lv_style_init(&section_label_style);

        /* 只设置标签样式为绿色 */
        lv_style_set_text_font(&section_label_style, font_get_regular(24));
        lv_style_set_text_color(&section_label_style, lv_color_hex(0XAFF99C));

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
        lv_obj_align(btn_left, LV_ALIGN_BOTTOM_LEFT, 20, -20);

        // 创建向右旋转按钮
        lv_obj_t *btn_right = lv_btn_create(down_indicator_area);
        lv_obj_set_size(btn_right, 60, 40);
        lv_obj_align(btn_right, LV_ALIGN_BOTTOM_LEFT, 100, -20);

        // 按钮事件回调
        lv_obj_add_event_cb(btn_left, btn_left_event_cb, LV_EVENT_CLICKED, scale);
        lv_obj_add_event_cb(btn_right, btn_right_event_cb, LV_EVENT_CLICKED, scale);
            
    }
    else if (code == LV_EVENT_CLICKED)
    {
        // 如果是长按后的点击事件，则忽略
        if (is_long_pressed) 
        {
            is_long_pressed = false;  // 重置标志位
            return;
        }
        printf("点击提示长按\n");
    }
    
}

static void btn_left_event_cb(lv_event_t *e) 
{
    lv_obj_t *scale = lv_event_get_user_data(e);
    rotate_scale(scale, -6); // 向左旋转6度
}

// 向右旋转按钮事件回调
static void btn_right_event_cb(lv_event_t *e) 
{
    lv_obj_t *scale = lv_event_get_user_data(e);
    rotate_scale(scale, 6); // 向右旋转6度
}

// 摄像机图标点击回调
static void camera_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        camera_state = !camera_state;
        // 这里实现切换物理镜头的逻辑
        printf("切换物理镜头: %s\n", camera_state ? "后置" : "前置");
    }
}

static void record_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        printf("record mode clicked\n");
        //这里实现跳转到录像界面的逻辑
        lv_subject_set_int(&shooting_photo_subject, PAGE_SWITCH_SHOOTING_MODE);
    }
}


static void flash_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        flash_state = !flash_state;
        if(flash_state) 
        { 
            lv_img_set_src(flash_img, FLASH_ICON_2);
            //todo:拍照快会闪一下
            printf("start flash\n");
        }
        else 
        {
            lv_img_set_src(flash_img, FLASH_ICON_1);
        }
    }
}
static void gesture_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_GESTURE) 
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if(dir == LV_DIR_RIGHT && !left_panel_visible && !right_panel_visible) 
        {
            // 向右滑动，显示左侧面板
            //show_left_panel(lv_event_get_current_target(e));
        }
        else if(dir == LV_DIR_LEFT && !right_panel_visible && !left_panel_visible) 
        {
            // 向左滑动，显示右侧面板
            //show_right_panel(lv_event_get_current_target(e));
        }
        else if(dir == LV_DIR_LEFT && left_panel_visible) 
        {
            // 向左滑动，隐藏左侧面板
            //hide_left_panel();
        }
        else if(dir == LV_DIR_RIGHT && right_panel_visible) 
        {
            // 向右滑动，隐藏右侧面板
            //hide_right_panel();
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
    switch_page->old_page = &shooting_photo_page;

    switch (page_event)
    {
        case PAGE_SWITCH_SHOOTING_ADJ_PARAM:
            // switch_page->new_page = lv_page_agent_start_info_get();
            break;

        case PAGE_SWITCH_SHOOTING_FOCUS:
            // switch_page->new_page = lv_page_agent_start_info_get();
            break;

        case PAGE_SWITCH_SHOOTING_MODE:
            //加入栈表
            lv_stack_push(&shooting_photo_page);
            switch_page->new_page = lv_page_shooting_mode_get();
            break;

        case PAGE_SWITCH_SHOOTING_MULTI_FILTER:
            // switch_page->new_page = lv_page_agent_start_info_get();
            break;

        case PAGE_SWITCH_SHOOTING_SWITCH_PARAM:
            // switch_page->new_page = lv_page_agent_start_info_get();
            break;

        case PAGE_SWITCH_SHOOTING_SWITCH_VIDEO:
            // switch_page->new_page = lv_page_agent_start_info_get();
            break;

        case PAGE_SWITCH_SHOOTING_MODE_SWITCH:
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