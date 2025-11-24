#include <stdint.h>
#include "../lv_page_info.h"

static uint8_t init_flag = 0;
static uint8_t cur_depth = 0;
static lv_page_info_pt stack_list[LV_STACK_DEPTH];

void lv_stack_list_init()
{
    if (init_flag == 0) {
        lv_memset(stack_list, 0, sizeof(stack_list));
        init_flag = 1;
    }

    return;
}

void lv_stack_push(void *page_ptr)
{
    if (cur_depth == LV_STACK_DEPTH) {
        LV_LOG_WARN("stack is full, start swap.");
        lv_memmove(stack_list, &stack_list[1], (LV_STACK_DEPTH - 1) * sizeof(lv_page_info_pt));
        stack_list[LV_STACK_DEPTH - 1] = 0;
        cur_depth--;
    }

    stack_list[cur_depth] = page_ptr;
    cur_depth++;
    return;
}

void *lv_stack_pop()
{
    if (cur_depth == 0) {
        LV_LOG_WARN("stack is empty");
        return NULL;
    }

    lv_page_info_pt page_ptr = stack_list[cur_depth - 1];
    stack_list[cur_depth - 1] = NULL;
    cur_depth--;

    // printf("[%s:%d] ==> stack pop page id: %d\n", __FILE__, __LINE__, page_ptr->page_id);

    return page_ptr;
}