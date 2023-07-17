CC=gcc -std=c11 
CFLAGS=-c -Wall -Wextra -Werror 
DEBUG = -g

all: clean s21_decimal.a test gcov_report

s21_decimal: s21_decimal.o
	@$(CC) $(DEBUG) s21_decimal.o -lm -o s21_decimal
	@./s21_decimal

s21_decimal.o: s21_decimal.c s21_decimal.h
	@$(CC) $(CFLAGS)  s21_decimal.c s21_decimal.h

s21_decimal.a: s21_decimal.o
	@ar -rcs s21_decimal.a s21_decimal.o
	@ranlib $@
	@cp $@ lib$@

clean:
	@rm -rf *.out *.o *.gch  log.txt logTest.txt s21_decimal
	@rm -rf coverage *.a *.gcda *gcno *.info Test *.cfg


OS = $(shell uname)
ifeq ($(OS), Linux)
	LINUX_LIBS=-lsubunit -lrt -lpthread -lm
	CC += -D OS_LINUX -g
	CHECK_LEAKS=valgrind --leak-check=full -s --log-file=log.txt
	OPEN_GCOV=firefox coverage/src/index.html
	OUT_LEAKS= grep log.txt
else
	LINUX_LIBS=
	CC += -D OS_MAC
	CHECK_LEAKS=CK_FORK=no leaks --atExit --
	OPEN_GCOV=open coverage/index.html
endif

test:	clean
	@clang-format -n *.c *.h
	@$(CC) -g s21_decimal.c s21_decimal_test.c -lcheck $(LINUX_LIBS) -o ./Test 
	@$(CHECK_LEAKS) ./Test

style:
	@clang-format -n *.c *h
	
gcov_report: s21_decimal.c s21_decimal_test.c s21_decimal.h
	@$(CC) -fprofile-arcs -ftest-coverage s21_decimal_test.c s21_decimal.c -o Test -lcheck $(LINUX_LIBS) $(GCOV)
	@./Test
	@lcov -c -d . -o coverage.info
	@genhtml coverage.info -o coverage
	@$(OPEN_GCOV)

s21_decimal_test.o: s21_decimal_test.c s21_decimal.h
	@$(CC) $(CFLAGS) s21_decimal_test.c
