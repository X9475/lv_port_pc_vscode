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
#define EMPTY_PIC_PHOTO "V:tk1/realtime_shooting/empety_pic_photo.png"
#define PHOTOGRAPH_ICON_FOUR_GRID "V:tk1/realtime_shooting/photograph_icon_four-grid.png"
#define PHOTOGRAPH_ICON_ALLOWS_DOWN "V:tk1/realtime_shooting/photograph_icon_arrows_down.png"
#define PHOTOGRAPH_ICON_SHARE   "V:tk1/realtime_shooting/photograph_icon_share.png"
#define PHOTOGRAPH_ICON_STOP "V:tk1/realtime_shooting/photograph_icon_stop.png"
#define PHOTOGRAPH_ICON_TRASH_FILLED "V:tk1/realtime_shooting/photograph_icon_trash_filled.png"
#define PHOTOGRAPH_ICON_PLAY "V:tk1/realtime_shooting/photograph_icon_play.png"
#define PHOTOGRAPH_ICON_INFO "V:tk1/realtime_shooting/photograph_icon_information.png"
#define INERTIGENCE_ICON "V:tk1/realtime_shooting/intelligence_icon.png"
#define COMM_ICON_CANCEL "V:tk1/realtime_shooting/common_icon_cancel_button.png"
#define COMM_ICON_OK "V:tk1/realtime_shooting/common_icon_ok_button.png"
#define PHOTOGRAPH_PIC_FAIL "V:tk1/realtime_shooting/photograph_pic_fail.png"
#define PHOTOGRAPH_ICON_SINGLE "V:tk1/realtime_shooting/photograph_icon_single.png"
#define PHOTOGRAPH_ICON_SELECT "V:tk1/realtime_shooting/photograph_icon_select.png"
#define PHOTOGRAPH_ICON_UNSELECT "V:tk1/realtime_shooting/photograph_icon_unselect.png"
#define PHOTOGRAPH_ICON_SELECT_GREEN "V:tk1/realtime_shooting/photograph_icon_select_green.png"
#define LUT_ICON_NONE "V:tk1/realtime_shooting/lut_icon_none.png"
#define PHOTOGRAPH_ICON_GUIDE_GREEN "V:tk1/realtime_shooting/photograph_icon_guide_green.png"
#define PHOTOGRAPH_PIC_PHOTO "V:tk1/realtime_shooting/photograph_pic_photo.png"
#define PHOTOGRAPH_PIC_VIDEO "V:tk1/realtime_shooting/photograph_pic_video.png"
#define PHOTOGRAPH_PIC_TIME_LAPSE "V:tk1/realtime_shooting/photograph_pic_time_lapse.png"
#define PHOTOGRAPH_ICON_QUESTION "V:tk1/realtime_shooting/photograph_icon_question.png"
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
static void lv_photo_picture(lv_obj_t * parent, int type);
static void lv_realtime_shooting_rapid_coding(lv_obj_t * parent, int type);

static void recreate_video_list(lv_obj_t *parent);
static void create_video_list(lv_obj_t *parent);
static void update_center_contact_color(lv_obj_t *obj);
/**********************
 *  STATIC VARIABLES
 **********************/

static lv_timer_t *timer_2;//录像的秒数
static lv_timer_t *timer_1;//录像播放的进度条
static uint32_t record_sec = 0;
static uint32_t record_play_sec = 0;
static uint32_t record_total_play_sec = 0;

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
static lv_style_t video_time_style;
static lv_style_t style_bg;
static lv_style_t style_indicator;
static lv_style_t style_knob;
static lv_style_t style_file_info;
static lv_style_t style_multi_filter;

static lv_style_t style_black_background;

// 全局变量
static lv_obj_t * flash_img;
static lv_obj_t * zoom_label;
static lv_obj_t *photo_all_label;
static lv_obj_t *video_play_indicator_area;
static lv_obj_t *line_cont;
static uint8_t exec_count = 0;
static lv_timer_t *anim_timer = NULL;

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

static lv_obj_t *vieo_progress_bar;
static lv_obj_t *knob;
static lv_obj_t * video_play_time_label;

static lv_obj_t * slider;          // 滑动条对象
static bool is_playing = false;    // 播放状态标志

// 全局变量
static int selected_count = 0;
static lv_obj_t *selected_count_label = NULL;
static bool selection_mode = false;
static bool *item_selected = NULL; // 记录每个项目的选中状态
static int total_recod_cnt = 8; //todo ：获取实际的录像个数

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

 
    lv_style_init(&up_area_roller_style);
    lv_style_set_border_opa(&up_area_roller_style, LV_OPA_TRANSP);
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

    lv_style_init(&video_time_style);
    static lv_grad_dsc_t video_time_grad;
    video_time_grad.dir = LV_GRAD_DIR_VER;
    video_time_grad.stops_count = 3;
    video_time_grad.stops[0].color = lv_color_hex(0xC3C3C3);
    video_time_grad.stops[0].opa = LV_OPA_COVER;
    video_time_grad.stops[0].frac = 0;

    video_time_grad.stops[1].color = lv_color_hex(0xFFFFFF);
    video_time_grad.stops[1].opa = LV_OPA_COVER;
    video_time_grad.stops[1].frac = 128;

    video_time_grad.stops[1].color = lv_color_hex(0xC3C3C3);
    video_time_grad.stops[1].opa = LV_OPA_COVER;
    video_time_grad.stops[1].frac = 255;

    lv_style_set_bg_grad(&video_time_style, &down_grad);

    // 创建进度条背景样式
    lv_style_init(&style_bg);
    lv_style_set_bg_color(&style_bg, lv_color_hex(0xD8D8D8));
    lv_style_set_bg_opa(&style_bg, LV_OPA_30);
    lv_style_set_radius(&style_bg, 4);
    
    // 创建进度条指示器样式
    lv_style_init(&style_indicator);
    lv_style_set_bg_color(&style_indicator, lv_color_hex(0xD8D8D8));
    lv_style_set_bg_opa(&style_indicator, LV_OPA_COVER);
    lv_style_set_radius(&style_indicator, 0);

    // 创建进度条旋钮样式
    lv_style_init(&style_knob);
    lv_style_set_bg_color(&style_knob, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&style_knob, LV_OPA_COVER);

    // 创建进度条旋钮样式
    lv_style_init(&style_file_info);
    lv_style_set_bg_color(&style_file_info, lv_color_hex(0x121212));
    lv_style_set_bg_opa(&style_file_info, LV_OPA_COVER);
    lv_style_set_radius(&style_file_info, 20);
    lv_style_set_border_width(&style_file_info, 0);
    lv_style_set_pad_all(&style_file_info, 0);

    lv_style_init(&style_multi_filter);
    static lv_grad_dsc_t multi_filter_grad;
    multi_filter_grad.dir = LV_GRAD_DIR_HOR;
    multi_filter_grad.stops_count = 2;
    multi_filter_grad.stops[0].color = lv_color_hex(0x000000);
    multi_filter_grad.stops[0].opa = LV_OPA_TRANSP;
    multi_filter_grad.stops[0].frac = 0;

    multi_filter_grad.stops[1].color = lv_color_hex(0x000000);
    multi_filter_grad.stops[1].opa = LV_OPA_COVER;
    multi_filter_grad.stops[1].frac = 255;

    lv_style_set_bg_grad(&style_multi_filter, &multi_filter_grad);

    lv_style_init(&style_black_background);
    lv_style_set_radius(&style_black_background, 0);
    lv_style_set_pad_all(&style_black_background, 0);
    lv_style_set_border_width(&style_black_background, 0);
    lv_style_set_bg_color(&style_black_background, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&style_black_background, LV_OPA_COVER);


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

// 全局变量记录旋转状态
static int16_t initial_rotation = 210;  // 初始角度
static int16_t current_rotation = 0;    // 当前相对于初始角度的偏移
static int16_t max_left_rotation = -120; // 最大向左旋转角度
static int16_t left_rotation_amount = 0; // 已向左旋转的角度

static lv_scale_section_t * section;
// 旋转刻度盘的函数
void rotate_scale(lv_obj_t *scale, int16_t angle_change) 
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
        printf("Section range set to (0, 1) - Initial position\n");
    }
    else if (current_rotation == -60) 
    {
        // 向左转动60度：选定(9, 11)范围
        lv_scale_section_set_range(section, 9, 11);
        printf("Section range set to (9, 11) - 60° left rotation\n");
    }
    else if (current_rotation == -120) 
    {
        // 向左转动120度：选定(19, 20)范围
        lv_scale_section_set_range(section, 19, 20);
        printf("Section range set to (19, 20) - 120° left rotation\n");
    }
    else
    {
        // 非特殊角度：取消选中效果
        lv_scale_section_set_range(section, -1, -1);
        printf("No selection at %d°\n", current_rotation);
    }
    
    printf("New rotation: %d (absolute: %d)\n", current_rotation, initial_rotation + current_rotation);
}

// 向左旋转按钮事件回调
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

// 添加长按事件处理 - 显示转盘
static void zoom_btn_long_press_handler(lv_event_t * e) 
{
    static bool is_long_pressed = false;  // 全局标志位，用于标记是否已处理长按
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_LONG_PRESSED) 
    {
        is_long_pressed = true;  // 标记已处理长按
        printf("长按出现调整倍率\n");

        lv_obj_t *screen = lv_obj_create(NULL);
        lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
        lv_scr_load(screen);

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

static void screen_saver_timer_cb(lv_timer_t *timer)
{
    uint8_t *pdir = lv_timer_get_user_data(timer);

    circular_scroll_handle(line_cont, *pdir);
    //次数控制
    if(++exec_count >= 30) {
        lv_timer_del(anim_timer);
        anim_timer = NULL;
    }
}

static void scroll_saver_event_cb(lv_event_t *e)
{
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

        int32_t angle = -(diff_y) / 3;
        angle = LV_ABS(diff_y) >= 60? angle : 0;
        lv_obj_set_style_translate_x(child, x - 20, 0);
        lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
    }

    if (cur_idx != last_idx) {
        last_idx = cur_idx;
        if (!anim_timer) {
            //创建定时器，每40ms执行滚动动态
            anim_timer = lv_timer_create(screen_saver_timer_cb, 40, &dir);
            exec_count = 0;
        }
    }
}

static void video_mode_click_event_cb(lv_event_t *e)
{
    lv_obj_t *saver = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *mode = lv_event_get_user_data(e);
    if (LV_EVENT_CLICKED == code)
    {
        if(lv_obj_get_child(mode, 0) == saver)
        {
            printf("mode photo\n");
        }
        else if(lv_obj_get_child(mode, 1) == saver)
        {
            printf("mode video\n");
        }
        else if(lv_obj_get_child(mode, 2) == saver)
        {
            printf("mode time_lapse\n");
        }
    }
}

static void *video_mode_create(int iMode, lv_obj_t *cont, const char *path)
{
    lv_obj_t *mode = lv_obj_create(cont);
    lv_obj_remove_style_all(mode);
    lv_obj_set_size(mode, 376, 140);
    lv_obj_set_style_radius(mode, 30, 0);
    lv_obj_set_style_bg_opa(mode, LV_OPA_COVER, 0);

    if(iMode == 0)
    {
        lv_obj_align(mode, LV_ALIGN_TOP_LEFT, 26, 0);
        lv_obj_set_style_bg_color(mode, lv_color_hex(0x4169E1), LV_PART_MAIN);
        lv_obj_t *label = lv_label_create(mode);
        lv_label_set_text(label, "PHOTO");
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label, font_get_regular(30), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 154, 32);

        lv_obj_t *label1 = lv_label_create(mode);
        lv_label_set_text(label1, "拍照");
        lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label1, font_get_regular(28), 0);
        lv_obj_set_style_text_color(label1, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 154, 72);
    }
    else if(iMode == 1)
    {
        lv_obj_align(mode, LV_ALIGN_LEFT_MID, 26, 0);
        lv_obj_set_style_bg_color(mode, lv_color_hex(0x28272E), LV_PART_MAIN);

        lv_obj_t *label = lv_label_create(mode);
        lv_label_set_text(label, "VIDEO");
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label, font_get_regular(30), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 154, 32);

        lv_obj_t *label1 = lv_label_create(mode);
        lv_label_set_text(label1, "录像");
        lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label1, font_get_regular(28), 0);
        lv_obj_set_style_text_color(label1, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 154, 72);
    }
    else if(iMode == 2)
    {
        lv_obj_align(mode, LV_ALIGN_BOTTOM_LEFT, 26, 0);
        lv_obj_set_style_bg_color(mode, lv_color_hex(0xD1946A), LV_PART_MAIN);
        
        lv_obj_t *label = lv_label_create(mode);
        lv_label_set_text(label, "TIME-LAPSE");
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label, font_get_regular(30), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 154, 32);

        lv_obj_t *label1 = lv_label_create(mode);
        lv_label_set_text(label1, "延时摄影");
        lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label1, font_get_regular(28), 0);
        lv_obj_set_style_text_color(label1, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 154, 72);
    }

    lv_obj_t *image = lv_img_create(mode);
    lv_obj_set_size(image, 128, 128);
    lv_img_set_src(image, path);
    lv_obj_align(image, LV_ALIGN_LEFT_MID, 6, 0);

    return mode;
}

static void *line_container_create(lv_obj_t *cont)
{
    lv_obj_t *line_cont = lv_obj_create(cont);

    lv_obj_add_style(line_cont, &style_black_background, 0);
    lv_obj_set_size(line_cont, 20, 150);
    lv_obj_align(line_cont, LV_ALIGN_RIGHT_MID, -20, 0);

    lv_obj_set_flex_flow(line_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(line_cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(line_cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(line_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(line_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_row(line_cont, 8, 0);

    for (uint8_t i = 0; i < 39; i++)
    {
        lv_obj_t *line = lv_obj_create(line_cont);
        lv_obj_remove_style_all(line);
        lv_obj_set_size(line, 30, 2);
        lv_obj_set_style_bg_opa(line, LV_OPA_20, 0);
        lv_obj_set_style_bg_color(line, lv_color_hex(0xFFFFFF), 0);
    }

    //初始位置设置为中间的子对象
    lv_obj_scroll_to_view(lv_obj_get_child(line_cont, 19), LV_ANIM_OFF);
    circular_scroll_handle(line_cont, 0);

    return line_cont;
}

static const char *mode_list[3] = {
    PHOTOGRAPH_PIC_PHOTO,
    PHOTOGRAPH_PIC_VIDEO,
    PHOTOGRAPH_PIC_TIME_LAPSE
};

static void lv_video_mode_style(lv_obj_t * parent)
{
    lv_obj_add_style(parent, &style_black_background, 0);

    //创建录像模式列表转盘
    lv_obj_t *cont_col = lv_obj_create(parent);
    lv_obj_add_style(cont_col, &style_black_background, 0);

    // 移除flex布局，使用绝对布局
    lv_obj_set_size(cont_col, lv_pct(100), lv_pct(100));
    lv_obj_set_layout(cont_col, LV_LAYOUT_NONE);
    lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 26, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont_col, scroll_saver_event_cb, LV_EVENT_SCROLL, NULL);


    for (uint8_t i = 0; i < 3; i++)
    {
        lv_obj_t *video_mode = video_mode_create(i, cont_col, mode_list[i]);
        lv_obj_add_event_cb(video_mode, video_mode_click_event_cb, LV_EVENT_CLICKED, cont_col);
    }

    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, 1), LV_ANIM_OFF);
    lv_obj_send_event(cont_col, LV_EVENT_SCROLL, NULL);

    lv_scr_load_anim(parent, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);

    lv_obj_t *image = lv_img_create(parent);
    lv_obj_set_size(image, 42, 42);
    lv_img_set_src(image, PHOTOGRAPH_ICON_QUESTION);
    lv_obj_align(image, LV_ALIGN_RIGHT_MID, -50, 0);

    //右侧滚动条
    line_cont = line_container_create(parent);

    return;
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

    //todo:等待后续的动态图像
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
    //lv_obj_t *time_area = (lv_obj_t *)lv_timer_get_user_data(timer);

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



static void get_curr_playback_cnt_and_totol(int* iCurrCnt, int* iTotal) 
{
    *iCurrCnt = 3;
    *iTotal = 98;
}

// 定时器回调，更新当前显示第几张图片/总多少张图片
static void photo_timer_cb(lv_timer_t * timer) 
{
    int iCurrCnt = 0;
    int iTotal = 0;
    get_curr_playback_cnt_and_totol(&iCurrCnt, &iTotal);
    char photo_cnt[16];
    snprintf(photo_cnt, sizeof(photo_cnt), "全部%d/%d", iCurrCnt, iTotal);
    lv_label_set_text(photo_all_label, photo_cnt);
}

static void four_grid_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("four_grid icon clicked!");
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_scr_load(screen);
    lv_photo_picture(screen, 7);
}

static void single_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("single icon clicked!");
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_scr_load(screen);
}

static void select_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("select icon clicked!");
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_scr_load(screen);
}

static void lable_click_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    int *video_type = (int *)lv_event_get_user_data(e);
    if(code == LV_EVENT_CLICKED)
    {
        printf("video_type:%d\n", *video_type);
        // 关闭界面
        lv_obj_add_flag(lv_obj_get_parent(lv_event_get_target(e)), LV_OBJ_FLAG_HIDDEN);

        //todo:更改lable的标签数值
        if(*video_type == 0)
        {
            lv_label_set_text(photo_all_label, "全部98"); //todo：后续根据实际情况去显示对应的值
        }
        else if(*video_type == 1)
        {
            lv_label_set_text(photo_all_label, "照片89"); //todo：后续根据实际情况去显示对应的值
        }
        else if(*video_type == 2)
        {
            lv_label_set_text(photo_all_label, "视频7"); //todo：后续根据实际情况去显示对应的值
        }
        else if(*video_type == 3)
        {
            lv_label_set_text(photo_all_label, "延时摄影3"); //todo：后续根据实际情况去显示对应的值
        }
    }
}
static void allows_down_icon_click_event(lv_event_t * e)
{
    static int video_type[4] = {0,1,2,3};
    // 处理点击事件的逻辑
    LV_LOG_USER("allows down icon clicked!");
    lv_obj_t* icon = lv_event_get_target(e);
    lv_obj_t* temp_parent = lv_obj_get_parent(icon);  // 获取父对象
    lv_obj_t* parent = lv_obj_get_parent(temp_parent);  // 获取父对象
    lv_obj_t *video_pop_obj = lv_obj_create(parent);
    lv_obj_set_size(video_pop_obj, 270, 274);
    lv_obj_set_style_bg_color(video_pop_obj, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_radius(video_pop_obj, 40, 0);
    lv_obj_set_style_border_width(video_pop_obj, 0, 0);
    lv_obj_align(video_pop_obj, LV_ALIGN_TOP_LEFT, 106, 75);

    lv_obj_t* label = lv_label_create(video_pop_obj);
    lv_label_set_text(label, "全部 (98)"); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XAFF99C), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 30);

    // 点击照片标签关闭界面
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label, lable_click_event, LV_EVENT_CLICKED, &video_type[0]);
    

    lv_obj_t* label1 = lv_label_create(video_pop_obj);
    lv_label_set_text(label1, "照片 (89)"); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label1, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label1, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 91);
    // 点击照片标签关闭界面
    lv_obj_add_flag(label1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label1, lable_click_event, LV_EVENT_CLICKED, &video_type[1]);

    lv_obj_t* label2 = lv_label_create(video_pop_obj);
    lv_label_set_text(label2, "视频 (7)"); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label2, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label2, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 152);

    lv_obj_add_flag(label2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label2, lable_click_event, LV_EVENT_CLICKED, &video_type[2]);

    lv_obj_t* label3 = lv_label_create(video_pop_obj);
    lv_label_set_text(label3, "延时摄影 (3)"); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label3, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label3, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label3, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 213);

    lv_obj_add_flag(label3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label3, lable_click_event, LV_EVENT_CLICKED, &video_type[3]);

}

static void photo_icon_share_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("photo icon share clicked!");
    lv_obj_t *screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_scr_load(screen2);
}

static void photo_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("photo icon clicked!");
    lv_obj_t *screen2 = lv_obj_create(lv_event_get_current_target(e));
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_scr_load(screen2);
    lv_photo_picture(screen2, 2);
}

static void photo_info_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("photo info icon clicked!");
    lv_obj_t *screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_scr_load(screen2);
    lv_photo_picture(screen2, 6);
}

static void trash_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("trash icon clicked!");
}

static void cancel_icon_click_event(lv_event_t * e)
{
    // 返回当前界面，暂不删除文件
    LV_LOG_USER("cancel icon clicked!");
}

static void ok_icon_click_event(lv_event_t * e)
{
    // 删除当前文件
    LV_LOG_USER("ok icon clicked!");
    //todo:删除当前文件
}


static void cancel_buton_click_event(lv_event_t * e)
{
    // 返回当前界面，暂不删除文件
    LV_LOG_USER("cancel icon clicked!");
}

static void retry_buton_click_event(lv_event_t * e)
{
    LV_LOG_USER("retry buton clicked!");
}

// 更新时间显示标签
static void update_time_label(void)
{
    char play_time_str[20] = {0};
    snprintf(play_time_str, sizeof(play_time_str), "%02d:%02d:%02d/%02d:%02d:%02d", record_play_sec / 3600, record_play_sec / 60, record_play_sec % 60, record_total_play_sec / 3600, record_total_play_sec / 60, record_total_play_sec % 60);
    lv_label_set_text(video_play_time_label, play_time_str);
}

// 播放完成处理函数
static void playback_finished(void)
{
    is_playing = false;
    lv_timer_pause(timer_1);
    
    // 确保滑动条在最大值位置
    lv_slider_set_value(slider, 100, LV_ANIM_ON);
}

// 播放定时器回调函数
static void play_timer_cb(lv_timer_t * timer)
{
    if(record_play_sec < record_total_play_sec) 
    {
        record_play_sec ++; // 每次增加1000毫秒
        
        // 更新滑动条值（0-100范围）
        lv_slider_set_value(slider, record_play_sec * 100 / record_total_play_sec, LV_ANIM_OFF);
        
        // 更新时间显示标签
        update_time_label();
    }
    else 
    {
        // 播放完成处理
        playback_finished();
    }
}

// 滑动条事件回调函数
static void slider_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) 
    {
        // 根据滑动条位置计算当前时间
        int32_t slider_val = lv_slider_get_value(slider);
        record_play_sec = (slider_val * record_total_play_sec) / 100; 
        
        // 更新时间显示
        update_time_label();
        
        // 如果正在播放，暂停播放（用户拖动时暂停）
        if(is_playing) 
        {
            lv_timer_pause(timer_1);
        }
    }
    else if(code == LV_EVENT_RELEASED) 
    {
        // 滑动条释放后，如果之前是播放状态则继续播放
        if(is_playing) 
        {
            lv_timer_resume(timer_1);
        }
    }
}

// 播放/暂停按钮回调
static void play_pause_btn_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_CLICKED) 
    {
        if(!is_playing) 
        {
            // 开始播放
            is_playing = true;
            lv_timer_resume(timer_1);
            
            // 如果播放完成，重置到开始
            if(record_play_sec >= record_total_play_sec) 
            {
                record_play_sec = 0;
                lv_slider_set_value(slider, 0, LV_ANIM_OFF);
                update_time_label();
            }
        } 
        else 
        {
            // 暂停播放
            is_playing = false;
            lv_timer_pause(timer_1);
        }
    }
}

// 存储联系人对象指针的数组
static lv_obj_t *contact_objs[20];
static void page_back_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_CLICKED == code)
    {
        printf("contact page return back\n");
        //TODO: 回到上个页面

    }
}

static void contact_page_click_event_cb(lv_event_t *e)
{
    lv_obj_t *saver = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *mode = lv_event_get_user_data(e);
    if (LV_EVENT_CLICKED == code)
    {
        printf("mode time_lapse\n");
    }
}

static void update_center_contact_color(lv_obj_t *obj)
{
    // 获取滚动位置
    lv_coord_t scroll_y = lv_obj_get_scroll_y(obj);
    lv_coord_t height = lv_obj_get_height(obj);
    
    // 计算中间位置
    lv_coord_t center_y = scroll_y + height / 2;
    
    // 重置所有联系人的背景颜色
    for (uint8_t i = 0; i < 20; i++) {
        lv_obj_set_style_bg_color(contact_objs[i], lv_color_hex(0x000000), LV_PART_MAIN);
    }
    
    // 找到最接近中间位置的联系人
    int closest_index = -1;
    lv_coord_t min_distance = INT16_MAX;
    
    for (uint8_t i = 0; i <= 20; i++) {
        lv_coord_t contact_y = i * (160 + 12) + 80; // 联系人的中心Y坐标
        lv_coord_t distance = LV_ABS(contact_y - center_y);
        
        if (distance < min_distance) {
            min_distance = distance;
            closest_index = i-1;
        }
    }
    
    // 高亮中间的联系人
    if (closest_index >= 0 && closest_index <= 20) {
        lv_obj_set_style_bg_color(contact_objs[closest_index], lv_color_hex(0x2A3534), LV_PART_MAIN);
    }
}


static void contact_scroll_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    lv_obj_t *first = lv_obj_get_child(cont, 0);
    lv_area_t first_a;
    lv_obj_get_coords(first, &first_a);

    update_center_contact_color(cont); //高亮选中联系人

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

        int32_t angle = -(diff_y) / 3;
        angle = LV_ABS(diff_y) >= 60? angle : 0;
        lv_obj_set_style_translate_x(child, x - 20, 0);
        lv_obj_set_style_transform_rotation(child, angle, LV_PART_MAIN);
    }

    if (cur_idx != last_idx) {
        last_idx = cur_idx;
        if (!anim_timer) {
            //创建定时器，每40ms执行滚动动态
            anim_timer = lv_timer_create(screen_saver_timer_cb, 40, &dir);
            exec_count = 0;
        }
    }
}
static void lv_photo_picture(lv_obj_t * parent, int type)
{
    if(type == 0)
    {
        lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

        lv_obj_t * empty_photo_icon = lv_img_create(parent);
        lv_img_set_src(empty_photo_icon, EMPTY_PIC_PHOTO);
        lv_obj_set_size(empty_photo_icon, 380, 210);
        lv_obj_align(empty_photo_icon, LV_ALIGN_TOP_LEFT, 61, 60);

        //设置文字到主菜单
        lv_obj_t *label = lv_label_create(parent);
        lv_label_set_text(label, "暂无内容");
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label, font_get_regular(26), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);

        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 290);
    }
    else if(type == 1)
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
        lv_obj_set_size(up_indicator_area, 502, 156);
        lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_add_style(up_indicator_area, &up_area_style, 0);

        lv_obj_t * photo_icon_four_grid = lv_img_create(up_indicator_area);
        lv_img_set_src(photo_icon_four_grid, PHOTOGRAPH_ICON_FOUR_GRID);
        lv_obj_set_size(photo_icon_four_grid, 40, 40);
        lv_obj_align(photo_icon_four_grid, LV_ALIGN_TOP_LEFT, 30, 20);

        lv_obj_add_flag(photo_icon_four_grid, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_four_grid, four_grid_icon_click_event, LV_EVENT_CLICKED, NULL);

        
        photo_all_label = lv_label_create(up_indicator_area);
        lv_label_set_text(photo_all_label, "全部1/6"); //后续根据实际情况去显示对应的值
        lv_obj_set_style_text_opa(photo_all_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(photo_all_label, font_get_regular(32), 0);
        lv_obj_set_style_text_color(photo_all_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(photo_all_label, LV_ALIGN_TOP_RIGHT, -215, 19);

        lv_obj_t * photo_icon_allows_down = lv_img_create(up_indicator_area);
        lv_img_set_src(photo_icon_allows_down, PHOTOGRAPH_ICON_ALLOWS_DOWN);
        lv_obj_set_size(photo_icon_allows_down, 40, 40);
        lv_obj_align_to(photo_icon_allows_down, photo_all_label, LV_ALIGN_OUT_RIGHT_MID, 5, 0);  // 图标右侧，垂直居中

        lv_obj_t * photo_icon_trash_filled = lv_img_create(up_indicator_area);
        lv_img_set_src(photo_icon_trash_filled, PHOTOGRAPH_ICON_TRASH_FILLED);
        lv_obj_set_size(photo_icon_trash_filled, 40, 40);
        lv_obj_align(photo_icon_trash_filled, LV_ALIGN_TOP_LEFT, 432, 20);
        lv_obj_add_flag(photo_icon_trash_filled, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_trash_filled, trash_icon_click_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t * photo_icon_share = lv_img_create(parent);
        lv_img_set_src(photo_icon_share, PHOTOGRAPH_ICON_SHARE);
        lv_obj_set_size(photo_icon_share, 40, 40);
        lv_obj_align(photo_icon_share, LV_ALIGN_TOP_LEFT, 30, 185);
        lv_obj_add_flag(photo_icon_share, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_share, photo_icon_share_click_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t * photo_icon_stop = lv_img_create(parent);
        lv_img_set_src(photo_icon_stop, PHOTOGRAPH_ICON_STOP);
        lv_obj_set_size(photo_icon_stop, 100, 100);
        lv_obj_align(photo_icon_stop, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(photo_icon_stop, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_stop, photo_icon_click_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t * buttom_line = lv_obj_create(parent);
        lv_obj_set_size(buttom_line, 6, 38);
        lv_obj_set_style_radius(buttom_line, 4, 0);
        lv_obj_set_style_bg_color(buttom_line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align(buttom_line, LV_ALIGN_TOP_LEFT, 473, 183); 
        lv_obj_set_style_opa(buttom_line, LV_OPA_COVER, 0);

        // 创建底部矩形渐变框
        lv_obj_t *down_indicator_area = lv_obj_create(parent);
        lv_obj_set_size(down_indicator_area, 502, 156);
        lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_add_style(down_indicator_area, &down_area_style, 0);

        lv_obj_t * photo_icon_info = lv_img_create(down_indicator_area);
        lv_img_set_src(photo_icon_info, PHOTOGRAPH_ICON_INFO);
        lv_obj_set_size(photo_icon_info, 40, 40);
        lv_obj_align(photo_icon_info, LV_ALIGN_BOTTOM_LEFT, 30, -20);
        lv_obj_add_flag(photo_icon_info, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_info, photo_info_icon_click_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t * photo_intell_icon = lv_img_create(down_indicator_area);
        lv_img_set_src(photo_intell_icon, INERTIGENCE_ICON);
        lv_obj_set_size(photo_intell_icon, 66, 66);
        lv_obj_align(photo_intell_icon, LV_ALIGN_BOTTOM_RIGHT, -20, -20);

        lv_obj_t *video_time_label = lv_label_create(down_indicator_area);
        lv_obj_add_style(video_time_label, &video_time_style, LV_PART_MAIN);
        lv_label_set_text(video_time_label, "30:30"); //后续根据实际情况去显示对应的值
        lv_obj_set_style_text_opa(video_time_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(video_time_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(video_time_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(video_time_label, LV_ALIGN_TOP_LEFT, 215, 95);

        // 创建定时器更新界面显示
        lv_timer_create(photo_timer_cb, 500, NULL); // 每500ms更新一次
    }
    else if(type == 2)
    {
        // 创建实时取景背景（假设是图像或视频组件）
        lv_obj_t * live_view = lv_image_create(parent);
        lv_obj_add_style(live_view, &style, 0);
        lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
        lv_image_set_src(live_view, "V:png/img_camera_backup.png");
        lv_img_set_zoom(live_view, 128);
        lv_obj_center(live_view);
        
        // 创建顶部矩形渐变框
        video_play_indicator_area = lv_obj_create(parent);
        lv_obj_set_size(video_play_indicator_area, 502, 156);
        lv_obj_align(video_play_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_add_style(video_play_indicator_area, &up_area_style, 0);

        lv_obj_t * photo_icon_four_grid = lv_img_create(video_play_indicator_area);
        lv_img_set_src(photo_icon_four_grid, PHOTOGRAPH_ICON_FOUR_GRID);
        lv_obj_set_size(photo_icon_four_grid, 40, 40);
        lv_obj_align(photo_icon_four_grid, LV_ALIGN_TOP_LEFT, 30, 20);

        //修改为录像总时长，当前正在播放的时间
        video_play_time_label = lv_label_create(video_play_indicator_area);
        char play_time_str[20] = {0};
        record_total_play_sec = 40; //后续根据实际情去获取录像的总时长
        snprintf(play_time_str, sizeof(play_time_str), "00:00:00/%02d:%02d:%02d", record_total_play_sec / 3600, record_total_play_sec / 60, record_total_play_sec % 60);
        lv_label_set_text(video_play_time_label, play_time_str);
        lv_obj_set_style_text_letter_space(video_play_time_label, 2, LV_PART_MAIN); // 设置字符间距
        lv_obj_set_style_text_opa(video_play_time_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(video_play_time_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(video_play_time_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(video_play_time_label, LV_ALIGN_TOP_MID, 0, 20);

        lv_obj_t * photo_icon_trash_filled = lv_img_create(video_play_indicator_area);
        lv_img_set_src(photo_icon_trash_filled, PHOTOGRAPH_ICON_TRASH_FILLED);
        lv_obj_set_size(photo_icon_trash_filled, 40, 40);
        lv_obj_align(photo_icon_trash_filled, LV_ALIGN_TOP_LEFT, 432, 20);

        lv_obj_t * photo_icon_share = lv_img_create(parent);
        lv_img_set_src(photo_icon_share, PHOTOGRAPH_ICON_SHARE);
        lv_obj_set_size(photo_icon_share, 40, 40);
        lv_obj_align(photo_icon_share, LV_ALIGN_TOP_LEFT, 30, 185);

        lv_obj_t * photo_icon_stop = lv_img_create(parent);
        lv_img_set_src(photo_icon_stop, PHOTOGRAPH_ICON_PLAY);
        lv_obj_set_size(photo_icon_stop, 100, 100);
        lv_obj_align(photo_icon_stop, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(photo_icon_stop, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_stop, photo_icon_click_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t * buttom_line = lv_obj_create(parent);
        lv_obj_set_size(buttom_line, 6, 38);
        lv_obj_set_style_radius(buttom_line, 4, 0);
        lv_obj_set_style_bg_color(buttom_line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align(buttom_line, LV_ALIGN_TOP_LEFT, 473, 183); 
        lv_obj_set_style_opa(buttom_line, LV_OPA_COVER, 0);

        // 创建底部矩形渐变框
        lv_obj_t *down_indicator_area = lv_obj_create(parent);
        lv_obj_set_size(down_indicator_area, 502, 156);
        lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_add_style(down_indicator_area, &down_area_style, 0);
    

        // 创建滑动条
        slider = lv_slider_create(down_indicator_area);
        lv_obj_set_size(slider, 457, 6);
        lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -36);

        // 应用样式
        lv_obj_add_style(slider, &style_bg, LV_PART_MAIN);
        lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
        lv_slider_set_range(slider, 0, 100);

        // lv_obj_t *knob_obj = lv_obj_create(down_indicator_area);
        // lv_obj_set_size(knob_obj, 40, 20);
        // lv_obj_set_style_radius(knob_obj, 15, 0);
        // lv_obj_set_style_bg_color(knob_obj, lv_color_hex(0xFFFFFF), 0);
        // lv_obj_align_to(knob_obj, slider, LV_ALIGN_LEFT_MID, 0, 0);
        // lv_obj_clear_flag(knob_obj, LV_OBJ_FLAG_SCROLLABLE);   
        
        lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_ALL, NULL);
    
    
        //创建播放定时器（初始为暂停状态）
        timer_1 = lv_timer_create(play_timer_cb, 1000, NULL); // 100ms间隔
    }
    else if(type == 3)
    {
        // 创建实时取景背景（假设是图像或视频组件）
        lv_obj_t * live_view = lv_image_create(parent);
        lv_obj_add_style(live_view, &style, 0);
        lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
        lv_image_set_src(live_view, "V:png/img_camera_backup.png");
        lv_img_set_zoom(live_view, 128);
        lv_obj_center(live_view);

        // 在顶层创建模态弹窗
        lv_obj_t* top_screen = lv_obj_create(lv_layer_top());
        lv_obj_set_size(top_screen, 502, 410);
        lv_obj_center(top_screen);

        lv_obj_set_style_bg_color(top_screen, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(top_screen, LV_OPA_80, 0);
        lv_obj_set_style_border_width(top_screen, 0, 0);
        lv_obj_set_style_pad_all(top_screen, 0, 0);
        lv_obj_set_style_radius(top_screen, 0, 0);

        //设置文字到主菜单
        lv_obj_t *label = lv_label_create(top_screen);
        lv_label_set_text(label, "删除此文件");
        lv_obj_set_style_text_opa(label, LV_OPA_90, 0);
        lv_obj_set_style_text_font(label, font_get_regular(30), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 165);

        lv_obj_t *btn_obj = lv_obj_create(top_screen);
        lv_obj_set_size(btn_obj, 148, 70);
        lv_obj_set_style_radius(btn_obj, 51, 0);
        lv_obj_set_style_opa(btn_obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(btn_obj, lv_color_hex(0x2C2C2E), 0);
        lv_obj_set_style_border_width(btn_obj, 0, 0);
        lv_obj_align(btn_obj, LV_ALIGN_TOP_LEFT, 78, 310);
        lv_obj_clear_flag(btn_obj, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t * comm_icon_cancel = lv_img_create(btn_obj);
        lv_img_set_src(comm_icon_cancel, COMM_ICON_CANCEL);
        lv_obj_set_size(comm_icon_cancel, 50, 50);
        lv_obj_align(comm_icon_cancel, LV_ALIGN_CENTER, 0, 0);

        lv_obj_add_event_cb(btn_obj, cancel_icon_click_event, LV_EVENT_CLICKED, NULL);
        
        lv_obj_t *btn1_obj = lv_obj_create(top_screen);
        lv_obj_set_size(btn1_obj, 148, 70);
        lv_obj_set_style_radius(btn1_obj, 51, 0);
        lv_obj_set_style_opa(btn1_obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(btn1_obj, lv_color_hex(0xAFF99C), 0);
        lv_obj_set_style_border_width(btn1_obj, 0, 0);
        lv_obj_align(btn1_obj, LV_ALIGN_TOP_LEFT, 276, 310);
        lv_obj_clear_flag(btn1_obj, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t * comm_icon_ok = lv_img_create(btn1_obj);
        lv_img_set_src(comm_icon_ok, COMM_ICON_OK);
        lv_obj_set_size(comm_icon_ok, 50, 50);
        lv_obj_align(comm_icon_ok, LV_ALIGN_CENTER, 0, 0);

        lv_obj_add_event_cb(btn1_obj, ok_icon_click_event, LV_EVENT_CLICKED, NULL);

    }
    else if(type == 4)
    {
        lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

        lv_obj_t *btn_obj = lv_obj_create(parent);
        lv_obj_set_size(btn_obj, 208, 92);
        lv_obj_set_style_radius(btn_obj, 16, 0);
        lv_obj_set_style_opa(btn_obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(btn_obj, lv_color_hex(0x121212), 0);
        lv_obj_set_style_border_width(btn_obj, 0, 0);
        lv_obj_align(btn_obj, LV_ALIGN_CENTER, 0, 0);

        //设置文字到主菜单
        lv_obj_t *label = lv_label_create(btn_obj);
        lv_label_set_text(label, "分享成功");
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label, font_get_regular(32), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    }
    else if(type == 5)
    {
        lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
        lv_obj_set_style_border_width(parent, 0, 0);

        lv_obj_t * photo_pic_fail_icon = lv_img_create(parent);
        lv_img_set_src(photo_pic_fail_icon, PHOTOGRAPH_PIC_FAIL);
        lv_obj_set_size(photo_pic_fail_icon, 258, 258);
        lv_obj_align(photo_pic_fail_icon, LV_ALIGN_TOP_LEFT, 115, 32);

        lv_obj_t *btn_obj = lv_obj_create(parent);
        lv_obj_set_size(btn_obj, 148, 70);
        lv_obj_set_style_radius(btn_obj, 51, 0);
        lv_obj_set_style_opa(btn_obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(btn_obj, lv_color_hex(0x2C2C2E), 0);
        lv_obj_set_style_border_width(btn_obj, 0, 0);
        lv_obj_align(btn_obj, LV_ALIGN_TOP_LEFT, 78, 310);
        lv_obj_clear_flag(btn_obj, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = lv_label_create(btn_obj);
        lv_label_set_text(label, "取消");
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label, font_get_regular(30), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        lv_obj_add_event_cb(btn_obj, cancel_buton_click_event, LV_EVENT_CLICKED, NULL);
        
        lv_obj_t *btn1_obj = lv_obj_create(parent);
        lv_obj_set_size(btn1_obj, 148, 70);
        lv_obj_set_style_radius(btn1_obj, 51, 0);
        lv_obj_set_style_opa(btn1_obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(btn1_obj, lv_color_hex(0xAFF99C), 0);
        lv_obj_set_style_border_width(btn1_obj, 0, 0);
        lv_obj_align(btn1_obj, LV_ALIGN_TOP_LEFT, 276, 310);
        lv_obj_clear_flag(btn1_obj, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label1 = lv_label_create(btn1_obj);
        lv_label_set_text(label1, "重试");
        lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label1, font_get_regular(30), 0);
        lv_obj_set_style_text_color(label1, lv_color_hex(0X000000), 0);
        lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

        lv_obj_add_event_cb(btn1_obj, retry_buton_click_event, LV_EVENT_CLICKED, NULL);

    }
    else if(type == 6)
    {
        lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
        lv_obj_set_style_border_width(parent, 0, 0);
        lv_obj_set_style_pad_all(parent, 0, 0);

        //创建返回按钮
        create_back_button(parent);

        lv_obj_t *file_name_obj = lv_obj_create(parent);
        lv_obj_set_size(file_name_obj, 462, 98);
        lv_obj_add_style(file_name_obj, &style_file_info, LV_PART_MAIN);
        lv_obj_align(file_name_obj, LV_ALIGN_TOP_MID, 0, 72);

        lv_obj_t *file_name_label = lv_label_create(file_name_obj);
        lv_label_set_text(file_name_label, "文件名");

        lv_obj_set_style_text_opa(file_name_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(file_name_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(file_name_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(file_name_label, LV_ALIGN_LEFT_MID, 30, 0);

        lv_obj_t *filename_label = lv_label_create(file_name_obj);
        lv_label_set_text(filename_label, "343454656566.JPG"); //todo:后续添加具体文件名

        lv_obj_set_style_text_opa(filename_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(filename_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(filename_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(filename_label, LV_ALIGN_RIGHT_MID, -30, 0);

        lv_obj_t *shooting_time_obj = lv_obj_create(parent);
        lv_obj_set_size(shooting_time_obj, 462, 98);
        lv_obj_add_style(shooting_time_obj, &style_file_info, LV_PART_MAIN);
        lv_obj_align(shooting_time_obj, LV_ALIGN_TOP_MID, 0, 184);

        lv_obj_t *shooting_time_label = lv_label_create(shooting_time_obj);
        lv_label_set_text(shooting_time_label, "拍摄时间");
        lv_obj_set_style_text_opa(shooting_time_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(shooting_time_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(shooting_time_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(shooting_time_label, LV_ALIGN_LEFT_MID, 30, 0);
        
        lv_obj_t *time_label = lv_label_create(shooting_time_obj);
        lv_label_set_text(time_label, "2025/08/01 13:46:23"); //todo:后续添加获取到的文件名
        lv_obj_set_style_text_opa(time_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(time_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(time_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(time_label, LV_ALIGN_RIGHT_MID, -30, 0);

        lv_obj_t *file_change_obj = lv_obj_create(parent);
        lv_obj_set_size(file_change_obj, 462, 98);
        lv_obj_add_style(file_change_obj, &style_file_info, LV_PART_MAIN);
        lv_obj_align(file_change_obj, LV_ALIGN_TOP_MID, 0, 296);

        lv_obj_t *file_size_label = lv_label_create(file_change_obj);
        lv_label_set_text(file_size_label, "文件大小");
        lv_obj_set_style_text_opa(file_size_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(file_size_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(file_size_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(file_size_label, LV_ALIGN_LEFT_MID, 30, 0);

        lv_obj_t *filesize_label = lv_label_create(file_change_obj);
        lv_label_set_text(filesize_label, "4.00MB"); //todo:后续展示实际获取到的文件大小
        lv_obj_set_style_text_opa(filesize_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(filesize_label, font_get_regular(28), 0);
        lv_obj_set_style_text_color(filesize_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(filesize_label, LV_ALIGN_RIGHT_MID, -30, 0);
    }
    else if(type == 7)
    {
        total_recod_cnt = 98; //todo ：获取实际的录像个数
        lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

        // 创建滚动容器内部的内容容器（用于存放所有录像项目）
        lv_obj_t *content_cont = lv_obj_create(parent);
        lv_obj_remove_style_all(content_cont);
        lv_obj_set_size(content_cont, 502, 410);
        lv_obj_set_flex_flow(content_cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollbar_mode(content_cont, LV_SCROLLBAR_MODE_OFF); //滚动条完全不显示
        lv_obj_set_style_bg_opa(content_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(content_cont, 0, 0);
        lv_obj_set_style_pad_all(content_cont, 0, 0);
        lv_obj_set_style_pad_top(content_cont, 10, 0);     // 上边距10pt
        lv_obj_set_style_pad_bottom(content_cont, 10, 6);   // 下边距6pt
        lv_obj_set_style_pad_left(content_cont, 10, 0);    // 左边距10pt
        lv_obj_set_style_pad_right(content_cont, 10, 0);   // 右边距10pt

        int total_rows = (total_recod_cnt + 1) / 2; // 向上取整
        // 创建所有行容器
        for(int row = 0; row < total_rows; row++) 
        {
            // 创建行容器（每行包含2列）
            lv_obj_t *row_cont = lv_obj_create(content_cont);
            lv_obj_set_size(row_cont, 482, 194); // 高度与单个项目相同
            lv_obj_set_flex_flow(row_cont, LV_FLEX_FLOW_ROW); //子对象从左向右依次排列
            lv_obj_set_flex_align(row_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
            lv_obj_set_style_bg_opa(row_cont, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_width(row_cont, 0, 0);
            lv_obj_set_style_pad_all(row_cont, 0, 0);

            lv_obj_set_style_margin_bottom(row_cont, 6, 0);         // 下边距
            
            // 在当前行创建项目（最多2个）
            for(int col = 0; col < 2; col++) 
            {
                int item_index = row * 2 + col;
                if(item_index >= total_recod_cnt)
                {
                    break; // 如果项目数不足则退出
                }

                // 每个录像项目的容器统一处理
                lv_obj_t *item_cont = lv_obj_create(row_cont);
                lv_obj_set_size(item_cont, 238, 194);
            
                // 设置列间距（项目间的水平间距）
                if((col + 1) % 2) 
                {
                    // 第一个项目：右边距6pt
                    lv_obj_set_style_margin_right(item_cont, 3, 0);
                    lv_obj_set_style_bg_color(item_cont, lv_color_hex(0xAFF99C), 0);
                } 
                else 
                {
                    lv_obj_set_style_margin_left(item_cont, 3, 0);
                    lv_obj_set_style_bg_color(item_cont, lv_color_hex(0xAFF99C), 0);
                }

                lv_obj_set_style_border_width(item_cont, 0, 0);
                lv_obj_set_style_radius(item_cont, 15, 0);
                lv_obj_set_style_pad_all(item_cont, 0, 0);
                //todo:获取照片或者录像的icon用于填充item_cont


                // 时间标签
                lv_obj_t *time_label = lv_label_create(item_cont);
                lv_label_set_text(time_label, "00:30");
                lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
                lv_obj_set_style_text_font(time_label, font_get_regular(24), 0);
                lv_obj_align(time_label, LV_ALIGN_BOTTOM_LEFT, 10, -10);
            }
        }
            
        // 创建顶部矩形渐变框
        lv_obj_t *up_indicator_area = lv_obj_create(parent);
        lv_obj_set_size(up_indicator_area, 502, 156);
        lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_add_style(up_indicator_area, &up_area_style, 0);
        
        lv_obj_t * photo_icon_single = lv_img_create(up_indicator_area);
        lv_img_set_src(photo_icon_single, PHOTOGRAPH_ICON_SINGLE);
        lv_obj_set_size(photo_icon_single, 40, 40);
        lv_obj_align(photo_icon_single, LV_ALIGN_TOP_LEFT, 30, 20);

        lv_obj_add_flag(photo_icon_single, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_single, single_icon_click_event, LV_EVENT_CLICKED, NULL);
        
        photo_all_label = lv_label_create(up_indicator_area);
        lv_label_set_text(photo_all_label, "全部98"); //todo：后续根据实际情况去显示对应的值
        lv_obj_set_style_text_opa(photo_all_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(photo_all_label, font_get_regular(32), 0);
        lv_obj_set_style_text_color(photo_all_label, lv_color_hex(0XFFFFFF), 0);
        lv_obj_align(photo_all_label, LV_ALIGN_TOP_RIGHT, -215, 19);

        lv_obj_t * photo_icon_allows_down = lv_img_create(up_indicator_area);
        lv_img_set_src(photo_icon_allows_down, PHOTOGRAPH_ICON_ALLOWS_DOWN);
        lv_obj_set_size(photo_icon_allows_down, 40, 40);
        lv_obj_align_to(photo_icon_allows_down, photo_all_label, LV_ALIGN_OUT_RIGHT_MID, 5, 0);  // 图标右侧，垂直居中
        lv_obj_add_flag(photo_icon_allows_down, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_add_event_cb(photo_icon_allows_down, allows_down_icon_click_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t * photo_icon_select = lv_img_create(up_indicator_area);
        lv_img_set_src(photo_icon_select, PHOTOGRAPH_ICON_SELECT);
        lv_obj_set_size(photo_icon_select, 40, 40);
        lv_obj_align(photo_icon_select, LV_ALIGN_TOP_LEFT, 432, 20);

        lv_obj_add_flag(photo_icon_select, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(photo_icon_select, select_icon_click_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t * buttom_line = lv_obj_create(parent);
        lv_obj_set_size(buttom_line, 6, 38);
        lv_obj_set_style_radius(buttom_line, 4, 0);
        lv_obj_set_style_bg_color(buttom_line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align(buttom_line, LV_ALIGN_RIGHT_MID, 0, 0); 
        lv_obj_set_style_opa(buttom_line, LV_OPA_COVER, 0);

    }
    else if(type == 8)
    {
        create_video_list(parent);
    }
    else if(type == 9)
    {
        lv_obj_add_style(parent, &style_black_background, 0);
        lv_obj_set_style_border_width(parent, 0, 0);
        lv_obj_set_style_pad_all(parent, 0, 0);

        //创建联系人
        lv_obj_t *cont_col = lv_obj_create(parent);
        lv_obj_add_style(cont_col, &style_black_background, 0);

        // 移除flex布局，使用绝对布局
        lv_obj_set_size(cont_col, lv_pct(100), lv_pct(100));
        lv_obj_set_layout(cont_col, LV_LAYOUT_NONE);
        lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
        lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 0, 0);
        lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
        lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_event_cb(cont_col, contact_scroll_event_cb, LV_EVENT_SCROLL, NULL);

        for (uint8_t i = 0; i < 20; i++)
        {
            lv_obj_t *contact = lv_obj_create(cont_col);
            lv_obj_remove_style_all(contact);
            lv_obj_set_size(contact, 370, 160);
            lv_obj_set_style_radius(contact, 98, 0);
            lv_obj_set_style_bg_opa(contact, LV_OPA_COVER, 0);
            lv_obj_align(contact, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_style_bg_color(contact, lv_color_hex(0x000000), LV_PART_MAIN);

            lv_obj_t *image = lv_img_create(contact);
            lv_obj_set_size(image, 130, 130);
            lv_obj_set_style_radius(image, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_clip_corner(image, true, 0);
            lv_img_set_src(image, mode_list[0]);
            lv_obj_align(image, LV_ALIGN_LEFT_MID, 6, 0);

            lv_obj_t *label = lv_label_create(contact);
            lv_label_set_text(label, "Mami");
            lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
            lv_obj_set_style_text_font(label, font_get_regular(34), 0);
            lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
            lv_obj_align(label, LV_ALIGN_LEFT_MID, 156, 0);

            // 使用绝对位置：每个图片垂直排列，水平位置为0（最左边）
            lv_obj_set_pos(contact, 0, i * (160 + 12));

            lv_obj_add_flag(contact, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(contact, contact_page_click_event_cb, LV_EVENT_CLICKED, cont_col);

            // 存储联系人对象指针
            contact_objs[i] = contact;
        }

        lv_obj_scroll_to_view(lv_obj_get_child(cont_col, 0), LV_ANIM_OFF);
        lv_obj_set_style_bg_color(lv_obj_get_child(cont_col, 0), lv_color_hex(0x2A3534), LV_PART_MAIN);
        

        lv_scr_load_anim(parent, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);

        //返回按钮
        lv_obj_t *back = lv_img_create(parent);
        lv_obj_set_size(back, 50, 50);
        lv_img_set_src(back, ICON_BACK);
        lv_obj_align_to(back, parent, LV_ALIGN_TOP_LEFT, 30, 20);
        lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

        //右侧滚动条
        line_cont = line_container_create(parent);


    }
}

static void versatile_filters_item_event_cb(lv_event_t * e)
{
    // 获取事件目标对象（容器）
    lv_obj_t * cont = lv_event_get_target(e);

    // 获取容器的坐标区域
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);

    // 计算容器垂直中心点的y坐标（改为垂直方向）
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    uint32_t i;
    // 获取容器中子对象的数量
    uint32_t child_cnt = lv_obj_get_child_count(cont);

    // 遍历所有子对象
    for(i = 0; i < child_cnt; i++) 
    {
        // 获取第i个子对象
        lv_obj_t * child = lv_obj_get_child(cont, i);
        
        // 获取子对象的坐标区域
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);
        //printf("i=%d, x1=%d, y1=%d, x2=%d, y2=%d\n", i, child_a.x1, child_a.y1, child_a.x2,  child_a.y2);
        // 计算子对象垂直中心点的y坐标（改为垂直方向）
        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        //printf("i=%d, y1=%d, height=%d, child_y_center=%d\n", i, child_a.y1, lv_area_get_height(&child_a), child_y_center);
        // 计算子对象中心与容器中心的垂直距离（绝对值）
        int32_t diff_y = child_y_center - cont_y_center;

        //printf("i=%d, child_y_center=%d, cont_y_center=%d, diff_y=%d\n", i, child_y_center, cont_y_center, diff_y);
        /* 根据圆形轨迹计算X轴偏移量（改为水平平移） */
        int32_t x;
        
        x= 0.002798 * diff_y * diff_y;  //拟合公式，大概数值
        
        if (LV_ABS(diff_y) >= 108)//阈值范围可调
        {
            
            lv_obj_set_style_translate_x(child, x+24, 0);
        }
        else
        {
            /* 根据计算的X坐标设置子对象的水平平移（改为水平方向） */
            lv_obj_set_style_translate_x(child, x, 0);

        }
    }
}


// 定义文件路径数组（替换原来的图标指针数组）
static const char *versatile_filters_icon_paths[] = {
    "V:tk1/realtime_shooting/lut_icon_none.png",
    "V:tk1/realtime_shooting/lut_icon_none.png", 
    "V:tk1/realtime_shooting/lut_icon_none.png",
    "V:tk1/realtime_shooting/lut_icon_none.png",
    "V:tk1/realtime_shooting/lut_icon_none.png"
};

#define ICON_COUNT (sizeof(versatile_filters_icon_paths) / sizeof(versatile_filters_icon_paths[0]))

static void app_icon_event_cb(lv_event_t * e)
{
    lv_obj_t * app_obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) 
    {
        LV_LOG_USER("Clicked");
    }
}
static void lv_versatile_filters(lv_obj_t * parent)
{
    //创建返回按钮
    lv_obj_t * return_icon = lv_img_create(parent);
    lv_img_set_src(return_icon, ICON_BACK);
    lv_obj_set_size(return_icon, 50, 50);
    lv_obj_align(return_icon, LV_ALIGN_TOP_LEFT, 30, 20);

    lv_obj_t * title_label = lv_label_create(parent);
    lv_label_set_text(title_label, "滤镜滤镜");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_set_style_text_font(title_label, font_get_regular(30), 0);
    lv_obj_align_to(title_label, return_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);  // 图标右侧，垂直居中

    // 创建右部矩形渐变框
    lv_obj_t *right_indicator_area = lv_obj_create(parent);
    lv_obj_set_size(right_indicator_area, 375, 340);
    lv_obj_align(right_indicator_area, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(right_indicator_area, &style_multi_filter, 0);

    lv_obj_set_style_border_width(right_indicator_area, 0, 0);
    lv_obj_set_style_radius(right_indicator_area, 0, 0);
    lv_obj_set_style_pad_all(right_indicator_area, 0, 0);


    lv_obj_t * versatile_filters_item = lv_obj_create(parent);
    lv_obj_remove_style_all(versatile_filters_item);
    lv_obj_set_size(versatile_filters_item, 223, 410);
    //lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    lv_obj_align(versatile_filters_item, LV_ALIGN_RIGHT_MID, 0, 0);

    // 移除flex布局，使用绝对布局
    lv_obj_set_layout(versatile_filters_item, LV_LAYOUT_NONE);

    lv_obj_add_event_cb(versatile_filters_item, versatile_filters_item_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_clip_corner(versatile_filters_item, true, 0); //超出对象边界的对象被裁剪
    lv_obj_set_scroll_dir(versatile_filters_item, LV_DIR_VER); //滚动方式为垂直滚动
    lv_obj_set_scroll_snap_y(versatile_filters_item, LV_SCROLL_SNAP_CENTER);//设置垂直滚动时的对齐方式为居中对齐
    lv_obj_set_scrollbar_mode(versatile_filters_item, LV_SCROLLBAR_MODE_OFF); //关闭滚动条显示
    lv_obj_set_style_pad_all(versatile_filters_item, 0, 0); //设置内边矩为0

    uint32_t i;
    lv_obj_t * image;
    for (int i = 0; i < ICON_COUNT; i++) 
    {
        image = lv_image_create(versatile_filters_item);
        lv_obj_set_size(image, 96, 96);
        lv_image_set_src(image, versatile_filters_icon_paths[i]);
        lv_obj_set_user_data(image, versatile_filters_icon_paths[i]);

        // 使用绝对位置：每个图片垂直排列，水平位置为0（最左边）
        lv_obj_set_pos(image, 0, i * (96 + 12));  // 96是图片高度，12是间距

        lv_obj_add_flag(image, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(image, app_icon_event_cb, LV_EVENT_CLICKED, NULL);
    }

    //初始滚动到中间项
    uint32_t center_index = ICON_COUNT / 2;
    lv_obj_t * center_child = lv_obj_get_child(versatile_filters_item, center_index);
    lv_obj_scroll_to_view(center_child, LV_ANIM_OFF);

    // 创建完所有对象后手动发送滚动事件，触发初始布局
    //lv_obj_send_event(versatile_filters_item, LV_EVENT_SCROLL, NULL);

    
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

// void create_video_player_ui(lv_obj_t * parent)
// {

//     // 创建实时取景背景（假设是图像或视频组件）
//     lv_obj_t * live_view = lv_image_create(parent);
//     lv_obj_add_style(live_view, &style, 0);
//     lv_obj_set_size(live_view, LV_HOR_RES, LV_VER_RES);
//     lv_image_set_src(live_view, "V:png/img_camera_backup.png");
//     lv_img_set_zoom(live_view, 128);
//     lv_obj_center(live_view);
    
//     // 创建顶部矩形渐变框
//     video_play_indicator_area = lv_obj_create(parent);
//     lv_obj_set_size(video_play_indicator_area, 502, 156);
//     lv_obj_align(video_play_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
//     lv_obj_add_style(video_play_indicator_area, &up_area_style, 0);

//     //修改为录像总时长，当前正在播放的时间
//     video_play_time_label = lv_label_create(video_play_indicator_area);
//     char play_time_str[20] = {0};
//     record_total_play_sec = 40; //后续根据实际情去获取录像的总时长
//     snprintf(play_time_str, sizeof(play_time_str), "00:00:00/%02d:%02d:%02d", record_total_play_sec / 3600, record_total_play_sec / 60, record_total_play_sec % 60);
//     lv_label_set_text(video_play_time_label, play_time_str);
//     lv_obj_set_style_text_letter_space(video_play_time_label, 2, LV_PART_MAIN); // 设置字符间距
//     lv_obj_set_style_text_opa(video_play_time_label, LV_OPA_COVER, 0);
//     lv_obj_set_style_text_font(video_play_time_label, font_get_regular(28), 0);
//     lv_obj_set_style_text_color(video_play_time_label, lv_color_hex(0XFFFFFF), 0);
//     lv_obj_align(video_play_time_label, LV_ALIGN_TOP_MID, 0, 20);


//     // 创建底部矩形渐变框
//     lv_obj_t *down_indicator_area = lv_obj_create(parent);
//     lv_obj_set_size(down_indicator_area, 502, 156);
//     lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
//     lv_obj_add_style(down_indicator_area, &down_area_style, 0);

//     // 创建滑动条
//     slider = lv_slider_create(down_indicator_area);
//     lv_obj_set_size(slider, 457, 6);
//     lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -36);

//     lv_slider_set_range(slider, 0, 100);
//     lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_ALL, NULL);
    
//     // // 创建播放/暂停按钮
//     // lv_obj_t * btn = lv_btn_create(down_indicator_area);
//     // lv_obj_set_size(btn, 80, 40);
//     // lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);
    
//     // lv_obj_add_event_cb(btn, play_pause_btn_cb, LV_EVENT_CLICKED, NULL);
    
//     //创建播放定时器（初始为暂停状态）
//     timer_1 = lv_timer_create(play_timer_cb, 1000, NULL); // 100ms间隔
//     //lv_timer_pause(timer_1);
// }


static void update_selection_ui(void)
{
    // 可以在这里更新顶部栏的显示，例如显示选择模式提示
    if(selected_count_label) 
    {
        lv_obj_clear_flag(selected_count_label, LV_OBJ_FLAG_HIDDEN);
    }
}

static int get_item_index(lv_obj_t *item_cont)
{
    // 这里需要根据您的实际结构来获取索引
    // 简单实现：通过遍历所有项目来匹配
    lv_obj_t *content_cont = lv_obj_get_parent(lv_obj_get_parent(item_cont));
    int index = 0;
    
    for(int i = 0; i < lv_obj_get_child_cnt(content_cont); i++) 
    {
        lv_obj_t *row_cont = lv_obj_get_child(content_cont, i);
        for(int j = 0; j < lv_obj_get_child_cnt(row_cont); j++) 
        {
            lv_obj_t *child = lv_obj_get_child(row_cont, j);
            if(child == item_cont) 
            {
                return index;
            }

            index++;
        }
    }

    return -1;
}

static void update_selected_count(void)
{
    if(selected_count_label) 
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "已选择%d", selected_count);
        lv_label_set_text(selected_count_label, buf);
    }
}

// 更新项目选中状态UI
static void update_item_selection_ui(lv_obj_t *item_cont, bool selected)
{
    // 从用户数据获取之前创建的图标
    lv_obj_t *check_icon = lv_obj_get_user_data(item_cont);

    // 添加选中效果（例如边框高亮）
    if(selected) 
    {
        lv_obj_clear_flag(check_icon, LV_OBJ_FLAG_HIDDEN);  // 显示现有图标
    } 
    else 
    {
        lv_obj_add_flag(check_icon, LV_OBJ_FLAG_HIDDEN);    // 隐藏现有图标
    }
}

// 选中项目事件处理
static void select_item_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    static bool long_press_handled = false;  // 标记长按是否已处理
    static lv_obj_t *last_long_press_obj = NULL;  // 记录最后一次长按的对象

    if(code == LV_EVENT_LONG_PRESSED) 
    {
        // 进入选择模式
        selection_mode = true;
        update_selection_ui();
        
        // 切换当前项目的选中状态
        int index = get_item_index(obj);
        if(index >= 0) 
        {
            item_selected[index] = !item_selected[index];
            selected_count += item_selected[index] ? 1 : -1;
            update_selected_count();
            update_item_selection_ui(obj, item_selected[index]);

            // 记录长按处理状态
            long_press_handled = true;
            last_long_press_obj = obj;
        }
    }
    else if(code == LV_EVENT_CLICKED && selection_mode) 
    {

        // 检查是否为长按后的首次点击
        if(long_press_handled && obj == last_long_press_obj)
        {
            // 忽略长按对象的首次点击事件
            long_press_handled = false;
            last_long_press_obj = NULL;
            return;
        }

        // 在选择模式下点击切换选中状态
        int index = get_item_index(obj);
        if(index >= 0) 
        {
            item_selected[index] = !item_selected[index];
            selected_count += item_selected[index] ? 1 : -1;
            update_selected_count();
            update_item_selection_ui(obj, item_selected[index]);
        }
    }
}

// 删除选中的录像
static void delete_selected_items(lv_obj_t *parent)
{
    if(selected_count == 0) return;
    
    // 从后往前删除，避免索引变化问题
    for(int i = total_recod_cnt - 1; i >= 0; i--) 
    {
        if(item_selected[i]) 
        {
            // todo:删除对应的录像文件（实际业务逻辑）

            // 从数组中移除
            for(int j = i; j < total_recod_cnt - 1; j++) 
            {
                item_selected[j] = item_selected[j + 1];
            }
            total_recod_cnt--;
        }
    }
    
    // 重新创建UI
    recreate_video_list(parent);
    
    // 重置选择状态
    selected_count = 0;
    selection_mode = false;
    update_selected_count();
}

// 删除图标点击事件
static void delete_icon_click_event(lv_event_t *e)
{
    lv_obj_t *parent = (lv_obj_t*)lv_event_get_user_data(e);
    delete_selected_items(parent);
}

static void create_video_list(lv_obj_t *parent)
{
    
    // 初始化选中状态数组
    if(item_selected)
    {
        free(item_selected);   
    }
    item_selected = (bool*)malloc(total_recod_cnt * sizeof(bool));
    memset(item_selected, 0, total_recod_cnt * sizeof(bool));
    
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    // 创建滚动容器内部的内容容器
    lv_obj_t *content_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(content_cont);
    lv_obj_set_size(content_cont, 502, 410);
    lv_obj_set_flex_flow(content_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(content_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(content_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content_cont, 0, 0);
    lv_obj_set_style_pad_all(content_cont, 0, 0);
    lv_obj_set_style_pad_top(content_cont, 10, 0);
    lv_obj_set_style_pad_bottom(content_cont, 10, 6);
    lv_obj_set_style_pad_left(content_cont, 10, 0);
    lv_obj_set_style_pad_right(content_cont, 10, 0);

    int total_rows = (total_recod_cnt + 1) / 2;
    
    for(int row = 0; row < total_rows; row++) 
    {
        lv_obj_t *row_cont = lv_obj_create(content_cont);
        lv_obj_set_size(row_cont, 482, 194);
        lv_obj_set_flex_flow(row_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_bg_opa(row_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row_cont, 0, 0);
        lv_obj_set_style_pad_all(row_cont, 0, 0);
        lv_obj_set_style_margin_bottom(row_cont, 6, 0);

        for(int col = 0; col < 2; col++) 
        {
            int item_index = row * 2 + col;
            if(item_index >= total_recod_cnt)
            {
               break; 
            }

            lv_obj_t *item_cont = lv_obj_create(row_cont);
            lv_obj_set_size(item_cont, 238, 194);

            if((col + 1) % 2) 
            {
                lv_obj_set_style_margin_right(item_cont, 3, 0);
            } 
            else 
            {
                lv_obj_set_style_margin_left(item_cont, 3, 0);
            }
            
            lv_obj_set_style_bg_color(item_cont, lv_color_hex(0x103050), 0);
            lv_obj_set_style_border_width(item_cont, 0, 0);
            lv_obj_set_style_radius(item_cont, 15, 0);
            lv_obj_set_style_pad_all(item_cont, 0, 0);

            //创建选中图标，先隐藏
            lv_obj_t * check_icon = lv_img_create(item_cont);
            lv_img_set_src(check_icon, PHOTOGRAPH_ICON_SELECT_GREEN);
            lv_obj_set_size(check_icon, 40, 40);
            lv_obj_align(check_icon, LV_ALIGN_TOP_LEFT, 188, 144);
            lv_obj_add_flag(check_icon, LV_OBJ_FLAG_HIDDEN);

            // 将图标指针存储到用户数据中
            lv_obj_set_user_data(item_cont, check_icon);

            // 添加事件处理
            lv_obj_add_flag(item_cont, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(item_cont, select_item_event, LV_EVENT_ALL, NULL);

            // 时间标签
            lv_obj_t *time_label = lv_label_create(item_cont);
            lv_label_set_text(time_label, "00:30");
            lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
            lv_obj_set_style_text_font(time_label, font_get_regular(24), 0);
            lv_obj_align(time_label, LV_ALIGN_BOTTOM_LEFT, 10, -10);
        }
    }

    // 创建顶部区域
    lv_obj_t *up_indicator_area = lv_obj_create(parent);
    lv_obj_set_size(up_indicator_area, 502, 156);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);

    lv_obj_t * photo_icon_single = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_single, PHOTOGRAPH_ICON_SINGLE);
    lv_obj_set_size(photo_icon_single, 40, 40);
    lv_obj_align(photo_icon_single, LV_ALIGN_TOP_LEFT, 30, 20);

    lv_obj_add_flag(photo_icon_single, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_icon_single, single_icon_click_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t * photo_icon_unselect = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_unselect, PHOTOGRAPH_ICON_UNSELECT);
    lv_obj_set_size(photo_icon_unselect, 40, 40);
    lv_obj_align(photo_icon_unselect, LV_ALIGN_TOP_LEFT, 171, 20);

    // 选中数量标签（初始隐藏）
    selected_count_label = lv_label_create(up_indicator_area);
    lv_label_set_text(selected_count_label, "已选择0");
    lv_obj_set_style_text_color(selected_count_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(selected_count_label, font_get_regular(32), 0);
    lv_obj_align(selected_count_label, LV_ALIGN_TOP_LEFT, 216, 19);
    lv_obj_add_flag(selected_count_label, LV_OBJ_FLAG_HIDDEN);

    // 删除按钮
    lv_obj_t *delete_icon = lv_img_create(up_indicator_area);
    lv_img_set_src(delete_icon, PHOTOGRAPH_ICON_TRASH_FILLED); // 替换为实际的删除图标路径
    lv_obj_set_size(delete_icon, 40, 40);
    lv_obj_align(delete_icon, LV_ALIGN_TOP_RIGHT, -30, 20);
    lv_obj_add_flag(delete_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(delete_icon, delete_icon_click_event, LV_EVENT_CLICKED, parent);
}

static void recreate_video_list(lv_obj_t *parent)
{
    // 删除所有子对象
    lv_obj_clean(parent);
    
    // 重新创建列表
    create_video_list(parent);
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

        //模式列表页录像、延时摄影 ---已完成
        //lv_video_mode_style(scr);

        //照片合集样式---已实现
        //lv_realtime_shooting_photos_mode(scr);

        //实时拍摄切换、加载动画等待，等待交互点动画 ,0:切换中，1:加载中--已实现
        //lv_realtime_shooting_switch_wait(scr, 0);

        //实时取景焦距对焦 ---已实现， 实时取景焦距已实现
        //lv_realtime_shooting_focus(scr);

        //实时取景录像，延时摄影, 0:录像---已实现，界面边缘滑动已实现，延时摄影后续版本再实现
        lv_realtime_shooting_video(scr, 0);

        //相册功能---已基本实现，缺少AI对话，等待素材后再实现
        //lv_photo_picture(scr, 9);

        //调试函数，后续删除
        //create_video_player_ui(scr);

        //百变滤镜功能---已实现
        //lv_versatile_filters(scr);

        //快速编码设置 0拍摄，1录像 2延时摄影---已实现
        //lv_realtime_shooting_rapid_coding(scr, 0);
    }

    return;
}
#endif