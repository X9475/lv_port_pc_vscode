#include "../lv_switch_interface.h"

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
#define EMPTY_PIC_PHOTO "V:tk1/realtime_shooting/empety_pic_photo.png"

lv_subject_t  album_subject;
static lv_switch_page_pt switch_page;

static lv_timer_t *play_timer = NULL;//录像播放的进度条
static lv_timer_t *hidden_timer = NULL;

static lv_obj_t *screen = NULL;
static lv_obj_t *photo_all_label;
static lv_obj_t *photo_icon_stop;
static lv_obj_t *photo_icon_info;
static lv_obj_t *photo_intell_icon;
static lv_obj_t *video_time_label;
static lv_obj_t * photo_icon_allows_down;
static lv_obj_t * video_play_time_label;
static lv_obj_t * slider;          // 滑动条对象

static lv_style_t screen_style;
static lv_style_t up_area_style;
static lv_style_t down_area_style;
static lv_style_t video_time_style;
static lv_style_t style_bg;
static lv_style_t style_indicator;
static lv_style_t style_knob;

static bool mutex_init_flag = false;
static lv_mutex_t timer_mutex;
static uint32_t record_total_play_sec = 0;
static uint32_t record_play_sec = 0;
static bool is_playing = false;    // 播放状态标志
static bool is_icon_hidden = false;
static int total_files = 2;          // 总文件数（示例值
static int current_file_index = 1;    // 当前文件索引
// 定义阈值
#define LEFT_EDGE_THRESHOLD 200
#define RIGHT_EDGE_THRESHOLD (LV_HOR_RES - LEFT_EDGE_THRESHOLD)

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_start_agent_slider_event(lv_event_t *e);

static void album_icon_click_event(lv_event_t * e);
static void screen_click_event(lv_event_t * e);
static void delete_click_event(lv_event_t * e);
static void gesture_event_handler(lv_event_t * e);

// 图标类型枚举
typedef enum 
{
    ICON_FOUR_GRID,
    ICON_TRASH,
    ICON_SHARE,
    ICON_STOP,
    ICON_INFO,
    ICON_INTELL
} icon_type_t;

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_ALBUM_NULL,         //相册为空界面
    PAGE_SWITCH_ALBUM_SHARE_CONTACT,//相册分享选择联系人界面
    PAGE_SWITCH_ALBUM_SHARE_SUC,    //相册分享成功界面
    PAGE_SWITCH_ALBUM_INFO_SHOW,    //相册文件信息展示界面
    PAGE_SWITCH_ALBUM_FOUR_GRID,    //相册四宫格界面
    PAGE_SWITCH_ALBUM_AI,           //相册AI对话界面
    PAGE_SWITCH_SHOOT_PHOTO,        //相册拍摄界面
    PAGE_SWITCH_BACK                //返回
};

static lv_page_info_t album_page = {
    .page_id = PAGE_FUNCTIONAL_ALBUM,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_album_get()
{
    return &album_page;
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

    album_page.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_mutex_lock(&timer_mutex);
    if (play_timer) lv_timer_del(play_timer);
    play_timer = NULL;

    if (hidden_timer) lv_timer_del(hidden_timer);
    hidden_timer = NULL;

    album_page.status = STATUS_EXITING;//准备离开
    lv_mutex_unlock(&timer_mutex);
    
    lv_obj_remove_event_cb(act_screen, gesture_event_handler);

    lv_style_reset(&screen_style);
    lv_style_reset(&up_area_style);
    lv_style_reset(&video_time_style);
    lv_style_reset(&style_indicator);
    lv_style_reset(&style_bg);
    lv_style_reset(&down_area_style);
    lv_style_reset(&style_knob);

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

    lv_style_init(&video_time_style);
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

}

static void lv_page_subject_init()
{
    lv_subject_init_int(&album_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_subject);
}
static void lv_page_load(lv_obj_t *cont)
{

    // 添加手势检测到实时取景背景
    lv_obj_add_event_cb(act_screen, gesture_event_handler, LV_EVENT_GESTURE, NULL); 

    // 创建顶部矩形渐变框
    lv_obj_t *up_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(up_indicator_area, 502, 156);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);

    lv_obj_t * photo_icon_four_grid = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_four_grid, PHOTOGRAPH_ICON_FOUR_GRID);
    lv_obj_set_size(photo_icon_four_grid, 40, 40);
    lv_obj_align(photo_icon_four_grid, LV_ALIGN_TOP_LEFT, 50, 20);

    lv_obj_add_flag(photo_icon_four_grid, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_icon_four_grid, album_icon_click_event, LV_EVENT_CLICKED, (void *)ICON_FOUR_GRID); //四宫格图标点击事件

    
    photo_all_label = lv_label_create(up_indicator_area);
    //todo:后续根据实际的值去获取当前的值，并设置到label上
    lv_label_set_text(photo_all_label, "全部1/6");
    lv_obj_set_style_text_opa(photo_all_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(photo_all_label, font_get_regular(32), 0);
    lv_obj_set_style_text_color(photo_all_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(photo_all_label, LV_ALIGN_TOP_RIGHT, -215, 19);

    photo_icon_allows_down = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_allows_down, PHOTOGRAPH_ICON_ALLOWS_DOWN);
    lv_obj_set_size(photo_icon_allows_down, 40, 40);
    lv_obj_align_to(photo_icon_allows_down, photo_all_label, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t * photo_icon_trash_filled = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_trash_filled, PHOTOGRAPH_ICON_TRASH_FILLED);
    lv_obj_set_size(photo_icon_trash_filled, 40, 40);
    lv_obj_align(photo_icon_trash_filled, LV_ALIGN_TOP_RIGHT, -50, 20);
    lv_obj_add_flag(photo_icon_trash_filled, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_icon_trash_filled, album_icon_click_event, LV_EVENT_CLICKED, (void *)ICON_TRASH);  //删除图标处理事件

    lv_obj_t * photo_icon_share = lv_img_create(cont);
    lv_img_set_src(photo_icon_share, PHOTOGRAPH_ICON_SHARE);
    lv_obj_set_size(photo_icon_share, 40, 40);
    lv_obj_align(photo_icon_share, LV_ALIGN_TOP_LEFT, 30, 185);
    lv_obj_add_flag(photo_icon_share, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_icon_share, album_icon_click_event, LV_EVENT_CLICKED, (void *)ICON_SHARE);   //分享图标点击处理事件

    photo_icon_stop = lv_img_create(cont);
    lv_img_set_src(photo_icon_stop, PHOTOGRAPH_ICON_STOP);
    lv_obj_set_size(photo_icon_stop, 100, 100);
    lv_obj_align(photo_icon_stop, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(photo_icon_stop, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_icon_stop, album_icon_click_event, LV_EVENT_CLICKED, (void *)ICON_STOP);   //播放图点击标处理事件

    // 为整个live_view添加点击事件，用于恢复暂停图标
    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, screen_click_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t * buttom_line = lv_obj_create(cont);
    lv_obj_set_size(buttom_line, 6, 50);
    lv_obj_set_style_radius(buttom_line, 4, 0);
    lv_obj_set_style_bg_color(buttom_line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(buttom_line, LV_ALIGN_TOP_LEFT, 475, 183); 
    lv_obj_set_style_opa(buttom_line, LV_OPA_COVER, 0);

    // 创建底部矩形渐变框
    lv_obj_t *down_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(down_indicator_area, 502, 156);
    lv_obj_align(down_indicator_area, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(down_indicator_area, &down_area_style, 0);

    photo_icon_info = lv_img_create(down_indicator_area);
    lv_img_set_src(photo_icon_info, PHOTOGRAPH_ICON_INFO);
    lv_obj_set_size(photo_icon_info, 40, 40);
    lv_obj_align(photo_icon_info, LV_ALIGN_BOTTOM_LEFT, 50, -20);
    lv_obj_add_flag(photo_icon_info, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_icon_info, album_icon_click_event, LV_EVENT_CLICKED, (void *)ICON_INFO);  //录像信息点击处理事件

    photo_intell_icon = lv_img_create(down_indicator_area);
    lv_img_set_src(photo_intell_icon, INERTIGENCE_ICON);
    lv_obj_set_size(photo_intell_icon, 66, 66);
    lv_obj_align(photo_intell_icon, LV_ALIGN_BOTTOM_RIGHT, -50, -20);
    lv_obj_add_flag(photo_intell_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_intell_icon, album_icon_click_event, LV_EVENT_CLICKED, (void *)ICON_INTELL);  //智能AI点击事件

    video_time_label = lv_label_create(down_indicator_area);
    lv_obj_add_style(video_time_label, &video_time_style, LV_PART_MAIN);
    //todo:后续根据获取的最新的录像的实际可播放时间显示
    lv_label_set_text(video_time_label, "30:30"); 
    lv_obj_set_style_text_opa(video_time_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(video_time_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(video_time_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(video_time_label, LV_ALIGN_TOP_LEFT, 215, 95);

    return;
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

        //右边缘向左滑动 - 切换到拍摄界面
        if(dir == LV_DIR_LEFT && point.x > RIGHT_EDGE_THRESHOLD) 
        {
            printf("右边缘向左滑动,切换到拍摄或界面\n");
            lv_subject_set_int(&album_subject, PAGE_SWITCH_SHOOT_PHOTO);
        }
        else 
        {
            printf("其他手势或条件不满足\n");
        }
    }
}

static void delete_video_file(void)
{
    // 在顶层创建模态弹窗
    lv_obj_set_size(top_screen, 502, 410);
    lv_obj_center(top_screen);

    lv_obj_set_style_bg_color(top_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(top_screen, LV_OPA_80, 0);
    lv_obj_set_style_border_width(top_screen, 0, 0);
    lv_obj_set_style_pad_all(top_screen, 0, 0);
    lv_obj_set_style_radius(top_screen, 0, 0);

    //设置文字到主菜单
    lv_obj_t *label = lv_label_create(top_screen);
    lv_label_set_text(label, "删除此文件？");
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

    lv_obj_add_event_cb(btn_obj, delete_click_event, LV_EVENT_CLICKED, (void *)0);
    
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

    lv_obj_add_event_cb(btn1_obj, delete_click_event, LV_EVENT_CLICKED,  (void *)1);
}

// 更新显示到下一个文件
static void update_display_to_next_file(void)
{
    // 更新文件计数标签
    char label_text[32];
    snprintf(label_text, sizeof(label_text), "全部%d/%d", current_file_index, total_files);
    lv_label_set_text(photo_all_label, label_text);
    
    // todo: 这里需要实现实际的文件加载逻辑
    // lv_image_set_src(live_view, get_next_file_path());
    
    LV_LOG_USER("已更新显示到文件 %d", current_file_index);
}

static void delete_click_event(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    icon_type_t icon_type = (icon_type_t)lv_event_get_user_data(e); // 0:取消, 1:确定
    LV_LOG_USER("delete icon clicked, type=%d", icon_type);

    if(icon_type == 0)
    {
        LV_LOG_USER("取消删除文件");
    }
    else
    {
        LV_LOG_USER("确认删除文件");
        //todo:删除此文件

        total_files --;
        // 检查是否还有文件
        if (total_files > 0) 
        {
            update_display_to_next_file();
        } 
        else 
        {
            LV_LOG_USER("所有文件已删除，跳转到空界面");
            lv_subject_set_int(&album_subject, PAGE_SWITCH_ALBUM_NULL);
        }

    }

    // 删除模态弹窗
    if (top_screen) 
    {
        lv_obj_remove_style_all(top_screen);
        lv_obj_clean(top_screen);
    }
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

    //处于运行状态则暂停定时器
    if (play_timer && !lv_timer_get_paused(play_timer))
    {
        lv_timer_pause(play_timer);
    }
    
    // 确保滑动条在最大值位置
    lv_slider_set_value(slider, 100, LV_ANIM_ON);
}

// 播放定时器回调函数
static void play_timer_cb(lv_timer_t * timer)
{
    lv_mutex_lock(&timer_mutex);
    if (album_page.status == STATUS_EXITING)
    {
        lv_mutex_unlock(&timer_mutex);
        return;
    }

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
    lv_mutex_unlock(&timer_mutex);
}

// 屏幕点击事件处理函数
static void screen_click_event(lv_event_t * e)
{
    // 如果图标被隐藏，点击屏幕恢复暂停图标
    if (is_icon_hidden) 
    {
        // 暂停播放定时器，防止record_play_sec继续增加
        if (play_timer && !lv_timer_get_paused(play_timer))
        {
            lv_timer_pause(play_timer);
        }

        // 显示播放图标
        lv_obj_clear_flag(photo_icon_stop, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(photo_icon_stop, PHOTOGRAPH_ICON_STOP);

        lv_obj_add_flag(video_play_time_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_HIDDEN);

        is_playing = false;
        is_icon_hidden = false;

        //录像时长和当前进度恢复为0
        //record_play_sec = 0;

        // 恢复部分控件
        lv_obj_clear_flag(photo_icon_stop, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(photo_icon_info, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(photo_intell_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(video_time_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(photo_all_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(photo_icon_allows_down, LV_OBJ_FLAG_HIDDEN);
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
        if (is_playing && !lv_timer_get_paused(play_timer))
        {
            lv_timer_pause(play_timer);
        }
    }
    else if(code == LV_EVENT_RELEASED) 
    {
        // 滑动条释放后，如果之前是播放状态则继续播放
        if (is_playing && lv_timer_get_paused(play_timer))
        {
            lv_timer_resume(play_timer);
        }
    }
}

// 定时器回调函数，用于延迟隐藏播放按钮和显示进度条
static void play_icon_timer_cb(lv_timer_t * timer)
{
    lv_mutex_lock(&timer_mutex);
    if (album_page.status == STATUS_EXITING)
    {
        lv_mutex_unlock(&timer_mutex);
        return;
    }

    // 隐藏播放按钮
    lv_obj_add_flag(photo_icon_stop, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(photo_icon_info, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(photo_intell_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(video_time_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(photo_all_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(photo_icon_allows_down, LV_OBJ_FLAG_HIDDEN);

    is_icon_hidden = true;

    //修改为录像总时长，当前正在播放的时间
    video_play_time_label = lv_label_create(screen);
    char play_time_str[20] = {0};
    record_total_play_sec = 40; //后续根据实际情去获取录像的总时长
    snprintf(play_time_str, sizeof(play_time_str), "00:00:00/%02d:%02d:%02d", record_total_play_sec / 3600, record_total_play_sec / 60, record_total_play_sec % 60);
    lv_label_set_text(video_play_time_label, play_time_str);
    lv_obj_set_style_text_letter_space(video_play_time_label, 2, LV_PART_MAIN); // 设置字符间距
    lv_obj_set_style_text_opa(video_play_time_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(video_play_time_label, font_get_regular(28), 0);
    lv_obj_set_style_text_color(video_play_time_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align_to(video_play_time_label,  screen, LV_ALIGN_TOP_MID, 0, 20);

    // 创建滑动条
    slider = lv_slider_create(screen);
    lv_obj_set_size(slider, 457, 6);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_LEFT, 20, -36);

    // 应用样式
    lv_obj_add_style(slider, &style_bg, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
    lv_slider_set_range(slider, 0, 100);

    // 设置滑动条为不可交互状态
    lv_obj_clear_flag(slider, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(slider, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    
    //lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_ALL, NULL);

    // 设置滑动条初始值为当前播放进度
    lv_slider_set_value(slider, record_play_sec * 100 / record_total_play_sec, LV_ANIM_OFF);

    //创建播放定时器
    if (NULL == play_timer)
    {
        play_timer = lv_timer_create(play_timer_cb, 1000, NULL);
        lv_timer_set_auto_delete(play_timer, false);
    }
    else if (play_timer && lv_timer_get_paused(play_timer))
    {
        lv_timer_reset(play_timer);
        lv_timer_resume(play_timer);
    }

    //处于运行状态则暂停定时器
    if (!lv_timer_get_paused(hidden_timer)) {
        lv_timer_pause(hidden_timer);
    }
    lv_mutex_unlock(&timer_mutex);
}

// 统一处理点击事件函数
static void album_icon_click_event(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    icon_type_t icon_type = (icon_type_t)lv_event_get_user_data(e);
    LV_LOG_USER("album icon clicked, type=%d", icon_type);

    switch(icon_type) {
        case ICON_FOUR_GRID:
            // 四宫格图标点击处理逻辑
            lv_subject_set_int(&album_subject, PAGE_SWITCH_ALBUM_FOUR_GRID);
            break;

        case ICON_TRASH:
            // 删除图标处理逻辑
            delete_video_file();
            break;

        case ICON_SHARE:
            lv_subject_set_int(&album_subject, PAGE_SWITCH_ALBUM_SHARE_CONTACT);
            break;

        case ICON_STOP:
            // 播放图标处理逻辑
            if (!is_playing && !is_icon_hidden) 
            {
                is_playing = true;
                // 立即修改播放图标为播放中状态
                lv_img_set_src(obj, PHOTOGRAPH_ICON_PLAY);

                // 创建定时器，1秒后执行隐藏和显示操作
                if (NULL == hidden_timer)
                {
                    hidden_timer = lv_timer_create(play_icon_timer_cb, 1000, NULL);
                    lv_timer_set_auto_delete(hidden_timer, false);
                }
                else if (hidden_timer && lv_timer_get_paused(hidden_timer))
                {
                    lv_timer_reset(hidden_timer);
                    lv_timer_resume(hidden_timer);
                }
            }
            break;

        case ICON_INFO:
            // 录像信息点击处理逻辑
            lv_subject_set_int(&album_subject, PAGE_SWITCH_ALBUM_INFO_SHOW);
            break;

        case ICON_INTELL:
            // 智能AI点击处理逻辑
            lv_subject_set_int(&album_subject, PAGE_SWITCH_ALBUM_AI);
            break;

        default:
            break;
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
    switch_page->old_page = &album_page;

    switch (page_event)
    {
        case PAGE_SWITCH_ALBUM_NULL:
            //加入栈表
            lv_stack_push(&album_page);
            switch_page->new_page = lv_page_album_none_get();
            break;

        case PAGE_SWITCH_ALBUM_SHARE_CONTACT:
            //加入栈表
            lv_stack_push(&album_page);
            switch_page->new_page = lv_page_album_share_get();
            break;

        case PAGE_SWITCH_ALBUM_INFO_SHOW:
            //加入栈表
            lv_stack_push(&album_page);
            switch_page->new_page = lv_page_album_info_get();
            break;


        case PAGE_SWITCH_ALBUM_FOUR_GRID:
            //加入栈表
            lv_stack_push(&album_page);
            switch_page->new_page = lv_page_album_four_grid_get();
            break;

        case PAGE_SWITCH_ALBUM_AI:
            //加入栈表
            lv_stack_push(&album_page);
            switch_page->new_page = lv_page_album_ai_get();
            break;

        case PAGE_SWITCH_SHOOT_PHOTO:
            switch_page->new_page = lv_page_shooting_photo_get();

            //动画参数设置
            switch_page->anim_transt.anim_support = true;
            switch_page->anim_transt.old_type = LV_ANIM_BOX_NONE;
            switch_page->anim_transt.new_type = LV_ANIM_BOX_SLIDE;
            lv_transition_anim_slide_param_set(
                        &switch_page->anim_transt.new_params,
                        &switch_page->new_page->page,
                        100,
                        TYPE_FUNC,
                        LV_DIR_LEFT);
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