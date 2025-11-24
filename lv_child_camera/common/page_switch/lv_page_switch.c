#include "../lv_page_info.h"

//页面切换主题
lv_subject_t switch_subject;
extern lv_obj_t *top_screen;
extern lv_obj_t *act_screen;
static lv_switch_page_pt switch_menu = NULL;
static lv_switch_page_pt switch_menu_setting = NULL;
extern lv_page_info_pt lv_page_menu_setting_info_get();
extern lv_page_info_pt lv_page_menu_info_get();

//记录当前显示的页面信息
static lv_page_info_current_t g_current_page_info = {PAGE_NONE, NULL};

//不添加屏幕手势过滤列表
static uint32_t gesture_disable_list[] = {
    PAGE_FUNCTIONAL_ALBUM,
    PAGE_FUNCTIONAL_ALBUM_DELETE,
    PAGE_FUNCTIONAL_ALBUM_FOUR_GRID
};

//设备当前状态
static LV_PAGE_STAGE_ENUM g_device_state = LV_PAGE_STAGE_ADDING;
static LV_PAGE_TYPE_ENUM last_page_type = TYPE_NONE;
static lv_page_info_pt last_page = NULL;

/// @brief 页面切换观察者回调函数
static void page_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
/// @brief 清楚旧的页面回调函数
static void page_switch_delete_page(void *param);
/// @brief 添加页面手势事件过滤，false表示不过滤
static bool page_add_gesture_event_filter(lv_page_info_pt newpage);
/// @brief 页面滑动事件处理
static void page_gesture_event_hander(lv_event_t *e);
/// @brief 清除页面上的所有事件对象
static void lv_delete_all_event_obj(lv_obj_t *obj);
/// @brief 屏幕滑动方向计算
static int lv_gesture_diraction_judgement(lv_event_t *e);

void lv_dev_stage_set(uint8_t stage) { g_device_state = stage; }
uint8_t lv_dev_stage_get(void) { return g_device_state; }

void lv_page_type_set(int32_t type) { last_page_type = type; }
int32_t lv_page_type_get() { return last_page_type; }

lv_page_info_current_pt lv_current_page_info_get() { return &g_current_page_info; }
void lv_current_page_info_set(uint32_t id, void *ptr)
{
    g_current_page_info.page_id = id;
    g_current_page_info.current_page = ptr;
}

void page_switch_subject_init()
{
    lv_subject_init_pointer(&switch_subject, NULL);
    lv_subject_add_observer(&switch_subject, page_switch_observer_cb, NULL);
    return;
}

static void page_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    lv_switch_page_pt switch_page = lv_subject_get_pointer(subject);
    if (NULL == switch_page) return;

    lv_page_info_pt new_page = switch_page->new_page;

    LV_LOG_INFO("page switch begin");
    new_page->status = STATUS_RUNNNIG;
    new_page->construct_cb(new_page);

    //记录当前页面信息
    lv_current_page_info_set(new_page->page_id, (void*)new_page);

    if (g_device_state == LV_PAGE_STAGE_RUNNING)
    {
        lv_obj_add_event_cb(new_page->page, page_gesture_event_hander, LV_EVENT_PRESSED, NULL);
        lv_obj_add_event_cb(new_page->page, page_gesture_event_hander, LV_EVENT_RELEASED, NULL);

        if (new_page != lv_page_menu_info_get() && new_page != lv_page_menu_setting_info_get())
        {
            //特殊处理，上下边缘不添加触发区域
            if (!page_add_gesture_event_filter(new_page))
            {
                //顶部滑动触发区域
                lv_obj_t *top_gesture_area = lv_obj_create(new_page->page);
                lv_obj_set_size(top_gesture_area, 250, 30);
                lv_obj_align(top_gesture_area, LV_ALIGN_TOP_MID, 0, 0);
                lv_obj_set_style_border_opa(top_gesture_area, LV_OPA_TRANSP, 0);
                lv_obj_set_style_bg_opa(top_gesture_area, LV_OPA_TRANSP, 0);
                lv_obj_add_flag(top_gesture_area, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_clear_flag(top_gesture_area, LV_OBJ_FLAG_SCROLLABLE);

                //底部滑动触发区域
                lv_obj_t *bottom_gesture_area = lv_obj_create(new_page->page);
                lv_obj_set_size(bottom_gesture_area, 250, 30);
                lv_obj_align(bottom_gesture_area, LV_ALIGN_BOTTOM_MID, 0, 0);
                lv_obj_set_style_border_opa(bottom_gesture_area, LV_OPA_TRANSP, 0);
                lv_obj_set_style_bg_opa(bottom_gesture_area, LV_OPA_TRANSP, 0);
                lv_obj_add_flag(bottom_gesture_area, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_clear_flag(bottom_gesture_area, LV_OBJ_FLAG_SCROLLABLE);
            }

            last_page = new_page;
            lv_page_type_set(TYPE_FUNCTIONAL);
            // printf("===> page type: %d\n", lv_page_type_get());
        }
    }

    lv_refr_now(NULL);
    lv_async_call(page_switch_delete_page, switch_page);
    return;
}

static void page_switch_delete_page(void *param)
{
    lv_switch_page_pt switch_page = (lv_switch_page_pt)param;
    lv_page_info_pt old_page = switch_page->old_page;

    if (NULL != old_page)
    {
        printf("[%s:%d] ==> delete id: %d\n", __FILE__, __LINE__, old_page->page_id);
        old_page->destruct_cb();
        lv_delete_all_event_obj(old_page->page);
        lv_obj_del(old_page->page);
    }

    lv_free(switch_page);
    switch_page = NULL;
    return;
}

static void lv_delete_all_event_obj(lv_obj_t *obj)
{
    LV_ASSERT_NULL(obj);

    uint32_t event_cnt = lv_obj_get_event_count(obj);
    for (uint32_t i = 0; i < event_cnt; i++)
    {
        lv_obj_remove_event(obj, i);
    }

    return;
}

static bool page_add_gesture_event_filter(lv_page_info_pt newpage)
{
    bool result = false;
    if (NULL == newpage) return result;

    uint32_t count = sizeof(gesture_disable_list) / sizeof(uint32_t);
    for (uint32_t i = 0; i < count; i++)
    {
        if (gesture_disable_list[i] == newpage->page_id) {
            result = true;
            break;
        }
    }

    return result;
}

static void page_gesture_event_hander(lv_event_t *e)
{
    static bool menu_setting_flag = false;
    static bool menu_flag = true;

    // lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    // lv_indev_wait_release(lv_indev_get_act());

    //特殊处理，一些页面自带点击滑动属性，过滤
    lv_page_info_current_pt pcurrent = lv_current_page_info_get();
    if (NULL != pcurrent->current_page)
    {
        lv_page_info_pt current = pcurrent->current_page;
        if (page_add_gesture_event_filter(current))
        {
            printf("===> id: %d, page_gesture_event_hander not allowed.\n", current->page_id);
            return;
        }
    }

    int dir = lv_gesture_diraction_judgement(e);
    if (dir == 0) return;
    printf("===> gesture dir: %d\n", dir);

    if (lv_page_type_get() == TYPE_MENU && dir == LV_DIR_TOP) return;
    if (lv_page_type_get() == TYPE_MENU_SETTING && dir == LV_DIR_BOTTOM) return;

    switch (dir)
    {
        case LV_DIR_BOTTOM:
            if (lv_page_type_get() == TYPE_NONE && menu_setting_flag == true) break;

            if (lv_page_type_get() == TYPE_FUNCTIONAL)
            {
                lv_stack_push(last_page);
                lv_obj_add_flag(last_page->page, LV_OBJ_FLAG_HIDDEN);

                //显示全局设置页
                switch_menu_setting = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                LV_ASSERT_MALLOC(switch_menu_setting);
                lv_memset(switch_menu_setting, 0, sizeof(lv_switch_page_t));
                switch_menu_setting->new_page = lv_page_menu_setting_info_get();
                switch_menu_setting->new_page->reserved = last_page;
                lv_subject_set_pointer(&switch_subject, switch_menu_setting);
                lv_page_type_set(TYPE_MENU_SETTING);
            }
            else if (lv_page_type_get() == TYPE_NONE && menu_flag == true)
            {
                menu_setting_flag = true;
                menu_flag = false;

                //显示全局设置页
                switch_menu_setting = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                LV_ASSERT_MALLOC(switch_menu_setting);
                lv_memset(switch_menu_setting, 0, sizeof(lv_switch_page_t));
                switch_menu_setting->new_page = lv_page_menu_setting_info_get();
                switch_menu_setting->old_page = lv_page_menu_info_get();
                lv_subject_set_pointer(&switch_subject, switch_menu_setting);
            }
            break;
        case LV_DIR_TOP:
            if (lv_page_type_get() == TYPE_NONE && menu_flag == true) break;

            if (lv_page_type_get() == TYPE_FUNCTIONAL)
            {
                //显示菜单页
                switch_menu = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                LV_ASSERT_MALLOC(switch_menu);
                lv_memset(switch_menu, 0, sizeof(lv_switch_page_t));
                switch_menu->new_page = lv_page_menu_info_get();
                switch_menu->old_page = last_page;
                lv_subject_set_pointer(&switch_subject, switch_menu);

                lv_page_type_set(TYPE_NONE);//全局设置页和菜单页循环切
                menu_flag = true;
                menu_setting_flag = false;
            }
            else if (lv_page_type_get() == TYPE_MENU_SETTING)
            {
                if (lv_page_menu_setting_info_get()->reserved != NULL)
                {
                    lv_page_info_pt last_page = lv_stack_pop();
                    lv_obj_clear_flag(last_page->page, LV_OBJ_FLAG_HIDDEN);
                    //删除全局设置页面
                    lv_page_menu_setting_info_get()->destruct_cb();
                    lv_page_menu_setting_info_get()->reserved = NULL;
                    lv_obj_del(lv_page_menu_setting_info_get()->page);
                    lv_page_type_set(TYPE_FUNCTIONAL);//回到功能页
                }
                else
                {//之前从全局设置页跳走过，显示菜单页
                    switch_menu = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                    LV_ASSERT_MALLOC(switch_menu);
                    lv_memset(switch_menu, 0, sizeof(lv_switch_page_t));
                    switch_menu->new_page = lv_page_menu_info_get();
                    switch_menu->old_page = lv_page_menu_setting_info_get();
                    lv_subject_set_pointer(&switch_subject, switch_menu);
                    lv_page_type_set(TYPE_NONE);
                    menu_flag = true;
                    menu_setting_flag = false;
                }
            }
            else if (lv_page_type_get() == TYPE_NONE && menu_setting_flag == true)
            {
                menu_flag = true;
                menu_setting_flag = false;

                //显示菜单页
                switch_menu = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                LV_ASSERT_MALLOC(switch_menu);
                lv_memset(switch_menu, 0, sizeof(lv_switch_page_t));
                switch_menu->new_page = lv_page_menu_info_get();
                switch_menu->old_page = lv_page_menu_setting_info_get();
                lv_subject_set_pointer(&switch_subject, switch_menu);
            }
            break;
        default:
            break;
    }
}

static int lv_gesture_diraction_judgement(lv_event_t *e)
{
    static lv_page_move_area_t touch_state = {0};

    //消抖时间阈值毫秒
    const uint32_t DEBOUNCE_TIME = 50;
    const lv_coord_t DISTANCE_THRESHOLD = 30;

    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED)
    {
        lv_point_t point;
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        //左右边缘120范围内禁止响应
        if (point.x < 120 || point.x > 382) return 0;

        touch_state.start_y = point.y;
        touch_state.start_time = lv_tick_get();
        touch_state.is_pressed = true;
        printf("=======>1\n");
        return 0;
    }

    if (code == LV_EVENT_RELEASED)
    {
        lv_point_t point;
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        touch_state.end_y = point.y;
        //如果前一个没有按下，此处不处理
        if (touch_state.is_pressed != true) return;

        printf("=======>2\n");
        //计算时间差和距离差
        const uint32_t elapsed = lv_tick_elaps(touch_state.start_time);
        const lv_coord_t delta = touch_state.end_y - touch_state.start_y;

        //消抖判断
        if (elapsed < DEBOUNCE_TIME || LV_ABS(delta) < DISTANCE_THRESHOLD) {
            touch_state.is_pressed = false;
            printf("elapsed: %d, delta: %d\n", elapsed, delta);
            return 0;
        }

        lv_dir_t dir = (delta > 0) ? LV_DIR_BOTTOM : LV_DIR_TOP;
        if ((dir == LV_DIR_BOTTOM && touch_state.start_y > 50) || (dir == LV_DIR_TOP && touch_state.start_y < 360))
        {
            printf("start_y: %d\n", touch_state.start_y);
            return 0;
        }

        return dir;
    }

    return 0;
}