# esp-iot-home

Как собрать проект?

Воспользоваться инструкцией по установке и настройке ESP8266_RTOS_SDK
https://github.com/espressif/ESP8266_RTOS_SDK

установить toolchain от бранча release/v3.3 (https://github.com/espressif/ESP8266_RTOS_SDK/tree/release/v3.3)
toolchain 5.2.0

Далее используем master branch

1. клонируем репозиторий актуальной версии
git clone https://github.com/espressif/ESP8266_RTOS_SDK.git

2. Откатываемся на основной рабочий коммит, который я использую (другие коммиты так же имеют место быть, но я в основу на данный момент взял именно этот коммит)
git reset --hard 89caba682505b0dad5e0eaa27cc595135cf38b11

3. обновляем подмодули
git submodule update --recursive
git submodule foreach git reset --hard

