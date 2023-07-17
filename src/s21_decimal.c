#include "s21_decimal.h"

#include <math.h>

decimal_extended from_decimal_to_extended(decimal_extended value,
                                          s21_decimal value_main) {
  value.bits[0] = value_main.bits[0];
  value.bits[1] = value_main.bits[1];
  value.bits[2] = value_main.bits[2];
  value.bits[3] = 0;
  value.bits[4] = 0;
  value.bits[5] = 0;
  value.bits[6] = 0;
  value.bits[7] = value_main.bits[3];
  return value;
}

// Арифметические операторы
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  decimal_extended extended_1 = {}, extended_2 = {},
                   extended_r = {{0, 0, 0, 0, 0, 0, 0, 0}};
  int error = NORM, stepen = 0, flag_minus = 0;
  extended_1 = from_decimal_to_extended(extended_1, value_1);
  extended_2 = from_decimal_to_extended(extended_2, value_2);
  stepen = s21_normalize(&extended_1, &extended_2);
  if (get_bit(value_1.bits[3], 31) == get_bit(value_2.bits[3], 31)) {
    core_add(extended_1, extended_2, &extended_r);
    get_bit(value_1.bits[3], 31) == 1 ? flag_minus = 1 : 0;
  } else {
    if (ext_is_greater_or_equal(extended_1, extended_2) == 0) {
      if (get_bit(extended_2.bits[7], 31)) {
        flag_minus = 1;
      }
      change_places(&extended_1, &extended_2);
    } else {
      get_bit(value_1.bits[3], 31) ? flag_minus = 1 : 0;
    }
    core_sub(extended_1, extended_2, &extended_r);
  }
  if (get_pos_high_bit(extended_r) > 95 && stepen > 0) {
    banking_round(&extended_r, &stepen);
  }
  if (extended_r.bits[3] != 0 || extended_r.bits[4] != 0 ||
      extended_r.bits[5] != 0) {
    if (flag_minus == 0) {
      error = PL_INF;
    } else {
      error = MIN_INF;
    }
  }
  *result = decimal_to_result(extended_r);
  result->bits[3] = stepen << 16;
  flag_minus == 1 ? result->bits[3] = setBit(result->bits[3], 31, 1) : 0;
  error != 0 ? result = to_null(result) : 0;
  is_null(*result) ? result = to_null(result) : 0;
  return error;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = NORM, stepen = 0, flag_minus = 0;
  decimal_extended extended_1 = {}, extended_2 = {},
                   extended_r = {{0, 0, 0, 0, 0, 0, 0, 0}};
  extended_1 = from_decimal_to_extended(extended_1, value_1);
  extended_2 = from_decimal_to_extended(extended_2, value_2);
  stepen = s21_normalize(&extended_1, &extended_2);
  if (get_bit(value_1.bits[3], 31) != get_bit(value_2.bits[3], 31)) {
    get_bit(value_1.bits[3], 31) ? change_places(&extended_1, &extended_2) : 0;
    core_add(extended_1, extended_2, &extended_r);
    get_bit(value_1.bits[3], 31) ? flag_minus = 1 : 0;
  } else {
    if ((my_is_greater(value_1, value_2) && get_bit(value_1.bits[3], 31) &&
         get_bit(value_2.bits[3], 31)) ||
        (my_is_greater(value_2, value_1) && !get_bit(value_1.bits[3], 31) &&
         !get_bit(value_2.bits[3], 31))) {
      flag_minus = 1;
    }
    my_is_greater(value_1, value_2) ? 0
                                    : change_places(&extended_1, &extended_2);
    core_sub(extended_1, extended_2, &extended_r);
  }
  if (get_pos_high_bit(extended_r) > 95 && stepen > 0) {
    banking_round(&extended_r, &stepen);
  }
  if (extended_r.bits[3] != 0 || extended_r.bits[4] != 0 ||
      extended_r.bits[5] != 0) {
    if (flag_minus == 0) {
      error = PL_INF;
    } else {
      error = MIN_INF;
    }
  }
  *result = decimal_to_result(extended_r);
  result->bits[3] = stepen << 16;
  flag_minus != 0 ? result->bits[3] = setBit(result->bits[3], 31, 1) : 0;
  error != 0 ? result = to_null(result) : 0;
  is_null(*result) ? result = to_null(result) : 0;
  return error;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = NORM, stepen = 0, flag_minus = 0;
  decimal_extended extended_1 = {}, extended_2 = {},
                   extended_r = {{0, 0, 0, 0, 0, 0, 0, 0}};
  extended_1 = from_decimal_to_extended(extended_1, value_1);
  extended_2 = from_decimal_to_extended(extended_2, value_2);
  stepen = get_ten(value_1.bits[3]) + get_ten(value_2.bits[3]);
  core_mul(extended_1, extended_2, &extended_r);
  get_bit(value_1.bits[3], 31) + get_bit(value_2.bits[3], 31) == 1
      ? flag_minus = 1
      : 0;
  if ((get_pos_high_bit(extended_r) > 95 && stepen > 0) || stepen > 28) {
    banking_round(&extended_r, &stepen);
  }
  if (extended_r.bits[3] != 0 || extended_r.bits[4] != 0 ||
      extended_r.bits[5] != 0) {
    if (get_bit(value_1.bits[3], 31) + get_bit(value_2.bits[3], 31) == 1) {
      error = MIN_INF;
    } else {
      error = PL_INF;
    }
  }
  *result = decimal_to_result(extended_r);
  result->bits[3] = stepen << 16;
  flag_minus ? result->bits[3] = setBit(result->bits[3], 31, 1) : 0;
  error != 0 ? result = to_null(result) : 0;
  is_null(*result) ? result = to_null(result) : 0;
  return error;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = NORM;
  decimal_extended extended_1 = {}, extended_2 = {},
                   extended_r = {{0, 0, 0, 0, 0, 0, 0, 0}};
  if (is_null(value_1) || is_null(value_2)) {
    if (is_null(value_1)) {
      to_null(result);
      error = 0;
    }
    if (is_null(value_2)) {
      error = N_NAN;
    }
  } else {
    extended_1 = from_decimal_to_extended(extended_1, value_1);
    extended_2 = from_decimal_to_extended(extended_2, value_2);
    int stepen = 0, flag_minus = 0, temp_exp = 0;
    temp_exp = get_ten(value_1.bits[3]);
    while (!ext_is_greater_or_equal(extended_1, extended_2)) {
      core_add(ext_left_shift_by_count(extended_1, 3),
               ext_left_shift_by_count(extended_1, 1), &extended_1);
      temp_exp++;
    }
    stepen = temp_exp - get_ten(value_2.bits[3]);
    decimal_extended x = core_div(extended_1, extended_2, &extended_r);
    while ((x.bits[0] != 0 || x.bits[1] != 0 || x.bits[2] != 0) &&
           temp_exp < 28) {
      core_add(ext_left_shift_by_count(extended_1, 3),
               ext_left_shift_by_count(extended_1, 1), &extended_1);
      x = core_div(extended_1, extended_2, &extended_r);
      temp_exp++, stepen++;
    }
    if (get_pos_high_bit(extended_r) > 95 && stepen > 0) {
      banking_round(&extended_r, &stepen);
    }
    get_bit(value_1.bits[3], 31) + get_bit(value_2.bits[3], 31) == 1
        ? flag_minus = 1
        : 0;
    decimal_extended ten = {{10, 0, 0, 0, 0, 0, 0, 0}};
    while (stepen < 0) {
      extended_r = core_mul(extended_r, ten, &extended_r);
      stepen++;
    }
    extended_r.bits[3] != 0 || extended_r.bits[4] != 0 ||
            extended_r.bits[5] != 0
        ? error = 1
        : 0;
    error == 1 &&flag_minus == 1 ? error = 2 : 0;
    *result = decimal_to_result(extended_r);
    result->bits[3] = stepen << 16;
    flag_minus ? result->bits[3] = setBit(result->bits[3], 31, 1) : 0;
  }
  error != 0 ? result = to_null(result) : 0;
  is_null(*result) ? result = to_null(result) : 0;
  return error;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0, stepen = 0;
  decimal_extended extended_1 = {{0, 0, 0, 0, 0, 0, 0, 0}};
  decimal_extended extended_2 = {{0, 0, 0, 0, 0, 0, 0, 0}};
  decimal_extended extended_r = {{0, 0, 0, 0, 0, 0, 0, 0}};
  extended_1 = from_decimal_to_extended(extended_1, value_1);
  extended_2 = from_decimal_to_extended(extended_2, value_2);
  stepen = s21_normalize(&extended_1, &extended_2);
  if (ext_is_greater_or_equal(extended_2, extended_1) == 1) {
    extended_r = from_decimal_to_extended(extended_r, value_1);
  } else if (ext_is_greater_or_equal(extended_2, extended_1) == 2) {
    to_null(result);
  } else {
    core_mod(extended_1, extended_2, &extended_r);
  }
  *result = decimal_to_result(extended_r);
  result->bits[3] = stepen << 16;
  get_bit(value_1.bits[3], 31)
      ? result->bits[3] = setBit(result->bits[3], 31, 1)
      : 0;
  is_null(*result) ? to_null(result) : 0;
  return error;
}

// Операторы сравнение
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  decimal_extended extended_1 = {}, extended_2 = {};
  int error = 0;
  extended_1 = from_decimal_to_extended(extended_1, value_1);
  extended_2 = from_decimal_to_extended(extended_2, value_2);
  s21_normalize(&extended_1, &extended_2);
  if (get_bit(extended_1.bits[7], 31) > get_bit(extended_2.bits[7], 31)) {
    error = 0;
  } else if (get_bit(extended_1.bits[7], 31) <
             get_bit(extended_2.bits[7], 31)) {
    error = 1;
  } else {
    for (int i = 6; i >= 0; i--) {
      if (extended_1.bits[i] < extended_2.bits[i]) {
        error = 0;
        break;
      } else if (extended_1.bits[i] == extended_2.bits[i]) {
        error = 2;
      } else {
        error = 1;
        break;
      }
    }
  }
  get_bit(extended_1.bits[7], 31) &&
          get_bit(extended_2.bits[7], 31) &&error != 2
      ? error = !error
      : 0;
  error == 2 ? error = 0 : 0;
  is_null(value_1) && is_null(value_2) ? error = 0 : 0;
  return error;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  decimal_extended extended_1 = {}, extended_2 = {};
  int error = 1;
  extended_1 = from_decimal_to_extended(extended_1, value_1);
  extended_2 = from_decimal_to_extended(extended_2, value_2);
  s21_normalize(&extended_1, &extended_2);
  for (int i = 6; i >= 0; i--) {
    if (extended_1.bits[i] != extended_2.bits[i]) {
      error = 0;
    }
  }
  get_bit(extended_1.bits[7], 31) != get_bit(extended_2.bits[7], 31) ? error = 0
                                                                     : 0;
  is_null(value_1) && is_null(value_2) ? error = 1 : 0;
  return error;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2);
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_greater_or_equal(value_1, value_2);
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_greater(value_1, value_2);
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_equal(value_1, value_2);
}

//Преобразователи
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int error = 0, flag_minus = 0;
  if (dst == NULL) {
    error = 1;
  } else {
    if (src < 0) {
      src *= -1;
      flag_minus = 1;
    }
    dst->bits[0] = src;
    dst->bits[1] = 0;
    dst->bits[2] = 0;
    if (flag_minus) {
      dst->bits[3] = setBit(dst->bits[3], 31, 1);
    } else {
      dst->bits[3] = 0;
    }
  }
  return error;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int error = 0;
  if (dst == NULL) {
    error = 1;
  } else {
    decimal_extended extended_1 = {{0, 0, 0, 0, 0, 0, 0, 0}};
    extended_1 = from_decimal_to_extended(extended_1, src);
    int exp = get_ten(src.bits[3]), flag = 0;
    if (get_bit(src.bits[3], 31)) {
      flag = 1;
    }
    while (exp != 0) {
      extended_1 = div_by_ten(&extended_1);
      exp--;
    }
    if (get_bit(extended_1.bits[0], 31) || extended_1.bits[1] != 0 ||
        extended_1.bits[2] != 0) {
      error = 1;
    } else {
      *dst = extended_1.bits[0];
      flag ? *dst *= -1 : 0;
    }
  }
  return error;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int error = 0;
  if (dst == NULL) {
    to_null(&src);
    error = 1;
  } else if (is_null(src)) {
    *dst = 0.0;
  } else {
    int minus = get_bit(src.bits[3], 31);
    long int scale = pow(10, give_scale_factor(src.bits[3]));
    *dst = convert_dec_to(&src, &error);
    *dst = *dst / scale;
    if (minus == 1) {
      *dst *= -1;
    }
  }
  error != 0 ? error = 1 : 0;
  return error;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error = 0;
  if (dst == NULL || is_exception(&src)) {
    if (src == 0.0) {
      error = 0;
      to_null(dst);
    } else {
      error = 1;
    }
    src = 0.0;
  } else {
    to_null(dst);
    s21_decimal buff = {{1, 0, 0, 0}};
    s21_decimal f_1 = {{1, 0, 0, 0}};
    s21_decimal f_2 = {{2, 0, 0, 0}};
    unsigned int minus = 0, mant = 0;
    int scale = 0;
    if (src < 0) {
      src *= -1;
      minus = 1;
    }
    unsigned int src_int = *(unsigned int *)&src;
    scale = (src_int >> 23) - 127;
    mant = (src_int << 9) >> 1;
    mant = setBit(mant, 31, 1);
    int temp_scale = scale;
    for (int i = 31; i >= 0; i--) {
      temp_scale = scale;
      if (get_bit(mant, i)) {
        if (scale > 0) {
          while (temp_scale > 0) {
            error = s21_mul(buff, f_2, &buff);
            temp_scale -= 1;
          }
        } else if (scale < 0) {
          while (temp_scale < 0) {
            error = s21_mul(buff, f_2, &buff);
            temp_scale += 1;
          }
          s21_div(f_1, buff, &buff);
        } else {
          buff = f_1;
        }
        s21_add(*dst, buff, dst);
        buff.bits[0] = 1;
        buff.bits[1] = 0;
        buff.bits[2] = 0;
        buff.bits[3] = 0;
      }
      scale--;
    }
    error != 0 ? error = 1 : 0;
    *dst = my_round(*dst);
    minus ? dst->bits[3] = setBit(dst->bits[3], 31, 1) : 0;
    is_null(*dst) || error != 0 ? to_null(dst) : 0;
  }
  return error;
}

// Другие функции
int s21_floor(s21_decimal value, s21_decimal *result) {
  int error = 0;
  if (result == NULL) {
    error = 1;
  } else {
    decimal_extended div = {{10, 0, 0, 0, 0, 0, 0, 0}};
    decimal_extended extended_1 = {};
    decimal_extended extended_2 = {{1, 0, 0, 0, 0, 0, 0, 0}};
    extended_1 = from_decimal_to_extended(extended_1, value);
    int exp = get_ten(value.bits[3]);
    int flag = 0, flag_2 = 0;
    for (int i = exp; i != 0; i--) {
      if (!ext_is_greater_or_equal(extended_1, div)) {
        flag_2 = 1;
        break;
      }
      extended_1 = div_by_ten(&extended_1);
      flag++;
    }
    result->bits[3] = 0;
    if (!flag_2) {
      if (get_bit(value.bits[3], 31)) {
        if (flag != 0) {
          core_add(extended_1, extended_2, &extended_1);
        }
        *result = decimal_to_result(extended_1);
        result->bits[3] = setBit(result->bits[3], 31, 1);
      } else {
        *result = decimal_to_result(extended_1);
      }
    } else {
      to_null(result);
    }
  }
  return error;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  int error = 0;
  if (result == NULL) {
    error = 1;
  } else {
    *result = value;
    if (get_bit(result->bits[3], 31)) {
      result->bits[3] = setBit(result->bits[3], 31, 0);
    } else {
      result->bits[3] = setBit(result->bits[3], 31, 1);
    }
    is_null(value) ? to_null(result) : 0;
  }
  return error;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int error = 0;
  if (result == NULL) {
    error = 1;
  } else {
    int exp = get_ten(value.bits[3]), flag_minus = 0;
    decimal_extended extended_1 = {};
    decimal_extended move = {{1, 0, 0, 0, 0, 0, 0, 0}};
    decimal_extended extended_res = {};
    decimal_extended extended_2 = {{1, 0, 0, 0, 0, 0, 0, 0}};
    extended_1 = from_decimal_to_extended(extended_1, value);
    decimal_extended extended_temp = extended_1;
    decimal_extended div = {{10, 0, 0, 0, 0, 0, 0, 0}};
    flag_minus = get_bit(value.bits[3], 31);
    while (exp != 0) {
      if (!ext_is_greater_or_equal(extended_1, div)) {
        break;
      }
      extended_temp = div_by_ten(&extended_temp);
      exp--;
    }
    decimal_extended extended_fin = extended_temp;
    exp = get_ten(value.bits[3]);
    while (exp != 0) {
      core_add(ext_left_shift_by_count(extended_temp, 3),
               ext_left_shift_by_count(extended_temp, 1), &extended_temp);
      core_add(ext_left_shift_by_count(move, 3),
               ext_left_shift_by_count(move, 1), &move);
      exp--;
    }
    core_sub(extended_1, extended_temp, &extended_res);
    core_sub(move, extended_res, &extended_1);
    if (ext_is_greater_or_equal(extended_res, extended_1)) {
      core_add(extended_fin, extended_2, &extended_fin);
    }
    *result = decimal_to_result(extended_fin);
    if (flag_minus) {
      result->bits[3] = setBit(result->bits[3], 31, 1);
    } else {
      result->bits[3] = 0;
    }
  }
  return error;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int error = 0;
  decimal_extended div = {{10, 0, 0, 0, 0, 0, 0, 0}};
  if (result == NULL) {
    error = 1;
  } else {
    int flag = 0;
    decimal_extended extended_1 = {};
    extended_1 = from_decimal_to_extended(extended_1, value);
    int exp = get_ten(value.bits[3]);
    while (exp != 0) {
      if (!ext_is_greater_or_equal(extended_1, div)) {
        flag = 1;
        break;
      }
      extended_1 = div_by_ten(&extended_1);
      exp--;
    }
    if (!flag) {
      *result = decimal_to_result(extended_1);
      result->bits[3] = setBit(result->bits[3], 31, get_bit(value.bits[3], 31));
      is_null(value) ? to_null(result) : 0;
    } else {
      to_null(result);
    }
  }
  return error;
}

/***************** Вспомогательные функции **********************/

int get_bit(unsigned int x, int position) { return (x >> position % 32) & 1; }

int setBit(unsigned int value, int position, int bit) {
  unsigned int result = 0;
  unsigned int mask = 1 << position % 32;
  if (bit) {
    result = value | mask;
  } else {
    result = value & ~mask;
  }
  return result;
}

int get_pos_high_bit(decimal_extended value) {
  int i = 0, j = 0, rez = 0, k = 0;
  while (j != 7) {
    while (i != 32) {
      if (get_bit(value.bits[j], i) == 1) {
        rez = k;
      }
      i++;
      k++;
    }
    i = 0;
    j++;
  }
  return rez;
}

int get_ten(unsigned int value) {
  value = value << 1;
  value = value >> 17;
  return value;
}

int s21_normalize(decimal_extended *ext_1, decimal_extended *ext_2) {
  int high = 0;
  if (get_ten(ext_1->bits[7]) >= get_ten(ext_2->bits[7])) {
    high = get_ten(ext_1->bits[7]);
  } else if (get_ten(ext_1->bits[7]) < get_ten(ext_2->bits[7])) {
    high = get_ten(ext_2->bits[7]);
  }
  int ten_1 = get_ten(ext_1->bits[7]) - get_ten(ext_2->bits[7]);
  if (ten_1 > 0) {
    while (ten_1 != 0) {
      core_add(ext_left_shift_by_count(*ext_2, 3),
               ext_left_shift_by_count(*ext_2, 1), ext_2);
      ten_1--;
    }
  } else if (ten_1 < 0) {
    while (ten_1 != 0) {
      core_add(ext_left_shift_by_count(*ext_1, 3),
               ext_left_shift_by_count(*ext_1, 1), ext_1);
      ten_1++;
    }
  }
  return high;
}

int change_places(decimal_extended *value_1, decimal_extended *value_2) {
  decimal_extended buf = *value_1;
  *value_1 = *value_2;
  *value_2 = buf;
  return 0;
}

decimal_extended ext_left_shift_by_count(decimal_extended ext_value,
                                         int count) {
  int j = 0;
  int buff_2 = 0;
  while (count != 0) {
    while (j != 7) {
      int buff_1 = buff_2;
      buff_2 = get_bit(ext_value.bits[j], 31);
      ext_value.bits[j] <<= 1;
      ext_value.bits[j] = setBit(ext_value.bits[j], 0, buff_1);
      j++;
    }
    j = 0;
    count--;
  }
  return ext_value;
}

decimal_extended ext_right_shift_by_count(decimal_extended value, int count) {
  while (count != 0) {
    int buff_1 = 0, buff_2 = 0, buff_3 = 0, buff_4 = 0, buff_5 = 0, buff_6 = 0;
    buff_1 = get_bit(value.bits[1], 0);
    buff_2 = get_bit(value.bits[2], 0);
    buff_3 = get_bit(value.bits[3], 0);
    buff_4 = get_bit(value.bits[4], 0);
    buff_5 = get_bit(value.bits[5], 0);
    buff_6 = get_bit(value.bits[6], 0);
    value.bits[0] = value.bits[0] >> 1;
    value.bits[1] = value.bits[1] >> 1;
    value.bits[2] = value.bits[2] >> 1;
    value.bits[3] = value.bits[3] >> 1;
    value.bits[4] = value.bits[4] >> 1;
    value.bits[5] = value.bits[5] >> 1;
    value.bits[6] = value.bits[6] >> 1;
    value.bits[0] = setBit(value.bits[0], 31, buff_1);
    value.bits[1] = setBit(value.bits[1], 31, buff_2);
    value.bits[2] = setBit(value.bits[2], 31, buff_3);
    value.bits[3] = setBit(value.bits[3], 31, buff_4);
    value.bits[4] = setBit(value.bits[4], 31, buff_5);
    value.bits[5] = setBit(value.bits[5], 31, buff_6);
    count--;
  }
  return value;
}

int is_null(s21_decimal value) {
  int r = 0;
  if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0) {
    r = 1;
  }
  return r;
}

int ext_is_null(decimal_extended value) {
  int r = 0;
  if (!value.bits[0] && !value.bits[1] && !value.bits[2] && !value.bits[3] &&
      !value.bits[4] && !value.bits[5] && !value.bits[6]) {
    r = 1;
  }
  return r;
}

s21_decimal *to_null(s21_decimal *value) {
  value->bits[0] = 0;
  value->bits[1] = 0;
  value->bits[2] = 0;
  value->bits[3] = 0;
  return value;
}

decimal_extended div_by_ten(decimal_extended *value) {
  decimal_extended test = {{0, 0, 0, 0, 0, 0, 0, 0}};
  decimal_extended x = {{10, 0, 0, 0, 0, 0, 0, 0}};
  if (ext_is_greater_or_equal(x, *value)) {
    value->bits[0] = 0;
  } else {
    core_div(*value, x, &test);
    *value = test;
  }
  return *value;
}

int get_bit_full(decimal_extended val, int bit) {
  int res = 0;
  unsigned int mask = 1u << (bit % 32);
  res = val.bits[bit / 32] & mask;
  return !!res;
}

int my_is_greater(s21_decimal value_1, s21_decimal value_2) {
  decimal_extended extended_1 = {}, extended_2 = {};
  extended_1 = from_decimal_to_extended(extended_1, value_1);
  extended_2 = from_decimal_to_extended(extended_2, value_2);
  s21_normalize(&extended_1, &extended_2);
  int res = 0;
  for (int i = 223; i >= 0; i--) {
    int bit = get_bit_full(extended_1, i);
    int bit1 = get_bit_full(extended_2, i);
    if (bit1 == 0 && bit == 1) {
      res = 1;
      break;
    } else if (bit1 == 1 && bit == 0) {
      res = 0;
      break;
    }
  }
  return res;
}

int ext_is_greater_or_equal(decimal_extended extended_1,
                            decimal_extended extended_2) {
  int res = 2;
  for (int i = 223; i >= 0; i--) {
    int bit = get_bit(extended_1.bits[i / 32], i % 32);
    int bit1 = get_bit(extended_2.bits[i / 32], i % 32);
    if (bit1 == 0 && bit == 1) {
      res = 1;
      break;
    } else if (bit1 == 1 && bit == 0) {
      res = 0;
      break;
    }
  }
  return res;
}

decimal_extended core_add(decimal_extended extended, decimal_extended extended2,
                          decimal_extended *extended_r) {
  int j = 0, x = 0;
  while (j < 7) {
    int i = 0;
    while (i < 32) {
      if (get_bit(extended.bits[j], i) + get_bit(extended2.bits[j], i) + x ==
          0) {
        extended_r->bits[j] = setBit(extended_r->bits[j], i, 0);
        x = 0;
      } else if (get_bit(extended.bits[j], i) + get_bit(extended2.bits[j], i) +
                     x ==
                 2) {
        extended_r->bits[j] = setBit(extended_r->bits[j], i, 0);
        x = 1;
      } else if (get_bit(extended.bits[j], i) + get_bit(extended2.bits[j], i) +
                     x ==
                 1) {
        extended_r->bits[j] = setBit(extended_r->bits[j], i, 1);
        x = 0;
      } else if (get_bit(extended.bits[j], i) + get_bit(extended2.bits[j], i) +
                     x >
                 2) {
        extended_r->bits[j] = setBit(extended_r->bits[j], i, 1);
        x = 1;
      }
      i++;
    }
    j++;
  }
  return *extended_r;
}

decimal_extended core_sub(decimal_extended extended, decimal_extended extended2,
                          decimal_extended *extended_r) {
  int j = 0, x = 0;
  while (j < 7) {
    int i = 0;
    while (i < 32) {
      if (get_bit(extended.bits[j], i) > get_bit(extended2.bits[j], i)) {
        if (get_bit(extended.bits[j], i) + x == 1) {
          extended_r->bits[j] = setBit(extended_r->bits[j], i, 1);
        } else {
          extended_r->bits[j] = setBit(extended_r->bits[j], i, 0);
          x = 0;
        }
      } else if (get_bit(extended.bits[j], i) ==
                 get_bit(extended2.bits[j], i)) {
        if (x == 1) {
          extended_r->bits[j] = setBit(extended_r->bits[j], i, 1);
        } else {
          extended_r->bits[j] = setBit(extended_r->bits[j], i, 0);
        }
      } else if (get_bit(extended.bits[j], i) < get_bit(extended2.bits[j], i)) {
        if (get_bit(extended.bits[j], i) + x == 1) {
          extended_r->bits[j] = setBit(extended_r->bits[j], i, 0);
        } else {
          extended_r->bits[j] = setBit(extended_r->bits[j], i, 1);
          x = 1;
        }
      }
      i++;
    }
    j++;
  }
  return *extended_r;
}

decimal_extended core_mul(decimal_extended value_1, decimal_extended value_2,
                          decimal_extended *res) {
  decimal_extended temp = {{0, 0, 0, 0, 0, 0, 0, 0}};
  int i = 0, j = 0;
  while (i <= 223) {
    if (get_bit(value_2.bits[j], i)) {
      core_add(temp, ext_left_shift_by_count(value_1, i), &temp);
    }
    i++;
    i % 32 == 0 ? j++ : 0;
  }
  *res = temp;
  return *res;
}

decimal_extended core_div(decimal_extended extended, decimal_extended extended2,
                          decimal_extended *extended_r) {
  decimal_extended res = {{0, 0, 0, 0, 0, 0, 0, 0}};
  int x = get_pos_high_bit(extended) - get_pos_high_bit(extended2);
  if (x > 0) {
    extended2 = ext_left_shift_by_count(extended2, x);
  }
  if (!ext_is_null(extended2)) {
    int i = 0;
    while (i != x + 1) {
      if (ext_is_greater_or_equal(extended, extended2) == 1 ||
          ext_is_greater_or_equal(extended, extended2) == 2) {
        core_sub(extended, extended2, &extended);
        res = ext_left_shift_by_count(res, 1);
        res.bits[0] = setBit(res.bits[0], 0, 1);
        extended2 = ext_right_shift_by_count(extended2, 1);
        i++;
      } else {
        res = ext_left_shift_by_count(res, 1);
        extended2 = ext_right_shift_by_count(extended2, 1);
        i++;
      }
    }
  }
  *extended_r = res;
  return extended;
}

int banking_round(decimal_extended *value, int *stepen) {
  int temp = 0;
  decimal_extended res = {};
  while ((get_pos_high_bit(*value) > 95 && *stepen > 0) || *stepen > 28) {
    res = *value;
    res = div_by_ten(&res);
    core_add(ext_left_shift_by_count(res, 3), ext_left_shift_by_count(res, 1),
             &res);
    core_sub(*value, res, &res);
    temp = res.bits[0];
    *stepen -= 1;
    *value = div_by_ten(value);
  }
  if (temp > 5 || (temp == 5 && value->bits[0] % 2 == 0)) {
    decimal_extended y = {{1, 0, 0, 0, 0, 0, 0, 0}};
    core_add(*value, y, value);
  }
  return temp;
}

int check_last_null(decimal_extended *value, int stepen) {
  decimal_extended res = {{0, 0, 0, 0, 0, 0, 0, 0}};
  decimal_extended div = {{10, 0, 0, 0, 0, 0, 0, 0}};
  if (ext_is_greater_or_equal(*value, div) == 1 ||
      ext_is_greater_or_equal(*value, div) == 2) {
    res = core_mod(*value, div, &res);
  }
  if ((res.bits[0] == 0 && res.bits[1] == 0 && res.bits[2] == 0) &&
      (ext_is_greater_or_equal(*value, div) == 1 ||
       ext_is_greater_or_equal(*value, div) == 2) &&
      stepen > 0) {
    while ((res.bits[0] == 0 && res.bits[1] == 0 && res.bits[2] == 0) &&
           (ext_is_greater_or_equal(*value, div) == 1 ||
            ext_is_greater_or_equal(*value, div) == 2) &&
           stepen >= 1) {
      stepen--;
      core_div(*value, div, value);
      res = core_mod(*value, div, &res);
    }
  }
  return stepen;
}

s21_decimal decimal_to_result(decimal_extended value) {
  s21_decimal res = {{0, 0, 0, 0}};
  res.bits[0] = value.bits[0];
  res.bits[1] = value.bits[1];
  res.bits[2] = value.bits[2];
  return res;
}

decimal_extended core_mod(decimal_extended value_1, decimal_extended value_2,
                          decimal_extended *res_1) {
  decimal_extended res = {{0, 0, 0, 0, 0, 0, 0, 0}};
  core_div(value_1, value_2, &res);
  res = core_mul(res, value_2, &res);
  core_sub(value_1, res, res_1);
  return *res_1;
}

double convert_dec_to(s21_decimal *src, int *error) {
  double dec1 = 1.0, dec = 0.0;
  int cell = 0;
  for (int j = 0; j < 3; j++) {
    if (src->bits[j] == 0) {
      cell += 32;
      continue;
    } else {
      if (j != 0) {
        *error = 1;
        break;
      }
      for (int i = 0; i < 32; i++) {
        dec += powl(2, cell) * get_bit(src->bits[j], i);
        cell++;
      }
      dec1 *= dec;
    }
  }
  if (*error == 2) {
    int scale = give_scale_factor(src->bits[3]);
    dec1 /= pow(10, scale);
    dec1 = (int)dec1;
    if (dec1 < 0)
      *error = 1;
    else
      *error = 0;
  } else if (*error == 3) {
    if (dec1 > 2147483647.0 || dec1 < -2147483648.0)
      *error = 1;
    else
      *error = 0;
  }
  if (dec1 > pow(2, 96)) {
    *error = 1;
  }
  if (get_ten(src->bits[3]) > 28 || (int)dec1 == 0) {
    dec1 = 0.0;
  }
  return dec1;
}

int give_scale_factor(int bit) {
  int copy_bit = 0;
  int cell = 0;
  for (int i = 16; i < 23; i++) {
    copy_bit += powl(2, cell) * get_bit(bit, i);
    cell++;
  }
  return copy_bit;
}

int is_exception(float *value) {
  int error = 0;
  float a = 0.0, b = -1.0, c = 1.0;
  if (*value == b / a || *value == c / a || *value != *value) {
    error = 1;
  }
  if (fabs(*value) < 1e-28) {
    error = 1;
  }
  return error;
}

s21_decimal my_round(s21_decimal x) {
  unsigned int temp_exp = x.bits[3] >> 16;
  decimal_extended buff = {};
  buff = from_decimal_to_extended(buff, x);
  decimal_extended max_val = {
      {0b111111111111111111111111, 0, 0, 0, 0, 0, 0, 0}};
  decimal_extended one = {{1, 0, 0, 0, 0, 0, 0, 0}};
  decimal_extended ostatok = {};
  decimal_extended temp = {};
  while (ext_is_greater_or_equal(buff, max_val) == 1) {
    temp = buff;
    div_by_ten(&temp);
    core_add(ext_left_shift_by_count(temp, 3), ext_left_shift_by_count(temp, 1),
             &temp);
    core_sub(buff, temp, &ostatok);
    div_by_ten(&buff);
    temp_exp -= 1;
  }
  if (ostatok.bits[0] >= 5) {
    core_add(buff, one, &buff);
  }
  temp_exp = check_last_null(&buff, temp_exp);
  x = decimal_to_result(buff);
  x.bits[3] = temp_exp << 16;
  return x;
}
