#include "../lv_page_info.h"

//页面切换主题
lv_subject_t switch_subject;
extern lv_obj_t *top_screen;
extern lv_obj_t *act_screen;
static lv_switch_page_pt switch_menu = NULL;
static lv_switch_page_pt switch_menu_setting = NULL;
extern lv_page_info_pt lv_page_menu_setting_info_get();
extern lv_page_info_pt lv_page_menu_info_get();

//不添加屏幕手势过滤列表
static uint32_t gesture_disable_list[] = {};
//全屏幕手势事件注册回调标志
static bool flag_init = false;

//设备当前状态
static LV_PAGE_STAGE_ENUM g_device_state = LV_PAGE_STAGE_ADDING;
static LV_PAGE_TYPE_ENUM last_page_type = TYPE_NONE;
static lv_page_info_pt last_page = NULL;

/// @brief 页面切换观察者回调函数
static void page_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
/// @brief 清楚旧的页面回调函数
static void page_switch_delete_page(void *param);
/// @brief 添加页面手势事件过滤
static bool page_add_gesture_event_filter(lv_page_info_pt newpage);
/// @brief 页面手势事件回调函数
static void page_gesture_event_cb(lv_event_t *e);
/// @brief 清除页面上的所有事件对象
static void lv_delete_all_event_obj(lv_obj_t *obj);

void lv_dev_stage_set(uint8_t stage) { g_device_state = stage; }
uint8_t lv_dev_stage_get(void) { return g_device_state; }

void lv_page_type_set(int32_t type) { last_page_type = type; }
int32_t lv_page_type_get() { return last_page_type; }

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

    if (g_device_state == LV_PAGE_STAGE_RUNNING)
    {
        if (!flag_init) {
            flag_init = true;
            lv_obj_add_event_cb(act_screen, page_gesture_event_cb, LV_EVENT_GESTURE, NULL);
        }

        if (new_page != lv_page_menu_info_get() && new_page != lv_page_menu_setting_info_get())
        {
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

    uint8_t count = sizeof(gesture_disable_list) / sizeof(uint32_t);
    for (uint32_t i = 0; i < count; i++)
    {
        if (gesture_disable_list[i] == newpage->page_id) {
            result = true;
            break;
        }
    }

    return result;
}

static void page_gesture_event_cb(lv_event_t *e)
{
    static bool menu_setting_flag = false;
    static bool menu_flag = true;

    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_indev_wait_release(lv_indev_get_act());

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
            else if (lv_page_type_get() == TYPE_MENU)
            {
                if (lv_page_menu_info_get()->reserved != NULL)
                {
                    lv_page_info_pt last_page = lv_stack_pop();
                    lv_obj_clear_flag(last_page->page, LV_OBJ_FLAG_HIDDEN);
                }

                //删除菜单页
                lv_page_menu_info_get()->destruct_cb();
                lv_page_menu_info_get()->reserved = NULL;
                lv_obj_del(lv_page_menu_info_get()->page);
                lv_page_type_set(TYPE_FUNCTIONAL);//回到功能页(不可少)
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
                lv_stack_push(last_page);
                lv_obj_add_flag(last_page->page, LV_OBJ_FLAG_HIDDEN);

                //显示菜单页
                switch_menu = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                LV_ASSERT_MALLOC(switch_menu);
                lv_memset(switch_menu, 0, sizeof(lv_switch_page_t));
                switch_menu->new_page = lv_page_menu_info_get();
                switch_menu->new_page->reserved = last_page;
                lv_subject_set_pointer(&switch_subject, switch_menu);
                lv_page_type_set(TYPE_MENU);
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