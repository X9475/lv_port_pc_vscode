#ifndef MENU_UI_H
#define MENU_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_page_info.h"

//菜单
extern lv_subject_t menu_subject;
lv_page_info_pt lv_page_menu_info_get();

#ifdef __cplusplus
}
#endif

#endif