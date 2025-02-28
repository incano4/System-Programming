#!/usr/bin/env bash

YELLOW='\033[0;93m'
NC='\033[0m'
TEST_DIR='./testdir'
LONG_NAME='QJNbg8GuuD8ptlSR1Y9ZOxhkggGLrsXRgDvU0rrXtM1ChIpOk9oAHvhOqNrW8vDeu9Jc4nYWttNif9yTRZUJs7SRn5fjIvlPGnO1vOkW6STZk4GWzMd5LUW8QmVbHnV0WXogUxeGSloH25ccK5nwHgu91QQTnApJGuzpguJliZ77CtyckIzj6v3DLV5OyjjjjjjcdR4IjoUbo1yvLWVjoc7TMlp6YgesDJKSlA8g3y3jxGPQR8PIE'
# LONG_NAME='QJNbg9XNde2hG8ptlS1YZOhHFHHHkggsXRGGGGASDSADSADSADGgDvU0rrXtM1ChIpOk9oAHvhOqGGGGGGNrW8vDeu9Jc4nYWttNif9yTRZUJs7SRn5fjIvlPGnO1vOkW6STZk4GWzMd5LUW8QmVbHnV0WXogUxeGSloH25ccK5nwHgu91QQTnApJGuzpguJliZ77CtyckIzj6v3DLV5OyjjjjjjjJJJJjjcdR4IjoUbo1yvLPQR8PIE'


make -f Makefile clean
make -f Makefile


echo -e "${YELLOW}Все тесты выполняются в директории ./testdir${NC}"

echo -e "${YELLOW}1. Проверка создания файла:${NC}"
./exec_file -c $TEST_DIR/test_file.txt

echo -e "${YELLOW}2. Проверка удаления существующего файла:${NC}"
./exec_file -d $TEST_DIR/test_file.txt

echo -e "${YELLOW}3. Проверка удаления несуществующего файла:${NC}"
./exec_file -d $TEST_DIR/non_existed_test_file.txt

echo -e "${YELLOW}4. Проверка создания файла по абсолютному пути:${NC}"
./exec_file -c /home/incano/System-Programming/'Task 5'/testdir/absolute_file.txt

echo -e "${YELLOW}5. Проверка создания файла с недопустимыми символами:${NC}"
./exec_file -c $TEST_DIR/test?file.txt

echo -e "${YELLOW}6. Проверка пустой строки в качестве пути:${NC}"
./exec_file -c

echo -e "${YELLOW}7. Проверка использования обоих ключей:${NC}"
./exec_file -c $TEST_DIR/create -d $TEST_DIR/remove

echo -e "${YELLOW}8. Проверка отсутствия ключа:${NC}"
./exec_file $TEST_DIR/no_key

echo -e "${YELLOW}9. Проверка максимальной длины пути:${NC}"
./exec_file -c "$TEST_DIR/$LONG_NAME"
# вывести количество символов для проверки длины:
printf $LONG_NAME | wc -m

echo -e "${YELLOW}10. Проверка создания файла с концовкой на "/":${NC}"
./exec_file -c $TEST_DIR/invalid_file/

echo -e "${YELLOW}11. Проверка буфера на переполнение:${NC}"
# для этого теста поменять значение BUFFER_SIZE в main.h
./exec_file -c $TEST_DIR/buffer_overflow

