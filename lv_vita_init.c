#include <stdint.h>

// LittleVGL
#include <lvgl/lvgl.h>

// VITASDK
// input
#include <psp2kern/ctrl.h>
// touch
#include <psp2/touch.h>

void lv_vita_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            // Put a pixel to the display.
            set_pixel(x, y, *color_p);
            color_p++;
        }
    }
    // Indicate you are ready with the flushing
    lv_disp_flush_ready(disp);
}

bool lv_vita_touchpad_read(lv_indev_t* indev, lv_indev_data_t* data) {
    SceTouchData touchData;
    int nBufs = sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touchData, 1);

    if(nBufs < 0) {
        // error
        return false;
    }

    data->state = (touchData.reportNum > 0) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if(data->state == LV_INDEV_STATE_PR) {
        // FIXME: Need to verify types
        // FIXME: Need to iterate through touchData.reports via touchData.reportNum
        data->point.x = touchData.report[0].x;
        data->point.y = touchData.report[0].y;
    }

    // Return `false` because we are not buffering and no more data to read
    return false;
}

bool lv_vita_controller_read(lv_indev_t* indev, lv_indev_data_t* data) {
    SceCtrlData ctrlData;
    int c = ksceCtrlPeekBufferPositive(0, &ctrlData, 1);

    if(c < 0) {
        // error
    }

    // FIXME: Was a button pressed lately?
    if(ctrlData.buttons == 0) {
        // No button pressed.
        data->state = LV_INDEV_STATE_REL;
    } else {
        // Translate SCE_CTRL button information to LVGL
        // Sadly, LVGL doesn't seem to do button combinations.
        // Ontop of that, where do I bind the rest? o.o'
        unsigned int btn = ctrlData.buttons;
        unsigned int lvbtn = 0;
             if(btn & SCE_CTRL_UP)       lvbtn = LV_KEY_UP;
        else if(btn & SCE_CTRL_DOWN)     lvbtn = LV_KEY_DOWN;
        else if(btn & SCE_CTRL_LEFT)     lvbtn = LV_KEY_LEFT;
        else if(btn & SCE_CTRL_RIGHT)    lvbtn = LV_KEY_RIGHT;
        else if(btn & SCE_CTRL_CROSS)    lvbtn = LV_KEY_ENTER;
        else if(btn & SCE_CTRL_CIRCLE)   lvbtn = LV_KEY_ESC;
        else if(btn & SCE_CTRL_LTRIGGER) lvbtn = LV_KEY_PREV;
        else if(btn & SCE_CTRL_RTRIGGER) lvbtn = LV_KEY_NEXT;
        data->state = LV_INDEV_STATE_PR;
        data->key = lvbtn;
    }
    return false; // no buffering
}

void lv_vita_init() {
    // Display buffer
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX / 10);

    // Display driver
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = lv_vita_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_t* vita_display = lv_disp_drv_register(&disp_drv);

    // Touch - front only. Dunno what to do with the back...
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    lv_indev_drv_t fronttouch_drv;
    lv_indev_drv_init(&fronttouch_drv);
    fronttouch_drv.type = LV_INDEV_TYPE_POINTER;
    fronttouch_drv.read_cb = lv_vita_touchpad_read;
    lv_indev_t* vita_front_touch = lv_indev_drv_register(&fronttouch_drv);

    // Keypad
    ksceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITAL);
    lv_indev_drv_t controller_drv;
    lv_indev_drv_init(&controller_drv);
    controller_drv.type = LV_INDEV_TYPE_KEYPAD;
    controller_drv.read_cb = lv_vita_controller_read;
    lv_indev_t* vita_controller = lv_indev_drv_register(&controller_drv);

}