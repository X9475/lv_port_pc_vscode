#include "../lv_switch_interface.h"

#define PHOTOGRAPH_PIC_PHOTO "V:tk1/realtime_shooting/photograph_pic_photo.png"
#define ICON_BACK "V:tk1/realtime_shooting/photograph_icon_back.png"

lv_subject_t  album_share_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;

static lv_obj_t *screen = NULL;
static lv_obj_t *line_cont;
static lv_timer_t *anim_timer = NULL;
// 存储联系人对象指针的数组
static lv_obj_t *contact_objs[20];

static uint8_t exec_count = 0;
static uint8_t conctact_total = 20;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void contact_scroll_event_cb(lv_event_t *e);
static void contact_page_click_event_cb(lv_event_t *e);
static void *line_container_create(lv_obj_t *cont);
static void circular_scroll_handle(lv_obj_t *cont, uint8_t dir);

//待跳转的页面种类
//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_SUC,
    PAGE_SWITCH_FAIL,
    PAGE_SWITCH_BACK
};

static lv_page_info_t album_page_share = {
    .page_id = PAGE_FUNCTIONAL_ALBUM_SHARE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_album_share_get()
{
    return &album_page_share;
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
    album_page_share.page = screen;
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
    lv_subject_init_int(&album_share_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_share_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_share_subject);
}

static void page_back_event_cb(lv_event_t *e)
{
    lv_subject_set_int(&album_share_subject, PAGE_SWITCH_BACK);
}
static void lv_page_load(lv_obj_t *cont)
{
    //创建联系人
    lv_obj_t *cont_col = lv_obj_create(cont);
    lv_obj_add_style(cont_col, &screen_style, 0);

    // 移除flex布局，使用绝对布局
    lv_obj_set_size(cont_col, lv_pct(100), lv_pct(100));
    lv_obj_set_layout(cont_col, LV_LAYOUT_NONE);
    lv_obj_set_scroll_snap_y(cont_col, LV_SCROLL_SNAP_CENTER);
    lv_obj_align(cont_col, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_scroll_dir(cont_col, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(cont_col, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont_col, contact_scroll_event_cb, LV_EVENT_SCROLL, NULL);

    //todo：要获取到联系人的头像和名称，以及联系人总数conctact_total
    for (uint8_t i = 0; i < conctact_total; i++)
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
        lv_img_set_src(image, PHOTOGRAPH_PIC_PHOTO);
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
    

    lv_scr_load_anim(cont, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);

    //返回按钮
    lv_obj_t *back = lv_img_create(cont);
    lv_obj_set_size(back, 50, 50);
    lv_img_set_src(back, ICON_BACK);
    lv_obj_align_to(back, cont, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back, page_back_event_cb, LV_EVENT_CLICKED, NULL);

    //右侧滚动条
    line_cont = line_container_create(cont);

    return;
}

static void *line_container_create(lv_obj_t *cont)
{
    lv_obj_t *line_cont = lv_obj_create(cont);

    lv_obj_add_style(line_cont, &screen_style, 0);
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

static void contact_page_click_event_cb(lv_event_t *e)
{
    lv_obj_t *saver = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *mode = lv_event_get_user_data(e);
    if (LV_EVENT_CLICKED == code)
    {
        
        for(int i = 0; i<20; i++)
        {
            if(lv_obj_get_child(mode, i) == saver)
            {
                LV_LOG_WARN("share contact %d", i + 1);
                //todo:通过回执，分享成功跳转到分享成功界面
                if(i + 1 < 10)
                {
                    lv_subject_set_int(&album_share_subject, PAGE_SWITCH_SUC);
                }
                else
                {
                    lv_subject_set_int(&album_share_subject, PAGE_SWITCH_FAIL);
                }
            }
        }
    }
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

static void update_center_contact_color(lv_obj_t *obj)
{
    // 获取滚动位置
    lv_coord_t scroll_y = lv_obj_get_scroll_y(obj);
    lv_coord_t height = lv_obj_get_height(obj);
    
    // 计算中间位置
    lv_coord_t center_y = scroll_y + height / 2;
    
    // 重置所有联系人的背景颜色
    for (uint8_t i = 0; i < conctact_total; i++) 
    {
        lv_obj_set_style_bg_color(contact_objs[i], lv_color_hex(0x000000), LV_PART_MAIN);
    }
    
    // 找到最接近中间位置的联系人
    int closest_index = -1;
    lv_coord_t min_distance = INT16_MAX;
    
    for (uint8_t i = 0; i <= conctact_total; i++) 
    {
        lv_coord_t contact_y = i * (160 + 12) + 80; // 联系人的中心Y坐标
        lv_coord_t distance = LV_ABS(contact_y - center_y);
        
        if (distance < min_distance) {
            min_distance = distance;
            closest_index = i-1;
        }
    }
    
    // 高亮中间的联系人
    if (closest_index >= 0 && closest_index <= conctact_total) 
    {
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

    if (cur_idx != last_idx) 
    {
        last_idx = cur_idx;
        if (!anim_timer) 
        {
            //创建定时器，每40ms执行滚动动态
            anim_timer = lv_timer_create(screen_saver_timer_cb, 40, &dir);
            exec_count = 0;
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
    switch_page->old_page = &album_page_share;

    switch (page_event)
    {
        case PAGE_SWITCH_SUC:
            switch_page->new_page = lv_page_album_share_suc_get();
            break;

        case PAGE_SWITCH_FAIL:
            switch_page->new_page = lv_page_album_share_retry_get();
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