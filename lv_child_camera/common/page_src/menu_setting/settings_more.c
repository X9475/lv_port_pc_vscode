#include "../lv_switch_interface.h"
#include <stdio.h>

#define SETTING_NUM     9

lv_subject_t settings_more_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;

//test旋钮转动菜单
typedef struct {
    int command;
} MenuCommand;
static MenuCommand g_cmd;

typedef struct
{
    bool init_flag;
    lv_timer_t *timer;
    lv_obj_t *mask;
    int index;
} rotate_ctl_s;
static rotate_ctl_s g_rotate_ctl_s = {false, NULL, NULL, 0};

static int last_tabindex = 0;
static lv_mutex_t mutex;
static void *input_thread(void* arg);
static void delete_mask_page(lv_timer_t *timer);
static void async_rotate_cb(void *cmd);

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void page_back_event_cb(lv_event_t *e);
static void *lv_more_setting_iterm_create(lv_obj_t *cont, const char *name);
static void setting_iterm_click_event_cb(lv_event_cb_t *e);

static const char *setting_list[SETTING_NUM] = {
    "熄屏时间", "时间展示形式", "存储管理", /*"语音控制开关", "锁屏密码",*/
    "滚轮振动开关", /*"振动幅度",*/ "远程预览开关", "关于相机", "恢复出厂设置", "认证标志", "设备二维码"
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SCREEN_OFF_TIME,        //熄屏时间
    PAGE_SWITCH_TIME_DISPLAY_FORMAT,    //时间展示形式
    PAGE_SWITCH_STORAGE_MANAGER,        //存储管理
    // PAGE_SWITCH_VOICE_CONTROL_SWITCH,   //语音控制开关
    // PAGE_SWITCH_VOICE_LOCK_PASSWORD,    //锁屏密码
    PAGE_SWITCH_VOICE_ROLLER_VIBRAT,    //滚动振动开关
    // PAGE_SWITCH_VIBRATION_AMPLITUDE,    //振动幅度
    PAGE_SWITCH_REMOTE_PREVIEW_SWITCH,  //远程预览开关
    PAGE_SWITCH_CAMERA_ABOUT,           //关于相机
    PAGE_SWITCH_FACTORY_RESTORE,        //恢复出厂设置
    PAGE_SWITCH_CERTIFICATION_MARK,     //认证标志
    PAGE_SWITCH_CAMERA_QRCODE,          //相机二维码
    PAGE_SWITCH_BACK
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

    //绘制当前页面
    lv_page_load(screen);
    lv_page_type_set(TYPE_MENU_SETTING_TWO);

    settings_more_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
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
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    //滚动列表
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_set_size(cont_col, lv_pct(100), 340);
    lv_obj_add_style(cont_col, &screen_style, 0);
    lv_obj_align_to(cont_col, cont, LV_ALIGN_TOP_MID, 20, 70);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);

    for (uint8_t i = 0; i < SETTING_NUM; i++)
    {
        lv_obj_t *btn = lv_more_setting_iterm_create(cont_col, setting_list[i]);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(btn, setting_iterm_click_event_cb, LV_EVENT_CLICKED, setting_list[i]);
    }

    int new_index = (last_tabindex + 1) > (SETTING_NUM - 1)? (SETTING_NUM - 1):(last_tabindex + 1);
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, new_index), LV_ANIM_OFF);

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
            // if (current_page->page_id == PAGE_FUNCTIONAL_MENU_SETTING)
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

                    if (NULL == g_rotate_ctl_s.timer)
                    {
                        g_rotate_ctl_s.timer = lv_timer_create(delete_mask_page, 1000, NULL);
                    }

                    //获取当前中间项目索引
                    lv_obj_t *cont_col = lv_obj_get_child(settings_more_page_info.page, 1);
                    const int child_count = lv_obj_get_child_cnt(cont_col);
                    for (int i = 0; i < child_count; i++)
                    {
                        lv_obj_t *child = lv_obj_get_child(cont_col, i);
                        lv_area_t child_a;
                        lv_obj_get_coords(child, &child_a);
                        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
                        if (LV_ABS(child_y_center - 232) < 100) 
                        {
                            g_rotate_ctl_s.index = i;
                            break;
                        }
                    }
                }

                // lv_mutex_lock(&mutex);
                lv_async_call(async_rotate_cb, &g_cmd.command);
                // lv_mutex_unlock(&mutex);

                lv_timer_reset(g_rotate_ctl_s.timer);
            }
        }
    }

    return NULL;
}

static void async_rotate_cb(void *cmd)
{
    printf("=====11\n");
    // lv_mutex_lock(&mutex);
    static int current_index = 0;
    current_index = g_rotate_ctl_s.index;
    static int offset = 0; //1:正向，-1:反向

    //更新索引
    offset = *(int*)cmd > 1 ? 1 : -1;
    current_index += offset;

    //边界检查
    if (current_index >= (SETTING_NUM - 1))
    {
        current_index = SETTING_NUM - 1;
    } 
    else if (current_index <= 0)
    {
        current_index = 0;
    }

    g_rotate_ctl_s.index = current_index;
    lv_obj_t *cont_col = lv_obj_get_child(settings_more_page_info.page, 1);
    lv_obj_scroll_to_view(lv_obj_get_child(cont_col, current_index), LV_ANIM_OFF);
    // lv_mutex_unlock(&mutex);
    return;
}

static void delete_mask_page(lv_timer_t *timer)
{
    printf("delete_mask_page\n");
    lv_obj_del(g_rotate_ctl_s.mask);
    lv_timer_del(g_rotate_ctl_s.timer);
    g_rotate_ctl_s.timer = NULL;
    g_rotate_ctl_s.init_flag = false;
    lv_obj_add_flag(settings_more_page_info.page, LV_OBJ_FLAG_GESTURE_BUBBLE);

    return;
}
/***********************************************test end************************************************************/

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_BACK);
}

static void *lv_more_setting_iterm_create(lv_obj_t *cont, const char *name)
{
    //背景
    lv_obj_t *btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 462, 98);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x121212), 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    //文字
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, fzlthb_28, 0);
    lv_obj_align_to(label, btn, LV_ALIGN_LEFT_MID, 30, 0);

    //进入指示
    lv_obj_t *image = lv_img_create(btn);
    lv_obj_set_size(image, 7, 13);
    lv_img_set_src(image, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_img_set_angle(image, 1800);
    lv_img_set_zoom(image, 160);
    lv_obj_align_to(image, btn, LV_ALIGN_RIGHT_MID, -31, 0);

    return btn;
}

static void setting_iterm_click_event_cb(lv_event_cb_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    const char *name = lv_event_get_user_data(e);

    if (LV_EVENT_CLICKED == code)
    {
        if (lv_strcmp(name, "熄屏时间") == 0)
        {
            last_tabindex = 0;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_SCREEN_OFF_TIME);
        }
        else if (lv_strcmp(name, "时间展示形式") == 0)
        {
            last_tabindex = 1;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_TIME_DISPLAY_FORMAT);
        }
        else if (lv_strcmp(name, "存储管理") == 0)
        {
            last_tabindex = 2;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_STORAGE_MANAGER);
        }
        // else if (lv_strcmp(name, "语音控制开关") == 0)
        // {
        //     lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_VOICE_CONTROL_SWITCH);
        // }
        // else if (lv_strcmp(name, "锁屏密码") == 0)
        // {
        //     lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_VOICE_LOCK_PASSWORD);
        // }
        else if (lv_strcmp(name, "滚轮振动开关") == 0)
        {
            last_tabindex = 3;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_VOICE_ROLLER_VIBRAT);
        }
        // else if (lv_strcmp(name, "振动幅度") == 0)
        // {
        //     lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_VIBRATION_AMPLITUDE);
        // }
        else if (lv_strcmp(name, "远程预览开关") == 0)
        {
            last_tabindex = 4;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_REMOTE_PREVIEW_SWITCH);
        }
        else if (lv_strcmp(name, "关于相机") == 0)
        {
            last_tabindex = 5;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_CAMERA_ABOUT);
        }
        else if (lv_strcmp(name, "恢复出厂设置") == 0)
        {
            last_tabindex = 6;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_FACTORY_RESTORE);
        }
        else if (lv_strcmp(name, "认证标志") == 0)
        {
            last_tabindex = 7;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_CERTIFICATION_MARK);
        }
        else if (lv_strcmp(name, "设备二维码") == 0)
        {
            last_tabindex = 8;
            lv_subject_set_int(&settings_more_subject, PAGE_SWITCH_CAMERA_QRCODE);
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
    switch_page->old_page = &settings_more_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_SCREEN_OFF_TIME:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_hold_time_info_get();
            break;
        case PAGE_SWITCH_TIME_DISPLAY_FORMAT:
            lv_stack_push(&settings_more_page_info);        
            switch_page->new_page = lv_page_time_display_info_get();
            break;
        case PAGE_SWITCH_STORAGE_MANAGER:
            lv_stack_push(&settings_more_page_info);        
            switch_page->new_page = lv_page_storage_manage_info_get();
            break;
        // case PAGE_SWITCH_VOICE_CONTROL_SWITCH:
        //     // lv_stack_push(&settings_more_page_info);
        //     // switch_page->new_page = lv_page_hold_time_info_get();
        //     break;
        // case PAGE_SWITCH_VOICE_LOCK_PASSWORD:
        //     // lv_stack_push(&settings_more_page_info);
        //     // switch_page->new_page = lv_page_time_display_info_get();
        //     break;
        case PAGE_SWITCH_VOICE_ROLLER_VIBRAT:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_roller_vibrat_info_get();
            break;
        // case PAGE_SWITCH_VIBRATION_AMPLITUDE:
        //     lv_stack_push(&settings_more_page_info);
        //     switch_page->new_page = lv_page_vibrat_amplitude_info_get();
        //     break;
        case PAGE_SWITCH_REMOTE_PREVIEW_SWITCH:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_remote_preview_info_get();
            break;
        case PAGE_SWITCH_CAMERA_ABOUT:
            lv_stack_push(&settings_more_page_info);        
            switch_page->new_page = lv_page_about_camera_info_get();
            break;
        case PAGE_SWITCH_FACTORY_RESTORE:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_factory_restore_info_get();
            break;
        case PAGE_SWITCH_CERTIFICATION_MARK:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_cert_mask_info_get();
            break;
        case PAGE_SWITCH_CAMERA_QRCODE:
            lv_stack_push(&settings_more_page_info);
            switch_page->new_page = lv_page_settingQr_info_get();
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