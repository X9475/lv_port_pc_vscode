#include "../lv_switch_interface.h"

#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"
#define ICON_COUNT (sizeof(versatile_filters_icon_paths) / sizeof(versatile_filters_icon_paths[0]))

// 定义文件路径数组（替换原来的图标指针数组）
static const char *versatile_filters_icon_paths[] = 
{
    "V:tk1/realtime_shooting/lut_icon_none.png",
    "V:tk1/realtime_shooting/lut_icon_none.png", 
    "V:tk1/realtime_shooting/lut_icon_none.png",
    "V:tk1/realtime_shooting/lut_icon_none.png",
    "V:tk1/realtime_shooting/lut_icon_none.png"
};

lv_subject_t  shooting_multi_filter_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t style_multi_filter;

static lv_obj_t *screen = NULL;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void versatile_filters_item_event_cb(lv_event_t * e);
static void app_icon_event_cb(lv_event_t * e);

static void back_click_cb(lv_event_t * e);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_BACK
};

static lv_page_info_t shooting_multi_filter_page = {
    .page_id = PAGE_FUNCTIONAL_SHOOTING_MULTI_FILTER,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_shooting_multi_filter_get()
{
    return &shooting_multi_filter_page;
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
    shooting_multi_filter_page.page = screen;
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
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&shooting_multi_filter_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&shooting_multi_filter_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&shooting_multi_filter_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_add_style(cont, &screen_style, 0);
    //创建返回按钮
    lv_obj_t * return_icon = lv_img_create(cont);
    lv_img_set_src(return_icon, ICON_BACK);
    lv_obj_set_size(return_icon, 50, 50);
    lv_obj_align(return_icon, LV_ALIGN_TOP_LEFT, 30, 20);

    lv_obj_add_flag(return_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(return_icon, back_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * title_label = lv_label_create(cont);
    lv_label_set_text(title_label, "滤镜滤镜");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_set_style_text_font(title_label, font_get_regular(30), 0);
    lv_obj_align_to(title_label, return_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);  // 图标右侧，垂直居中

    // 创建右部矩形渐变框
    lv_obj_t *right_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(right_indicator_area, 375, 340);
    lv_obj_align(right_indicator_area, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(right_indicator_area, &style_multi_filter, 0);

    lv_obj_set_style_border_width(right_indicator_area, 0, 0);
    lv_obj_set_style_radius(right_indicator_area, 0, 0);
    lv_obj_set_style_pad_all(right_indicator_area, 0, 0);


    lv_obj_t * versatile_filters_item = lv_obj_create(cont);
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

    return;
}

static void back_click_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_CLICKED) 
    {
        printf("back photograph\n");
        lv_subject_set_int(&shooting_multi_filter_subject, PAGE_SWITCH_NEXT);
    }
}

static void app_icon_event_cb(lv_event_t * e)
{
    lv_obj_t * app_obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) 
    {
        LV_LOG_USER("Clicked");
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


static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_WARN("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发
    
    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &shooting_multi_filter_page;

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