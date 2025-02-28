#!/usr/bin/env bash

function create_user {
    echo "${USER}:x:${MINIMUM_UID}:${MINIMUM_UID}::/home/${USER}:${USER_SHELL}" >> /etc/passwd
    # 99999 - через сколько истечет пароль, 7 - дней, за сколько предупредить юзера
    echo "${USER}:${PASSWORD}:${LAST_CHANGE}:0:99999:7:::" >> /etc/shadow
    echo "${USER}:x:$MINIMUM_UID:" >> /etc/group
    mkdir -p /home/${USER}
    chown $USER /home/${USER}
}

function password_hash {
    salt=$(openssl rand -base64 16)
    PASSWORD=$(openssl passwd -salt ${salt} -6 ${PASSWORD})

    # устанавливаем последнее время изменения пароля (по сути, просто указываем, когда создали)
    LAST_CHANGE="$(date +%s)"
    LAST_CHANGE=$(($LAST_CHANGE/86400))
}

function shell_select {
    SHELLS=(sh bash rbash dash)
    echo "Выберете оболочку"
    num=1
    for i in ${SHELLS[@]}; do
        # делаем строку "красивой" - выравниваем до 8 символов, чтобы было одинаковое расстояние между колоннами
        printf '%-8s\n' "[$(( num++ ))]$i" 
    done | column
    read -p "[1-4]: " var

    case $var in
    1)
        USER_SHELL=/bin/sh
        # echo $USER_SHELL
        ;;
    2)
        USER_SHELL=/bin/bash
        ;;
    3)
        USER_SHELL=/bin/rbash
        ;;
    4)
        USER_SHELL=/bin/dash
        ;;
    *)
        shell_select
        ;;
    esac
}


read -p "Введите имя пользователя: " USER;


# проверка наличия такого имени пользователя
USER_EXIST=$(grep ^$USER: -c /etc/passwd) # ^ и двоеточие, чтобы выбрать имя полностью, и не было частичных совпадений

if [ $USER_EXIST = 1 ]; then
    echo "Пользователь с таким именем уже существует"
    exit 0
fi


read -p "Придумайте пароль. Чтобы не создавать пароль, нажмите Enter: " PASSWORD;
read -p "Пользователь будет администратором? [y/n]: " ADMIN;


if [ $ADMIN = y ]; then
    if [ -z "$PASSWORD" ]; then
        echo "У администратора должен быть пароль"
        exit 0
    else
        sed -i "/^sudo:x:/ s/$/,$USER/"  /etc/group
    fi
fi


# вычисляем ближайший доступный UID
MINIMUM_UID=1000
UID_CHECK=$(grep $MINIMUM_UID -c /etc/passwd)


while [ $UID_CHECK = 1 ]; do 
    MINIMUM_UID=$(($MINIMUM_UID + 1))
    UID_CHECK=$(grep $MINIMUM_UID -c /etc/passwd)
done


if [ -n "$PASSWORD" ]; then
    password_hash
fi

shell_select
create_user