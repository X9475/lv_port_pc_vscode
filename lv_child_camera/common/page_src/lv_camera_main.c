#include "lv_camera_main.h"

lv_obj_t *top_screen = NULL;
lv_obj_t *act_screen = NULL;
static bool g_signup_flag = false;
static lv_switch_page_pt switch_page;

void lv_camera_init()
{
    //字体管理初始化
    lv_font_init();
    //栈列表初始化
    lv_stack_list_init();
    //页面切换主题初始化
    page_switch_subject_init();
    //toast主题初始化
    lv_toast_page_subject_init();
    //abnormal主题初始化
    lv_anormal_page_subject_init();
    //创建顶层屏幕
    top_screen = lv_layer_top();
    //创建活动屏幕
    act_screen = lv_screen_active();
    //lv_obj_set_style_bg_opa(act_screen, LV_OPA_TRANSP, 0);

    return;
}

void lv_camera_deinit()
{
    lv_font_deinit();
    return;
}

void lv_main()
{
    switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
    lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
    LV_ASSERT_MALLOC(switch_page);

    if (lv_dev_stage_get() == LV_PAGE_STAGE_ADDING)
    {//添加流程阶段
        switch_page->new_page = lv_page_screenlock_info_get();
        // switch_page->new_page = lv_page_about_camera_info_get();
        // switch_page->new_page = lv_page_shooting_photo_get();
        // switch_page->new_page = lv_page_album_get();
    }
    else if (lv_dev_stage_get() == LV_PAGE_STAGE_RUNNING)
    {//正式运行阶段
        switch_page->new_page = lv_page_menu_info_get();
    }

    lv_subject_set_pointer(&switch_subject, switch_page);

    // lv_subject_set_int(&toast_subject, PAGE_TOAST_FORMAT_DOING);
    return;
}