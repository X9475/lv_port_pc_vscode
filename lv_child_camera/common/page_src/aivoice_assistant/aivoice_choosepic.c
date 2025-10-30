#include "../lv_switch_interface.h"

lv_subject_t choosepic_adj_param_subject;
static lv_switch_page_pt switch_page;

static void lv_page_construct(void);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_page_load(lv_obj_t *cont);
extern lv_obj_t *picextend_item_cont;
static lv_obj_t *screen = NULL;
static lv_obj_t *dialog_obj;
static lv_obj_t *selected_count_label = NULL;

static lv_style_t screen_style;
static lv_style_t style_mask;
static lv_style_t style_mask1;
static lv_style_t style_mask2;
static lv_style_t up_area_style;

static bool *item_selected = NULL; // 记录每个项目的选中状态
static int total_recod_cnt = 8; //todo ：获取实际的录像个数
static int selected_count = 0;
// 选中项目事件处理
static bool selection_mode = false;
static void select_item_event(lv_event_t *e);
static void single_icon_click_event(lv_event_t * e);


static int get_item_index(lv_obj_t *item_cont);
static void update_selected_count(void);
// 更新项目选中状态UI
static void update_item_selection_ui(lv_obj_t *item_cont, bool selected);

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t aichoosepic_page_info = {
    .page_id = PAGE_PHOTO_SELETE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_choosepic_param_get()
{
    return &aichoosepic_page_info;
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

    aichoosepic_page_info.page = screen;
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
    grad3.stops[0].opa = LV_OPA_90;
    grad3.stops[1].color = lv_color_hex(0x000000);
    grad3.stops[1].opa = LV_OPA_TRANSP;
    grad3.stops[0].frac = 0;
    grad3.stops[1].frac = 255;
    lv_style_init(&up_area_style);
    lv_style_copy(&up_area_style, &screen_style);
    lv_style_set_bg_grad(&up_area_style, &grad3);
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&choosepic_adj_param_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&choosepic_adj_param_subject, lv_switch_observer_cb, NULL);
    return;
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
    switch_page->old_page = &aichoosepic_page_info;

    switch (page_event)
    {
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
    lv_subject_deinit(&choosepic_adj_param_subject);
}

static const char *png = "../lv_port_pc_vscode/assert/icon/screen_saver.png";
static void lv_page_load(lv_obj_t *cont)
{
    /*****************************test**********/
    lv_page_info_pt page_info = lv_page_aidialog_info_get();
    page_info->reserved = png;

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
            lv_img_set_src(check_icon, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
            lv_obj_set_size(check_icon, 40, 40);
            lv_obj_align(check_icon, LV_ALIGN_TOP_LEFT, 188, 144);
            lv_obj_add_flag(check_icon, LV_OBJ_FLAG_HIDDEN);

            // 将图标指针存储到用户数据中
            lv_obj_set_user_data(item_cont, check_icon);

            // 添加事件处理
            lv_obj_add_flag(item_cont, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(item_cont, select_item_event, LV_EVENT_ALL, NULL);
        }
    }

    // 创建顶部区域
    lv_obj_t *up_indicator_area = lv_obj_create(cont);
    lv_obj_set_size(up_indicator_area, 502, 156);
    lv_obj_align(up_indicator_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(up_indicator_area, &up_area_style, 0);
    
    // 添加返回按钮
    lv_obj_t * photo_backup = lv_img_create(up_indicator_area);
    lv_img_set_src(photo_backup, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_set_size(photo_backup, 40, 40);
    lv_obj_align(photo_backup, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(photo_backup, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photo_backup, single_icon_click_event, LV_EVENT_CLICKED, NULL);

    // 添加所有照片标题
    lv_obj_t *title_label = lv_label_create(cont);
    lv_label_set_text(title_label, "所有照片");
    lv_obj_set_style_text_opa(title_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(title_label, fzlthr_28, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title_label, LV_ALIGN_OUT_TOP_LEFT, 70, 25);
    // lv_obj_align_to(title_label, photo_backup, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    
}
static void single_icon_click_event(lv_event_t * e)
{
    lv_subject_set_int(&choosepic_adj_param_subject, PAGE_SWITCH_BACK);
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
static void update_selected_count(void)
{
    if(selected_count_label) 
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "已选择%d", selected_count);
        lv_label_set_text(selected_count_label, buf);
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