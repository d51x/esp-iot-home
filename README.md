# esp-iot-home

Как собрать проект?

Воспользоваться инструкцией по установке и настройке ESP8266_RTOS_SDK
https://github.com/espressif/ESP8266_RTOS_SDK

установить toolchain от бранча release/v3.3 (https://github.com/espressif/ESP8266_RTOS_SDK/tree/release/v3.3)
toolchain 5.2.0

Далее используем master branch

1. клонируем репозиторий актуальной версии
git clone https://github.com/espressif/ESP8266_RTOS_SDK.git

2. Откатываемся на основной рабочий коммит, который я использую (другие коммиты так же имеют место быть, но я в основу на данный момент взял именно этот коммит).
Данный коммит является последним для тулчейна 5.2.0, после этого SDK перевели на тулчейн 8.4.0 (но у меня пока с ним не взлетела сборка)

git reset --hard db10c3cbceb435ba99edcc1ef7886fdc4b5064d8

Будет использоваться для сборки проекта IDF v3.4-dev-340-gdb10c3cb-dirty

3. обновляем подмодули
git submodule update --recursive
git submodule foreach git reset --hard

