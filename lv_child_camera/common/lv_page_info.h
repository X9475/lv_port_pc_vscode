#ifndef LV_PAGE_INFO_H
#define LV_PAGE_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "font_manager.h"
#include "page_switch/lv_stack.h"
#include "page_switch/lv_page_switch.h"
#include "../../lvgl/lvgl.h"

typedef void (* construct_func)();
typedef void (* destruct_func)();

/// @brief 设备当前状态
typedef enum
{
    LV_PAGE_STAGE_NONE = 0,      ///<< 初始无状态
    LV_PAGE_STAGE_ADDING,        ///<< 添加流程阶段
    LV_PAGE_STAGE_RUNNING,       ///<< 正式运行阶段
} LV_PAGE_STAGE_ENUM;

/// @brief 页面的ID枚举
typedef enum
{
    //functional
    PAGE_FUNCTIONAL_NONE,               ///<< 初始无页面
    PAGE_FUNCTIONAL_QRCODE,             ///<< 显示注册二维码
    PAGE_FUNCTIONAL_SIGNUP_SUCCESS,     ///<< 注册成功
    PAGE_FUNCTIONAL_SIGNUP_FAILED,      ///<< 注册失败
    PAGE_FUNCTIONAL_NETWORK_4G,         ///<< 4G网络异常
    PAGE_FUNCTIONAL_FIRST_BOOTUP,       ///<< 第一次启动
    PAGE_FUNCTIONAL_AGENT_START,        ///<< 智能助手开启
    PAGE_FUNCTIONAL_MENU,               ///<< 菜单界面
    PAGE_FUNCTIONAL_MENU_SETTING,       ///<< 菜单设置界面
    PAGE_FUNCTIONAL_MORE_SETTINGS,      ///<< 更多设置
    PAGE_FUNCTIONAL_ALBUM,              ///<< 相册初始界面
    PAGE_FUNCTIONAL_ALBUM_NONE,         ///<< 相册为空界面
    PAGE_FUNCTIONAL_ALBUM_SHARE,        ///<< 相册分享界面
    PAGE_FUNCTIONAL_ALBUM_SHARE_SUC,    ///<< 相册分享成功界面
    PAGE_FUNCTIONAL_ALBUM_SHARE_RETRY,  ///<< 相册分享成功界面
    PAGE_FUNCTIONAL_ALBUM_INFO,         ///<< 相册信息界面
    PAGE_FUNCTIONAL_ALBUM_AI,           ///<< 相册AI界面
    PAGE_FUNCTIONAL_ALBUM_FOUR_GRID,    ///<< 相册四宫格界面
    PAGE_FUNCTIONAL_ALBUM_DELETE,       ///<< 相册删除界面
    
    PAGE_FUNCTIONAL_SHOOTING_PHOTO,     ///<< 拍摄首界面
    PAGE_FUNCTIONAL_SHOOTING_MODE,      ///<< 拍摄录制界面
    PAGE_FUNCTIONAL_SHOOTING_WAIT,      ///<< 拍摄模式切换等待界面
    PAGE_FUNCTIONAL_SHOOTING_VIDEO,     ///<< 录制模式界面
    PAGE_FUNCTIONAL_AUDIO_EFFECT,       ///<< 音频效果
    PAGE_FUNCTIONAL_SAVER_STYLE,        ///<< 屏保样式
    PAGE_FUNCTIONAL_RECORD_TIME,        ///<< 单次录像时长设置
    PAGE_FUNCTIONAL_HOLD_TIME,          ///<< 息屏时间设置
    PAGE_FUNCTIONAL_DISPLAY_TIME,       ///<< 时间展示格式设置
    PAGE_FUNCTIONAL_STORAGE_MGR,        ///<< 存储管理
    PAGE_FUNCTIONAL_VIBRAT_AMPLITUDE,   ///<< 振动幅度
    PAGE_FUNCTIONAL_ABOUT_CAMERA,       ///<< 关于相机
    PAGE_FUNCTIONAL_FACTORY_RESTORE,    ///<< 恢复出厂设置
    PAGE_FUNCTIONAL_CERT_MASK,          ///<< 认证标志

    //abnormal
    PAGE_ABNORMAL_NONE,
    PAGE_ABNORMAL_LOW_BATTERY,          ///<< 电池电量小于20%
    PAGE_ABNORMAL_MINUTE_BATTERY,       ///<< 电池电量小于3%
    PAGE_ABNORMAL_HIGH_TAMPERATURE,     ///<< 电池高温异常
    PAGE_ABNORMAL_LOW_TAMPERATURE,      ///<< 电池低温异常
    PAGE_ABNORMAL_STORAGE_EXHAUSTED,    ///<< 存储即将耗尽
    PAGE_ABNORMAL_UPDATING,             ///<< 升级中
    PAGE_ABNORMAL_USB_TRANSMIT,         ///<< USB传输
    PAGE_ABNORMAL_USB_FLASH_MODE,       ///<< U盘模式中
    PAGE_ABNORMAL_NETWORK,              ///<< 网络异常
    PAGE_ABNORMAL_RETRY,                ///<< 重试
    PAGE_ABNORMAL_CONFIRM,              ///<< 异常确认

    //toast
    PAGE_TOAST_NONE,
    PAGE_TOAST_FORMAT_DOING,            ///<< 格式化中
    PAGE_TOAST_FORMAT_SUCCESS,          ///<< 格式化成功
    PAGE_TOAST_FORMAT_ERROR,            ///<< 格式化失败
    PAGE_TOAST_SWICTH_DOING,            ///<< 切换中
    PAGE_TOAST_SWICTH_SUCCESS,          ///<< 切换成功
    PAGE_TOAST_SWICTH_ERROR,            ///<< 切换失败
    PAGE_TOAST_FACTORY_RESTORE_DOING,   ///<< 恢复出厂设置中
    PAGE_TOAST_FACTORY_RESTORE_SUCCESS, ///<< 恢复出厂设置成功
    PAGE_TOAST_FACTORY_RESTORE_ERROR,   ///<< 恢复出厂设置失败
    PAGE_TOAST_STORAGE_EXHAUSTED,       ///<< 存储已耗尽，无法拍摄
    PAGE_TOAST_CONFIRM,                 ///<< toast确认

    PAGE_NONE
} LV_PAGE_ENUM;

/// @brief 页面信息结构体
typedef struct
{
    uint32_t page_id;               //页面ID，LV_PAGE_ENUM
    lv_obj_t *page;                 //页面指针，指向当前加载的页面
    construct_func construct_cb;    //构造函数, 初始化资源及加载页面
    destruct_func destruct_cb;      //析构函数, 退出时回收页面资源
    void *reserved;                 //保留字段
} lv_page_info_t, *lv_page_info_pt;

typedef struct
{
    lv_page_info_pt new_page;
    lv_page_info_pt old_page;
} lv_switch_page_t, *lv_switch_page_pt;

#ifdef __cplusplus
}
#endif

#endif