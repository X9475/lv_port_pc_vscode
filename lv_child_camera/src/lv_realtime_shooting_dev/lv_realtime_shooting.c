/**
 * @file lv_realtime_shooting_dev.c
 * @author your name (you@domain.com)
 * @brief UI设备设置源文件
 * @version 0.1
 * @date 2025-09-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_realtime_shooting.h"

#if (LV_CHILD_CAMERA != 0) && (LV_CHILD_CAMERA_REALTIME_SHOOTING != 0)

/*********************
 *      DEFINES
 *********************/
// 图片路径定义（需要替换为实际路径）
#define FLASH_ICON_1 "V:tk1/realtime_shooting/photograph_icon_flash_lamp_off.png"
#define FLASH_ICON_2 "V:tk1/realtime_shooting/photograph_icon_flash_lamp_on.png"
#define RECORD_ICON "V:tk1/realtime_shooting/photograph_icon_record.png"
#define CAMERA_ICON "V:tk1/realtime_shooting/photograph_icon_switchcamera.png"
#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"
#define PHOTOS_MODE_ICON "V:tk1/realtime_shooting/photograph_pic_time lapse2.png"
#define PHOTOS_ICON_LUT "V:tk1/realtime_shooting/photograph_icon_lut.png"
#define PHOTOS_ICON_PARAMETER "V:tk1/realtime_shooting/photograph_icon_parameter.png"
#define FOCUS "V:tk1/realtime_shooting/focus.png"
#define VIDEO_BACKGROUND "V:tk1/realtime_shooting/video_background.png"
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void realtime_shooting_style_init();
static void lv_page_open();
static void create_back_button(lv_obj_t * parent);
static void lv_realtime_shooting_video(lv_obj_t * parent, int type);
static void rapid_coding_click_cb(lv_event_t * e);
static void lv_realtime_shooting_rapid_coding(lv_obj_t * parent, int type);
/**********************
 *  STATIC VARIABLES
 **********************/

static lv_timer_t *timer_2;//录像的秒数
static uint32_t record_sec = 0;

static lv_style_t style;
static lv_style_t up_area_style;
static lv_style_t down_area_style;

static lv_style_t camera_button_style;
static lv_style_t btn_style;
static lv_style_t roller_style;
static lv_style_t select_roller_style;
static lv_style_t up_area_roller_style;
static lv_style_t separator_line_style;
static lv_style_t realtime_style;

// 全局变量
static lv_obj_t * flash_img;
static lv_obj_t * zoom_label;
static bool flash_state = false;
static bool camera_state = false;

static const char * photograph_left_options = "倒计时\n画面比例\n触发方式";
static const char * video_left_options = "分辨率\n画面比例\n帧率";
static const char * time_lapse_left_options = "分辨率\n画面比例\n倍率";
/* 右边滚轮不同选项对应的内容 */
static const char * ratio_options = "1:1\n4:3\n16:9";
static const char * timer_options = "5s\nOFF\n3s";
static const char * trigger_options = "表情\nOFF\n手势";

static const char * resolution_options ="4K\n1080P";
static const char * frame_options ="25\n30\n50";

static const char * time_lapse_resolution_options ="2.7K\n1080P\n720P";
static const char * time_lapse_ratio_options ="8K星空\n4K日出日落\n8X星空";

static lv_obj_t *time_area;
/* 全局变量保存右边滚轮对象 */
static lv_obj_t * right_roller;
static lv_obj_t * separator_line; // 分隔线对象

static lv_obj_t * left_panel = NULL;    // 左侧面板
static lv_obj_t * right_panel = NULL;   // 右侧面板
static bool left_panel_visible = false;
static bool right_panel_visible = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_child_camera()
{
    lv_font_init();
    lv_page_open();
}

/**********************
 *  STATIC FUNCTIONS
 **********************/
static void realtime_shooting_style_init()
{
    lv_style_init(&style);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_border_width(&style, 0);
    lv_style_set_radius(&style, 0);

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
    lv_style_copy(&up_area_style, &style);
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
    lv_style_copy(&down_area_style, &style);
    lv_style_set_bg_grad(&down_area_style, &down_grad);

    //btn_style
    lv_style_init(&btn_style);
    lv_style_set_bg_color(&btn_style, lv_color_white());
    lv_style_set_radius(&btn_style, 70);
    lv_style_set_shadow_opa(&btn_style, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&btn_style, LV_OPA_TRANSP); // 设置背景透明度

    //camera_button_style
    lv_style_init(&camera_button_style);
    lv_style_set_bg_color(&camera_button_style, lv_color_hex(0xFFFFFF));
    lv_style_set_radius(&camera_button_style, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_opa(&camera_button_style, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&camera_button_style, LV_OPA_10);

    // roller_style
    lv_style_init(&roller_style);
    lv_style_set_bg_opa(&roller_style, LV_OPA_TRANSP);
    lv_style_set_bg_color(&roller_style, lv_color_black());
    lv_style_set_border_opa(&roller_style, LV_OPA_TRANSP);
    lv_style_set_border_width(&roller_style, 0);
    lv_style_set_text_line_space(&roller_style, 54);  // 行间距
    lv_style_set_text_align(&roller_style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_pad_left(&roller_style, 0);      // 左内边距10px
    

    lv_style_init(&select_roller_style);
    lv_style_set_bg_opa(&select_roller_style, LV_OPA_COVER);
    lv_style_set_bg_color(&select_roller_style, lv_color_black());
    lv_style_set_text_color(&select_roller_style, lv_color_hex(0XAFF99C));
    lv_style_set_text_font(&select_roller_style, font_get_regular(48));      // 字体大小48px
    lv_style_set_border_opa(&select_roller_style, LV_OPA_TRANSP);
    lv_style_set_border_width(&select_roller_style, 0);
    lv_style_set_text_line_space(&select_roller_style, 51);  // 行间距
    lv_style_set_text_align(&select_roller_style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_pad_left(&select_roller_style, 0);      // 左内边距0px

    //up_area_roller_style
    // static lv_grad_dsc_t roller_grad;
    // roller_grad.dir = LV_GRAD_DIR_VER;
    // roller_grad.stops_count = 2;
    // roller_grad.stops[0].color = lv_color_hex(0x000000);
    // roller_grad.stops[0].opa = LV_OPA_60;
    // roller_grad.stops[1].color = lv_color_hex(0x000000);
    // roller_grad.stops[1].opa = LV_OPA_TRANSP;
    // roller_grad.stops[0].frac = 0;
    // roller_grad.stops[1].frac = 255;
    lv_style_init(&up_area_roller_style);
    lv_style_set_border_opa(&up_area_roller_style, LV_OPA_TRANSP);
    //lv_style_set_bg_grad(&up_area_roller_style, &roller_grad);
    lv_style_set_bg_color(&up_area_roller_style, lv_color_hex(0x000000));
    lv_style_set_bg_grad_color(&up_area_roller_style, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&up_area_roller_style, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&up_area_roller_style, LV_OPA_60);
    lv_style_set_bg_grad_opa(&up_area_roller_style, LV_OPA_0);

    
    ////separator_line_style
    static lv_grad_dsc_t separator_line_grad;
    separator_line_grad.dir = LV_GRAD_DIR_VER;
    separator_line_grad.stops_count = 5;
    separator_line_grad.stops[0].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[0].opa = LV_OPA_TRANSP;
    separator_line_grad.stops[0].frac = 0;   // 起始位置（0%）

    separator_line_grad.stops[1].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[1].opa = LV_OPA_80;
    separator_line_grad.stops[1].frac = 64;  // 约25%位置（64/255）

    separator_line_grad.stops[2].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[2].opa = LV_OPA_100;
    separator_line_grad.stops[2].frac = 128; // 中间位置（50%）

    separator_line_grad.stops[3].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[3].opa = LV_OPA_80;
    separator_line_grad.stops[3].frac = 192; // 约75%位置（192/255

    separator_line_grad.stops[4].color = lv_color_hex(0x38383A);
    separator_line_grad.stops[4].opa = LV_OPA_0;
    separator_line_grad.stops[4].frac = 255; // 结束位置（100%)

    lv_style_init(&separator_line_style);
    lv_style_set_bg_grad(&separator_line_style, &separator_line_grad);
    lv_style_set_bg_opa(&separator_line_style, LV_OPA_COVER);

    lv_style_init(&realtime_style);
    lv_style_set_bg_color(&realtime_style, lv_color_hex(0x1C1C1E));
    lv_style_set_bg_opa(&realtime_style, LV_OPA_COVER);
    lv_style_set_radius(&realtime_style, 20);
    lv_style_set_border_opa(&realtime_style, LV_OPA_TRANSP);
    lv_style_set_pad_all(&realtime_style, 0);
}


// 假设的底层接口获取焦距倍率
float get_zoom_level_from_hardware() {
    // 这里应该是从硬件获取的实际值
    static float zoom_level = 1.0f;
    return zoom_level;
}

// 闪电图标点击回调
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
        } 
        else 
        {
            lv_img_set_src(flash_img, FLASH_ICON_1);
        }
    }
}

// 录像按钮点击回调
static void record_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        // TODO:这里实现跳转到录像界面的逻辑
        lv_realtime_shooting_video(lv_scr_act(), 0);
    }
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

// 定时器回调，更新焦距倍率
static void timer_cb(lv_timer_t * timer) 
{
    float zoom_level = get_zoom_level_from_hardware();
    char zoom_str[16];
    snprintf(zoom_str, sizeof(zoom_str), "%.1fX", zoom_level);
    lv_label_set_text(zoom_label, zoom_str);
}


static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * menu = lv_event_get_user_data(e);

    if(lv_menu_back_button_is_root(menu, obj)) 
    {
        printf("跳转上一界面\n");
    }
}


// 添加长按事件处理 - 显示转盘
static void zoom_btn_long_press_handler(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_LONG_PRESSED) 
    {
        printf("长按出现调整倍率\n");
    }
    else if(code == LV_EVENT_CLICKED)
    {
        printf("点击提示长按\n");
    }
}

// 显示左侧面板
static void show_left_panel(lv_obj_t * parent)
{
    // if(left_panel == NULL) 
    // {
    //     // 创建右侧面板
    //     left_panel = lv_obj_create(parent);
    //     lv_obj_set_size(left_panel, 219, 410);//特意画宽20，覆盖右边的圆角
    //     lv_obj_set_style_bg_color(left_panel, lv_color_hex(0x000000), 0);
    //     lv_obj_set_style_bg_opa(left_panel, LV_OPA_90, 0);
    //     lv_obj_set_style_border_width(left_panel, 0, 0);
    //     lv_obj_set_style_radius(left_panel, 40, 0);
    //     lv_obj_set_style_clip_corner(left_panel, true, 0);
    //     lv_obj_align(left_panel, LV_ALIGN_TOP_LEFT, 0, 0);
    //     lv_obj_set_style_pad_all(left_panel, 0, LV_PART_MAIN);
    // }
    
    // // 动画显示左侧面板
    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_var(&a, left_panel);
    // lv_anim_set_values(&a, -292, 0);
    // lv_anim_set_time(&a, 250);
    // lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    // lv_anim_start(&a);
    
    // left_panel_visible = true;
}


static void multi_effect_filter_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        printf("enter_multi_sffect\n");
        //todo:跳转到百变滤镜
    }
}

static void parameter_adj_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        printf("enter_parameter_adj\n");
    }
}

// 显示右侧面板
static void show_right_panel(lv_obj_t * parent)
{
    if(right_panel == NULL) 
    {
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
    }
    
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

// 隐藏左侧面板
static void hide_left_panel(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, left_panel);
    lv_anim_set_values(&a, 0, -292);
    lv_anim_set_time(&a, 250);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_start(&a);
    
    left_panel_visible = false;
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
            show_right_panel(lv_event_get_current_target(e));
        }
        else if(dir == LV_DIR_LEFT && left_panel_visible) 
        {
            // 向左滑动，隐藏左侧面板
            //hide_left_panel();
        }
        else if(dir == LV_DIR_RIGHT && right_panel_visible) 
        {
            // 向右滑动，隐藏右侧面板
            hide_right_panel();
        }
    }
}
static void lv_realtime_shooting_mode(lv_obj_t * parent) 
{
    // 创建实时取景背景（假设是图像或视频组件）
    lv_obj_t * live_view = lv_image_create(parent);
    lv_obj_add_style(live_view, &style, 0);
    lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
    lv_image_set_src(live_view, "V:png/img_camera_backup.png");
    lv_img_set_zoom(live_view, 128);
    lv_obj_center(live_view);

    // 添加手势检测到实时取景背景
    lv_obj_add_event_cb(parent, gesture_event_handler, LV_EVENT_GESTURE, NULL);

    // 创建顶部矩形渐变框
    lv_obj_t *up_indicator_area = lv_obj_create(parent);
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
    lv_obj_t *down_indicator_area = lv_obj_create(parent);
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
    lv_timer_create(timer_cb, 500, NULL); // 每500ms更新一次
}

static void setup_menu_back_button(lv_obj_t * menu) 
{
    // 创建返回按钮样式
    static lv_style_t back_btn_style;
    lv_style_init(&back_btn_style);
    
    // 设置返回按钮的背景图像
    lv_style_set_bg_img_src(&back_btn_style, ICON_BACK);
    
    // 将样式应用到菜单的根返回按钮
    //lv_menu_set_root_back_button_style(menu, &back_btn_style);
}
static void lv_realtime_shooting_photos_mode(lv_obj_t * parent)
{
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    //创建返回按钮
    create_back_button(parent);

    lv_obj_t * photo_mode_icon = lv_img_create(parent);
    lv_img_set_src(photo_mode_icon, PHOTOS_MODE_ICON);
    lv_obj_align(photo_mode_icon, LV_ALIGN_TOP_LEFT, 45, 55);

    //设置文字到主菜单
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "延时摄影");
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(36), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_set_style_text_letter_space(label, 8, LV_PART_MAIN); // 设置字符间距
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 163, 283);

    lv_obj_t *label2 = lv_label_create(parent);
    lv_label_set_text(label2, "相机静置，适合日出日落等流逝效果");
    lv_obj_set_style_text_opa(label2, LV_OPA_60, 0);
    lv_obj_set_style_text_font(label2, font_get_regular(26), 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(0XEBEBF5), 0);
    lv_obj_align(label2, LV_ALIGN_TOP_LEFT, 32, 336);

}

static void lv_realtime_shooting_switch_wait(lv_obj_t * parent, int type)
{
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    lv_obj_t * animimg0 = lv_animimg_create(parent);
    lv_obj_align(animimg0, LV_ALIGN_TOP_LEFT, 98, 43);

    //设置文字到主菜单
    lv_obj_t *label = lv_label_create(parent);
    if(type == 0)
    {
        lv_label_set_text(label, "切换中");
    }
    else if(type == 1)
    {
       lv_label_set_text(label, "加载中"); 
    }
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(30), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 320);
}

// void lv_example_line_1(void)
// {
//     /*Create an array for the points of the line*/
//     static lv_point_precise_t line_points[] = { {192, 151}, {192, 121}, {218, 121}};

//     /*Create style*/
//     static lv_style_t style_line;
//     lv_style_init(&style_line);
//     lv_style_set_line_width(&style_line, 20);
//     lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
//     lv_style_set_line_rounded(&style_line, true);

//     /*Create a line and apply the new style*/
//     lv_obj_t * line1;
//     line1 = lv_line_create(lv_screen_active());
//     lv_line_set_points(line1, line_points, 3);     /*Set the points*/
//     lv_obj_add_style(line1, &style_line, 0);
//     lv_obj_center(line1);
    
// }
static void lv_realtime_shooting_focus(lv_obj_t * parent)
{
    // 创建实时取景背景（假设是图像或视频组件）
    lv_obj_t * live_view = lv_image_create(parent);
    lv_obj_add_style(live_view, &style, 0);
    lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
    lv_image_set_src(live_view, "V:png/img_camera_backup.png");
    lv_img_set_zoom(live_view, 128);
    lv_obj_center(live_view);

    // 创建顶部矩形渐变框
    lv_obj_t *up_indicator_area = lv_obj_create(parent);
    lv_obj_set_size(up_indicator_area, 502, 70);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);

    // 创建顶部闪电图标
    flash_img = lv_img_create(up_indicator_area);
    lv_img_set_src(flash_img, FLASH_ICON_1);
    lv_obj_align(flash_img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(flash_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(flash_img, flash_click_cb, LV_EVENT_CLICKED, NULL);

    //先传递一个实际坐标位置，后续根据BSP提供的屏幕坐标修改变焦框位置
    lv_obj_t * focus_icon = lv_img_create(parent);
    lv_img_set_src(focus_icon, FOCUS);
    lv_obj_align(focus_icon, LV_ALIGN_TOP_LEFT, 192, 121);

    // 创建底部矩形渐变框
    lv_obj_t *down_indicator_area = lv_obj_create(parent);
    lv_obj_set_size(down_indicator_area, 502, 156);
    lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(down_indicator_area, &down_area_style, 0);

    // 创建左下角录像buton区域
    lv_obj_t *rec_btn = lv_btn_create(down_indicator_area);
    lv_obj_set_size(rec_btn, 140, 70);
    lv_obj_align(rec_btn, LV_ALIGN_BOTTOM_LEFT, 30, -30);
    lv_obj_add_style(rec_btn, &btn_style, LV_PART_MAIN);

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
    lv_timer_create(timer_cb, 1000, NULL); // 每500ms更新一次
    
}

static void timer_callback_2(lv_timer_t *timer)
{
    lv_obj_t *label = lv_obj_get_child(time_area, 0);
    lv_obj_t *time_area = (lv_obj_t *)lv_timer_get_user_data(timer);

    lv_label_set_text_fmt(label, "%02d:%02d:%02d", record_sec/3600, record_sec / 60, record_sec % 60);
    record_sec++ ;
    return;
}

static void lv_realtime_shooting_video(lv_obj_t * parent, int type)
{
    // 创建实时取景背景（假设是图像或视频组件）
    lv_obj_t * live_view = lv_image_create(parent);
    lv_obj_add_style(live_view, &style, 0);
    lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
    lv_image_set_src(live_view, "V:png/img_camera_backup.png");
    lv_img_set_zoom(live_view, 128);
    lv_obj_center(live_view);

    // 创建顶部矩形渐变框
    lv_obj_t *up_indicator_area = lv_obj_create(parent);
    lv_obj_set_size(up_indicator_area, 502, 70);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);

    lv_obj_t *led = lv_obj_create(up_indicator_area);
    lv_obj_remove_style_all(led);
    lv_obj_set_style_bg_opa(led, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(led, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(led, lv_color_hex(0xFF5C5C), LV_PART_MAIN);
    lv_obj_align(led, LV_ALIGN_TOP_LEFT, 188, 36);
    lv_obj_set_size(led, 10, 10);

    time_area = lv_obj_create(up_indicator_area);
    lv_obj_remove_style_all(time_area);
    lv_obj_set_size(time_area, 105, 41);
    lv_obj_set_style_bg_opa(time_area, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_align(time_area, LV_ALIGN_TOP_LEFT, 208, 20);

    lv_obj_t *label = lv_label_create(time_area);
    lv_label_set_text(label, "00:00:00");
    lv_obj_set_style_text_font(label, font_get_regular(24), LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_center(label);
    timer_2 = lv_timer_create(timer_callback_2, 1000, time_area);

    // 创建底部矩形渐变框
    lv_obj_t *down_indicator_area = lv_obj_create(parent);
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
    lv_timer_create(timer_cb, 1000, NULL); // 每500ms更新一次
}

/* 左边滚轮值改变事件回调 */
static void left_roller_event_cb(lv_event_t * e)
{
    static int32_t last_index = -1;
    lv_obj_t *left_roller = lv_event_get_target(e);
    uint16_t selected = lv_roller_get_selected(left_roller);
    int *type = (int *)lv_event_get_user_data(e);

    if (last_index != lv_roller_get_selected(left_roller))
    {//选项发生变化
        last_index = lv_roller_get_selected(left_roller);
        printf("type=%d\n", *type);
        /* 根据左边滚轮的选择更新右边滚轮内容 */
        switch(selected) {
            case 0: 
                if(*type == 0)
                {
                    lv_roller_set_options(right_roller, timer_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择OFF
                }
                else if(*type == 1)
                {
                    lv_roller_set_options(right_roller, resolution_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择1080p
                }
                else if(*type == 2)
                {
                    lv_roller_set_options(right_roller, time_lapse_resolution_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择1080P
                }
                break;
            case 1: 
                if(*type == 0)
                {
                    lv_roller_set_options(right_roller, ratio_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择4:3
                }
                else if(*type == 1)
                {
                    lv_roller_set_options(right_roller, ratio_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择4:3
                }
                else if(*type == 2)
                {
                    lv_roller_set_options(right_roller, ratio_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择4:3
                }
                break;
            case 2: 
                if(*type == 0)
                {
                    lv_roller_set_options(right_roller, trigger_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择OFF
                }
                else if(*type == 1)
                {   
                    lv_roller_set_options(right_roller, frame_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择30
                }
                else if(*type == 2)
                {
                    lv_roller_set_options(right_roller, time_lapse_ratio_options, LV_ROLLER_MODE_NORMAL);
                    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择4X日出日落
                }
                break;
        }
    }
}

static void right_roller_event_cb(lv_event_t * e)
{
    static int32_t last_index = -1;
    lv_obj_t *right_roller = lv_event_get_target(e);
    uint16_t selected = lv_roller_get_selected(right_roller);

    if (last_index != lv_roller_get_selected(right_roller))
    {//选项发生变化
        last_index = lv_roller_get_selected(right_roller);

        char selected_text[32] = {0};
        lv_roller_get_selected_str(right_roller, selected_text, sizeof(selected_text));
        LV_LOG_USER("Roller changed: Index=%d, Text=%s", selected, selected_text);

        /* 根据左边滚轮的选择更新右边滚轮内容 */
        switch(selected) {
            case 0: 
                LV_LOG_WARN("This is a event message");
                break;
            case 1: 
                LV_LOG_WARN("This is a event1 message");
                break;
            case 2: 
                LV_LOG_WARN("This is a event2 message");
                break;
        }
    }
}

/* 创建渐变分隔线 */
static void create_separator_line(lv_obj_t * parent)
{
    separator_line = lv_obj_create(parent);
    lv_obj_remove_style_all(separator_line); // 移除所有默认样式
    
    // 设置分隔线尺寸和位置
    lv_obj_set_size(separator_line, 1, 272); 
    lv_obj_align(separator_line, LV_ALIGN_TOP_LEFT, 301, 88);
    //创建垂直渐变效果
    lv_obj_add_style(separator_line, &separator_line_style, 0);

}

static void rapid_coding_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) 
    {
        printf("返回上一级\n");
    }
}

static void create_back_button(lv_obj_t * parent)
{
    lv_obj_t * rapid_coding_icon = lv_img_create(parent);
    lv_img_set_src(rapid_coding_icon, ICON_BACK);
    lv_obj_set_size(rapid_coding_icon, 50, 50);
    lv_obj_align(rapid_coding_icon, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(rapid_coding_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(rapid_coding_icon, rapid_coding_click_cb, LV_EVENT_CLICKED, NULL);
    
}

static void lv_realtime_shooting_rapid_coding(lv_obj_t * parent, int type)
{
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    //创建返回按钮
    create_back_button(parent);

    // 创建顶部矩形渐变框
    lv_obj_t *up_roller_indicator_area = lv_obj_create(parent);
    lv_obj_set_size(up_roller_indicator_area, 502, 50);
    lv_obj_align(up_roller_indicator_area, LV_ALIGN_TOP_LEFT, 0, 100);
    lv_obj_add_style(up_roller_indicator_area, &up_area_roller_style, 0);

    /* 创建左边滚轮 */
    lv_obj_t * left_roller = lv_roller_create(parent);
    if(type == 0) 
    {
        lv_roller_set_options(left_roller, photograph_left_options, LV_ROLLER_MODE_NORMAL);
    }
    else if(type == 1) 
    {
        lv_roller_set_options(left_roller, video_left_options, LV_ROLLER_MODE_NORMAL);
    }
    else if(type == 2) 
    {
        lv_roller_set_options(left_roller, time_lapse_left_options, LV_ROLLER_MODE_NORMAL);
    }

    lv_roller_set_selected(left_roller, 1, LV_ANIM_OFF); // 默认选择画面比例
    lv_roller_set_visible_row_count(left_roller, 3);
    lv_obj_set_width(left_roller, 192);
    lv_obj_set_height(left_roller, 272);
    lv_obj_align(left_roller, LV_ALIGN_TOP_LEFT, 60, 88);
    lv_obj_set_style_text_font(left_roller, font_get_regular(34), 0);
    static int left_type = 0;
    left_type = type;
    lv_obj_add_event_cb(left_roller, left_roller_event_cb, LV_EVENT_VALUE_CHANGED, &left_type);

    /* 创建渐变分隔线 */
    create_separator_line(parent);

    /* 创建右边滚轮 */
    right_roller = lv_roller_create(parent);
    lv_roller_set_options(right_roller, ratio_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(right_roller, 1, LV_ANIM_OFF); // 默认选择画面比例
    lv_roller_set_visible_row_count(right_roller, 3);
    lv_obj_set_width(right_roller, 150);
    lv_obj_set_height(right_roller, 272);
    lv_obj_align(right_roller, LV_ALIGN_TOP_LEFT, 352, 88);
    lv_obj_set_style_text_font(right_roller,font_get_regular(34), 0);
    lv_obj_add_event_cb(right_roller, right_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    lv_obj_add_style(left_roller, &roller_style, LV_PART_MAIN);
    lv_obj_add_style(right_roller, &roller_style, LV_PART_MAIN);

    lv_obj_add_style(left_roller, &select_roller_style, LV_PART_SELECTED);
    lv_obj_add_style(right_roller, &select_roller_style, LV_PART_SELECTED);
}


static void lv_page_open()
{
    // TODO: 根据业务区分调用
    {
        realtime_shooting_style_init();
        lv_obj_t * scr = lv_scr_act();
        lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
        //实时取景拍摄模式--已实现
        //lv_realtime_shooting_mode(scr);

        //模式列表页录像、延时摄影 ---未实现

        //照片合集样式---已实现
        //lv_realtime_shooting_photos_mode(scr);

        //实时拍摄切换、加载动画等待，等待交互点动画 ,0:切换中，1:加载中--已实现
        //lv_realtime_shooting_switch_wait(scr, 0);

        //实时取景焦距、对焦 ---未实现
        //lv_realtime_shooting_focus(scr);

        //实时取景录像，延时摄影, 0:录像---已实现，界面边缘滑动已实现，延时摄影后续版本再实现
        //lv_realtime_shooting_video(scr, 0);

        //相册功能---未实现

        //百变滤镜功能---未实现

        //快速编码设置 0拍摄，1录像 2延时摄影---已实现
        //lv_realtime_shooting_rapid_coding(scr, 0);
    }

    return;
}
#endif