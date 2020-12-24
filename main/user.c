#include "user.h"
#include "button.h"

static const char *TAG = "USER";

#ifdef CONFIG_MCP23017_ISR
void test_mcp23017_isr_cb1(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 0, &val);
    val = !val;
        ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 0, val);    
}

void test_mcp23017_isr_cb2(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 1, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);    
    mcp23017_write_pin(mcp23017_h, 1, val);    
}

void test_mcp23017_isr_cb3(char *buf)
{

    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 2, &val);
    val = !val;
        ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 2, val);    
}

void test_mcp23017_isr_cb4(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 3, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);    
    mcp23017_write_pin(mcp23017_h, 3, val);    
}

void test_mcp23017_isr_cb5(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 4, &val);
    val = !val;
        ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 4, val);    
}

void test_mcp23017_isr_cb6(char *buf)
{
    static uint8_t val = 0;

    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;
    mcp23017_read_pin(mcp23017_h, 5, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 5, val);
}

void test_mcp23017_isr_cb7(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 6, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 6, val);
}

void test_mcp23017_isr_cb8(char *buf)
{
    
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 7, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 7, val);
}

#endif

// void test1(char *buf, void *args);
// void test2(char *buf, void *args);

// void test_recv1(char *buf, void *args);
// void test_recv2(char *buf, void *args);

void pressed1_cb();
void pressed2_cb();
void pressed3_cb();
void hold_2s_cb();
void hold_3s_cb();
void hold_10s_cb();

void user_setup(void *args)
{
    ESP_LOGW(TAG, LOG_FMT() );
    
    // if ( softuart_open(0, 9600, 0 /*RX*/, 2 /*TX*/) ) {
    //     ESP_LOGW(TAG, "SoftUart0 opened" );
    // } else {
    //     ESP_LOGE(TAG, "SoftUart0 open ERROR" );
    // }
    //http_handlers_count++;

    #define RELAY_LED_PIN 2
    relay_h = relay_create( "Led", RELAY_LED_PIN, RELAY_LEVEL_HIGH /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ , false);
    relay_write(relay_h,  RELAY_STATE_CLOSE);    

    //button_handle_t btn_g4_h = configure_push_button(GPIO_NUM_4, BUTTON_ACTIVE_HIGH);
    button_handle_t btn_g4_h = configure_push_button(GPIO_NUM_4, BUTTON_ACTIVE_LOW);
    if (btn_g4_h) 
    {
        // регистрируем коллбек короткого нажатия
        #define MAX_SHORT_PRESSED_COUNT 3
        button_cb *short_pressed_cb = calloc(MAX_SHORT_PRESSED_COUNT, sizeof(button_cb));
        // заполним массив указателями на функции
        short_pressed_cb[0] = &pressed1_cb;
        short_pressed_cb[1] = NULL; //&pressed2_cb; // NULL если не требуется обрабатывать 2-ое (n-Раз) нажатие
        short_pressed_cb[2] = &pressed3_cb;

        // 1..3 коротких нажатий в течение 500 мсек
        button_set_on_presscount_cb(btn_g4_h, 500, MAX_SHORT_PRESSED_COUNT, short_pressed_cb);

        // сработает при отпускании после 1 сек не зависимо сколько держали по времени
        #define BTN_HOLD_1_SEC 2
        button_add_on_release_cb(btn_g4_h, BTN_HOLD_1_SEC, hold_2s_cb, NULL);
        
        // сработает при удержании более 2 сек
        #define BTN_HOLD_2_SEC 3
        button_add_on_press_cb(btn_g4_h, BTN_HOLD_2_SEC, hold_3s_cb, NULL);        

        // сработает при удержании более 10 сек
        #define BTN_HOLD_10_SEC 10
        button_add_on_press_cb(btn_g4_h, BTN_HOLD_10_SEC, hold_10s_cb, NULL);
    }
}

void user_http_init(void *args)
{
    // функция вызывается после user_setup и старта webserver'a
    ESP_LOGW(TAG, LOG_FMT() );

    #ifdef CONFIG_MCP23017_HTTP
    mcp23017_http_set_btn_name(mcp23017_h, 15, "Кнопка 1");
    mcp23017_http_set_btn_name(mcp23017_h, 13, "Кнопка 2");
    mcp23017_http_set_btn_name(mcp23017_h, 11, "Кнопка 3");
    #endif

    #ifdef CONFIG_LED_CONTROL_HTTP
    //ledcontrol_http_add_group(ledc_h, "RGB Controller", 1, 5);
    ledcontrol_http_add_group(ledc_h, "Белая подсветка", 2, 6);
    #endif    

    #ifdef CONFIG_COMPONENT_MCP23017
        //mcp23017_test_task(mcp23017_h);
        #ifdef CONFIG_MCP23017_ISR
        // 1 - сразу при нажатии
        // 2 - только после отпускания
        mcp23017_isr_handler_add(mcp23017_h, 15, 2, test_mcp23017_isr_cb8, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 14, 1, test_mcp23017_isr_cb7, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 13, 1, test_mcp23017_isr_cb6, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 12, 1, test_mcp23017_isr_cb5, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 11, 1, test_mcp23017_isr_cb4, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 10, 1, test_mcp23017_isr_cb3, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 9, 2, test_mcp23017_isr_cb2, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 8, 2, test_mcp23017_isr_cb1, mcp23017_h);
        #endif
    #endif
}

// функция вызывается после user_setup и старта mqtt
// в этой функции можно зарегистрировать свои кастомные колбеки на отправку и получение данных
void user_mqtt_init(void *args)
{
    ESP_LOGW(TAG, LOG_FMT() );

    //mqtt_add_periodic_publish_callback( "test1", test1, NULL);
    //mqtt_add_periodic_publish_callback( "test2", test2, NULL);

    //mqtt_add_receive_callback("recv1", 1, test_recv1, NULL);
    //mqtt_add_receive_callback("recv2", 1, test_recv2, NULL);


}

// функция вызывает в основном цикле каждую секунду
void user_loop(uint32_t sec)
{
    //int level = gpio_get_level(GPIO_NUM_4);
    //ESP_LOGW( TAG, "gpio4 = %d", level);
    // // Set Addr: B4 C0 A8 01 01 00 1E
    // uint8_t cmd[7] = { 0xB4, 0xC0, 0xA8, 0x01, 0x01, 0x00, 0x1E};
    
    // //if ( softuart_puts(0, cmd)) 
    // if ( softuart_write_bytes(0, cmd, 7)) 
    // {
    //     ESP_LOGW(TAG, "SoftUart0 softuart_puts OK" );
    // } 
    // else 
    // {
    //     ESP_LOGE(TAG, "SoftUart0 softuart_puts ERROR" );
    // }
    
    // //pauseTask(50);
    
    // uint8_t readed = 0;
    // char buf[7];
    // readed = softuart_read_buf(0, buf, 7);


    // if ( readed == 0 ) 
    // {
    //     ESP_LOGE(TAG, "SoftUart0 no data available" );
    // } 
    // else 
    // {
    //     ESP_LOGW(TAG, "SoftUart0 readed %d, buf: ", readed );
    //     for ( uint8_t i=0; i < readed; i++)
    //     {
    //         ESP_LOGW(TAG, "0x%02X ", buf[i] );
    //     }
        
    // }

        #if CONFIG_SENSOR_SHT21 && CONFIG_COMPONENT_LCD2004

            char s[20];
            //char *s = (char *) calloc( 20 + 1, sizeof(char*));
            //memset(s, 0, 20 + 1);

            if ( xSemaphoreLCD2004 != NULL && xSemaphoreTake( xSemaphoreLCD2004, I2C_SEMAPHORE_WAIT ) == pdTRUE ) 
            {
                sprintf(s, "Tmp: %2.1f Hum: %2.1f", sht21_get_temp(), sht21_get_hum());
                lcd2004_print(1, s );

                //lcd2004_set_cursor_position( 1, 1);
                //lcd2004_print_string( s );

                //memset(s, 0, 20 + 1);
                sprintf(s, "Freemem: %5d", esp_get_free_heap_size());
                lcd2004_print(2, s );

                //memset(s, 0, 20 + 1);
                get_uptime(s);
                lcd2004_print(3, s );
                //lcd2004_set_cursor_position( 1, 2);
                //lcd2004_print_string( s );
                //free(s);
                
                xSemaphoreGive( xSemaphoreLCD2004 );
            }           
        #endif    
}

#ifdef CONFIG_USER_WEB_PRINT
// функция вывод данные в пользовательском блоке на главной
void user_web_main(httpd_req_t *req)
{
    ESP_LOGW(TAG, LOG_FMT() );

    //httpd_resp_sendstr_chunk(req, "Hello!");
    //USER_WEB_PRINT("Hello User!");
    //USER_WEB_PRINT("Hello User1!");
    //USER_WEB_PRINT("Hello User2!");
    //USER_WEB_PRINT("Hello User3!");

    //char data[20];
    //sprintf(data, "<br>Temp: %2.1f", sht21_get_temp());
    //USER_WEB_PRINT(data);
    //USER_WEB_PRINT("<br>");

    //mcp23017_print_button(mcp23017_h, req, "mcp13", 13);
    //relay_print_button(req, "btn1", 1);
    //relay_print_button(req, "btn2", 2);
    //relay_print_button(req, "btn3", 3);
}
#endif 

#ifdef CONFIG_USER_WEB_CONFIG
void user_web_options(httpd_req_t *req)
{
    ESP_LOGW(TAG, LOG_FMT() );
       
    //USER_WEB_PRINT("Hello User Options!");
    //USER_WEB_PRINT("Hello User1 Options!");
    //USER_WEB_PRINT("Hello User2 Options!");
    //USER_WEB_PRINT("Hello User3 Options!");

}


void user_process_param(httpd_req_t *req, void *args)
{
    ESP_LOGW(TAG, LOG_FMT() );
    //ESP_LOGW(TAG, "Hello User process param!");
}
#endif

//mqtt_add_periodic_publish_callback( const char *topic, func_mqtt_send_cb fn_cb);
// void mqtt_add_receive_callback( const char *topic, func_mqtt_recv_cb fn_cb); -
/*
void test1(char **buf, void *args) {
    static uint32_t cnt = 0;
    itoa(cnt++, *buf, 10);
}

void test2(char **buf, void *args){
    static uint32_t cnt = 1000000;
    sprintf(*buf, "%d", cnt);
    cnt -= 10;
    if ( cnt == 0) cnt = 1000000;
}

void test_recv1(char *buf, void *args)
{
    ESP_LOGI(TAG, "received topic 'recv1' with data: %s", buf);
}

void test_recv2(char *buf, void *args)
{
    ESP_LOGI(TAG, "received topic 'recv2' with data: %s", buf);
}
*/

void IRAM_ATTR pressed1_cb()
{
    ESP_LOGW(TAG, LOG_FMT() );
}

void IRAM_ATTR pressed2_cb()
{
    ESP_LOGW(TAG, LOG_FMT() );
}

void IRAM_ATTR pressed3_cb()
{
    ESP_LOGW(TAG, LOG_FMT() );
}

void hold_2s_cb()
{
    ESP_LOGW(TAG, LOG_FMT() );
}

void hold_3s_cb()
{
    ESP_LOGW(TAG, LOG_FMT() );
}

void hold_10s_cb()
{
    ESP_LOGW(TAG, LOG_FMT() );
}
