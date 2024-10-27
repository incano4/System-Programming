#!/usr/bin/env bash

# $1 = -a or --algorithm
# $2 = algorithm type (.bz, .gz, .xz, .zstd)
# $3 = /path_to_conf_file

# в конф файле указана директория, список файлов для бэкапов, максимальное число для бэкапов (>5)
# путь до конф. файла должен работать как абсолютный, так и относительный
# target dir тоже как абсолютный, так и относительный
# бэкапы должны куда-то сохраняться
# если храним 5 бэкапов, а их уже 6, то создать их, а потом удаляем самые старые
# если в src dir будет еще одна директория, то иерархию надо сохранить


# кратко про ключи tar:
# -с create archive
# -f --file=ARCHIVE
# -j use bzip2
# -J use xz, or
    # --xz
    # --zstd
# -v print objects that u archive
# -z use gzip

# to unpack archives:
    # bz2: tar -xvjf archive.tar.bz2 
    # gz: tar -xzf archive.tar.gz
    # xz: tar -xpJf archive.tar.xz
    # zstd: tar --zstd -xpf archive.tar.zstd

# ---------------------------------------------------------------------------

# define variables
KEY=$1
ALG_TYPE=$2
CONFIG_YAML=$3
FILES=()
CREATION_TIME=()
CREATION_TIME_UNIX=()
ARCHIVE_LIST=()
MERGED=()

function parse_yaml {
    # processing data between 
    TARGET_DIR=$(awk '/target:/{check=1;next}/backup:/{check=0}check' "$CONFIG_YAML" | grep "directory" | awk '{print $2}')

    # remove "- " from string and parse elements into string
    TARGET_LIST=$(awk '/list:/{check=1;next}/backup:/{check=0}check' "$CONFIG_YAML" | grep "^ *- " | sed 's/^ *- //')
    IFS=$'\n' read -r -d '' -a list_array <<< "$(echo -e "$TARGET_LIST\n")"
    # replace end of the string with space and remove end-of-the-line-symbol 
    # LIST=$(echo "$TARGET_LIST" | tr '\n' ' ' | sed 's/ $//')

    # get ammount of backups and directory (, = between 1st and 2nd construction)
    BACKUP_NUMBER=$(awk '/backup:/,/number:/ {if ($1 == "number:") print $2}' "$CONFIG_YAML")
    BACKUP_DIR=$(awk '/backup:/,/directory:/ {if ($1 == "directory:") print $2}' "$CONFIG_YAML")
}


function find_files {
    if [ "${TARGET_LIST}" = "*" ]; then
        FILES=$(find "$TARGET_DIR" -name "*")" "
    else
        for i in ${TARGET_LIST[@]}; do
        FILES+="$TARGET_DIR/$i "
        done
    fi
}


function creating_archive {
    NAME=$(date +"%Y-%m-%d-%H-%M-%S")
        case $ALG_TYPE in
        bz)
            # tar -xvjf 
            tar -cjvf "${BACKUP_DIR}/${NAME}.tar.bz2" ${FILES[@]}
            # tar -cjvf "${BACKUP_DIR}/${NAME}.tar.bz2" $TARGET_DIR/$LIST
            # tar -cjvf /home/incano/hw2/$NAME.tar.bz2 dir file2
            ;;
        gz)
            tar -czf "${BACKUP_DIR}/${NAME}.tar.gz" ${FILES[@]}
            # tar -czf /home/incano/hw2/$NAME.tar.gz file1 file2
            ;;
        xz)
            tar -cpJf "${BACKUP_DIR}/${NAME}.tar.xz" ${FILES[@]}
            # tar -czf /home/incano/hw2/$NAME.tar.xz file1 file2
            ;;
        zstd)
            tar --zstd -cpf "${BACKUP_DIR}/${NAME}.tar.zstd" ${FILES[@]}
            # tar -czf /home/incano/hw2/$NAME.tar.zstd file1 file2
            ;;
        *)
            exit 0
            ;;
        esac
}


function check_archive {
    # get creation time of all files in backup dir
    while IFS= read -r file; do
       CREATION_TIME+=("$(stat "$file" | grep Birth | awk '{print $2,$3}')")
       ARCHIVE_LIST+=($file)
    done < <(find "$BACKUP_DIR" -type f -name "*.tar.*")

    # time to unix-time
    for i in "${!CREATION_TIME[@]}"; do
        CREATION_TIME_UNIX+=("$(date -d "${CREATION_TIME[i]}" +%s)")
    done

    # match filenames with creation time 
    for i in "${!CREATION_TIME_UNIX[@]}"; do
        MERGED+=("${CREATION_TIME_UNIX[$i]} ${ARCHIVE_LIST[$i]}")
    done

    # reverse sort by numbers
    SORTED=$(printf "%s\n" "${MERGED[@]}" | sort -rn)

    CREATION_TIME_UNIX=()
    ARCHIVE_LIST=()

    # split back time and filename
    while IFS= read -r line; do
        time=$(echo "$line" | awk '{print $1}')
        files=$(echo $line | awk '{print $2}')
        CREATION_TIME_UNIX+=("$time")
        ARCHIVE_LIST+=("$files")
    done <<< "$SORTED"

    # get filename that should be removed
    if [ "${#ARCHIVE_LIST[@]}" -gt "$BACKUP_NUMBER" ]; then
        REMOVE_FILES=(${ARCHIVE_LIST[@]:$BACKUP_NUMBER})
    fi

    for i in "${REMOVE_FILES[@]}"; do
        rm -rf $i
    done
}




if [ "$KEY" != "-a" ] && [ "$KEY" != "--algorithm" ]; then
    # exit 0
    # KEY=$1
    CONFIG_YAML=$KEY
    # CONFIG_YAML=$3
    ALG_TYPE=gz
fi

parse_yaml
find_files
creating_archive
check_archive




echo "Target Directory: $TARGET_DIR"
echo "Target List: $TARGET_LIST"
echo "Backup Number: $BACKUP_NUMBER"
echo "Backup Directory: $BACKUP_DIR"

# echo $FILES
# echo ${FILES[@]}
# echo ${#FILES[@]}
