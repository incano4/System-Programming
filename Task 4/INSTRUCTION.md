## Задание 1. Пересборка ядра
Необходимо узнать текущую версию ядра при помощи команды ``` uname -a ```, после чего поставить последнее стабильное ядро (в данном случае, 6.11.6). После чего загрузиться с него и подтвердить успех при помощи ``` uname -a ```.
<br> <br>
## Ход выполнения задания
Первым делом скачиваем архив с ядром с зеркала либо к себе на хост, либо сразу на виртуальную машину при помощи wget. Также потребуется файл с цифровой подписью, чтобы проверить целостность архива.
```
https://mirrors.edge.kernel.org/pub/linux/kernel/v6.x/linux-6.11.6.tar.xz
https://mirrors.edge.kernel.org/pub/linux/kernel/v6.x/linux-6.11.6.tar.sign
```

Если файлы были скачаны на хост, то переместить их можно при помощи команды scp, которая имеет следующий синтасис:
``` 
scp "<path_to_file>" <username>@<ip-address>:~/
```
Дальнейшая работа проводится на виртуальной машине. Извлекаем полученныые файлы из архива и проверяем.
```
unxz -v linux-6.11.6.tar.xz 
gpg --verify linux-6.11.6.tar.sign
```
Если по итогу получаем в выводе "Can't check signature: No public key", то выполняем ``` gpg --recv-keys <key> ```, после чего снова проводим верификацию.
![image](https://github.com/user-attachments/assets/fbd91cb3-efa6-4b53-9c6b-81b02827028b)
<br><br>
Если проблема осталась, то дополнительно устанавливаем ключи, после чего проблема должна решиться.
<br>
![image](https://github.com/user-attachments/assets/66648c70-ad53-4cfa-a06b-199f62753ecb)
<br><br>
Установка ключей: ``` gpg --locate-keys torvalds@kernel.org gregkh@kernel.org ```
<br>
![image](https://github.com/user-attachments/assets/804a7917-06cb-49a3-a4fd-4c9a9a61e7cd)
<br><br>

В случае успешной проверки распакавывем архив: ``` tar xvf linux-6.11.6.tar ``` и устанавливаем дополнительные пакеты, которые позволят осуществить сборку ядра.
``` 
sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev -y
```
### На этом этапе стоит выключить виртуальную машину, сделать снапшот и по возможности добавить ресурсов. Рекомендуемое количество ядер — 3, памяти — 8 Гб.
<br>
Далее необходимо в папку с ядром (linux-6.11.6) копировать файл конфигурации из директории /boot, однако можно взять ее с данного репозитория, поскольку она является преднастроенной.

Также необходимо создать сертификаты и поместить их в определенную директорию, указанную в конфигурационном файле. Сделать это можно при помощи команды:
``` 
openssl req -x509 -newkey rsa:4096 -keyout certs/mycert.pem -out certs/mycert.pem -nodes -days 3650
```

Далее можно начинать сборку ядра. Число, указанное вместе с параметром -j показывает количество потоков, которые будут зайдествованы в процессе. Ставим число, равно количеству ядер, умноженное на 2.
Параметр -s позволяет не выводить лишнюю информацию в консоль и отображает только ошибки и предупреждения. В случае, если будет предложено поставить новые модули - отказываемся,
в случае предложения о выборе одного модуля из нескольких - ставим тот, что указан по умолчанию.
``` 
make -s -j6
```

После успешной компиляции устанавливаем модули ядра и само ядро. В выводе будет показана новая установленная версия.
```
sudo make modules_install
sudo make install
```

Опционально, можно обновить initramfs до установленной версии ядра и загрузчик (grub):
```
sudo update-initramfs -c -k 6.11.6
sudo update-grub
```
<br>

Последним шагом необходимо перезапустить виртуальную машину и убедиться в том, что установилось ядро новой версии (``` uname -a ```).