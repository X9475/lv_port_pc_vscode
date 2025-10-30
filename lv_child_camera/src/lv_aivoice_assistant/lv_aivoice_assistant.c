/**
 * @file lv_aivoice_assistant.c
 * @author your name (you@domain.com)
 * @brief AI语音助手UI源文件
 * @version 0.1
 * @date 2025-09-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_aivoice_assistant.h"
#include <stdbool.h>

#if (LV_CHILD_CAMERA != 0) && (LV_CHILD_CAMERA_AIVOICE_ASSISTANT != 0)

#define PHOTOGRAPH_ICON_BACK "V:tk1/realtime_shooting/common_icon_back.png"
#define PHOTOGRAPH_ICON_UNSELECT "V:tk1/realtime_shooting/photograph_icon_unselect.png"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void style_init();
static void lv_page_open();
static void lv_page_close();
// ai对话备份
static void lv_aivoice_dialog_Backup2(lv_obj_t *cont);
// 智能体列表
static void lv_aivoice_agentList(lv_obj_t *cont);
// ai对话
static void lv_aivoice_dialog(lv_obj_t *cont);
// ai对话网络异常
static void lv_aivoice_dialog_network_abnormal(lv_obj_t *cont);
// ai按键说话
static void lv_aivoice_pressbutton_speak(lv_obj_t *cont);
// ai对话松手取消
static void lv_aivoice_nobutton_cancelspeak(lv_obj_t *cont);
// 说完loading
static void lv_aivoice_sepak_finish(lv_obj_t *cont);
//添加-选照片/照片选择/照片放大
static void lv_aivoice_photo_selection(lv_obj_t *cont);
// 通话连接中
static void lv_aivoice_call_connecting(lv_obj_t *cont);
// 通话连接成功
static void lv_aivoice_call_connected(lv_obj_t *cont);
// 通话听到声音
static void lv_aivoice_call_listening(lv_obj_t *cont);
// 通话回复问题
static void lv_aivoice_call_answer_question(lv_obj_t *cont);
// 通话开启静音模式
static void lv_aivoice_call_muted(lv_obj_t *cont);

static bool *item_selected = NULL; // 记录每个项目的选中状态
static int total_recod_cnt = 8; //todo ：获取实际的录像个数
static int selected_count = 0;

static void lv_back_button_event(lv_event_t *e);
static void lv_agent_button_event(lv_event_t *e);
static void lv_camera_button_event(lv_event_t *e);
static void lv_voice_button_event(lv_event_t *e);
static void lv_iphone_button_event(lv_event_t *e);
static void lv_voice_cancel_button_event(lv_event_t *e);
static void lv_voice_pressbut_event(lv_event_t *e);
static void lv_camera_hangupbut_event(lv_event_t *e);
static void single_icon_click_event(lv_event_t * e);
static void lv_itemcont_picture_event(lv_event_t * e);
static void lv_quit_button_event(lv_event_t *e);
// 选中项目事件处理
static void select_item_event(lv_event_t *e);
static bool selection_mode = false;
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *ai_dialog_page;
static lv_obj_t *cancel_image1;
static lv_obj_t *pressbut_image1;
static lv_obj_t *selected_count_label = NULL;
static lv_obj_t *picextend_item_cont;
static lv_obj_t *dialog_obj;
static lv_style_t style;
static lv_style_t style_mask;
static lv_style_t style_mask1;
static lv_style_t style_mask2;
static lv_style_t up_area_style;
static const lv_font_t *font_26;
static const lv_font_t *font_28;
static const lv_font_t *font_30;
static const lv_font_t *font_30B;
static const lv_font_t *font_32;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_child_camera()
{
    lv_font_init();
    lv_page_open();
}

/**********************
 *  STATIC FUNCTIONS
 **********************/
static void style_init()
{
    lv_style_init(&style);
    lv_style_set_radius(&style, 0);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_border_width(&style, 0);
    lv_style_set_bg_color(&style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&style, LV_OPA_COVER);

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
    lv_style_copy(&style_mask, &style);
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
    lv_style_copy(&style_mask1, &style);
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
    lv_style_copy(&style_mask2, &style);
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
    lv_style_copy(&up_area_style, &style);
    lv_style_set_bg_grad(&up_area_style, &grad3);
}

static lv_style_t style_line;
static lv_style_t screen_style;
static void lv_page_open()
{
    style_init();
    //屏幕对象
    ai_dialog_page = lv_obj_create(NULL);
    lv_obj_clear_flag(ai_dialog_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(ai_dialog_page, &style, 0);
    lv_obj_center(ai_dialog_page);

   // TODO: 根据业务区分调用
    {
        // ai对话备份2
        lv_aivoice_dialog_Backup2(ai_dialog_page);
        // 智能体列表（1期暂无开发）
        // lv_aivoice_agentList(ai_dialog_page);
        // 智能体列表-使用中（1期暂无开发）
        // lv_aivoice_agentList_use(ai_dialog_page);
        // ai对话
        // lv_aivoice_dialog(ai_dialog_page);
        // ai对话网络异常(暂不需要开发，徐工做)
        // lv_aivoice_dialog_network_abnormal(ai_dialog_page);
        // ai按键说话
        // lv_aivoice_pressbutton_speak(ai_dialog_page);
        // ai对话松手取消
        // lv_aivoice_nobutton_cancelspeak(ai_dialog_page);
        // // 1.说完loading  2.智能体回复  3.异常回答-语义无法理解  4.异常回答-网络异常 这四个动画暂时不用实现（已于刘磊工沟通）
        // lv_aivoice_sepak_finish(ai_dialog_page);
        // 添加-选照片/照片选择/照片放大
        // lv_aivoice_photo_selection(ai_dialog_page);
        // 通话连接中
        // lv_aivoice_call_connecting(ai_dialog_page);
        // 通话连接成功
        // lv_aivoice_call_connected(ai_dialog_page);
        // 通话听到声音
        // lv_aivoice_call_listening(ai_dialog_page);
        // 通话回复问题
        // lv_aivoice_call_answer_question(ai_dialog_page);
        // 通话开启静音模式
        // lv_aivoice_call_muted(ai_dialog_page);
    }

    // 加载页面
    lv_scr_load_anim(ai_dialog_page, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
}

static void lv_page_close()
{
    lv_font_manager_del_font(font_26);
    lv_font_deinit();
}

static void lv_back_iamge_event(lv_event_t *e)
{
    lv_obj_clear_flag(dialog_obj, LV_OBJ_FLAG_HIDDEN);
}

static void lv_back_button_event(lv_event_t *e)
{
    lv_obj_add_flag(dialog_obj, LV_OBJ_FLAG_HIDDEN);
}

static void lv_agent_button_event(lv_event_t *e)
{
    // TODO: 返回上一级页面
    lv_obj_clean(ai_dialog_page);
    ai_dialog_page = NULL;
}

static void lv_camera_button_event(lv_event_t *e)
{
    // TODO: 触发拍照功能
    printf("拍照按钮被点击\n");
}

static void lv_voice_button_event(lv_event_t *e)
{
    // TODO: 触发语音输入功能
    
    printf("语音输入按钮被点击\n");
}

static void lv_iphone_button_event(lv_event_t *e)
{
    // TODO: 触发拍照功能
    printf("拍照功能识别\n");
}

static void lv_mute_button_event(lv_event_t *e)
{
     // TODO: 静音
    printf("静音\n");
}

static void lv_voice_cancel_button_event(lv_event_t *e)
{
    lv_obj_set_style_img_recolor_opa(cancel_image1, LV_OPA_80, 0);
     // TODO: 松手取消
    printf("松手取消\n");
}

static void lv_voice_pressbut_event(lv_event_t *e)
{
    lv_obj_set_style_img_recolor_opa(pressbut_image1, LV_OPA_80, 0);
    printf("按键说话\n");
}
static void lv_camera_hangupbut_event(lv_event_t *e)
{
    printf("当前被挂断\n");
}

static void lv_reduce_img_event(lv_event_t *e)
{
    printf("点击缩放按钮\n");
}
static void lv_itemcont_picture_event(lv_event_t * e)
{
    printf("缩略图被点击\n");
    lv_obj_t *image = lv_event_get_target(e);
    lv_obj_set_size(image, 502, 410);
    lv_img_set_zoom(image, 128);
    lv_obj_set_size(image, lv_pct(100), lv_pct(100));
    lv_obj_align(image, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *reduce_img = lv_img_create(image);
    lv_img_set_src(reduce_img, "../lv_port_pc_vscode/assert/icon/ai_dialog_shrink.png");
    lv_obj_set_size(reduce_img, 60, 60);
    lv_obj_align(reduce_img, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(reduce_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(reduce_img, lv_reduce_img_event, LV_EVENT_CLICKED, NULL);
}

static void lv_quit_button_event(lv_event_t *e)
{
    // // TODO: 返回上一级页面
    // lv_obj_clean(ai_dialog_page);
    // ai_dialog_page = NULL;
    lv_obj_t *back = lv_event_get_target(e);
    lv_obj_t *parent = lv_obj_get_parent(back);

    lv_obj_set_size(parent, 120, 98);
    lv_obj_align(parent, LV_ALIGN_TOP_RIGHT, -30, 192);
    lv_img_set_zoom(parent, 128);
    lv_obj_set_style_clip_corner(parent, true, 0);
    lv_obj_set_style_radius(parent, 15, 0);
}

static void lv_aivoice_dialog_Backup2(lv_obj_t *cont)
{
    // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);
    lv_obj_add_flag(image1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(image1, lv_back_iamge_event, LV_EVENT_CLICKED, NULL);

    // 加阴影
    dialog_obj = lv_obj_create(cont);
    lv_obj_remove_style_all(dialog_obj);
    lv_obj_add_style(dialog_obj, &style_mask1, 0);
    lv_obj_set_size(dialog_obj, 502, 70);
    lv_obj_align(dialog_obj, LV_ALIGN_TOP_MID, 0, 0);

    // 添加返回按钮
    lv_obj_t *back_button = lv_img_create(dialog_obj);
    lv_img_set_src(back_button, "../lv_port_pc_vscode/assert/icon/common_icon_back.png");
    lv_obj_set_size(back_button, 40, 40);
    lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_obj_add_flag(back_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back_button, lv_back_button_event, LV_EVENT_CLICKED, NULL);

    // 添加AI对话标题
    if (NULL == font_28) font_28 = font_get_regular(28);
    lv_obj_t *title_label = lv_label_create(dialog_obj);
    lv_label_set_text(title_label, "AI对话");
    lv_obj_set_style_text_opa(title_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(title_label, font_28, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(title_label, back_button, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    
    // 添加智能体
    lv_obj_t *agent_button = lv_img_create(dialog_obj);
    lv_img_set_src(agent_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_parent.png");
    lv_obj_set_size(agent_button, 40, 40);
    lv_obj_align(agent_button, LV_ALIGN_TOP_RIGHT, -30, 20);
    lv_obj_add_flag(agent_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(agent_button, lv_agent_button_event, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_flag(dialog_obj, LV_OBJ_FLAG_HIDDEN);   //隐藏

    // 添加语音输入按钮
    lv_obj_t *voice_button = lv_img_create(cont);
    lv_img_set_src(voice_button, "../lv_port_pc_vscode/assert/icon/ai_dialog.png");
    lv_obj_set_size(voice_button, 442, 90);
    lv_obj_align(voice_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(voice_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_CLICKED, NULL);

    // 添加电话按钮
    lv_obj_t *iphone_button = lv_img_create(voice_button);
    lv_img_set_src(iphone_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_phone.png");
    lv_obj_align(iphone_button, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_flag(iphone_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(iphone_button, lv_iphone_button_event, LV_EVENT_CLICKED, NULL);

    // 添加拍照按钮
    lv_obj_t *camera_button = lv_img_create(voice_button);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_picture.png");
    lv_obj_align(camera_button, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_button_event, LV_EVENT_CLICKED, NULL);

  
    // 添加底部提示文字
    if (NULL == font_30) font_30 = font_get_regular(30);
    lv_obj_t *bottom_label = lv_label_create(voice_button);
    lv_label_set_text(bottom_label, "按住说话");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_30, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_CENTER, 0, 0);
}

// 智能体列表
static void lv_aivoice_agentList(lv_obj_t *cont)
{
    
}

// 智能体列表-使用中
static void lv_aivoice_agentList_use(lv_obj_t *cont)
{

}

// ai對話
static void lv_aivoice_dialog(lv_obj_t *cont)
{
     // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);

    // 添加语音输入按钮
    lv_obj_t *voice_button = lv_img_create(image1);
    lv_img_set_src(voice_button, "../lv_port_pc_vscode/assert/icon/ai_dialog.png");
    lv_obj_set_size(voice_button, 442, 90);
    lv_obj_align(voice_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(voice_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(voice_button, lv_voice_button_event, LV_EVENT_CLICKED, NULL);

    // 添加电话按钮
    lv_obj_t *iphone_button = lv_img_create(voice_button);
    lv_img_set_src(iphone_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_phone.png");
    lv_obj_align(iphone_button, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_flag(iphone_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(iphone_button, lv_iphone_button_event, LV_EVENT_CLICKED, NULL);

    // 添加拍照按钮
    lv_obj_t *camera_button = lv_img_create(voice_button);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_picture.png");
    lv_obj_align(camera_button, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_button_event, LV_EVENT_CLICKED, NULL);

  
    // 添加底部提示文字
    if (NULL == font_30) font_30 = font_get_regular(30);
    lv_obj_t *bottom_label = lv_label_create(voice_button);
    lv_label_set_text(bottom_label, "按住说话");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_30, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_CENTER, 0, 0);

    // picextend_item_cont = lv_obj_create(cont);
    // lv_obj_set_size(picextend_item_cont, 120, 98);
    // lv_obj_set_style_bg_color(picextend_item_cont, lv_color_hex(0x103050), 0);
    // lv_obj_set_style_border_width(picextend_item_cont, 0, 0);
    // lv_obj_set_style_radius(picextend_item_cont, 15, 0);
    // lv_obj_set_style_pad_all(picextend_item_cont, 0, 0);
    // lv_obj_align(picextend_item_cont, LV_ALIGN_TOP_RIGHT, -30, 192);
    // lv_obj_add_flag(picextend_item_cont, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_clear_flag(picextend_item_cont, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_add_event_cb(picextend_item_cont, lv_itemcont_picture_event, LV_EVENT_CLICKED, NULL);
        
    picextend_item_cont = lv_img_create(cont);
    lv_img_set_src(picextend_item_cont, "../lv_port_pc_vscode/assert/icon/screen_saver.png");
    lv_obj_set_size(picextend_item_cont, 120, 98);
    lv_obj_set_style_clip_corner(picextend_item_cont, true, 0);
    lv_obj_set_style_radius(picextend_item_cont, 15, 0);
    lv_obj_align(picextend_item_cont, LV_ALIGN_TOP_RIGHT, -30, 192);
    lv_obj_add_flag(picextend_item_cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(picextend_item_cont, lv_itemcont_picture_event, LV_EVENT_CLICKED, NULL);

    //创建关闭按钮
    lv_obj_t *quit_button = lv_img_create(picextend_item_cont);
    lv_img_set_src(quit_button, "../lv_port_pc_vscode/assert/icon/common_icon_cancel_button.png");
    lv_img_set_zoom(quit_button, 128);
    lv_obj_set_size(quit_button, 25, 25);
    lv_obj_align(quit_button, LV_ALIGN_TOP_RIGHT, -6, 6);
    lv_obj_add_flag(quit_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(quit_button, lv_quit_button_event, LV_EVENT_CLICKED, NULL);

    // lv_obj_add_flag(picextend_item_cont, LV_OBJ_FLAG_HIDDEN);
}

// ai对话网络异常
static void lv_aivoice_dialog_network_abnormal(lv_obj_t *cont)
{

}

// ai按键说话
static void lv_aivoice_pressbutton_speak(lv_obj_t *cont)
{
    // 创建AI对话背景图
    pressbut_image1 = lv_img_create(cont);
    lv_img_set_src(pressbut_image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(pressbut_image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(pressbut_image1, 128);
    lv_obj_center(pressbut_image1);

    //插入文字
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "松开发送，上移取消");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_26, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(bottom_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -120);

    //插入背景框
    lv_obj_t *voice_button = lv_img_create(cont);
    lv_img_set_src(voice_button, "../lv_port_pc_vscode/assert/icon/ai_dialog.png");
    lv_obj_set_size(voice_button, 442, 90);
    lv_obj_align(voice_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(voice_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(voice_button, lv_voice_pressbut_event, LV_EVENT_CLICKED, NULL);
}

// ai对话松手取消
static void lv_aivoice_nobutton_cancelspeak(lv_obj_t *cont)
{   
    // 创建AI对话背景图
    cancel_image1 = lv_img_create(cont);
    lv_img_set_src(cancel_image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(cancel_image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(cancel_image1, 128);
    lv_obj_center(cancel_image1);

     //插入文字
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "松手取消");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_26, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFF5C5C), 0);
    lv_obj_set_style_text_align(bottom_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -120);
    // lv_obj_add_flag(bottom_label, LV_OBJ_FLAG_HIDDEN); 
    // lv_obj_clear_flag(bottom_label, LV_OBJ_FLAG_HIDDEN);

    // 添加语音输入按钮
    lv_obj_t *voice_button = lv_img_create(cont);
    lv_img_set_src(voice_button, "../lv_port_pc_vscode/assert/icon/ai_dialog.png");
    lv_obj_set_size(voice_button, 442, 90);
    lv_obj_align(voice_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(voice_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(voice_button, lv_voice_cancel_button_event, LV_EVENT_CLICKED, NULL);
}

// 说完loading
static void lv_aivoice_sepak_finish(lv_obj_t *cont)
{

}

static void lv_aivoice_photo_selection(lv_obj_t *cont)
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
            lv_img_set_src(check_icon, "../lv_port_pc_vscode/assert/icon/photograph_icon_select_green.png");
            lv_obj_set_size(check_icon, 40, 40);
            lv_obj_align(check_icon, LV_ALIGN_TOP_LEFT, 188, 144);
            lv_obj_add_flag(check_icon, LV_OBJ_FLAG_HIDDEN);

            // 将图标指针存储到用户数据中
            lv_obj_set_user_data(item_cont, check_icon);

            // 添加事件处理
            lv_obj_add_flag(item_cont, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(item_cont, select_item_event, LV_EVENT_ALL, NULL);

            // 时间标签
            // lv_obj_t *time_label = lv_label_create(item_cont);
            // lv_label_set_text(time_label, "00:30");
            // lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
            // lv_obj_set_style_text_font(time_label, font_get_regular(24), 0);
            // lv_obj_align(time_label, LV_ALIGN_BOTTOM_LEFT, 10, -10);
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

    // 添加AI对话标题
    if (NULL == font_28) font_28 = font_get_regular(28);
    lv_obj_t *title_label = lv_label_create(cont);
    lv_label_set_text(title_label, "所有照片");
    lv_obj_set_style_text_opa(title_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(title_label, font_28, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title_label, LV_ALIGN_OUT_TOP_LEFT, 70, 25);
    // lv_obj_align_to(title_label, photo_backup, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
}

// 通话连接中
static void lv_aivoice_call_connecting(lv_obj_t *cont)
{
    // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);

    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &style_mask, 0);
    lv_obj_set_size(obj, 502, 114);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    // 添加电话按钮
    lv_obj_t *iphone_button = lv_img_create(cont);
    lv_img_set_src(iphone_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_on.png");
    lv_obj_add_flag(iphone_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(iphone_button, lv_iphone_button_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(iphone_button, LV_ALIGN_BOTTOM_LEFT, 40, -30);

    // 添加挂断按钮
    lv_obj_t *camera_button = lv_img_create(cont);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/pic_microphone_close.png");
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_hangupbut_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(camera_button, LV_ALIGN_BOTTOM_RIGHT, -40, -15);

    // 添加底部提示文字
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "连接中...");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_26, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -46);
}
// 通话连接成功
static void lv_aivoice_call_connected(lv_obj_t *cont)
{
    // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);
    
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &style_mask, 0);
    lv_obj_set_size(obj, 502, 114);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    // 添加电话按钮
    lv_obj_t *iphone_button = lv_img_create(cont);
    lv_img_set_src(iphone_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_on.png");
    lv_obj_add_flag(iphone_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(iphone_button, lv_iphone_button_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(iphone_button, LV_ALIGN_BOTTOM_LEFT, 40, -30);

    // 添加挂断按钮
    lv_obj_t *camera_button = lv_img_create(cont);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/pic_microphone_close.png");
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_hangupbut_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(camera_button, LV_ALIGN_BOTTOM_RIGHT, -40, -15);

    // 添加底部提示文字
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "你可以开始说话了");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_26, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -46);
}

// 通话听到声音
static void lv_aivoice_call_listening(lv_obj_t *cont)
{
    // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);
    
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &style_mask, 0);
    lv_obj_set_size(obj, 502, 114);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    // 添加电话按钮
    lv_obj_t *iphone_button = lv_img_create(cont);
    lv_img_set_src(iphone_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_on.png");
    lv_obj_add_flag(iphone_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(iphone_button, lv_iphone_button_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(iphone_button, LV_ALIGN_BOTTOM_LEFT, 40, -30);

    // 添加挂断按钮
    lv_obj_t *camera_button = lv_img_create(cont);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/pic_microphone_close.png");
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_hangupbut_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(camera_button, LV_ALIGN_BOTTOM_RIGHT, -40, -15);

    // 添加底部提示文字
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "正在听...");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_26, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -46);
}

// 通话回复问题
static void lv_aivoice_call_answer_question(lv_obj_t *cont)
{
    // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);
        
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &style_mask, 0);
    lv_obj_set_size(obj, 502, 114);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    // 添加打断图标
    lv_obj_t *interrupt_icon = lv_img_create(cont);
    lv_img_set_src(interrupt_icon, "../lv_port_pc_vscode/assert/icon/ai_interrupt.png");
    lv_obj_align(interrupt_icon, LV_ALIGN_BOTTOM_MID, 0, -88);

    // 添加电话按钮
    lv_obj_t *iphone_button = lv_img_create(cont);
    lv_img_set_src(iphone_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_on.png");
    lv_obj_add_flag(iphone_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(iphone_button, lv_iphone_button_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(iphone_button, LV_ALIGN_BOTTOM_LEFT, 40, -30);

    // 添加挂断按钮
    lv_obj_t *camera_button = lv_img_create(cont);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/pic_microphone_close.png");
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_hangupbut_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(camera_button, LV_ALIGN_BOTTOM_RIGHT, -40, -15);

    // 添加底部提示文字
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "说话或点击打断");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_26, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -46);
}
// 通话开启静音模式
static void lv_aivoice_call_muted(lv_obj_t *cont)
{
     // 创建AI对话背景图
    lv_obj_t *image1 = lv_img_create(cont);
    lv_img_set_src(image1, "../lv_port_pc_vscode/assert/icon/start_agent_2x.png");
    lv_obj_set_size(image1, lv_pct(100), lv_pct(100));
    lv_img_set_zoom(image1, 128);
    lv_obj_center(image1);
        
    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &style_mask, 0);
    lv_obj_set_size(obj, 502, 114);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    // 添加静音按钮
    lv_obj_t *mute_button = lv_img_create(cont);
    lv_img_set_src(mute_button, "../lv_port_pc_vscode/assert/icon/ai_dialog_off.png");
    lv_obj_add_flag(mute_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(mute_button, lv_mute_button_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(mute_button, LV_ALIGN_BOTTOM_LEFT, 40, -30);

    // 添加挂断按钮
    lv_obj_t *camera_button = lv_img_create(cont);
    lv_img_set_src(camera_button, "../lv_port_pc_vscode/assert/icon/pic_microphone_close.png");
    lv_obj_add_flag(camera_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(camera_button, lv_camera_hangupbut_event, LV_EVENT_CLICKED, NULL);
    lv_obj_align(camera_button, LV_ALIGN_BOTTOM_RIGHT, -40, -15);

    // 添加底部提示文字
    if (NULL == font_26) font_26 = font_get_regular(26);
    lv_obj_t *bottom_label = lv_label_create(cont);
    lv_label_set_text(bottom_label, "你已静音");
    lv_obj_set_style_text_opa(bottom_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(bottom_label, font_26, 0);
    lv_obj_set_style_text_color(bottom_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bottom_label, LV_ALIGN_BOTTOM_MID, 0, -44);
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

static void single_icon_click_event(lv_event_t * e)
{
    lv_obj_clear_flag(picextend_item_cont, LV_OBJ_FLAG_HIDDEN);
}

#endif
