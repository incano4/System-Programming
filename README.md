# System-Programming
### Задание 1.
``` auth.sh ``` — Вывод информации о входе/выходе пользователя из системы. Если пользователь все еще в системе — вывод информации об этом. 
``` creating_user.sh ``` — Создание пользователя без использования useradd. Опционально можно сделать пользователя администратором, задать пароль и выбрать используемую оболочку.

### Задание 2. Создание бэкапов
``` backup.sh ``` — Скрипт, позволяющий создавать архивы. 
``` archive.yaml ``` — Конфигурационный файл, в котором описываются файлы, подлежащие архивированию, директория, в которую сохраняется архив и максимально допустимое количество архивов в данной директории. Поддерживает как относительные, так и абсолютные пути.
Запуск: ```./backup.sh [-a|--algorithm] <gz|bz|xz|zstd> <path_to_archive.yaml>```. При отсутствии ключа, будет использован .gz-формат.

### Задание 3. IPC
``` task1 ``` — Запускает дочерний процесс в фоновом режиме, после чего устраняет его.
``` task1_child ``` — Дочерний процесс, работающий в фоновом режиме, пока его не завершит родительский процесс.
``` task2 ``` — Перехватчик сигналов. Выводит информацию о том, что определенный сигнал был послан данному процессу.

### Задание 4. Пересборка ядра
Необходимо пересобрать ядро и установить его более новую версию.
