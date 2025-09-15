#ifndef LV_START_UP_H
#define LV_START_UP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_child_camera_ui.h"

#if (LV_CHILD_CAMERA != 0) && (LV_CHILD_CAMERA_STARTUP != 0)

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_child_camera();

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_START_UP_H */