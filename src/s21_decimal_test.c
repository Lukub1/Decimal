#include "s21_decimal.h"

#include <check.h>

typedef struct {
  s21_decimal val_1;
  s21_decimal val_2;
  s21_decimal reference;
  int error;
} decimal_test_case;

typedef struct {
  s21_decimal val_1;
  s21_decimal reference;
  int error;
} decimal_other_test_case;

#define DecimalEquality(result, reference)               \
  for (int J = 0; J < 4; J++) {                          \
    ck_assert_int_eq(result.bits[J], reference.bits[J]); \
  }

#define Bits(float) (*(unsigned int *)&float)

decimal_test_case add_test_case[] = {
    {{{0x0000fff1, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000fff1, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0001ffe2, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // Просто сумма

    {{{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0xfffffffe, 0x00000001, 0x00000000, 0x00000000}},
     NORM},  // Сумма с переполнением

    {{{0x0000fff1, 0xffffffff, 0x00000000, 0x00000000}},
     {{0x0000fff1, 0xffffffff, 0x00000000, 0x00000000}},
     {{0x0001ffe2, 0xfffffffe, 0x00000001, 0x00000000}},
     NORM},  // Сумма с переполнение 2го разряда

    {{{0xffafd321, 0x000000af, 0x00000000, 0x00000000}},
     {{0xffafd000, 0x000000af, 0x00000000, 0x80000000}},
     {{0x00000321, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // Сумма с отрицательным

    {{{0xffafd000, 0x000000af, 0x00000000, 0x80000000}},
     {{0x00000321, 0x00000000, 0x00000000, 0x80000000}},
     {{0xffafd321, 0x000000af, 0x00000000, 0x80000000}},
     NORM},  // Сумма отрицательных

    {{{0x0000001e, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000291, 0x00000000, 0x00000000, 0x00020000}},
     {{0x00000e49, 0x00000000, 0x00000000, 0x00020000}},
     NORM},  // Сумма разных степеней

    {{{0x00000291, 0x00000000, 0x00000000, 0x00020000}},
     {{0x0000001e, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000e49, 0x00000000, 0x00000000, 0x00020000}},
     NORM},  // Сумма разных степеней наоборот

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // 0 + 0 = 0

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // 0 + -0 = 0

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // -0 + 0 = 0

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x0000ffff, 0x0000ffff, 0x0000ffff, 0x00000000}},
     {{0x0000ffff, 0x0000ffff, 0x0000ffff, 0x00000000}},
     NORM},  // -0 + x = x

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // Сумма максимального и минус максимального

    {{{0xfffffff0, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x001c0000}},
     {{0xfffffff8, 0xffffffff, 0xffffffff, 0x00000000}},
     NORM},  // Сумма с переполнением и округлением

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     PL_INF},  // Переполнение +бесконечность

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     MIN_INF},  // Переполнение -бесконечность

    // { {{0x82e25aed, 0xf9e825ad, 0xb03ef094, 0x800e0000}},
    //   {{0x481b1f98, 0xfbfd896d, 0x00046d77, 0x80180000}},
    //   {{0xb629b5da, 0xf9ea0c7f, 0xb03ef094, 0x800e0000}},
    //   NORM },  // ошибка при отбрасывании конечных нулей

    {{{0x00000032, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00007530, 0x00000000, 0x00000000, 0x80030000}},
     {{0x00000014, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // проверка округления конечных нулей (50 - 30000е-3 = 20)
};

START_TEST(s21_add_test) {
  s21_decimal result = {};
  int error =
      s21_add(add_test_case[_i].val_1, add_test_case[_i].val_2, &result);
  DecimalEquality(result, add_test_case[_i].reference);
  ck_assert_int_eq(error, add_test_case[_i].error);
}
END_TEST

decimal_test_case sub_test_case[] = {
    {{{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000fff1, 0x00000000, 0x00000000, 0x00000000}},
     {{0xffff000e, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // Просто разность

    {{{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0xfffffffe, 0x00000001, 0x00000000, 0x00000000}},
     {{0xffffffff, 0x00000000, 0x00000000, 0x80000000}},
     NORM},  // Разность с потерей порядка

    {{{0x0001ffe2, 0xfffffffe, 0x00000001, 0x00000000}},
     {{0x0000fff1, 0xffffffff, 0x00000000, 0x00000000}},
     {{0x0000fff1, 0xffffffff, 0x00000000, 0x00000000}},
     NORM},  // Разность с потерей 2х порядков

    {{{0x00000321, 0x000000af, 0x00000000, 0x00000000}},
     {{0xffafd000, 0x00000000, 0x00000000, 0x80000000}},
     {{0xffafd321, 0x000000af, 0x00000000, 0x00000000}},
     NORM},  // Разность с отрицательным

    {{{0xffafd321, 0x000000af, 0x00000000, 0x80000000}},
     {{0x00000321, 0x00000000, 0x00000000, 0x80000000}},
     {{0xffafd000, 0x000000af, 0x00000000, 0x80000000}},
     NORM},  // Разность отрицательных

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // 0 - 0 = 0

    {{{0x0000001e, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000291, 0x00000000, 0x00000000, 0x00020000}},
     {{0x00000927, 0x00000000, 0x00000000, 0x00020000}},
     NORM},  // Разность разных степеней

    {{{0xfffffff8, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x001c0000}},
     {{0xfffffff0, 0xffffffff, 0xffffffff, 0x00000000}},
     NORM},  // Разность от максимума

    // not my test
    {{{0x00000008, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000003, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000005, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000fff, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000794, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000086b, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0x00ffffff, 0x80000000}},
     {{0x87654234, 0x00000567, 0x00000000, 0x00000000}},
     {{0x87654233, 0x00000567, 0x01000000, 0x80000000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0x00ffffff, 0x00000000}},
     {{0x87654234, 0x00000567, 0x00000000, 0x00000000}},
     {{0x789abdcb, 0xfffffa98, 0x00ffffff, 0x00000000}},
     NORM},

    // { {{0xfffffff6, 0xffffffff, 0x00000009, 0x00010000}},
    //   {{0xd401a48e, 0x00000000, 0x00000000, 0x00000000}},
    //   {{0xb7ef926a, 0xfffffff7, 0x00000009, 0x00010000}},
    //   NORM },

    {{{0x54403d8c, 0x8b520559, 0x1fa981ad, 0x800c0000}},
     {{0x65f24dac, 0x0007a9ab, 0x00000000, 0x80060000}},
     {{0xd2927a8c, 0x9e9e7104, 0x1fa98138, 0x800c0000}},
     NORM},

    {{{0x910af4ca, 0xc045d5f2, 0x27e41b00, 0x00150000}},
     {{0x11cdd185, 0xf5bc6fc0, 0x02d50f3f, 0x80100000}},
     {{0xd20de5bf, 0xa74e7221, 0x1c539ded, 0x00110000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x00110000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x000A0000}},
     {{0xB32C07FC, 0x57CC1340, 0x56F358E7, 0x00110000}},
     NORM},

    {{{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000006, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000004, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0x00000002, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000006, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000004, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     PL_INF},  // Переполнение +бесконечность

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     MIN_INF},  // Переполнение -бесконечность
};

START_TEST(s21_sub_test) {
  s21_decimal result = {};
  int error =
      s21_sub(sub_test_case[_i].val_1, sub_test_case[_i].val_2, &result);
  DecimalEquality(result, sub_test_case[_i].reference);
  ck_assert_int_eq(error, sub_test_case[_i].error);
}
END_TEST

decimal_test_case mul_test_case[] = {
    {{{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000ffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0xffff0001, 0x0000fffe, 0x00000000, 0x00000000}},
     NORM},  // Просто произведение

    {{{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // x * 0 = 0

    {{{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // x * -0 = 0

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // 0 * 0 = 0

    {{{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0xffffffff, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000001, 0xfffffffe, 0x00000000, 0x80000000}},
     NORM},  // Произведение с отрицательным

    {{{0x14976758, 0x00000000, 0x00000000, 0x00050000}},
     {{0x000c25c7, 0x00000000, 0x00000000, 0x80040000}},
     {{0xBBC10D68, 0x0000FA22, 0x00000000, 0x80090000}},
     NORM},  // произведение разных с переполнением разряда

    // not my tests!!
    {{{0x0000000a, 0x00000000, 0x0000000a, 0x00010000}},
     {{0x02433305, 0x9701bb81, 0x1c529ce7, 0x00050000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     PL_INF},  // Переполнение в верх обнуляет до 0

    {{{0x0000000a, 0x00000000, 0x0000000a, 0x80010000}},
     {{0x02433305, 0x9701bb81, 0x1c529ce7, 0x00050000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     MIN_INF},  // Переполнение в вниз обнуляет до -0

    // { {{0xffffffff, 0x00000000, 0x00000000, 0x00030000}},
    //   {{0xcccc1234, 0xaaaaaabb, 0x00000000, 0x00080000}},
    //   {{0x3333edcc, 0x22216778, 0xaaaaaabb, 0x000b0000}},
    //   NORM },  // Ошибка при округлении конечных нулей

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x80000000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     PL_INF},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x80110000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x800A0000}},
     {{0x0DD89410, 0xFCA6D602, 0x72D0C9DB, 0x00050000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x00110000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x000A0000}},
     {{0x0DD89410, 0xFCA6D602, 0x72D0C9DB, 0x00050000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x80110000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x000A0000}},
     {{0x0DD89410, 0xFCA6D602, 0x72D0C9DB, 0x80050000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x00110000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x800A0000}},
     {{0x0DD89410, 0xFCA6D602, 0x72D0C9DB, 0x80050000}},
     NORM},
};

START_TEST(s21_mul_test) {
  s21_decimal result = {};
  int error =
      s21_mul(mul_test_case[_i].val_1, mul_test_case[_i].val_2, &result);
  DecimalEquality(result, mul_test_case[_i].reference);
  ck_assert_int_eq(error, mul_test_case[_i].error);
}
END_TEST

decimal_test_case div_test_case[] = {
    {{{0xffff0001, 0x0000fffe, 0x00000000, 0x00000000}},  // #1
     {{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000ffff, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000001, 0xfffffffe, 0x00000000,
       0x80000000}},  // #2 Произведение с отрицательным
     {{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0xffffffff, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  // #3 Сумма с переполнение 2го разряда

    {{{0xBBC10D68, 0x0000FA22, 0x00000000, 0x80090000}},
     {{0x14976758, 0x00000000, 0x00000000, 0x00050000}},
     {{0x000c25c7, 0x00000000, 0x00000000, 0x80040000}},
     NORM},  // #4 Сумма с отрицательным

    {{{0x00000001, 0x00000000, 0x00000000, 0x00000000}},  // #5
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x000186A0, 0x00000000, 0x00000000, 0x00000000}},  // #6
     {{0x0000000a, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00002710, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000004, 0x00000000, 0x00000000, 0x00000000}},  // #7
     {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x3333edcc, 0x22216778, 0xaaaaaabb, 0x00000000}},  // #8
     {{0xffffffff, 0x00000000, 0x00000000, 0x00000000}},
     {{0xcccc1234, 0xaaaaaabb, 0x00000000, 0x00000000}},
     NORM},

    {{{0x499602D2, 0x00000000, 0x00000000, 0x00000000}},  // #9
     {{0x00000005, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0eb79a2a, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000369, 0x00000000, 0x00000000, 0x00020000}},  // #10
     {{0x00000003, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000123, 0x00000000, 0x00000000, 0x00020000}},
     NORM},

    {{{0xCF6B7A15, 0x6BEF84A9, 0x00000028, 0x00100000}},  // #11
     {{0x00000010, 0x00000000, 0x00000000, 0x00000000}},
     {{0x65650D45, 0x83C2E293, 0x000062AF, 0x00140000}},
     NORM},

    {{{0xCF6B7A15, 0x6BEF84A9, 0x00000028, 0x00100000}},
     {{0x8CF31A51, 0x00000455, 0x00000000, 0x00000000}},
     {{0x47803385, 0x7B6F40D1, 0x00000008, 0x001C0000}},
     NORM},

    {{{0xCF6B7A15, 0x6BEF84A9, 0x00000028, 0x00100000}},
     {{0x8CF31A51, 0x00000455, 0x00000000, 0x80000000}},
     {{0x47803385, 0x7B6F40D1, 0x00000008, 0x801C0000}},
     NORM},

    {{{0xCF6B7A15, 0x6BEF84A9, 0x00000028, 0x80100000}},
     {{0x8CF31A51, 0x00000455, 0x00000000, 0x00000000}},
     {{0x47803385, 0x7B6F40D1, 0x00000008, 0x801C0000}},
     NORM},

    {{{0xCF6B7A15, 0x6BEF84A9, 0x00000028, 0x80100000}},
     {{0x8CF31A51, 0x00000455, 0x00000000, 0x80000000}},
     {{0x47803385, 0x7B6F40D1, 0x00000008, 0x001C0000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x00000000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x00000000}},
     {{0xF93DF44C, 0xE69EB74A, 0x328EC239, 0x00150000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x80000000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x00000000}},
     {{0xF93DF44C, 0xE69EB74A, 0x328EC239, 0x80150000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x00000000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x80000000}},
     {{0xF93DF44C, 0xE69EB74A, 0x328EC239, 0x80150000}},
     NORM},

    {{{0xB92F18FC, 0xCB29DA8B, 0xF0EE80AE, 0x80000000}},
     {{0x7C273FEA, 0x565E5286, 0x00000102, 0x80000000}},
     {{0xF93DF44C, 0xE69EB74A, 0x328EC239, 0x00150000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},  // деление на ноль
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     N_NAN},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},  // Деление на -ноль
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     N_NAN},

    {{{0x00000000, 0x00000000, 0x00000000,
       0x00000000}},  // Деление ноль на ноль
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     N_NAN},

    {{{0x00000000, 0x00000000, 0x00000000,
       0x00000000}},  // Деление ноль на -ноль
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     N_NAN},

    {{{0x00000000, 0x00000000, 0x00000000,
       0x00000000}},  // Деленеи ноль на число
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000000, 0x00000000, 0x00000000,
       0x00000000}},  // Деление ноль на - число
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000000, 0x00000000, 0x00000000,
       0x80000000}},  // Деление -ноль на число
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000000, 0x00000000, 0x00000000,
       0x80000000}},  // Деление -ноль на - число
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},
};

START_TEST(s21_div_test) {
  s21_decimal result = {};
  int error =
      s21_div(div_test_case[_i].val_1, div_test_case[_i].val_2, &result);
  DecimalEquality(result, div_test_case[_i].reference);
  ck_assert_int_eq(error, div_test_case[_i].error);
}
END_TEST

decimal_test_case mod_test_case[] = {
    {{{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000000b, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000009, 0x00000000, 0x00000000, 0x00010000}},
     NORM},

    {{{0x00002810, 0x00000000, 0x00000000, 0x00030000}},
     {{0x0000000b, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000164, 0x00000000, 0x00000000, 0x00030000}},
     NORM},

    {{{0x00000002, 0x00000000, 0x00000000, 0x00030000}},
     {{0x0000000b, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x00030000}},
     NORM},

    {{{0xC046A714, 0x000043E6, 0x00000000, 0x00030000}},
     {{0x0000000b, 0x00000000, 0x00000000, 0x00010000}},
     {{0x000001B4, 0x00000000, 0x00000000, 0x00030000}},
     NORM},

    {{{0x9CE50C1C, 0x195CBDE1, 0x00065317, 0x000E0000}},
     {{0x45A70FC2, 0x00000001, 0x00000000, 0x00000000}},
     {{0x69CA8C1C, 0x025742C7, 0x0000730E, 0x000E0000}},
     NORM},

    {{{0x9CE50C1C, 0x195CBDE1, 0x00065317, 0x800E0000}},
     {{0x45A70FC2, 0x00000001, 0x00000000, 0x00000000}},
     {{0x69CA8C1C, 0x025742C7, 0x0000730E, 0x800E0000}},
     NORM},

    {{{0x9CE50C1C, 0x195CBDE1, 0x00065317, 0x000E0000}},
     {{0x45A70FC2, 0x00000001, 0x00000000, 0x80000000}},
     {{0x69CA8C1C, 0x025742C7, 0x0000730E, 0x000E0000}},
     NORM},
};

START_TEST(s21_mod_test) {
  s21_decimal result = {};
  int error =
      s21_mod(mod_test_case[_i].val_1, mod_test_case[_i].val_2, &result);
  DecimalEquality(result, mod_test_case[_i].reference);
  ck_assert_int_eq(error, mod_test_case[_i].error);
}
END_TEST

decimal_other_test_case is_less_test_case[] = {
    {{{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     0},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
     1},

    {{{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     1},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80000000}},
     {{0x0000000f, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     0},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     0},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     0},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     0},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     1},

    {{{0x001485E8, 0x00000000, 0x00000000, 0x00030000}},
     {{0x00000541, 0x00000000, 0x00000000, 0x00000000}},
     0}};

START_TEST(s21_is_less_test) {
  int flag =
      s21_is_less(is_less_test_case[_i].val_1, is_less_test_case[_i].reference);
  ck_assert_int_eq(flag, is_less_test_case[_i].error);
}
END_TEST

decimal_other_test_case is_less_or_equal_test_case[] = {
    {{{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     0},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
     1},

    {{{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     1},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80000000}},
     {{0x0000000f, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     1},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     1},

    {{{0x001485E8, 0x00000000, 0x00000000, 0x00030000}},
     {{0x00000541, 0x00000000, 0x00000000, 0x00000000}},
     1}};

START_TEST(s21_is_less_or_equal_test) {
  int flag = s21_is_less_or_equal(is_less_or_equal_test_case[_i].val_1,
                                  is_less_or_equal_test_case[_i].reference);
  ck_assert_int_eq(flag, is_less_or_equal_test_case[_i].error);
}
END_TEST

decimal_other_test_case is_greater_test_case[] = {
    {{{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
     0},

    {{{0x001485E8, 0x00000000, 0x00000000, 0x00030000}},
     {{0x00000541, 0x00000000, 0x00000000, 0x00000000}},
     0}};

START_TEST(s21_is_greater_test) {
  int flag = s21_is_greater(is_greater_test_case[_i].val_1,
                            is_greater_test_case[_i].reference);
  ck_assert_int_eq(flag, is_greater_test_case[_i].error);
}
END_TEST

decimal_other_test_case is_greater_or_equal_test_case[] = {
    {{{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
     0},

    {{{0x001485E8, 0x00000000, 0x00000000, 0x00030000}},
     {{0x00000541, 0x00000000, 0x00000000, 0x00000000}},
     1}};

START_TEST(s21_is_greater_or_equal_test) {
  int flag =
      s21_is_greater_or_equal(is_greater_or_equal_test_case[_i].val_1,
                              is_greater_or_equal_test_case[_i].reference);
  ck_assert_int_eq(flag, is_greater_or_equal_test_case[_i].error);
}
END_TEST

decimal_other_test_case is_equal_test_case[] = {
    {{{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     0},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
     0},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80120000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80010000}},
     1},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     1},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x001c0000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x001c0000}},
     1},

    {{{0x001485E8, 0x00000000, 0x00000000, 0x00030000}},
     {{0x00000541, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x00000003, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000001E, 0x00000000, 0x00000000, 0x00010000}},
     1},
};

START_TEST(s21_is_equal_test) {
  int flag = s21_is_equal(is_equal_test_case[_i].val_1,
                          is_equal_test_case[_i].reference);
  ck_assert_int_eq(flag, is_equal_test_case[_i].error);
}
END_TEST

decimal_other_test_case is_not_equal_test_case[] = {
    {{{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
     1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     0},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     0},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     0},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
     0},

    {{{0x001485E8, 0x00000000, 0x00000000, 0x00030000}},
     {{0x00000541, 0x00000000, 0x00000000, 0x00000000}},
     0}};

START_TEST(s21_is_not_equal_test) {
  int flag = s21_is_not_equal(is_not_equal_test_case[_i].val_1,
                              is_not_equal_test_case[_i].reference);
  ck_assert_int_eq(flag, is_not_equal_test_case[_i].error);
}
END_TEST

typedef struct {
  float num;
  s21_decimal reference;
  int error;
} decimal_floatTo_test_case;

decimal_floatTo_test_case from_float_to_decimal_test_case[] = {
    {1.0, {{0x00000001, 0x00000000, 0x00000000, 0x00000000}}, NORM},

    {79228162514264337593543950335.1,
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {-79228162514264337593543950335.1,
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     1},

    {1e-29, {{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, 1},

    {-1e-29, {{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, 1},

    {NAN, {{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, 1},

    {INFINITY, {{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, 1},

    {-INFINITY, {{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, 1},

    {1.5, {{0x0000000f, 0x00000000, 0x00000000, 0x00010000}}, NORM},

    {-1.5, {{0x0000000f, 0x00000000, 0x00000000, 0x80010000}}, NORM},

    {0, {{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, NORM},

    {12345678,
     {{0x000BC614E, 0x00000000, 0x00000000, 0x00000000}},
     NORM},  //  Кейс 8 знаков для флоат

    {123.45678,
     {{0x000BC614E, 0x00000000, 0x00000000, 0x00050000}},
     NORM},  //  Кейс 8 знаков для флоат

    {123.125,
     {{0x00001E0F5, 0x00000000, 0x00000000, 0x00030000}},
     NORM},  //  Кейс 8 знаков для флоат

    {0.0123125, {{0x00001E0F5, 0x00000000, 0x00000000, 0x00070000}}, NORM},

    {12345.6789,
     {{0x000BC614F, 0x00000000, 0x00000000, 0x00030000}},
     NORM},  //  Кейс 9 знаков для флоат

    {8e40,
     {{0x000000000, 0x00000000, 0x00000000, 0x00000000}},
     1},  //  Кейс 9 знаков для флоат

    {8e-30,
     {{0x000000000, 0x00000000, 0x00000000, 0x00000000}},
     1},  //  Ошибка с21_див!!
};

START_TEST(s21_from_float_to_decimal_test) {
  s21_decimal result = {};
  int flag = s21_from_float_to_decimal(from_float_to_decimal_test_case[_i].num,
                                       &result);
  DecimalEquality(result, from_float_to_decimal_test_case[_i].reference);
  ck_assert_int_eq(flag, from_float_to_decimal_test_case[_i].error);
}
END_TEST

START_TEST(s21_from_float_to_decimal_NULL_test) {
  ck_assert_int_eq(s21_from_float_to_decimal(1.0, NULL), 1);
}
END_TEST

typedef struct {
  int num;
  s21_decimal reference;
  int error;
} decimal_intTo_test_case;

decimal_intTo_test_case from_int_to_decimal_test_case[] = {
    {10, {{0x0000000a, 0x00000000, 0x00000000, 0x00000000}}, NORM},

    {-10, {{0x0000000a, 0x00000000, 0x00000000, 0x80000000}}, NORM},

    {0, {{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, NORM},

    {12345678, {{0x000BC614E, 0x00000000, 0x00000000, 0x00000000}}, NORM},

    {2147483647, {{0x07FFFFFFF, 0x00000000, 0x00000000, 0x00000000}}, NORM},

    {-2147483647, {{0x07fffffff, 0x00000000, 0x00000000, 0x80000000}}, NORM}};

START_TEST(s21_from_int_to_decimal_test) {
  s21_decimal result = {};
  int flag =
      s21_from_int_to_decimal(from_int_to_decimal_test_case[_i].num, &result);
  DecimalEquality(result, from_int_to_decimal_test_case[_i].reference);
  ck_assert_int_eq(flag, from_int_to_decimal_test_case[_i].error);
}
END_TEST

START_TEST(s21_from_int_to_decimal_NULL_test) {
  ck_assert_int_eq(s21_from_int_to_decimal(1, NULL), 1);
}
END_TEST

typedef struct {
  s21_decimal dec;
  int reference;
  int error;
} decimal_INTTo_test_case;

decimal_INTTo_test_case from_decimal_to_int_test_case[] = {
    {{{0x00000001, 0x00000000, 0x00000000, 0x00000000}}, 1, NORM},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, 0, NORM},

    {{{0x00000088, 0x00000000, 0x00000000, 0x80000000}}, -136, NORM},

    {{{0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000}}, 0, 1},

    {{{0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000}}, 0, 1},

    {{{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80002000}}, 0, 1},

    {{{0x00000000, 0x00000000, 0x00000000, 0x80000000}}, 0, 0},

    {{{0x11111111, 0x00000011, 0x00000000, 0x00080000}}, 733, 0},

    {{{0x11111111, 0x00000111, 0x00000000, 0x80090000}}, -1172, 0},

    {{{0x00000000, 0x00000000, 0x11111111, 0x80020000}}, 0, 1},
};

START_TEST(s21_from_decimal_to_int_test) {
  int result = 0;
  int flag =
      s21_from_decimal_to_int(from_decimal_to_int_test_case[_i].dec, &result);
  ck_assert_int_eq(result, from_decimal_to_int_test_case[_i].reference);
  ck_assert_int_eq(flag, from_decimal_to_int_test_case[_i].error);
}
END_TEST

START_TEST(s21_from_decimal_to_int_NULL_test) {
  s21_decimal nulltestdec = {};
  ck_assert_int_eq(s21_from_decimal_to_int(nulltestdec, NULL), 1);
}
END_TEST

typedef struct {
  s21_decimal dec;
  float reference;
  int error;
} decimal_deciTofloat_test_case;

decimal_deciTofloat_test_case from_decimal_to_float_test_case[] = {
    {{{0x0000000a, 0x00000000, 0x00000000, 0x00000000}}, 10.0, NORM},

    {{{0x0000000a, 0x00000000, 0x00000000, 0x80000000}}, -10.0, NORM},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}}, 0.0, NORM},

    {{{0x000BC614E, 0x00000000, 0x00000000, 0x00000000}},
     12345678,
     NORM},  //  Кейс 8 знаков для флоат

    {{{0x000BC614E, 0x00000000, 0x00000000, 0x00050000}},
     123.45678,
     NORM},  //  Ответы равны но значения всё равно разные =/

    {{{0x00001E0F5, 0x00000000, 0x00000000, 0x00030000}},
     123.125,
     NORM},  //  Кейс 8 знаков для флоат

    {{{0x00001E0F5, 0x00000000, 0x00000000, 0x00070000}}, 0.0123125, NORM},

    {{{0x000BC614F, 0x00000000, 0x00000000, 0x00030000}},
     12345.6789,
     NORM},  //  Кейс 9 знаков для флоат

};

START_TEST(s21_from_decimal_to_float_test) {
  float result = 0.0;
  int flag = s21_from_decimal_to_float(from_decimal_to_float_test_case[_i].dec,
                                       &result);
#ifdef __linux__
  ck_assert_uint_eq(Bits(result),
                    Bits(from_decimal_to_float_test_case[_i].reference));
#elif __APPLE__
  ck_assert_float_eq(result, from_decimal_to_float_test_case[_i].reference);
#endif
  ck_assert_int_eq(flag, from_decimal_to_float_test_case[_i].error);
  s21_decimal max_dec = {0xfffffff, 0xfffffff, 0xfffffff, 0x00000000};
  int error = 1;
  flag = s21_from_decimal_to_float(max_dec, &result);
  ck_assert_int_eq(flag, error);
}
END_TEST

START_TEST(s21_from_decimal_to_float_NULL_test) {
  s21_decimal nulltestdec = {};
  ck_assert_int_eq(s21_from_decimal_to_float(nulltestdec, NULL), 1);
}
END_TEST

decimal_other_test_case floor_test_case[] = {
    {{{0x0000000f, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0xE110C39E, 0x00E1C0B3, 0x00000000, 0x80040000}},
     {{0x7E419033, 0x000005C7, 0x00000000, 0x80000000}},
     NORM},

    {{{0x000000ff, 0x00000000, 0x00000000, 0x80000000}},
     {{0x000000ff, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0xFF42F47D, 0x27D5BD5C, 0x00000004, 0x80120000}},
     {{0x0000004D, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0xFF42F47D, 0x27D5BD5C, 0x00000004, 0x00120000}},
     {{0x0000004C, 0x00000000, 0x00000000, 0x00000000}},
     NORM},
};

START_TEST(s21_floor_test) {
  s21_decimal result = {};
  int error = s21_floor(floor_test_case[_i].val_1, &result);
  DecimalEquality(result, floor_test_case[_i].reference);
  ck_assert_int_eq(error, floor_test_case[_i].error);
}
END_TEST

START_TEST(s21_floor_NULL_test) {
  s21_decimal nulltestdec = {};
  ck_assert_int_eq(s21_floor(nulltestdec, NULL), 1);
}
END_TEST

decimal_other_test_case round_test_case[] = {
    {{{0x00000011, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000010, 0x00000000, 0x00000000, 0x00010000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00002810, 0x00000000, 0x00000000, 0x00030000}},
     {{0x0000000a, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000010, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0x0000000f, 0x00000000, 0x00000000, 0x80010000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0x00000095, 0x00000000, 0x00000000, 0x80020000}},
     {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0x442EF47D, 0x250F326C, 0x00000004, 0x80120000}},
     {{0x0000004C, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0x000000ff, 0x00000000, 0x00000000, 0x80000000}},
     {{0x000000ff, 0x00000000, 0x00000000, 0x80000000}},
     NORM},
};

START_TEST(s21_round_test) {
  s21_decimal result = {};
  int error = s21_round(round_test_case[_i].val_1, &result);
  DecimalEquality(result, round_test_case[_i].reference);
  ck_assert_int_eq(error, round_test_case[_i].error);
}
END_TEST

START_TEST(s21_round_NULL_test) {
  s21_decimal nulltestdec = {};
  ck_assert_int_eq(s21_round(nulltestdec, NULL), 1);
}
END_TEST

decimal_other_test_case truncate_test_case[] = {
    // добавит кейс округления с конечной десяткой!!
    {{{0x000052f2, 0x00000000, 0x00000000, 0x00040000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x1b3d4441, 0x00000000, 0x00000000, 0x00060000}},
     {{0x000001c9, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x000007d0, 0x00000000, 0x00000000, 0x00030000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000000, 0x00000000, 0x00000000, 0x801b0000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x00000003, 0x00000000, 0x00000000, 0x80000000}},
     {{0x00000003, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0x0021e884, 0x00000000, 0x00000000, 0x00060000}},
     {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0x000000ff, 0x00000000, 0x00000000, 0x80000000}},
     {{0x000000ff, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

};

START_TEST(s21_truncate_test) {
  s21_decimal result = {};
  int error = s21_truncate(truncate_test_case[_i].val_1, &result);
  DecimalEquality(result, truncate_test_case[_i].reference);
  ck_assert_int_eq(error, truncate_test_case[_i].error);
}
END_TEST

START_TEST(s21_truncate_NULL_test) {
  s21_decimal nulltestdec = {};
  ck_assert_int_eq(s21_truncate(nulltestdec, NULL), 1);
}
END_TEST

decimal_other_test_case negate_test_case[] = {
    {{{0x0000fff1, 0x00000000, 0x00000000, 0x00000000}},
     {{0x0000fff1, 0x00000000, 0x00000000, 0x80000000}},
     NORM},

    {{{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000}},
     NORM},

    {{{0xffffffff, 0xffffffff, 0xffffffff, 0x801c0000}},
     {{0xffffffff, 0xffffffff, 0xffffffff, 0x001c0000}},
     NORM}};

START_TEST(s21_negate_test) {
  s21_decimal result = {};
  int error = s21_negate(negate_test_case[_i].val_1, &result);
  DecimalEquality(result, negate_test_case[_i].reference);
  ck_assert_int_eq(error, negate_test_case[_i].error);
}
END_TEST

START_TEST(s21_negate_NULL_test) {
  s21_decimal nulltestdec = {};
  ck_assert_int_eq(s21_negate(nulltestdec, NULL), 1);
}
END_TEST

Suite *s21_suite_create(void) {
  Suite *suite = suite_create("s21_decimal_test");
  TCase *tcase_core = tcase_create("Core_of_example");
  tcase_set_timeout(tcase_core, 10);

  int add_test_case_size = sizeof(add_test_case) / sizeof(add_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_add_test, 0, add_test_case_size);

  int sub_test_case_size = sizeof(sub_test_case) / sizeof(sub_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_sub_test, 0, sub_test_case_size);

  int mul_test_case_size = sizeof(mul_test_case) / sizeof(mul_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_mul_test, 0, mul_test_case_size);

  int div_test_case_size = sizeof(div_test_case) / sizeof(div_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_div_test, 0, div_test_case_size);

  int mod_test_case_size = sizeof(mod_test_case) / sizeof(mod_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_mod_test, 0, mod_test_case_size);

  int is_less_test_case_size =
      sizeof(is_less_test_case) / sizeof(is_less_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_is_less_test, 0, is_less_test_case_size);

  int is_less_or_equal_test_case_size = sizeof(is_less_or_equal_test_case) /
                                        sizeof(is_less_or_equal_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_is_less_or_equal_test, 0,
                      is_less_or_equal_test_case_size);

  int is_greater_test_case_size =
      sizeof(is_greater_test_case) / sizeof(is_greater_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_is_greater_test, 0,
                      is_greater_test_case_size);

  int is_greater_or_equal_test_case_size =
      sizeof(is_greater_or_equal_test_case) /
      sizeof(is_greater_or_equal_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_is_greater_or_equal_test, 0,
                      is_greater_or_equal_test_case_size);

  int is_equal_test_case_size =
      sizeof(is_equal_test_case) / sizeof(is_equal_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_is_equal_test, 0,
                      is_equal_test_case_size);

  int is_not_equal_test_case_size =
      sizeof(is_not_equal_test_case) / sizeof(is_not_equal_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_is_not_equal_test, 0,
                      is_not_equal_test_case_size);

  int from_float_to_decimal_test_case_size =
      sizeof(from_float_to_decimal_test_case) /
      sizeof(from_float_to_decimal_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_from_float_to_decimal_test, 0,
                      from_float_to_decimal_test_case_size);
  // tcase_add_test(tcase_core, s21_from_float_to_decimal_NULL_test);

  int from_decimal_to_float_test_case_size =
      sizeof(from_decimal_to_float_test_case) /
      sizeof(from_decimal_to_float_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_from_decimal_to_float_test, 0,
                      from_decimal_to_float_test_case_size);

  int from_decimal_to_int_test_case_size =
      sizeof(from_decimal_to_int_test_case) /
      sizeof(from_decimal_to_int_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_from_decimal_to_int_test, 0,
                      from_decimal_to_int_test_case_size);

  int from_int_to_decimal_test_case_size =
      sizeof(from_int_to_decimal_test_case) /
      sizeof(from_int_to_decimal_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_from_int_to_decimal_test, 0,
                      from_int_to_decimal_test_case_size);

  int floor_test_case_size =
      sizeof(floor_test_case) / sizeof(floor_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_floor_test, 0, floor_test_case_size);

  int round_test_case_size =
      sizeof(round_test_case) / sizeof(round_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_round_test, 0, round_test_case_size);

  int truncate_test_case_size =
      sizeof(truncate_test_case) / sizeof(truncate_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_truncate_test, 0,
                      truncate_test_case_size);

  int negate_test_case_size =
      sizeof(negate_test_case) / sizeof(negate_test_case[0]);
  tcase_add_loop_test(tcase_core, s21_negate_test, 0, negate_test_case_size);

  tcase_add_test(tcase_core, s21_from_int_to_decimal_NULL_test);
  tcase_add_test(tcase_core, s21_from_float_to_decimal_NULL_test);
  tcase_add_test(tcase_core, s21_from_decimal_to_float_NULL_test);
  tcase_add_test(tcase_core, s21_from_decimal_to_int_NULL_test);
  tcase_add_test(tcase_core, s21_negate_NULL_test);
  tcase_add_test(tcase_core, s21_truncate_NULL_test);
  tcase_add_test(tcase_core, s21_round_NULL_test);
  tcase_add_test(tcase_core, s21_floor_NULL_test);

  suite_add_tcase(suite, tcase_core);
  return suite;
}

int main(void) {
  Suite *suite = s21_suite_create();
  SRunner *suite_runner = srunner_create(suite);
  srunner_set_fork_status(suite_runner, CK_NOFORK);
  srunner_run_all(suite_runner, CK_NORMAL);
  srunner_free(suite_runner);
  decimal_other_test_case x = {};
  x.val_1.bits[0] = 0;
  x.reference.bits[0] = 0;
  x.error = 0;
  int n = x.error;
  return n;
}
