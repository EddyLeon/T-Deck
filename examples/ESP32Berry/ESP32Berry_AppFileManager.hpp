#pragma once
#include "ESP32Berry_Config.hpp"
#include "ESP32Berry_AppBase.hpp"
#include <ArduinoJson.h>
#include <ChatGPT.hpp>

class AppFileManager : public AppBase
{
private:
    lv_style_t msgStyle;
    lv_obj_t *textField;
    lv_obj_t *sendBtn;
    lv_obj_t *msgList;
    int display_width;
    WiFiClientSecure client;
    void draw_ui();
    void printFileList();

public:
    ChatGPT<WiFiClientSecure> *chat_gpt;
    AppFileManager(Display *display, System *system, Network *network, const char *title);
    ~AppFileManager();
    void add_msg(String msg);
    void delete_event_handler(lv_event_t *e);
    void clean_input_field();
    void close_app();
};