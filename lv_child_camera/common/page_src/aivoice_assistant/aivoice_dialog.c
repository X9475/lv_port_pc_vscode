#include "../lv_switch_interface.h"

lv_subject_t aidialog_adj_param_subject;
static lv_switch_page_pt switch_page;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_page_load(lv_obj_t *cont);

static void lv_back_button_event(lv_event_t *e);
static void lv_agent_button_event(lv_event_t *e);
static void lv_camera_button_event(lv_event_t *e);
static void lv_voice_button_event(lv_event_t *e);
static void lv_iphone_button_event(lv_event_t *e);
static void lv_itemcont_picture_event(lv_event_t * e);
static void lv_reduce_img_event(lv_event_t *e);

static lv_obj_t *screen = NULL;
static lv_obj_t *dialog_obj;
lv_obj_t *picextend_item_cont;

static lv_style_t screen_style;
static lv_style_t style_mask;
static lv_style_t style_mask1;
static lv_style_t style_mask2;
static lv_style_t up_area_style;
//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_CHOOSE_PIC,
    PAGE_SWITCH_CALL_STATE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t aidialog_page_info = {
    .page_id = PAGE_AIVOICE_DIALOG,
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

    //图层蒙板
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[0].opa = LV_OPA_TRANSP;
    grad.stops[1].color = lv_color_hex(0x000000);
    grad.stops[1].opa = LV_OPA_COVER;
    grad.stops[0].frac = 0;
    grad.stops[1].frac = 255;
    lv_style_init(&style_mask);
    lv_style_copy(&style_mask, &screen_style);
    lv_style_set_bg_grad(&style_mask, &grad);
    lv_style_set_bg_grad_dir(&style_mask, LV_GRAD_DIR_VER);

    //图层蒙板1
    static lv_grad_dsc_t grad1;
    grad1.dir = LV_GRAD_DIR_VER;
    grad1.stops_count = 2;
    grad1.stops[0].color = lv_color_hex(0x000000);
    grad1.stops[0].opa = LV_OPA_COVER;
    grad1.stops[1].color = lv_color_hex(0x000000);
    grad1.stops[1].opa = LV_OPA_TRANSP;
    grad1.stops[0].frac = 0;
    grad1.stops[1].frac = 255;
    lv_style_init(&style_mask1);
    lv_style_copy(&style_mask1, &screen_style);
    lv_style_set_bg_grad(&style_mask1, &grad1);
    lv_style_set_bg_grad_dir(&style_mask1, LV_GRAD_DIR_VER);

     //图层蒙板2
    static lv_grad_dsc_t grad2;
    grad2.dir = LV_GRAD_DIR_VER;
    grad2.stops_count = 2;
    grad2.stops[0].color = lv_color_hex(0x000000);
    grad2.stops[0].opa = LV_OPA_COVER;
    grad2.stops[1].color = lv_color_hex(0x000000);
    grad2.stops[1].opa = LV_OPA_TRANSP;
    grad2.stops[0].frac = 0;
    grad2.stops[1].frac = 255;
    lv_style_init(&style_mask2);
    lv_style_copy(&style_mask2, &screen_style);
    lv_style_set_bg_grad(&style_mask2, &grad2);
    lv_style_set_bg_grad_dir(&style_mask2, LV_GRAD_DIR_VER);

    //up_area_style 图层蒙板3
    static lv_grad_dsc_t grad3;
    grad3.dir = LV_GRAD_DIR_VER;
    grad3.stops_count = 2;
    grad3.stops[0].color = lv_color_hex(0x000000);
    grad3.stops[0].opa = LV_OPA_80;
    grad3.stops[1].color = lv_color_hex(0x000000);
    grad3.stops[1].opa = LV_OPA_80;
    grad3.stops[0].frac = 0;
    grad3.stops[1].frac = 255;
    lv_style_init(&up_area_style);
    lv_style_copy(&up_area_style, &screen_style);
    lv_style_set_bg_grad(&up_area_style, &grad3);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&aidialog_adj_param_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&aidialog_adj_param_subject, lv_switch_observer_cb, NULL);
    return;
}

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
        case PAGE_SWITCH_CHOOSE_PIC:
            lv_stack_push(&aidialog_page_info);
            switch_page->new_page = lv_page_choosepic_param_get();
            break;
        case PAGE_SWITCH_CALL_STATE:
            // switch_page->new_page = lv_page_callstate_param_get();
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

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&aidialog_adj_param_subject);
}

static void lv_back_button_event(lv_event_t *e)
{
    lv_obj_t *back = lv_event_get_target(e);
    lv_obj_t *parent = lv_obj_get_parent(back);
    lv_obj_add_flag(parent, LV_OBJ_FLAG_HIDDEN);
}

static void lv_agent_button_event(lv_event_t *e)
{
    // 添加智能体
}

static void lv_camera_button_event(lv_event_t *e)
{
    // 触发拍照功能
    lv_subject_set_int(&aidialog_adj_param_subject, PAGE_SWITCH_CHOOSE_PIC);
}

static void hide_dialog_timer(lv_timer_t *timer) 
{
    lv_obj_t *dialog_obj = lv_timer_get_user_data(timer);
    if(dialog_obj) {
        lv_obj_add_flag(dialog_obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_del(dialog_obj);
    }
    lv_timer_del(timer);
}
static void lv_voice_button_event(lv_event_t *e)
{
    lv_obj_t *back = lv_event_get_target(e);
    lv_obj_t *parent = lv_obj_get_parent(back);
    
    // 触发语音输入功能
    lv_event_code_t code = lv_event_get_code(e);
    
    switch(code) 
    {
        case LV_EVENT_LONG_PRESSED:
            {
                // 创建语音输入对话框
                dialog_obj = lv_obj_create(parent);
                lv_obj_remove_style_all(dialog_obj);
                lv_obj_add_style(dialog_obj, &up_area_style, 0);
                lv_obj_set_size(dialog_obj, 502, 410);
                lv_obj_align(dialog_obj, LV_ALIGN_TOP_MID, 0, 0);
                
                // 插入文字
                lv_obj_t *bottom_label = lv_label_create(dialog_obj);
                lv_label_set_text(bottom_label, "松开发送，上移取消");
                lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
                lv_obj_set_style_text_font(bottom_label, fzlthr_26, 0);
                lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
                lv_obj_set_style_text_align(bottom_label, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -120);
                
                // 保存标签对象引用，用于后续操作
                lv_obj_set_user_data(dialog_obj, bottom_label);
                break;
            }
            
        case LV_EVENT_RELEASED: 
            {
                if(dialog_obj) 
                {
                    lv_obj_t *bottom_label = lv_obj_get_user_data(dialog_obj);
                    if(bottom_label) 
                    {
                        lv_obj_add_flag(bottom_label, LV_OBJ_FLAG_HIDDEN);
                    }
                    lv_obj_add_flag(dialog_obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_del(dialog_obj);  // 彻底删除对象
                    dialog_obj = NULL;
                }
                break;
            }
            
        case LV_EVENT_PRESSING:
            {
                // 实时获取鼠标移动向量
                lv_point_t vector;
                lv_indev_get_vect(lv_indev_get_act(), &vector);
                // 设置移动阈值，避免微小移动误判
                const int16_t threshold = 5;
                
                if(LV_ABS(vector.x) > threshold || LV_ABS(vector.y) > threshold) 
                {
                    // 判断主要移动方向
                    if(LV_ABS(vector.x) > LV_ABS(vector.y)) 
                    {
                        if(vector.x > 0) 
                        {
                            LV_LOG_INFO("move right x: %d\n", vector.x);
                        } 
                        else 
                        {
                            LV_LOG_INFO("move left x: %d\n", -vector.x);
                        }
                    } 
                    else 
                    {
                        if(vector.y > 0) 
                        {
                            if(vector.y >= 6 && vector.y <= 11)
                            {
                                if(dialog_obj) 
                                {
                                    lv_obj_t *bottom_label = lv_obj_get_user_data(dialog_obj);
                                    if(bottom_label) 
                                    {
                                        lv_label_set_text(bottom_label, "松开发送，上移取消");
                                        lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
                                    }
                                }
                            }
                        } 
                        else 
                        {
                            // 检测到向上移动，取消语音输入
                            if(dialog_obj) 
                            {
                                lv_obj_t *bottom_label = lv_obj_get_user_data(dialog_obj);
                                if(bottom_label) 
                                {
                                    lv_label_set_text(bottom_label, "松手取消");
                                    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFF0000), 0);
                                }
                            }
                        }
                    }
                }
                break;
            }
        default:
            break;
    }
}


static void lv_iphone_button_event(lv_event_t *e)
{
    // TODO: 触发呼叫功能
    lv_subject_set_int(&aidialog_adj_param_subject, PAGE_SWITCH_CHOOSE_PIC);
}

static void lv_reduce_img_event(lv_event_t *e)
{
    lv_obj_t *back = lv_event_get_target(e);
    lv_obj_t *parent = lv_obj_get_parent(back);
    lv_obj_add_flag(parent, LV_OBJ_FLAG_HIDDEN);
}

static void lv_itemcont_picture_event(lv_event_t * e)
{
    lv_obj_t *image = lv_event_get_target(e);
    lv_obj_set_size(image, 502, 410);
    lv_img_set_zoom(image, 128);
    lv_obj_align(image, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *reduce_img = lv_img_create(image);
    lv_img_set_src(reduce_img, "../lv_port_pc_vscode/assert/icon/ai_dialog_shrink.png");
    lv_obj_set_size(reduce_img, 60, 60);
    lv_obj_align(reduce_img, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_add_flag(reduce_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(reduce_img, lv_reduce_img_event, LV_EVENT_CLICKED, NULL);
}
static void lv_page_load(lv_obj_t *cont)
{
    // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);

    // 加阴影
    dialog_obj = lv_obj_create(cont);
    lv_obj_remove_style_all(dialog_obj);
    lv_obj_add_style(dialog_obj, &style_mask1, 0);
    lv_obj_set_size(dialog_obj, 502, 70);
    lv_obj_align(dialog_obj, LV_ALIGN_TOP_MID, 0, 0);

    //返回按钮
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_set_size(back_btn, 70, 70);
    lv_obj_set_style_shadow_width(back_btn, 0, 0);
    lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(back_btn, lv_back_button_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(back_btn, cont, LV_ALIGN_TOP_LEFT, 20, 10);

    lv_obj_t *back = lv_img_create(back_btn);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_align(back, LV_ALIGN_CENTER, 3, 0);

    // 添加AI对话标题
    lv_obj_t *tip1_label = lv_label_create(dialog_obj);
    lv_label_set_text(tip1_label, "AI对话");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(tip1_label, fzlthr_28, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(tip1_label, back_btn, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    
    // 添加智能体
    lv_obj_t *agent_button = lv_img_create(dialog_obj);
    lv_img_set_src(agent_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_parent.png");
    lv_obj_set_size(agent_button, 40, 40);
    lv_obj_align(agent_button, LV_ALIGN_TOP_RIGHT, -30, 20);
    lv_obj_add_flag(agent_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(agent_button, lv_agent_button_event, LV_EVENT_CLICKED, NULL);

    // 添加语音输入按钮
    lv_obj_t *voice_button = lv_img_create(cont);
    lv_img_set_src(voice_button, "../lv_port_pc_vscode/assert/icon/ai_dialog.png");
    lv_obj_set_size(voice_button, 442, 90);
    lv_obj_align(voice_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(voice_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_LONG_PRESSED, NULL);
    lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_PRESSING, NULL);
    // 添加电话按钮
    lv_obj_t *iphone_button = lv_img_create(voice_button);
    lv_img_set_src(iphone_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_phone.png");
    lv_obj_align(iphone_button, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_flag(iphone_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(iphone_button, lv_iphone_button_event, LV_EVENT_CLICKED, NULL);

    // 添加照片按钮
    lv_obj_t *camera_button = lv_img_create(voice_button);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_picture.png");
    lv_obj_align(camera_button, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_button_event, LV_EVENT_CLICKED, NULL);

  
    // 添加底部提示文字
    lv_obj_t *bottom_label = lv_label_create(voice_button);
    lv_label_set_text(bottom_label, "按住说话");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, fzlthr_30, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_CENTER, 0, 0);

    if (NULL != aidialog_page_info.reserved)
    {
        picextend_item_cont = lv_img_create(cont);
        lv_img_set_src(picextend_item_cont, (char *)aidialog_page_info.reserved);
        lv_obj_set_size(picextend_item_cont, 120, 98);
        lv_obj_set_style_clip_corner(picextend_item_cont, true, 0);
        lv_obj_set_style_radius(picextend_item_cont, 15, 0);
        lv_obj_align(picextend_item_cont, LV_ALIGN_TOP_RIGHT, -30, 192);
        lv_obj_add_flag(picextend_item_cont, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(picextend_item_cont, lv_itemcont_picture_event, LV_EVENT_CLICKED, picextend_item_cont);
    }
}