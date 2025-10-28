#include "../lv_switch_interface.h"

#define PHOTOGRAPH_ICON_SELECT_GREEN "V:tk1/realtime_shooting/photograph_icon_select_green.png"
#define PHOTOGRAPH_ICON_SINGLE "V:tk1/realtime_shooting/photograph_icon_single.png"
#define PHOTOGRAPH_ICON_UNSELECT "V:tk1/realtime_shooting/photograph_icon_unselect.png"
#define PHOTOGRAPH_ICON_TRASH_FILLED "V:tk1/realtime_shooting/photograph_icon_trash_filled.png"

lv_subject_t  album_delete_subject;
static lv_switch_page_pt switch_page;

static lv_style_t screen_style;
static lv_style_t up_area_style;

static lv_obj_t *screen = NULL;
static lv_obj_t *selected_count_label = NULL;

static int selected_count = 0;
static bool selection_mode = false;
static bool *item_selected = NULL; // 记录每个项目的选中状态
static int total_recod_cnt = 8; //todo ：获取实际的录像个数

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_page_load(lv_obj_t *cont);
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void create_video_list(lv_obj_t *cont);
static void select_item_event(lv_event_t *e);
static void single_icon_click_event(lv_event_t * e);
static void delete_icon_click_event(lv_event_t *e);
static void update_selected_count(void);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_NEXT,
    PAGE_SWITCH_ALBUM,
    PAGE_SWITCH_BACK
};

static lv_page_info_t album_page_delete = {
    .page_id = PAGE_FUNCTIONAL_ALBUM_DELETE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct
};

lv_page_info_pt lv_page_album_delete_get()
{
    return &album_page_delete;
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
    album_page_delete.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    lv_style_reset(&screen_style);
    lv_style_reset(&up_area_style);
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
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&album_delete_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&album_delete_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&album_delete_subject);
}

static void album_info_back_click_cb(lv_event_t *e)
{
    lv_subject_set_int(&album_delete_subject, PAGE_SWITCH_ALBUM);
}

static void lv_page_load(lv_obj_t *cont)
{
    lv_obj_add_style(cont, &screen_style, 0);

    create_video_list(cont);

    return;
}

static void create_video_list(lv_obj_t *cont)
{
    // 初始化选中状态数组
    if(item_selected)
    {
        free(item_selected);   
    }
    item_selected = (bool*)malloc(total_recod_cnt * sizeof(bool));
    memset(item_selected, 0, total_recod_cnt * sizeof(bool));

    // 创建滚动容器内部的内容容器
    lv_obj_t *content_cont = lv_obj_create(cont);
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
    lv_obj_t *up_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(up_indicator_area, 502, 156);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);

    lv_obj_t * photo_icon_single = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_icon_single, PHOTOGRAPH_ICON_SINGLE);
    lv_obj_set_size(photo_icon_single, 40, 40);
    lv_obj_align(photo_icon_single, LV_ALIGN_TOP_LEFT, 50, 20);

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

    // 删除按钮
    lv_obj_t *delete_icon = lv_img_create(up_indicator_area);
    lv_img_set_src(delete_icon, PHOTOGRAPH_ICON_TRASH_FILLED); // 替换为实际的删除图标路径
    lv_obj_set_size(delete_icon, 40, 40);
    lv_obj_align(delete_icon, LV_ALIGN_TOP_RIGHT, -50, 20);
    lv_obj_add_flag(delete_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(delete_icon, delete_icon_click_event, LV_EVENT_CLICKED, cont);
}

static void recreate_video_list(lv_obj_t *parent)
{
    // 删除所有子对象
    lv_obj_clean(parent);
    
    // 重新创建列表
    create_video_list(parent);
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

    lv_subject_set_int(&album_delete_subject, PAGE_SWITCH_NEXT);
}

// 删除图标点击事件
static void delete_icon_click_event(lv_event_t *e)
{
    lv_obj_t *parent = (lv_obj_t*)lv_event_get_user_data(e);
    delete_selected_items(parent);
}

static void single_icon_click_event(lv_event_t * e)
{
    // 处理点击事件的逻辑
    LV_LOG_USER("single icon clicked!");
    lv_subject_set_int(&album_delete_subject, PAGE_SWITCH_BACK);
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

static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    int32_t page_event = lv_subject_get_int(subject);
    LV_LOG_INFO("[%s:%d] -- page switch event:%d", __FILE__, __LINE__, page_event);
    if (page_event == PAGE_SWITCH_NONE) return;//注意首次触发
    
    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);
    switch_page->old_page = &album_page_delete;

    switch (page_event)
    {
        case PAGE_SWITCH_NEXT:
            switch_page->new_page = lv_page_album_four_grid_get();
            break;

        case PAGE_SWITCH_ALBUM:
            switch_page->new_page = lv_page_album_get();
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