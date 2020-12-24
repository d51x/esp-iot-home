#pragma once


#ifndef _USER_H_
#define _USER_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "http_page_tpl.h"

#if CONFIG_USER_WEB_PRINT || CONFIG_USER_WEB_CONFIG
#define USER_WEB_PRINT(s) {\
    httpd_resp_sendstr_chunk(req, s); \
    }
#endif

void user_setup(void *args);

// функция вызывает в основном цикле каждую секунду
// вызывается сразу после user_setup и до старта wifi (подключения к точке)
// до user_http_init и до user_mqtt_init
void user_loop(uint32_t sec);

void user_http_init(void *args);  // для инициализации компонентов, отображаемых в web, например, кнопки для mcp23017

// функция вызывается после user_setup и старта mqtt
// в этой функции можно зарегистрировать свои кастомные колбеки на отправку и получение данных
void user_mqtt_init(void *args);

// функция вывод данные в пользовательском блоке на главной
#ifdef CONFIG_USER_WEB_PRINT
void user_web_main(httpd_req_t *req);
#endif

#ifdef CONFIG_USER_WEB_CONFIG
// функция выводит данные на странице /config
void user_web_options(httpd_req_t *req);

// функция пользовательского обработчика гет запросов /config
void user_process_param(httpd_req_t *req, void *args);
#endif

#endif