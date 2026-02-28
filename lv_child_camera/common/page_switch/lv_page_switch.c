#include "../lv_page_info.h"

//页面切换主题
lv_subject_t switch_subject;
extern lv_obj_t *top_screen;
extern lv_obj_t *act_screen;
static lv_switch_page_pt switch_up = NULL;
static lv_switch_page_pt switch_bottom = NULL;
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
static LV_PAGE_TYPE_ENUM last_page_type = TYPE_MAX;
static lv_page_info_pt last_page = NULL;

static void anim_completed_cb(lv_anim_t *anim);

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
    lv_page_info_pt old_page = switch_page->old_page;

    LV_LOG_INFO("page switch begin");
    new_page->status = STATUS_RUNNNIG;
    new_page->construct_cb(new_page);

    //支持动画效果
    if (switch_page->anim_transt.anim_support)
    {
        if (lv_current_page_info_get()->page_id == PAGE_FUNCTIONAL_MENU_SETTING) {
            //设置菜单页位于最上层
            lv_obj_move_foreground(old_page->page);
        }
        lv_page_transition_anim_create(&switch_page->anim_transt, 
                                        anim_completed_cb, switch_page, 
                                        anim_completed_cb, switch_page);
    }
    else
    {
        lv_async_call(page_switch_delete_page, switch_page);
    }

    //记录当前页面信息
    lv_current_page_info_set(new_page->page_id, (void*)new_page);

    if (g_device_state == LV_PAGE_STAGE_RUNNING)
    {
        last_page = new_page;
        lv_obj_add_event_cb(new_page->page, page_gesture_event_hander, LV_EVENT_PRESSED, NULL);
        lv_obj_add_event_cb(new_page->page, page_gesture_event_hander, LV_EVENT_RELEASED, NULL);
    }

    return;
}

static void anim_completed_cb(lv_anim_t *anim)
{
    lv_switch_page_pt switch_page = lv_anim_get_user_data(anim);
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
    printf("===> gesture dir: %d, type:%d\n", dir, lv_page_type_get());

    switch (dir)
    {
        case LV_DIR_BOTTOM:
            //除功能页和菜单页下滑，其余皆忽略
            if (lv_page_type_get() != TYPE_FUNCTIONAL && lv_page_type_get() != TYPE_MENU) break;

            //显示全局设置页
            switch_bottom = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
            LV_ASSERT_MALLOC(switch_bottom);
            lv_memset(switch_bottom, 0, sizeof(lv_switch_page_t));
            switch_bottom->new_page = lv_page_menu_setting_info_get();
            switch_bottom->old_page = last_page;
            lv_stack_push(last_page);

            //动画参数设置
            switch_bottom->anim_transt.anim_support = true;
            switch_bottom->anim_transt.old_type = LV_ANIM_BOX_NONE;
            switch_bottom->anim_transt.new_type = LV_ANIM_BOX_SLIDE;
            lv_transition_anim_slide_param_set(
                    &switch_bottom->anim_transt.new_params,
                    &switch_bottom->new_page->page,
                    250,
                    LV_DIR_BOTTOM);
            lv_subject_set_pointer(&switch_subject, switch_bottom);
            break;
        case LV_DIR_TOP:
            //除设置菜单底部上滑退出，其余皆忽略
            if (lv_page_type_get() != TYPE_MENU_SETTING_ONE) break;

            switch_up = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
            LV_ASSERT_MALLOC(switch_up);
            lv_memset(switch_up, 0, sizeof(lv_switch_page_t));
            switch_up->new_page = lv_stack_pop();
            switch_up->old_page = lv_page_menu_setting_info_get();

            if (switch_up->new_page->page_id != TYPE_MENU) {
                lv_page_type_set(TYPE_FUNCTIONAL);
            }

            //动画参数设置
            switch_up->anim_transt.anim_support = true;
            switch_up->anim_transt.old_type = LV_ANIM_BOX_SLIDE;
            switch_up->anim_transt.new_type = LV_ANIM_BOX_NONE;
            lv_transition_anim_slide_param_set(
                    &switch_up->anim_transt.old_params,
                    &switch_up->old_page->page,
                    250,
                    LV_DIR_TOP);
            lv_subject_set_pointer(&switch_subject, switch_up);
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
        if (touch_state.is_pressed != true) return 0;

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
        if ((dir == LV_DIR_BOTTOM && touch_state.start_y > 50) || (dir == LV_DIR_TOP && touch_state.start_y < 250))
        {
            printf("start_y: %d\n", touch_state.start_y);
            return 0;
        }

        lv_memset(&touch_state, 0, sizeof(lv_page_move_area_t));
        return dir;
    }

    return 0;
}