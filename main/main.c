#include "main.h"


static const char *TAG = "MAIN";

static void debug_register_http_print_data();
static void initialize_services(void *arg);

httpd_handle_t http_server = NULL;

void app_main(void)
{
    #ifdef CONFIG_DEBUG_UART1
        enable_debug_uart1();
        userlog("DEBUG UART1: %s\n", __func__);
    #endif

	//ESP_ERROR_CHECK(nvs_flash_init());

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    //if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //  ESP_ERROR_CHECK(nvs_flash_erase());
    //  ret = nvs_flash_init();
    //}
    ESP_ERROR_CHECK(ret);

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
	
    // get app version
    const esp_app_desc_t *app_desc = esp_ota_get_app_description();
    strncpy(FW_VER, app_desc->version, 32);
    strcpy(FW_VER, copy_str_from_str(FW_VER, "_"));
    free((void *)app_desc);

    // ========================================= MODULES initialization START
    #ifdef CONFIG_COMPONENT_DEBUG
        log_rtc_print_debug_str();
        log_rtc_init_debug_str();
    #endif

    initialize_modules();
    user_setup(NULL);

    xTaskCreate(initialize_services, "init_srv", 2048, 1000, 9, NULL);

    #ifdef CONFIG_SENSOR_MQTT
        mqtt_subscriber_init( http_server );
    #endif

    while (true) 
    {
        static uint32_t sec = 0;
        
        #ifdef CONFIG_DEBUG_PRINT_TASK_INFO
            if ( sec % 2 )    // каждые 2 сек
                print_tasks_info();
        #endif
        
        user_loop(sec);
        sec++;

        vTaskDelay(1000/ portTICK_RATE_MS);
    }

	
}


void initialize_modules()
{
    #ifdef CONFIG_SENSORS_GET
    http_handlers_count++; 
    sensors_init();
    #endif

    #ifdef CONFIG_SENSOR_PZEM004_T
    pzem_init(0);
    pzem_task_start(5);

        http_handlers_count += 1;
    #endif

    #ifdef CONFIG_COMPONENT_RELAY
    // TODO: load relay's data from nvs and apply
    
    #ifdef CONFIG_RELAY_CONFIG
    relay_load_nvs();
    #endif

    //#define RELAY_LED_PIN 2
    //relay_h = relay_create( "Led", RELAY_LED_PIN, RELAY_LEVEL_HIGH /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ , false);
    //relay_write(relay_h,  RELAY_STATE_CLOSE);

    // relay_red_h = relay_create( "Red", 15, RELAY_LEVEL_LOW /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ );
    // relay_write(relay_red_h,  RELAY_STATE_CLOSE);

    // relay_green_h = relay_create( "Green", 12, RELAY_LEVEL_LOW /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ );
    // relay_write(relay_green_h,  RELAY_STATE_CLOSE);

    // relay_blue_h = relay_create( "Blue", 13, RELAY_LEVEL_LOW /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ );
    // relay_write(relay_blue_h,  RELAY_STATE_CLOSE);   

        #ifdef CONFIG_RELAY_HTTP
        http_handlers_count += RELAY_HANDLERS_COUNT;
        #endif 
    #endif

    #ifdef CONFIG_COMPONENT_IR_RECV
    ir_rx = irrcv_init();
    if ( ir_rx != NULL ) {
		//		                btn_id	  btn_code   user_ctx      callback fun
        irrcv_add_button(ir_rx, 	 0, 0x00FF14EB, "button1", NULL /*ir_button1_press*/);
        irrcv_add_button(ir_rx, 	 1, 0x00FF04FB, "button2", NULL /*ir_button2_press*/);   
		
		//Start IR Receiver to receive a code
        irrcv_start( ir_rx );
    } else {
        ESP_LOGE(TAG, "failed to init ir receiver");
    }    
        #ifdef CONFIG_IR_RECV_HTTP
        http_handlers_count += IR_RECV_HANDLERS_COUNT;
        #endif
    #endif

    #ifdef CONFIG_COMPONENT_LCD2004
        lcd2004_init();
        lcd2004_test_task();
        #ifdef CONFIG_COMPONENT_LCD2004_HTTP
        http_handlers_count += LCD2004_HANDLERS_COUNT;
        #endif
    #endif

    #ifdef CONFIG_COMPONENT_MCP23017
        mcp23017_h = mcp23017_create(0x20 /*MCP23017_ADDR_DEFAULT*/ );
        #ifdef CONFIG_MCP23017_HTTP
        http_handlers_count += MCP23017_HANDLERS_COUNT;
        #endif 
        
    #endif

    #ifdef CONFIG_COMPONENT_PCF8574
        pcf8574_h = pcf8574_create(0x3F /*PCF8574_ADDR_DEFAULT*/ );
        pcf8574_test_task(pcf8574_h);
    #endif      

    #ifdef CONFIG_SENSOR_SHT21
        sht21_init();
        sht21_start(5);
    #endif

    #ifdef CONFIG_LED_CONTROLLER
    // ===== create led channels ============================
    ch_red = calloc(1, sizeof(ledcontrol_channel_t)); 
    ch_red->pin = 15;
    ch_red->channel = 0;
    ch_red->bright_tbl = TBL_32B;
    ch_red->name = "Красный";
    ch_red->group = 1;
    
    ch_green = calloc(1, sizeof(ledcontrol_channel_t));
    ch_green->pin = 12;
    ch_green->channel = 1;
    ch_green->bright_tbl = TBL_32B;
    ch_green->name = "Зеленый";
    ch_green->group = 1;

    ch_blue = calloc(1, sizeof(ledcontrol_channel_t)+1);
    ch_blue->pin = 13;
    ch_blue->channel = 2;
    ch_blue->bright_tbl = TBL_32B;
    ch_blue->name = "Синий";
    ch_blue->group = 1;

    ch_white = calloc(1, sizeof(ledcontrol_channel_t)+1);
    ch_white->pin = 2;
    ch_white->channel = 3;
    ch_white->bright_tbl = TBL_32B;
    ch_white->name = "Белый";
    ch_white->group = 2;

    // ===== create led controller ============================
    ledc_h = ledcontrol_create(500, 4);
    ledc = (ledcontrol_t *)ledc_h;

    // ==== register led channels to led controller =============
    ledc->register_channel(*ch_red);
    ledc->register_channel(*ch_green);
    ledc->register_channel(*ch_blue);
    ledc->register_channel(*ch_white);

    // ====== initialize led controller =======================
    ledc->init();  

    #ifdef CONFIG_LED_CONTROL_HTTP
    http_handlers_count += LED_CONTROL_HANDLERS_COUNT;
    #endif

    #ifdef CONFIG_RGB_CONTROLLER
        // === create and init RGB controller ================
        rgb_ledc = rgbcontrol_init(ledc, ch_red, ch_green, ch_blue);

        #ifdef CONFIG_RGB_CONTROLLER_HTTP
        http_handlers_count += RGB_CONTROL_HANDLERS_COUNT;
        #endif 

        #ifdef CONFIG_RGB_EFFECTS
        // ==== create and init rgb effects =======================
        // === setup effects to RGB controller  
        effects = effects_init(rgb_ledc, rgb_ledc->set_color_hsv);
        rgb_ledc->set_effects( effects );  
        #endif
    #endif

    #endif

    #ifdef I2C_HANDLERS_COUNT
    http_handlers_count += I2C_HANDLERS_COUNT;
    #endif

    #ifdef CONFIG_SENSOR_MQTT
    http_handlers_count += MQTT_SUBSCRIBER_HANDLERS_COUNT;
    #endif

}

void initialize_modules_mqtt()
{
    user_mqtt_init(NULL);

    #ifdef CONFIG_COMPONENT_RELAY
    relay_mqtt_init();
    #endif

    #ifdef CONFIG_COMPONENT_MCP23017
    mcp23017_mqtt_init(mcp23017_h);
    #endif

    #ifdef CONFIG_SENSOR_SHT21
    sht21_mqtt_init();
    #endif     

    #ifdef CONFIG_LED_CONTROLLER
    ledcontrol_mqtt_init(ledc_h);
        #ifdef CONFIG_RGB_EFFECTS
        rgbcontrol_mqtt_init(rgb_ledc);
        #endif
    #endif

    #ifdef CONFIG_SENSOR_PZEM004_T
    pzem_mqtt_init();
    #endif
}

#ifdef CONFIG_USER_WEB_PRINT
static void _user_web_main(http_args_t *args)
{
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    httpd_resp_sendstr_chunk(req, html_block_data_no_header_start);

    user_web_main(req);

    httpd_resp_sendstr_chunk(req, html_block_data_end);
}
#endif

#ifdef CONFIG_USER_WEB_CONFIG
static void _user_web_options(http_args_t *args)
{
    
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;
        
    char *buf = malloc( strlen(html_block_data_header_start) + 20 );
    sprintf(buf, html_block_data_header_start, "User options");
    httpd_resp_sendstr_chunk(req, buf);
    free(buf); 

    user_web_options(req);

    httpd_resp_sendstr_chunk(req, html_block_data_end);
}
#endif

void initialize_modules_http(httpd_handle_t _server)
{
    #ifdef CONFIG_SENSORS_GET
    sensors_http_init(_server);
    #endif

    debug_register_http_print_data();
    wifi_http_init( _server );
    ota_http_init( _server );

    #ifdef CONFIG_RELAY_HTTP
    relay_http_init( _server );
    #endif

    #ifdef CONFIG_IR_RECV_HTTP
    irrcv_http_init( _server, ir_rx );
    #endif

    #ifdef CONFIG_COMPONENT_I2C
    i2c_http_init( _server );
    #endif

    #ifdef CONFIG_COMPONENT_LCD2004_HTTP
    lcd2004_http_init( _server );
    #endif

    // USER HTTP INIT
    user_http_init(NULL);

    #ifdef CONFIG_MCP23017_HTTP
    //((mcp23017_t *) mcp23017_h)->http_buttons = 0b1010100000000000;
    mcp23017_http_init(_server, mcp23017_h);
    #endif

    mqtt_http_init(_server);

    #ifdef CONFIG_SENSOR_SHT21
    sht21_http_init(_server);
    #endif

    #ifdef CONFIG_LED_CONTROL_HTTP
    ledcontrol_http_init(http_server, ledc_h);

        #ifdef CONFIG_RGB_CONTROLLER_HTTP
            rgbcontrol_http_init(_server, rgb_ledc );
        #endif
    #endif   

    #ifdef CONFIG_SENSOR_PZEM004_T_WEB
        pzem_http_init(_server);
    #endif

    #if CONFIG_USER_WEB_PRINT || CONFIG_USER_WEB_CONFIG
    http_args_t *p = calloc(1,sizeof(http_args_t));
    #endif

    #ifdef CONFIG_USER_WEB_PRINT
    register_print_page_block( "user1", PAGES_URI[ PAGE_URI_ROOT], 0, _user_web_main, p, NULL, NULL  );       
    #endif

    #ifdef CONFIG_USER_WEB_CONFIG
    register_print_page_block( "user2", PAGES_URI[ PAGE_URI_CONFIG], 0, _user_web_options, p, user_process_param, NULL  );       
    #endif

}

static void initialize_services(void *arg)
{
    
    

    wifi_init();
    if ( wifi_cfg->mode != WIFI_MODE_AP)
    {
        sntp_start();
    } 
    else 
    {

    }

    webserver_init(&http_server);
    mqtt_init();
    if ( wifi_cfg->mode != WIFI_MODE_AP)
    {
        mqtt_start();
        initialize_modules_mqtt();
    } 
    else 
    {

    }
    
    initialize_modules_http( http_server );

    vTaskDelete(NULL);
}

static void main_debug_print(http_args_t *args)
{
    //ESP_LOGW(TAG, __func__ );
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    // esp_chip_info_t chip_info;
    // esp_chip_info(&chip_info);

    system_info_t *sys_info = malloc(sizeof(system_info_t));
    get_system_info(sys_info);
    char *mac = calloc(1,6);
    wifi_get_mac(mac);
    sys_info->chip_info.chip_id = get_chip_id((uint8_t *)mac);
    free(mac);

    char flash_size[10];
    switch (sys_info->mem_info.flash_size_map)
    {
        case FLASH_SIZE_4M_MAP_256_256:
            strcpy(flash_size, "512 Kb");
            break;
        
        case FLASH_SIZE_2M:
            strcpy(flash_size, "256 Kb");
            break;
                
        case FLASH_SIZE_8M_MAP_512_512:
            strcpy(flash_size, "1 MB");
            break;
                        
        case FLASH_SIZE_16M_MAP_512_512:
            strcpy(flash_size, "2 MB");
            break;       

        case FLASH_SIZE_32M_MAP_512_512:
            strcpy(flash_size, "4 MB");
            break;
        
        case FLASH_SIZE_16M_MAP_1024_1024:
            strcpy(flash_size, "2 MB");
            break;
                
        case FLASH_SIZE_32M_MAP_1024_1024:
            strcpy(flash_size, "4 MB");
            break;
                        
        case FLASH_SIZE_32M_MAP_2048_2048:
            strcpy(flash_size, "4 MB");
            break;                        
            
        case FLASH_SIZE_64M_MAP_1024_1024:
            strcpy(flash_size, "8 MB");
            break; 

        case FLASH_SIZE_128M_MAP_1024_1024:
            strcpy(flash_size, "16 MB");
            break;
        
        default:
            break;
    }
    httpd_resp_sendstr_chunk(req, "CHIP INFO<br>");
    httpd_resp_sendstr_chunk_fmt(req, "Model: %s rev.%d<br>ChipID: %d<br>"
        , (sys_info->chip_info.chip_model == 0) ? "esp8266" : "esp32"
        , sys_info->chip_info.chip_revision
        , sys_info->chip_info.chip_id        
        );

    httpd_resp_sendstr_chunk_fmt(req, 
    "<br>Flash: %s<br>"
    "SPI Flash Size: %s<br>"
    "Compile Size: %d Mb<br>"
    //, sys_info->chip_info.features
    , sys_info->chip_info.features & CHIP_FEATURE_EMB_FLASH ? "Embedded" : "External"
    , flash_size
    , sys_info->mem_info.flash_size / 0x100000 // 1024*1024
    );

    httpd_resp_sendstr_chunk_fmt(req, "Free Heap: %d<br>", sys_info->mem_info.free_heap_size);
    //httpd_resp_sendstr_chunk_fmt(req, "IDF Ver: %s<br>", sys_info->sdk_version);

    free(sys_info);
    
    esp_reset_reason_t reason = esp_reset_reason();
    httpd_resp_sendstr_chunk_fmt(req, "<br>Reset reason: %d (0x%02X) %s"
                , reason, reason, RESET_REASONS[reason]);

    #ifdef CONFIG_COMPONENT_DEBUG
                httpd_resp_sendstr_chunk_fmt(req, "<br>Debug str last: %s", log_rtc_get_debug_str(0));
                httpd_resp_sendstr_chunk_fmt(req, "<br>Debug str prev1: %s", log_rtc_get_debug_str(1));
                httpd_resp_sendstr_chunk_fmt(req, "<br>Debug str prev2: %s", log_rtc_get_debug_str(2));
                httpd_resp_sendstr_chunk_fmt(req, "<br>Debug str prev3: %s", log_rtc_get_debug_str(3));
    #endif
}

void debug_register_http_print_data()
{
    http_args_t *p = calloc(1,sizeof(http_args_t));
    register_print_page_block( "debug", PAGES_URI[ PAGE_URI_DEBUG], 0, main_debug_print, p, NULL, NULL  ); 
}