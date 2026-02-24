#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdint>

enum RoundMode { NEAREST, ZERO, UP, DOWN };

uint32_t add_float(uint32_t a, uint32_t b, RoundMode mode) {
    // Извлечение полей
    uint32_t sign_a = a >> 31, sign_b = b >> 31;
    int32_t exp_a = (a >> 23) & 0xFF, exp_b = (b >> 23) & 0xFF;
    uint32_t mant_a = a & 0x7FFFFF, mant_b = b & 0x7FFFFF;

    // Специальные случаи
    if (exp_a == 0xFF || exp_b == 0xFF) return 0x7FC00000;

    // Добавление скрытого бита
    mant_a |= (exp_a != 0) << 23;
    mant_b |= (exp_b != 0) << 23;

    // Выравнивание экспонент
    if (exp_a > exp_b) {
        mant_b >>= exp_a - exp_b;
        exp_b = exp_a;
    }
    else if (exp_b > exp_a) {
        mant_a >>= exp_b - exp_a;
        exp_a = exp_b;
    }

    // Сложение/вычитание
    uint32_t mant_r, sign_r;
    if (sign_a == sign_b) {
        mant_r = mant_a + mant_b;
        sign_r = sign_a;
    }
    else if (mant_a >= mant_b) {
        mant_r = mant_a - mant_b;
        sign_r = sign_a;
    }
    else {
        mant_r = mant_b - mant_a;
        sign_r = sign_b;
    }

    if (mant_r == 0) return sign_r << 31;

    // Нормализация
    int32_t exp_r = exp_a;
    while (!(mant_r & (1 << 24)) && mant_r) {
        mant_r <<= 1;
        exp_r--;
    }

    // Округление
    if (mant_r & (1 << 24)) {
        mant_r >>= 1;
        exp_r++;
        bool round_up = false;
        switch (mode) {
        case NEAREST: round_up = (mant_r & 1) && ((mant_r >> 1) & 1); break;
        case UP: round_up = !sign_r && (mant_r & 1); break;
        case DOWN: round_up = sign_r && (mant_r & 1); break;
        case ZERO: round_up = false;
        }
        if (round_up) mant_r++;
    }

    // Проверка границ
    if (exp_r >= 0xFF) return (sign_r << 31) | (0xFF << 23); // Infinity
    if (exp_r <= 0) return sign_r << 31; // Zero

    return (sign_r << 31) | ((exp_r & 0xFF) << 23) | (mant_r & 0x7FFFFF);
}

int main() {
    float x = 1.5f, y = 2.25f;
    uint32_t a, b;
    memcpy(&a, &x, 4);
    memcpy(&b, &y, 4);

    std::cout << std::hex << std::showbase;
    std::cout << "a = 0x" << a << " (" << x << ")\n";
    std::cout << "b = 0x" << b << " (" << y << ")\n\n";

    std::cout << "NEAREST: 0x" << add_float(a, b, NEAREST) << '\n';
    std::cout << "ZERO:    0x" << add_float(a, b, ZERO) << '\n';
    std::cout << "UP:      0x" << add_float(a, b, UP) << '\n';
    std::cout << "DOWN:    0x" << add_float(a, b, DOWN) << '\n';

    return 0;
}