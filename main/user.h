#pragma once


#ifndef _USER_H_
#define _USER_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "http_page_tpl.h"



void user_setup(void *args);
void user_loop(uint32_t sec);
void user_http_init(void *args);  // для инициализации компонентов, отображаемых в web, например, кнопки для mcp23017
void user_mqtt_init(void *args);  // для инициализации коллбеков для mqtt
void user_web_main(http_args_t *args);
void user_web_options(http_args_t *args);
void user_process_param(httpd_req_t *req, void *args);

#endif