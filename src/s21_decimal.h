#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <stddef.h>
#include <stdio.h>
typedef enum {
  NORM = 0,
  PL_INF = 1,
  MIN_INF = 2,
  N_NAN = 3,
} value_type_t;

typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  s21_decimal main_value;
  s21_decimal extanded_value;
} s21_decimal_ext;

typedef struct {
  unsigned int bits[8];
} decimal_extended;

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
int s21_normalize(decimal_extended *ext_1, decimal_extended *ext_2);
int change_places(decimal_extended *value_1, decimal_extended *value_2);
int get_bit(unsigned int x, int position);
int setBit(unsigned int value, int position, int bit);
int get_ten(unsigned int value);
int get_pos_high_bit(decimal_extended value);
int is_null(s21_decimal value);
int ext_is_null(decimal_extended value);
s21_decimal *to_null(s21_decimal *value);
int get_bit_full(decimal_extended val, int bit);
int my_is_greater(s21_decimal value_1, s21_decimal value_2);
int ext_is_greater_or_equal(decimal_extended extended_1,
                            decimal_extended extended_2);
decimal_extended ext_left_shift_by_count(decimal_extended ext_value, int count);
decimal_extended core_add(decimal_extended extended, decimal_extended extended2,
                          decimal_extended *extended_r);
decimal_extended core_sub(decimal_extended extended, decimal_extended extended2,
                          decimal_extended *extended_r);
decimal_extended core_mul(decimal_extended value_1, decimal_extended value_2,
                          decimal_extended *res);
decimal_extended core_div(decimal_extended extended, decimal_extended extended2,
                          decimal_extended *extended_r);
decimal_extended core_mod(decimal_extended value_1, decimal_extended value_2,
                          decimal_extended *res);
decimal_extended ext_right_shift_by_count(decimal_extended value, int count);
decimal_extended div_by_ten(decimal_extended *value);
int banking_round(decimal_extended *value, int *stepen);
int check_last_null(decimal_extended *value, int stepen);
s21_decimal decimal_to_result(decimal_extended value);
double convert_dec_to(s21_decimal *src, int *error);
int give_scale_factor(int bit);
int is_exception(float *value);
s21_decimal my_round(s21_decimal x);

#endif  // SRC_S21_DECIMAL_H_
