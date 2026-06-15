#include "../lv_switch_interface.h"

#define TASK_NUM    4

lv_subject_t aidialog_subject;
static lv_switch_page_pt switch_page;
static lv_obj_t *screen = NULL;
static lv_obj_t *ai_aux = NULL;
static lv_obj_t *bottom_label = NULL;
static lv_style_t screen_style;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_page_load(lv_obj_t *cont);
static void page_back_event_cb(lv_event_t *e);
static void ai_aux_click_event_cb(lv_event_t *e);
static void task_bar_item_event_cb(lv_event_t *e);
static void task_bar_item_create(lv_obj_t *cont, const char *path);
// static void lv_voice_button_event(lv_event_t *e);

static const char *taskbar_list[] = {
    "../lv_port_pc_vscode/assert/icon/ai_photo_shoot.png",
    "../lv_port_pc_vscode/assert/icon/ai_photo_album.png",
    "../lv_port_pc_vscode/assert/icon/ai_photo_call.png",
    "../lv_port_pc_vscode/assert/icon/ai_photo_back.png"
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SHOOT,
    PAGE_SWITCH_ALBUM,
    PAGE_SWITCH_PHONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t aidialog_page_info = {
    .page_id = PAGE_FUNCTIONAL_AIDIALOG,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_aidialog_info_get()
{
    return &aidialog_page_info;
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
    aidialog_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
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
    lv_subject_init_int(&aidialog_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&aidialog_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&aidialog_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 70, 70);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(back_btn, page_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(back_btn, cont, LV_ALIGN_TOP_LEFT, 20, 10);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 3, 0);

    //添加语音输入按钮
    // lv_obj_t *voice_button = lv_img_create(cont);
    // lv_img_set_src(voice_button, "../lv_port_pc_vscode/assert/icon/ai_dialog.png");
    // lv_obj_set_size(voice_button, 442, 90);
    // lv_obj_align(voice_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    // lv_obj_add_flag(voice_button, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_LONG_PRESSED, NULL);
    // lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_RELEASED, NULL);
    // lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_PRESSING, NULL);

    //添加底部提示文字
    bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "按住说话");
    lv_obj_set_style_text_font(bottom_label, fzlthr_26, 0);
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(bottom_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -45);

    //任务栏
    lv_obj_t *task_cont = lv_img_create(cont);
    lv_obj_set_size(task_cont, 380, 70);
    lv_img_set_src(task_cont, "../lv_port_pc_vscode/assert/icon/ai_photo_taskbar.png");
    lv_img_set_zoom(task_cont, 128);
    lv_obj_align(task_cont, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_obj_clear_flag(task_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(task_cont, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *task_bar = lv_obj_create(task_cont);
    lv_obj_set_size(task_bar, 380, 70);
    lv_obj_set_style_bg_opa(task_bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(task_bar, 0, 0);
    lv_obj_align(task_bar, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_flex_flow(task_bar, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(task_bar, LV_OBJ_FLAG_SCROLLABLE);
    //希望从左侧开始但留出10px空间
    lv_obj_set_flex_align(task_bar, 
                      LV_FLEX_ALIGN_START,//左对齐
                      LV_FLEX_ALIGN_CENTER,
                      LV_FLEX_ALIGN_CENTER);
    //整体向右偏移10像素
    lv_obj_set_style_pad_left(task_bar, 30, 0);
    for (int i = 0; i < TASK_NUM; i++)
    {
        task_bar_item_create(task_bar, taskbar_list[i]);
    }

    //展开任务栏开关
    ai_aux = lv_img_create(cont);
    lv_obj_set_size(ai_aux, 70, 70);
    lv_img_set_src(ai_aux, "../lv_port_pc_vscode/assert/icon/ai_photo_aux.png");
    lv_img_set_zoom(ai_aux, 128);
    lv_obj_add_flag(ai_aux, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(ai_aux, LV_ALIGN_BOTTOM_LEFT, 40, -30);
    lv_obj_add_event_cb(ai_aux, ai_aux_click_event_cb, LV_EVENT_CLICKED, task_cont);

    return;
}

static void task_bar_item_create(lv_obj_t *cont, const char *path)
{
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_set_size(obj, 70, 70);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(obj, task_bar_item_event_cb, LV_EVENT_CLICKED, lv_obj_get_parent(cont));

    lv_obj_t *img = lv_img_create(obj);
    lv_img_set_src(img, path);
    if (lv_obj_get_index(obj) != 3) {
        lv_img_set_zoom(img, 128);
    }
    else {//返回按键
        lv_obj_set_size(obj, 100, 70);
    }
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    return;
}

static void task_bar_item_event_cb(lv_event_t *e)
{
    lv_obj_t *task_cont = lv_event_get_user_data(e);
    int32_t idx = lv_obj_get_index(lv_event_get_target_obj(e));

    printf("====idx: %d\n", idx);
    switch (idx)
    {
        case 0://进拍摄
            lv_subject_set_int(&aidialog_subject, PAGE_SWITCH_SHOOT);
            break;
        case 1://进相册
            lv_subject_set_int(&aidialog_subject, PAGE_SWITCH_ALBUM);
            break;
        case 2://打电话
            lv_subject_set_int(&aidialog_subject, PAGE_SWITCH_PHONE);
            break;
        case 3: {
            if (ai_aux && lv_obj_is_valid(ai_aux)) {
                lv_obj_clear_flag(ai_aux, LV_OBJ_FLAG_HIDDEN);
            }
            if (bottom_label && lv_obj_is_valid(bottom_label)) {
                lv_obj_clear_flag(bottom_label, LV_OBJ_FLAG_HIDDEN);
            }
            lv_obj_add_flag(task_cont, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        default:
            LV_LOG_WARN("task bar idx invaild");
            break;
    }

    return;
}

static void ai_aux_click_event_cb(lv_event_t *e)
{
    lv_obj_t *task_cont = lv_event_get_user_data(e);

    //弹出任务栏
    if (ai_aux && lv_obj_is_valid(ai_aux)) {
        lv_obj_add_flag(ai_aux, LV_OBJ_FLAG_HIDDEN);
    }
    if (bottom_label && lv_obj_is_valid(bottom_label)) {
        lv_obj_add_flag(bottom_label, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_clear_flag(task_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_fade_in(task_cont, 200, 0);
    return;
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&aidialog_subject, PAGE_SWITCH_BACK);
}

// static void lv_voice_button_event(lv_event_t *e)
// {
//     lv_obj_t *back = lv_event_get_target(e);
//     lv_obj_t *parent = lv_obj_get_parent(back);
//
//     // 触发语音输入功能
//     lv_event_code_t code = lv_event_get_code(e);
//
//     switch(code) 
//     {
//         case LV_EVENT_LONG_PRESSED: {
//             // 插入文字
//             lv_obj_t *bottom_label = lv_label_create(parent);
//             lv_label_set_text(bottom_label, "松开发送，上移取消");
//             lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
//             lv_obj_set_style_text_font(bottom_label, fzlthr_26, 0);
//             lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
//             lv_obj_set_style_text_align(bottom_label, LV_TEXT_ALIGN_CENTER, 0);
//             lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -120);
//             break;
//         }
//         case LV_EVENT_RELEASED: {
//             lv_obj_t *bottom_label = lv_obj_get_user_data(parent);
//             lv_obj_add_flag(bottom_label, LV_OBJ_FLAG_HIDDEN);
//             break;
//         }
//         case LV_EVENT_PRESSING:
//             {
//                 // 实时获取鼠标移动向量
//                 lv_point_t vector;
//                 lv_indev_get_vect(lv_indev_get_act(), &vector);
//                 // 设置移动阈值，避免微小移动误判
//                 const int16_t threshold = 5;
//
//                 if(LV_ABS(vector.x) > threshold || LV_ABS(vector.y) > threshold) 
//                 {
//                     // 判断主要移动方向
//                     if(LV_ABS(vector.x) > LV_ABS(vector.y)) 
//                     {
//                         if(vector.x > 0) 
//                         {
//                             LV_LOG_INFO("move right x: %d\n", vector.x);
//                         } 
//                         else 
//                         {
//                             LV_LOG_INFO("move left x: %d\n", -vector.x);
//                         }
//                     } 
//                     else
//                     {
//                         if (vector.y > 0)
//                         {
//                             if (vector.y >= 6 && vector.y <= 11) {
//                                 // lv_label_set_text(bottom_label, "松开发送，上移取消");
//                                 // lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
//                             }
//                         }
//                         else
//                         {
//                             // lv_label_set_text(bottom_label, "松手取消");
//                             // lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFF0000), 0);
//                         }
//                     }
//                 }
//                 break;
//             }
//         default:
//             break;
//     }
// }

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    extern lv_page_info_pt lv_page_menu_info_get();

    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_INFO("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发

    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &aidialog_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_SHOOT:
            lv_stack_push(&aidialog_page_info);
            switch_page->new_page = lv_page_shooting_photo_get();
            break;
        case PAGE_SWITCH_ALBUM:
            lv_stack_push(&aidialog_page_info);
            switch_page->new_page = lv_page_album_get();
            break;
        case PAGE_SWITCH_PHONE:
            lv_stack_push(&aidialog_page_info);
            switch_page->new_page = lv_page_aiphone_get();
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