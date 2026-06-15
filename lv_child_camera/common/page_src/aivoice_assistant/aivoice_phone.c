#include "../lv_switch_interface.h"
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

lv_subject_t aiphone_subject;
static lv_switch_page_pt switch_page;

static enum {
    STATUS_CONNECTING,
    STATUS_SPEAKER,
    STATUS_LISTEN,
    STATUS_INTERRUPT,
};

static lv_obj_t *screen = NULL;
static lv_obj_t *bottom_label = NULL;
static lv_obj_t *bottom_lottie = NULL;
static lv_timer_t *update_timer = NULL;
static lv_style_t screen_style;
static lv_style_t style_mask;
static uint8_t *lottie_buffer = NULL;
static int32_t last_steps = -1;
static int32_t current_steps = STATUS_CONNECTING;

static void lv_page_construct(void *this);
static void lv_page_destruct(void);
static void lv_page_style_init();
static void lv_page_subject_init();
static void lv_page_subject_deinit();
static void lv_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void lv_page_load(lv_obj_t *cont);
static void hangup_click_event_cb(lv_event_t *e);
static void aivoice_status_test_input(lv_timer_t *timer);
static void aivoice_status_update_timer(lv_timer_t *timer);

static const char *phone_lottie[] = {
    "../lv_port_pc_vscode/assert/lottie/dialog_loading.json",
    "../lv_port_pc_vscode/assert/lottie/dialog_speak.json",
    "../lv_port_pc_vscode/assert/lottie/dialog_speak.json",
    NULL
};

static const char *phone_status[] = {
    "连接中...",
    "你可以开始说话了",
    "正在听...",
    "说话或点击打断",
};

//待跳转的页面种类
static enum PAGE_EVENT_ENUM
{
    PAGE_SWITCH_NONE,
    PAGE_SWITCH_BACK
};

static lv_page_info_t aiphone_page_info = {
    .page_id = PAGE_FUNCTIONAL_AIPHONE,
    .page = NULL,
    .reserved = NULL,
    .construct_cb = lv_page_construct,
    .destruct_cb = lv_page_destruct,
};

lv_page_info_pt lv_page_aiphone_get()
{
    return &aiphone_page_info;
}

static void lv_page_construct(void *this)
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

    if (lottie_buffer) 
    {
        lv_free(lottie_buffer);
        lottie_buffer = NULL;
    }
    //绘制当前页面
    lv_page_load(screen);
    aiphone_page_info.page = screen;
    return;
}

static void lv_page_destruct(void)
{
    if (lottie_buffer) {
        lv_free(lottie_buffer);
        lottie_buffer = NULL;
    }
    
    if (update_timer) {
        lv_timer_del(update_timer);
        update_timer = NULL;
    }

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
}

static void lv_page_subject_init()
{
    lv_subject_init_int(&aiphone_subject, PAGE_SWITCH_NONE);
    lv_subject_add_observer(&aiphone_subject, lv_switch_observer_cb, NULL);
    return;
}

static void lv_page_subject_deinit()
{
    lv_subject_deinit(&aiphone_subject);
}

static void lv_page_load(lv_obj_t *cont)
{
    //挂断按钮
    lv_obj_t *hangup = lv_img_create(cont);
    lv_obj_set_size(hangup, 100, 100);
    lv_img_set_src(hangup, "../lv_port_pc_vscode/assert/icon/ai_photo_hangup.png");
    lv_img_set_zoom(hangup, 128);
    lv_obj_add_flag(hangup, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(hangup, LV_ALIGN_BOTTOM_RIGHT, -25, -13);
    lv_obj_add_event_cb(hangup, hangup_click_event_cb, LV_EVENT_CLICKED, NULL);

    //创建文案显示
    bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, phone_status[STATUS_CONNECTING]);
    lv_obj_set_style_text_font(bottom_label, fzlthr_26, 0);
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(bottom_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -45);

    last_steps = -1;
    current_steps = STATUS_CONNECTING;

    //创建lottie动画
    bottom_lottie = lv_lottie_create(cont);
    lv_obj_align(bottom_lottie, LV_ALIGN_CENTER, 0, 0);

    //创建定时器自动更新
    update_timer = lv_timer_create(aivoice_status_update_timer, 500, &last_steps);
    lv_timer_set_auto_delete(update_timer, false);
    lv_timer_ready(update_timer);

    lv_timer_create(aivoice_status_test_input, 500, NULL);
    return;
}

static void aivoice_status_test_input(lv_timer_t *timer)
{
    static struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    int ret = poll(fds, 1, 0);

    if (ret > 0 && (fds[0].revents & POLLIN)) {
        char buffer[256];
        fgets(buffer, sizeof(buffer), stdin);
        printf("收到输入: %s", buffer);
        current_steps = atoi(buffer);
    }
}

static void aivoice_status_update_timer(lv_timer_t *timer)
{
    if (NULL == screen || !lv_obj_is_valid(screen)) return;

    int8_t *p_laststep = lv_timer_get_user_data(timer);

    if (*p_laststep != current_steps)
    {
        *p_laststep = current_steps;
        LV_LOG_WARN("aivoice_status_update curr: %d", current_steps);
        if (lottie_buffer) {
            lv_free(lottie_buffer);
            lottie_buffer = NULL;
        }

        switch (current_steps)
        {
            case STATUS_CONNECTING:
            {
                lottie_buffer = lv_calloc(1, 90*90*4);
                lv_lottie_set_buffer(bottom_lottie, 90, 90, lottie_buffer);
                lv_lottie_set_src_file(bottom_lottie, phone_lottie[STATUS_CONNECTING]);
                lv_obj_align(bottom_lottie, LV_ALIGN_CENTER, 0, 0);
                lv_label_set_text(bottom_label, phone_status[STATUS_CONNECTING]);
                break;
            }
            case STATUS_SPEAKER:
            {
                lottie_buffer = lv_calloc(1, 120*60*4);
                lv_lottie_set_buffer(bottom_lottie, 120, 60, lottie_buffer);
                lv_lottie_set_src_file(bottom_lottie, phone_lottie[STATUS_SPEAKER]);
                lv_obj_align(bottom_lottie, LV_ALIGN_CENTER, 0, 0);
                lv_label_set_text(bottom_label, phone_status[STATUS_SPEAKER]);
                break;
            }
            case STATUS_LISTEN:
            {
                lottie_buffer = lv_calloc(1, 120*60*4);
                lv_lottie_set_buffer(bottom_lottie, 120, 60, lottie_buffer);
                lv_lottie_set_src_file(bottom_lottie, phone_lottie[STATUS_LISTEN]);
                lv_obj_align(bottom_lottie, LV_ALIGN_CENTER, 0, 0);
                lv_label_set_text(bottom_label, phone_status[STATUS_LISTEN]);
                break;
            }
            case STATUS_INTERRUPT:
            {
                lv_label_set_text(bottom_label, phone_status[STATUS_INTERRUPT]);
                break;
            }
            default:
                break;
        }
    }
}

static void message_box_event_cb(lv_event_t *e)
{
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    lv_obj_clean(top_screen);

    if (index == 1)
    {
        //1、关闭相关业务处理
        
        lv_subject_set_int(&aiphone_subject, PAGE_SWITCH_BACK);
    }
}

static void message_box_customized(const char *text, lv_event_cb_t event_cb)
{
    lv_obj_t *blocker = lv_obj_create(top_screen);
    lv_obj_set_size(blocker, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(blocker, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(blocker, LV_OPA_80, 0);
    lv_obj_set_style_pad_all(blocker, 0, 0);
    lv_obj_set_style_border_width(blocker, 0, 0);
    lv_obj_set_style_shadow_width(blocker, 0, 0);
    lv_obj_clear_flag(blocker, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(blocker);

    //设置文字到主菜单
    lv_obj_t *label = lv_label_create(blocker);
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_opa(label, LV_OPA_90, 0);
    lv_obj_set_style_text_font(label, fzlthr_30, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0XFFFFFF), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 145);

    lv_obj_t *cancel_btn = lv_obj_create(blocker);
    lv_obj_set_size(cancel_btn, 148, 70);
    lv_obj_set_style_radius(cancel_btn, 51, 0);
    lv_obj_set_style_bg_opa(cancel_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cancel_btn, lv_color_hex(0x2C2C2E), 0);
    lv_obj_set_style_border_width(cancel_btn, 0, 0);
    lv_obj_clear_flag(cancel_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cancel_btn, LV_ALIGN_TOP_LEFT, 78, 310);
    lv_obj_add_event_cb(cancel_btn, event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)0);

    lv_obj_t *cancel = lv_img_create(cancel_btn);
    lv_img_set_src(cancel, "../lv_port_pc_vscode/assert/icon/common_icon_cancel_button.png");
    lv_obj_set_size(cancel, 50, 50);
    lv_obj_align(cancel, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *confirm_btn = lv_obj_create(blocker);
    lv_obj_set_size(confirm_btn, 148, 70);
    lv_obj_set_style_radius(confirm_btn, 51, 0);
    lv_obj_set_style_opa(confirm_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(confirm_btn, lv_color_hex(0xAFF99C), 0);
    lv_obj_set_style_border_width(confirm_btn, 0, 0);
    lv_obj_clear_flag(confirm_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(confirm_btn, cancel_btn, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
    lv_obj_add_event_cb(confirm_btn, event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)1);

    lv_obj_t *confirm = lv_img_create(confirm_btn);
    lv_img_set_src(confirm, "../lv_port_pc_vscode/assert/icon/common_icon_ok_button.png");
    lv_obj_set_size(confirm, 50, 50);
    lv_obj_align(confirm, LV_ALIGN_CENTER, 0, 0);
}

static void hangup_click_event_cb(lv_event_t *e)
{
    //先判断是否已经接通

    //二次确认是否挂断
    message_box_customized("确定挂断当前通话吗？", message_box_event_cb);
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
    switch_page->old_page = &aiphone_page_info;

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
