#include "../lv_switch_interface.h"

lv_subject_t inital_startup_subject;
static lv_switch_page_pt switch_page;

static lv_obj_t *screen = NULL;
static lv_style_t screen_style;
static lv_style_t indc_style;
static lv_style_t main_style;
static lv_anim_t arc_anim;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void long_pressed_event_cb(lv_event_t *e);
static void set_arc_value(void *obj, int32_t v);
static void anim_finish_cb(lv_anim_t *anim);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t inital_startup_page_info = {
    .page_id = PAGE_FUNCTIONAL_FIRST_BOOTUP,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_inital_startup_info_get()
{
    return &inital_startup_page_info;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();
    //加入栈表
    // lv_stack_push(&inital_startup_page_info);

    screen = lv_obj_create(NULL);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);

    inital_startup_page_info.page = screen;
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

    //indc_style
    lv_style_init(&indc_style);
    lv_style_set_arc_color(&indc_style, lv_color_hex(0x56D6AA));
    lv_style_set_arc_opa(&indc_style, LV_OPA_60);
    lv_style_set_arc_width(&indc_style, 12);

    //main_style
    lv_style_init(&main_style);
    lv_style_set_arc_color(&main_style, lv_color_hex(0xABABAB));
    lv_style_set_arc_opa(&main_style, LV_OPA_60);
    lv_style_set_arc_width(&main_style, 12);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&inital_startup_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&inital_startup_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&inital_startup_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //背景图
    lv_obj_t *click_face = lv_img_create(cont);
    lv_img_set_src(click_face, "V:tk1/icon/long_press_2x.png");
    lv_img_set_zoom(click_face, 128);
    lv_obj_align(click_face, LV_ALIGN_TOP_MID, -5, -65);

    //环形加载器
    lv_obj_t *arc = lv_arc_create(cont);
    lv_obj_set_size(arc, 226, 226);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_value(arc, 0);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_style(arc, &indc_style, LV_PART_INDICATOR);
    lv_obj_add_style(arc, &main_style, LV_PART_MAIN);
    lv_obj_align(arc, LV_ALIGN_TOP_MID, 0, 28);

    //设置隐形点击区域 LV_OPA_TRANSP
    lv_obj_t *click_area = lv_btn_create(cont);
    lv_obj_set_size(click_area, 180, 180);
    lv_obj_set_style_shadow_width(click_area, 0, 0);
    lv_obj_set_style_radius(click_area, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(click_area, true, 0);
    lv_obj_add_flag(click_area, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align_to(click_area, arc, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(click_area, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(click_area, long_pressed_event_cb, LV_EVENT_LONG_PRESSED, arc);
    lv_obj_add_event_cb(click_area, long_pressed_event_cb, LV_EVENT_RELEASED, arc);

    //文字说明
    lv_obj_t *tip1_label = lv_label_create(cont);
    lv_label_set_text(tip1_label, "长按一下");
    lv_obj_set_style_text_opa(tip1_label, LV_OPA_90, 0);
    lv_obj_set_style_text_font(tip1_label, fzlthr_34, 0);
    lv_obj_set_style_text_color(tip1_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip1_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip1_label, LV_ALIGN_TOP_MID, 0, 289);

    lv_obj_t *tip2_label = lv_label_create(cont);
    lv_label_set_text(tip2_label, "召唤你的专属搭子");
    lv_obj_set_style_text_opa(tip2_label, LV_OPA_80, 0);
    lv_obj_set_style_text_font(tip2_label, fzlthr_26, 0);
    lv_obj_set_style_text_color(tip2_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(tip2_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(tip2_label, LV_ALIGN_TOP_MID, 0, 338);

    return;
}

static void long_pressed_event_cb(lv_event_t *e)
{
    static bool long_flag = false;
    static bool release_flag = false;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *arc = lv_event_get_user_data(e);

    // printf("======================code: %d\n", code);

    if (code == LV_EVENT_LONG_PRESSED && long_flag == false)
    {
        long_flag = true;
        release_flag = false;

        lv_anim_init(&arc_anim);
        lv_anim_set_var(&arc_anim, arc);
        lv_anim_set_exec_cb(&arc_anim, set_arc_value);
        lv_anim_set_time(&arc_anim, 2000);
        lv_anim_set_repeat_count(&arc_anim, 1);
        lv_anim_set_values(&arc_anim, 0, 100);
        //设置动画结束后的回调函数
        lv_anim_set_ready_cb(&arc_anim, anim_finish_cb);
        lv_anim_start(&arc_anim);
    }

    if (code == LV_EVENT_RELEASED && release_flag == false)
    {
        long_flag = false;
        release_flag = true;

        // 获取当前进度值
        int32_t current_value = lv_arc_get_value(arc);

        lv_anim_pause(&arc_anim);
        lv_anim_set_values(&arc_anim, current_value, 0);//反向设置值
        lv_anim_set_time(&arc_anim, current_value * 20);//根据当前进度计算回退时间（按比例）
        lv_anim_set_ready_cb(&arc_anim, NULL);//移除原回调避免冲突
        lv_anim_set_playback_time(&arc_anim, 0);//清除可能的回放设置
        lv_anim_start(&arc_anim);
    }
}

static void set_arc_value(void *obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)obj, v);
}

static void anim_finish_cb(lv_anim_t *anim)
{
    lv_anim_del_all();
    lv_subject_set_int(&inital_startup_subject, PAGE_SWITCH_NEXT);
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
    switch_page->old_page = &inital_startup_page_info;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_agent_start_info_get();
            break;
        case PAGE_SWITCH_BACK:
            switch_page->new_page = lv_stack_pop();
            break;
        default:
            LV_LOG_WARN("[%s:%d] -- page switch event:%d invaild", __FILE__, __LINE__, page_event);
            break;
    }

    if (NULL == switch_page->new_page) return;
    lv_subject_set_pointer(&switch_subject, switch_page);
}

