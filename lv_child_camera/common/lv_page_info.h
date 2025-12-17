#ifndef LV_PAGE_INFO_H
#define LV_PAGE_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "font_manager.h"
#include "page_switch/lv_stack.h"
#include "page_switch/lv_page_switch.h"
#include "../../lvgl/lvgl.h"

typedef void (* construct_func)(void *);
typedef void (* destruct_func)();

/// @brief 页面状态枚举
typedef enum
{
    STATUS_NONE,    ///< 初始状态
    STATUS_RUNNNIG, ///< 正在运行
    STATUS_EXITING, ///< 退出中
} LV_STATUS_ENUM;

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
    
    PAGE_FUNCTIONAL_SHOOTING_PHOTO,         ///<< 拍摄首界面
    PAGE_FUNCTIONAL_SHOOTING_MODE,          ///<< 拍摄录制界面
    PAGE_FUNCTIONAL_SHOOTING_WAIT,          ///<< 拍摄模式切换等待界面
    PAGE_FUNCTIONAL_SHOOTING_VIDEO,         ///<< 录制模式界面
    PAGE_FUNCTIONAL_SHOOTING_ADJ_FOCUS,     ///<< 调焦界面
    PAGE_FUNCTIONAL_SHOOTING_ADJ_PARAM,     ///<< 拍摄录制调参数界面
    PAGE_FUNCTIONAL_SHOOTING_MULTI_FILTER,  ///<< 百变滤镜界面
    PAGE_FUNCTIONAL_SHOOTING_VIDEO_PARAM,   ///<< 录像参数调整界面


    PAGE_FUNCTIONAL_AUDIO_EFFECT,       ///<< 音频效果
    PAGE_FUNCTIONAL_AUDIO_PHOTO,        ///<< 拍照音
    PAGE_FUNCTIONAL_AUDIO_RING_BELL,    ///<< 呼叫铃声
    PAGE_FUNCTIONAL_AUDIO_ANSWER_BELL,  ///<< 接听铃声
    PAGE_FUNCTIONAL_AUDIO_ALARM_CLOCK,  ///<< 闹钟铃声
    PAGE_FUNCTIONAL_AUDIO_NOTIFY_SOUND, ///<< 消息提示音
    PAGE_FUNCTIONAL_AUDIO_KEYPAD_TONE,  ///<< 按键音
    PAGE_FUNCTIONAL_AUDIO_BEYOND_AREA,  ///<< 超出区域
    PAGE_FUNCTIONAL_SAVER_STYLE,        ///<< 屏保样式
    PAGE_FUNCTIONAL_RECORD_TIME,        ///<< 单次录像时长设置
    PAGE_FUNCTIONAL_HOLD_TIME,          ///<< 息屏时间设置
    PAGE_FUNCTIONAL_DISPLAY_TIME,       ///<< 时间展示格式设置
    PAGE_FUNCTIONAL_STORAGE_MGR,        ///<< 存储管理
    PAGE_FUNCTIONAL_ROLLER_VIBRAT,      ///<< 滚轮振动开关
    PAGE_FUNCTIONAL_VIBRAT_AMPLITUDE,   ///<< 振动幅度
    PAGE_FUNCTIONAL_REMOTE_PREVIEW,     ///<< 远程预览开关
    PAGE_FUNCTIONAL_ABOUT_CAMERA,       ///<< 关于相机
    PAGE_FUNCTIONAL_FORMAT_CONFIRM,     ///<< 格式化确认
    PAGE_FUNCTIONAL_FACTORY_RESTORE,    ///<< 恢复出厂设置
    PAGE_FUNCTIONAL_CERT_MASK,          ///<< 认证标志
    PAGE_FUNCTIONAL_SETTING_QRCODE,     ///<< 设置页二维码
    PAGE_FUNCTIONAL_SCREENLOCK,         ///<< 锁屏界面
    PAGE_FUNCTIONAL_POWEROFF,           ///<< 关机界面

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
    PAGE_TOAST_FORMAT_DOING,            ///<< 格式化中…
    PAGE_TOAST_FORMAT_SUCCESS,          ///<< 格式化成功
    PAGE_TOAST_FORMAT_ERROR,            ///<< 格式化失败
    PAGE_TOAST_SWICTH_DOING,            ///<< 切换中…
    PAGE_TOAST_SWICTH_SUCCESS,          ///<< 切换成功
    PAGE_TOAST_SWICTH_ERROR,            ///<< 切换失败
    PAGE_TOAST_FACTORY_RESTORE_DOING,   ///<< 恢复出厂设置中…
    PAGE_TOAST_FACTORY_RESTORE_SUCCESS, ///<< 恢复成功
    PAGE_TOAST_FACTORY_RESTORE_ERROR,   ///<< 恢复失败
    PAGE_TOAST_STORAGE_EXHAUSTED,       ///<< 存储容量耗尽无法拍摄
    PAGE_TOAST_NO_SDCARD,               ///<< 无SD卡无法拍摄和查看
    PAGE_TOAST_SDCARD_ABNORMAL,         ///<< SD卡异常无法拍摄和查看
    PAGE_TOAST_DEV_OFFLINE,             ///<< 设备离线，无法使用
    PAGE_TOAST_CALL_FAILED_NET_BAD,     ///<< 呼叫失败，网络异常
    PAGE_TOAST_ADD_SUCCESS,             ///<< 添加成功
    PAGE_TOAST_ADD_FAILED_RETRY,        ///<< 添加失败，请重试
    PAGE_TOAST_MAX_VALUME,              ///<< 已达到最大音量
    PAGE_TOAST_MIN_VALUME,              ///<< 已达到最小音量
    PAGE_TOAST_NETWORK_INSTABILITY,     ///<< 当前网络不稳定
    PAGE_TOAST_MESSAGE_FAILED,          ///<< 留言失败，网络异常
    PAGE_TOAST_SEND_FAILED,             ///<< 发送失败
    PAGE_TOAST_SETTING_MENU_SLIDE,      ///<< 设置菜单滑动提示
    PAGE_TOAST_MENU_SLIDE,              ///<< 菜单滑动提示
    PAGE_TOAST_ALBUM_RIGHT_SLIDE,       ///<< 拍摄页右滑进相册提示
    PAGE_TOAST_ALBUM_LEFT_SLIDE,        ///<< 拍摄页左滑进页面调整
    PAGE_TOAST_CONFIRM,                 ///<< toast确认

    //aivoice
    PAGE_AIVOICE_NONE,                  
    PAGE_AIVOICE_DIALOG,                ///<< AI对话
    PAGE_PHOTO_SELETE,                  ///<< 照片选择
    PAGE_CALL_STATUS,                   ///<< 通话状态
    PAGE_AIVOICE_CONNECTING,            ///<< 连接中
    PAGE_AIVOICE_CONNECTED,             ///<< 连接成功
    PAGE_AIVOICE_LISTENING,             ///<< 听到声音
    PAGE_AIVOICE_ANSWER_QUESTION,       ///<< 通话回复问题
    PAGE_AIVOICE_CALL_MUTED,            ///<< 静音

    PAGE_NONE
} LV_PAGE_ENUM;

/// @brief 页面信息结构体
typedef struct
{
    uint32_t page_id;               //页面ID，LV_PAGE_ENUM
    lv_obj_t *page;                 //页面指针，指向当前加载的页面
    construct_func construct_cb;    //构造函数, 初始化资源及加载页面
    destruct_func destruct_cb;      //析构函数, 退出时回收页面资源
    LV_STATUS_ENUM status;          //页面状态，LV_STATUS_ENUM
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