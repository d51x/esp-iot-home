# esp-iot-home

# Как собрать проект?

Воспользоваться инструкцией по установке и настройке ESP8266_RTOS_SDK
https://github.com/espressif/ESP8266_RTOS_SDK

установить toolchain от бранча release/v3.3 (https://github.com/espressif/ESP8266_RTOS_SDK/tree/release/v3.3)
toolchain 5.2.0

Далее используем master branch

### 1. клонируем репозиторий актуальной версии
*git clone https://github.com/espressif/ESP8266_RTOS_SDK.git*

### 2. Откатываемся на основной рабочий коммит
я использую этот коммит(другие коммиты так же имеют место быть, но я в основу на данный момент взял именно этот коммит).
Данный коммит является последним для тулчейна 5.2.0, после этого SDK перевели на тулчейн 8.4.0 (но у меня пока с ним не взлетела сборка)

*git reset --hard db10c3cbceb435ba99edcc1ef7886fdc4b5064d8*

### 3. Применяем разные важные фиксы (был поломан pwm и чтение adc что-то портило)

**3.1 bugfix(pwm): support fractional phase**

*git cherry-pick -x 72cbf0d55ebc27c0707e868338fe1867d3a38d6d*

после этого может потребоваться разрешить конфликты, а именно, пойти и поправить файл components/esp8266/driver/pwm.c

далее,
*git add components/esp8266/driver/pwm.c*
*git commit*

**3.2 bugfix(pwm_driver_error): PWM cannot drive GPIO properly because there is no...**

*git cherry-pick -x -m 1 61c3c1154e6e6901dc1f2cb6dd67d71fb805a86d*

**3.3 fix(adc): fix rf state error when read adc**

*git cherry-pick -x -m 1 7f99618d9e27a726a512e22ebe81ccbd474cc530*

**3.4 fix(freertos): disable other task before disable nmi**

*git cherry-pick -x -m 1 ea4f93b7278ddb786ec5c8697a51e8d24e59d484 *

Будет использоваться для сборки проекта 
**IDF v3.4-dev-344-g0a918648-dirty**

### 4. обновляем подмодули

git submodule update --recursive

git submodule foreach git reset --hard

**4.1 обновление esp-mqtt до одной из крайних версий**

*cd ESP8266_RTOS_SDK/components/esp-mqtt*

*git checkout master*

*git reset --hard 9ea804e0ab5368d5ab53ae2301a5fec9d1f12f1a *