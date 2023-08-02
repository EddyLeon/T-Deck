#include "ESP32Berry_AppFileManager.hpp"

static AppFileManager *instance = NULL;
static System *thisSystem = NULL;

AppFileManager::AppFileManager(Display *display, System *system, Network *network, const char *title)
    : AppBase(display, system, network, title)
{
    instance = this;
    thisSystem = system;
    display_width = display->get_display_width();
    this->draw_ui();
    this->printFileList();
}

AppFileManager::~AppFileManager() {}

extern "C" void tg_textarea_event_cb_thunk(lv_event_t *e)
{
    instance->_display->textarea_event_cb(e);
}

extern "C" void delete_event_handler_thunk(lv_event_t *e)
{
    instance->delete_event_handler(e);
}

void fileManagertask(void *msg)
{
    std::string str = std::string((char *)msg);

    instance->_display->lv_port_sem_take();
    instance->add_msg(str.c_str());
    instance->_display->lv_port_sem_give();

    vTaskDelete(NULL);
}

void AppFileManager::delete_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    char *fileName = (char *)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED)
    {
        // instance->_system->deleteFile(fileName);
        Serial.print("File Name: ");
        Serial.println(fileName);
        lv_obj_del(obj->parent);
    }
}

void AppFileManager::draw_ui()
{
    lv_style_init(&msgStyle);
    lv_style_set_bg_color(&msgStyle, lv_color_white());
    lv_style_set_pad_ver(&msgStyle, 6);
    lv_style_set_border_color(&msgStyle, lv_color_hex(0x989898));
    lv_style_set_border_width(&msgStyle, 2);
    lv_style_set_border_opa(&msgStyle, LV_OPA_50);
    lv_style_set_border_side(&msgStyle, LV_BORDER_SIDE_BOTTOM);

    msgList = lv_list_create(ui_AppPanel);
    lv_obj_set_size(msgList, display_width, 160);
    lv_obj_set_align(msgList, LV_ALIGN_TOP_MID);
    lv_obj_align(msgList, LV_ALIGN_TOP_MID, 0, -10);
    lv_obj_set_style_border_opa(msgList, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(msgList, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void AppFileManager::add_msg(String msg)
{
    lv_obj_t *text = lv_list_add_text(msgList, msg.c_str());
    lv_obj_add_style(text, &msgStyle, 0);
    lv_label_set_long_mode(text, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_scroll_to_y(msgList, lv_obj_get_scroll_y(msgList) + lv_obj_get_height(msgList), LV_ANIM_ON);

    lv_obj_t *btn = lv_btn_create(text);
    lv_obj_set_size(btn, 38, 35);

    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_t *btnLabel = lv_label_create(btn);
    lv_label_set_text(btnLabel, "Del");
    lv_obj_center(btnLabel);
    lv_obj_add_event_cb(btn, delete_event_handler_thunk, LV_EVENT_CLICKED, (void *)msg.c_str());
}

void AppFileManager::clean_input_field()
{
    lv_textarea_set_text(textField, "");
}

void AppFileManager::close_app()
{
    _display->goback_main_screen();
    lv_obj_del(_bodyScreen);
    delete this;
}

void deleteFile(const char *path)
{
    if (instance->_system->deleteFile(path))
    {
    }
}

void AppFileManager::printFileList()
{
    File root = instance->_system->storage.open("/");

    while (true)
    {

        File entry = root.openNextFile();

        if (!entry)
        {
            break;
        }

        xTaskCreate(fileManagertask, "fileManagertask", 10000, (void *)entry.name(), 1, NULL);
        delay(5);
        entry.close();
    }
}