#include "lv_camera_main.h"

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

    return;
}

void lv_camera_deinit()
{
    lv_font_deinit();
    return;
}

void lv_main()
{
    ///展示二维码
    if (lv_dev_stage_get() == LV_PAGE_STAGE_ADDING)
    {
        switch_page = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
        lv_memset(switch_page, 0, sizeof(lv_switch_page_t));
        LV_ASSERT_MALLOC(switch_page);

        switch_page->new_page = lv_page_qrcode_info_get();
        // switch_page->new_page = lv_page_menu_info_get();
        lv_subject_set_pointer(&switch_subject, switch_page);
    }

    return;
}