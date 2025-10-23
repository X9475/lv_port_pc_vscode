#include "../lv_switch_interface.h"

#define PHOTOGRAPH_ICON_SINGLE "V:tk1/realtime_shooting/photograph_icon_single.png"
#define PHOTOGRAPH_ICON_SELECT "V:tk1/realtime_shooting/photograph_icon_select.png"
#define PHOTOGRAPH_ICON_UNSELECT "V:tk1/realtime_shooting/photograph_icon_unselect.png"
#define PHOTOGRAPH_ICON_SELECT_GREEN "V:tk1/realtime_shooting/photograph_icon_select_green.png"
#define PHOTOGRAPH_ICON_ALLOWS_DOWN "V:tk1/realtime_shooting/photograph_icon_arrows_down.png"
#define PHOTOGRAPH_ICON_TRASH_FILLED "V:tk1/realtime_shooting/photograph_icon_trash_filled.png"


lv_subject_t  album_four_grid_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t up_area_style;

static lv_obj_t *screen = NULL;
static lv_obj_t *photo_all_label;

static lv_obj_t *video_pop_obj = NULL;
static lv_obj_t *content_cont = NULL;
lv_obj_t * photo_icon_select;
lv_obj_t * photo_icon_allows_down;
lv_obj_t * album_buttom_line;

// 全局变量
static int selected_count = 0;
static lv_obj_t *selected_count_label = NULL;
static bool selection_mode = false;
static bool *item_selected = NULL; // 记录每个项目的选中状态
static int current_video_type = 0; // 当前选择的视频类型：0-全部,1-照片,2-视频,3-延时摄影
static int total_recod_cnt = 0;
static int item_index = 0;
static int photo_total = 0;
static int video_total = 0;
static int time_lapse_total = 0;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void single_icon_click_event(lv_event_t * e);
static void allows_down_icon_click_event(lv_event_t * e);
static void select_icon_click_event(lv_event_t * e);
static void lable_click_event(lv_event_t * e);
static void item_click_event(lv_event_t * e);
static void recreate_content_layout(lv_obj_t *cont);


//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_DELETE,
    PAGE_SWITCH_BACK
};

//文件类型
static enum ALBUM_FILE_TYPE_ENUM
{
    ALBUM_FILE_ALL,
    ALBUM_FILE_PHOTO,
    ALBUM_FILE_VIDEO,
    ALBUM_FILE_TIME_LAPSE
};

static lv_page_info_t album_page_four_grid = {
    .page_id = PAGE_FUNCTIONAL_ALBUM_FOUR_GRID,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_album_four_grid_get()
{
    return &album_page_four_grid;
}

static void lv_page_construct(void)
{
    //样式初始化
    lv_page_style_init();
    //主题初始化
    lv_page_subject_init();
    //加入栈表
    // lv_stack_push(&agent_start_page_info);

    screen = lv_obj_create(act_screen);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(screen, &screen_style, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(screen);

    //绘制当前页面
    lv_page_load(screen);
    album_page_four_grid.page = screen;
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
    lv_style_set_pad_all(&up_area_style, 0);
    lv_style_set_border_width(&up_area_style, 0);
    lv_style_set_radius(&up_area_style, 0);
    lv_style_set_bg_grad(&up_area_style, &grad);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&album_four_grid_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_four_grid_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_four_grid_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_add_style(cont, &screen_style, 0);

    total_recod_cnt = 98;   //todo ：获取实际的录像个数
    photo_total = 89;       //todo ：获取实际的照片个数
    video_total = 7;        //todo ：获取实际的视频个数
    time_lapse_total = 3;    //todo ：获取实际的延时摄影个数

    //创建初始化录像全部布局
    recreate_content_layout(cont);

    return;
}

static const char* get_display_text_by_type(int type) 
{
    static char display_text[32]; // 静态缓冲区用于存储生成的文本

    switch(type) 
    {
        case ALBUM_FILE_ALL: 
            snprintf(display_text, sizeof(display_text), "全部%d", total_recod_cnt);
            return display_text;

        case ALBUM_FILE_PHOTO: 
            snprintf(display_text, sizeof(display_text), "照片%d", photo_total);
            return display_text;

        case ALBUM_FILE_VIDEO: 
            snprintf(display_text, sizeof(display_text), "视频%d", video_total);
            return display_text;

        case ALBUM_FILE_TIME_LAPSE: 
            snprintf(display_text, sizeof(display_text), "延时摄影%d", time_lapse_total);
            return display_text;

        default: 
            snprintf(display_text, sizeof(display_text), "全部%d", total_recod_cnt);
            return display_text;
    }
}

// 根据类型获取对应的记录数量
static int get_record_count_by_type(int type) 
{
    switch(type) 
    {
        case ALBUM_FILE_ALL: 
            return total_recod_cnt;     // 全部

        case ALBUM_FILE_PHOTO: 
            return photo_total;         // 照片

        case ALBUM_FILE_VIDEO: 
            return video_total;         // 视频

        case ALBUM_FILE_TIME_LAPSE: 
            return time_lapse_total;    // 延时摄影

        default: 
            return total_recod_cnt;
    }
}

// 重新创建内容布局
static void recreate_content_layout(lv_obj_t *cont) 
{
    // 删除旧的内容容器
    if (content_cont != NULL && lv_obj_is_valid(content_cont)) 
    {
        lv_obj_del(content_cont);
    }
    
    // 获取当前类型的记录数量
    int recod_cnt = get_record_count_by_type(current_video_type);
    
    // 创建新的内容容器
    content_cont = lv_obj_create(cont);
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

    int total_rows = (recod_cnt + 1) / 2;
    
    // 创建所有行容器
    for(int row = 0; row < total_rows; row++) 
    {
        // 创建行容器
        lv_obj_t *row_cont = lv_obj_create(content_cont);
        lv_obj_set_size(row_cont, 482, 194);
        lv_obj_set_flex_flow(row_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_bg_opa(row_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row_cont, 0, 0);
        lv_obj_set_style_pad_all(row_cont, 0, 0);
        lv_obj_set_style_margin_bottom(row_cont, 6, 0);

        // 在当前行创建项目（最多2个）
        for(int col = 0; col < 2; col++) 
        {
            int item_index = row * 2 + col;
            if(item_index >= recod_cnt)
            {
                break;
            }

            // 每个录像项目的容器
            lv_obj_t *item_cont = lv_obj_create(row_cont);
            lv_obj_set_size(item_cont, 238, 194);
        
            // 设置列间距
            if((col + 1) % 2) 
            {
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
            
            // 设置项目为可点击
            lv_obj_add_flag(item_cont, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(item_cont, item_click_event, LV_EVENT_CLICKED, (void*)(intptr_t)item_index);

            // lv_obj_t * photo_icon = lv_img_create(item_cont);
            // lv_img_set_src(photo_icon, PHOTOGRAPH_ICON_SINGLE);
            // //lv_img_set_src(photo_icon,  "V:png/img_camera_backup.png");
            // lv_obj_set_size(photo_icon, 238, 194);
            // lv_obj_align(photo_icon, LV_ALIGN_CENTER, 0, 0);

            // 时间标签
            lv_obj_t *time_label = lv_label_create(item_cont);
            lv_label_set_text(time_label, "00:30");
            lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
            lv_obj_set_style_text_font(time_label, font_get_regular(24), 0);
            lv_obj_align(time_label, LV_ALIGN_BOTTOM_LEFT, 10, -10);
        }
    }

    // 创建顶部矩形渐变框
    lv_obj_t *up_indicator_area = lv_obj_create(cont);
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
    lv_label_set_text(photo_all_label,  get_display_text_by_type(current_video_type)); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(photo_all_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(photo_all_label, font_get_regular(32), 0);
    lv_obj_set_style_text_color(photo_all_label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(photo_all_label, LV_ALIGN_TOP_RIGHT, -215, 19);

    photo_icon_allows_down = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_allows_down, PHOTOGRAPH_ICON_ALLOWS_DOWN);
    lv_obj_set_size(photo_icon_allows_down, 40, 40);
    lv_obj_align_to(photo_icon_allows_down, photo_all_label, LV_ALIGN_OUT_RIGHT_MID, 5, 0);  // 图标右侧，垂直居中
    lv_obj_add_flag(photo_icon_allows_down, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(photo_icon_allows_down, allows_down_icon_click_event, LV_EVENT_CLICKED, NULL);

    photo_icon_select = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_select, PHOTOGRAPH_ICON_SELECT);
    lv_obj_set_size(photo_icon_select, 40, 40);
    lv_obj_align(photo_icon_select, LV_ALIGN_TOP_LEFT, 432, 20);

    lv_obj_add_flag(photo_icon_select, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_icon_select, select_icon_click_event, LV_EVENT_CLICKED, NULL);

    album_buttom_line = lv_obj_create(cont);
    lv_obj_set_size(album_buttom_line, 10, 48);
    lv_obj_set_style_radius(album_buttom_line, 4, 0);
    lv_obj_set_style_bg_color(album_buttom_line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(album_buttom_line, LV_ALIGN_TOP_LEFT, 475, 183); 
    lv_obj_set_style_opa(album_buttom_line, LV_OPA_COVER, 0);
}

static void item_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("item icon clicked!");
    lv_subject_set_int(&album_four_grid_subject, PAGE_SWITCH_NEXT);
}

static void select_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("select icon clicked!");
    lv_subject_set_int(&album_four_grid_subject, PAGE_SWITCH_DELETE);
    // lv_obj_add_flag(photo_icon_select, LV_OBJ_FLAG_HIDDEN);
    // lv_obj_add_flag(photo_all_label, LV_OBJ_FLAG_HIDDEN);
    // lv_obj_add_flag(photo_icon_allows_down, LV_OBJ_FLAG_HIDDEN);
    // lv_obj_add_flag(album_buttom_line, LV_OBJ_FLAG_HIDDEN);


    // lv_obj_t * photo_icon_unselect = lv_img_create(screen);
    // lv_img_set_src(photo_icon_unselect, PHOTOGRAPH_ICON_UNSELECT);
    // lv_obj_set_size(photo_icon_unselect, 40, 40);
    // lv_obj_align(photo_icon_unselect, LV_ALIGN_TOP_LEFT, 171, 20);

    // // 选中数量标签（初始隐藏）
    // selected_count_label = lv_label_create(screen);
    // lv_label_set_text(selected_count_label, "已选择0");
    // lv_obj_set_style_text_color(selected_count_label, lv_color_white(), 0);
    // lv_obj_set_style_text_font(selected_count_label, font_get_regular(32), 0);
    // lv_obj_align(selected_count_label, LV_ALIGN_TOP_LEFT, 216, 25);

    // // 删除按钮
    // lv_obj_t *delete_icon = lv_img_create(screen);
    // lv_img_set_src(delete_icon, PHOTOGRAPH_ICON_TRASH_FILLED); // 替换为实际的删除图标路径
    // lv_obj_set_size(delete_icon, 40, 40);
    // lv_obj_align(delete_icon, LV_ALIGN_TOP_RIGHT, -30, 20);
    // lv_obj_add_flag(delete_icon, LV_OBJ_FLAG_CLICKABLE);

}


static void allows_down_icon_click_event(lv_event_t * e)
{
    static int video_type[4] = {0,1,2,3};
    // 处理点击事件的逻辑
    LV_LOG_USER("allows down icon clicked!");

    // 如果弹窗已存在，则关闭它
    if (video_pop_obj != NULL && lv_obj_is_valid(video_pop_obj)) 
    {
        lv_obj_del(video_pop_obj);
        video_pop_obj = NULL;
        return;
    }

    lv_obj_t* icon = lv_event_get_target(e);
    lv_obj_t* temp_parent = lv_obj_get_parent(icon);  // 获取父对象
    lv_obj_t* parent = lv_obj_get_parent(temp_parent);  // 获取父对象

    video_pop_obj = lv_obj_create(parent);
    lv_obj_set_size(video_pop_obj, 270, 274);
    lv_obj_set_style_bg_color(video_pop_obj, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_radius(video_pop_obj, 40, 0);
    lv_obj_set_style_border_width(video_pop_obj, 0, 0);
    lv_obj_align(video_pop_obj, LV_ALIGN_TOP_LEFT, 106, 75);

    char text_buffer[32] = {0};
    snprintf(text_buffer, sizeof(text_buffer), "全部(%d)", total_recod_cnt);
    lv_obj_t* label = lv_label_create(video_pop_obj);
    lv_label_set_text(label, text_buffer); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XAFF99C), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 30);

    // 点击全部标签关闭界面
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label, lable_click_event, LV_EVENT_CLICKED, &video_type[0]);
    
    snprintf(text_buffer, sizeof(text_buffer), "照片(%d)", photo_total);
    lv_obj_t* label1 = lv_label_create(video_pop_obj);
    lv_label_set_text(label1, text_buffer); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label1, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label1, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label1, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 91);
    // 点击照片标签关闭界面
    lv_obj_add_flag(label1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label1, lable_click_event, LV_EVENT_CLICKED, &video_type[1]);

    snprintf(text_buffer, sizeof(text_buffer), "视频(%d)", video_total);
    lv_obj_t* label2 = lv_label_create(video_pop_obj);
    lv_label_set_text(label2, text_buffer); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label2, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label2, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 152);

    lv_obj_add_flag(label2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label2, lable_click_event, LV_EVENT_CLICKED, &video_type[2]);

    snprintf(text_buffer, sizeof(text_buffer), "延时摄影(%d)", time_lapse_total);
    lv_obj_t* label3 = lv_label_create(video_pop_obj);
    lv_label_set_text(label3, text_buffer); //todo：后续根据实际情况去显示对应的值
    lv_obj_set_style_text_opa(label3, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label3, font_get_regular(24), 0);
    lv_obj_set_style_text_color(label3, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 213);

    lv_obj_add_flag(label3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(label3, lable_click_event, LV_EVENT_CLICKED, &video_type[3]);

}

static void lable_click_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    int *video_type = (int *)lv_event_get_user_data(e);
    if(code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("video_type:%d", *video_type);

        // 关闭弹窗界面
        if (video_pop_obj != NULL && lv_obj_is_valid(video_pop_obj)) 
        {
            lv_obj_del(video_pop_obj);
            video_pop_obj = NULL;
        }

        // 更新当前视频类型
        current_video_type = *video_type;

        //重新创建内容布局
        recreate_content_layout(screen);
    }
}

static void single_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("single icon clicked!");
    lv_subject_set_int(&album_four_grid_subject, PAGE_SWITCH_BACK);
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
    switch_page->old_page = &album_page_four_grid;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_album_get();
            break;

        case PAGE_SWITCH_DELETE:
            switch_page->new_page = lv_page_album_delete_get();
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