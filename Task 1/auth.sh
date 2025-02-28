#!/usr/bin/env bash

# define user to check his login
USER=$1

# define arrays and variables
NEW_SESSION=()
SESSION_ID=()
SESSION_START=()
SESSION_CLOSE=()

YELLOW='\033[0;93m'
NC='\033[0m' # No Color
num=0
# data from journalctl
# journalctl -u 'systemd-logind' | awk ' {print  $1,$2,$3...} '
# $1 = Sep
# $2 = 07
# $3 = 11:52:07
# $6 = new/removed 
# $8 = 12 # номер сессии
# $11 = incano


# creating array that contains info about new session
while IFS= read -r line; do
    NEW_SESSION+=("$line")
done < <(journalctl -u 'systemd-logind' | awk '{print $1,$2,$3,$6,$7,$8,$11}' | grep $USER | tail -3)

# array with session id
while IFS= read -r line; do
    SESSION_ID+=("$line")
done < <(journalctl -u 'systemd-logind' | grep $USER | awk '{print $8}' | tail -3)


# array with time and date and time of session start (log into system)
while IFS= read -r line; do
    SESSION_START+=("$line")
done < <(journalctl -u 'systemd-logind' | grep $USER | awk '{print $1,$2,$3}' | tail -3)


# define log out time and date for each session with same id
for i in "${SESSION_ID[@]}"; do
    ID=$i
    while IFS= read -r line; do
        SESSION_CLOSE+=("$line")
    done < <(journalctl -u 'systemd-logind' | grep "Removed session $ID\."| awk '{print $1,$2,$3}' | tail -1)
done

# define end of the session for each id
for i in "${SESSION_ID[@]}"; do
    ID=$i
    while IFS= read -r line; do
        REMOVED_SESSION+=("$line")
    done < <(journalctl -u 'systemd-logind' | grep "Removed session $ID\." | awk '{print  $1,$2,$3,$6,$7,$8,$11}' | tail -1)
done


# compare arrays with time and date
for i in "${!SESSION_START[@]}"; do

    # data to unix-time
    time_login=$(date -d "${SESSION_START[i]}" +%s)
    time_logout=$(date -d "${SESSION_CLOSE[i]}" +%s)

    # compare unix-time
    if (( time_login > time_logout )); then
        ID=${SESSION_ID[i]}
        REMOVED_SESSION[i]="${YELLOW}Session $ID is active now.${NC}"
    fi
done


if [ ${#SESSION_CLOSE_TIME[@]} -ne 3 ]; then
    REMOVED_SESSION+=("Session $ID is active now")
fi


for i in "${NEW_SESSION[@]}"; do
    echo -n "$i "
    echo -e "${REMOVED_SESSION[num]}"
    num=$(($num+1))
done

echo



# а че если таких номеров сессий еще не будет? что тогда? пока что сессия 1-ххх повторяются, 
# но когда дойдем до 75, что произойдет?