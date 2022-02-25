#pragma once
#include "common.hpp"
#include <iostream>

using namespace std;

inline void bit_print_reverse(uint64_t x){
    for (uint32_t i = 0; i < HW2; ++i)
        cerr << (1 & (x >> i));
    cerr << endl;
}

inline void bit_print(uint64_t x){
    for (uint32_t i = 0; i < HW2; ++i)
        cerr << (1 & (x >> (HW2_M1 - i)));
    cerr << endl;
}

inline void bit_print_board_reverse(uint64_t x){
    for (uint32_t i = 0; i < HW2; ++i){
        cerr << (1 & (x >> i));
        if (i % HW == HW_M1)
            cerr << endl;
    }
    cerr << endl;
}

inline void bit_print_board(uint64_t x){
    for (uint32_t i = 0; i < HW2; ++i){
        cerr << (1 & (x >> (HW2_M1 - i)));
        if (i % HW == HW_M1)
            cerr << endl;
    }
    cerr << endl;
}

void input_board(uint64_t *p, uint64_t *o){
    char elem;
    *p = 0ULL;
    *o = 0ULL;
    for (int i = 0; i < HW2; ++i){
        cin >> elem;
        if (elem == '0')
            *p |= 1ULL << (HW2_M1 - i);
        else if (elem == '1')
            *o |= 1ULL << (HW2_M1 - i);
    }
}

void print_board(uint64_t p, uint64_t o){
    for (int i = 0; i < HW2; ++i){
        if (1 & (p >> (HW2_M1 - i)))
            cerr << '0';
        else if (1 & (o >> (HW2_M1 - i)))
            cerr << '1';
        else
            cerr << '.';
        if (i % HW == HW_M1)
            cerr << endl;
    }
}

inline uint32_t pop_count(uint64_t x){
    x = x - ((x >> 1) & 0x5555555555555555ULL);
	x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
	x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
	x = (x * 0x0101010101010101ULL) >> 56;
    return (uint32_t)x;
}

inline uint32_t pop_digit(uint64_t x, int place){
    return (uint32_t)(1ULL & (x >> place));
}

inline uint64_t white_line_mirror(uint64_t x){
    uint64_t a = (x ^ (x >> 7)) & 0x00AA00AA00AA00AAULL;
    x = x ^ a ^ (a << 7);
    a = (x ^ (x >> 14)) & 0x0000CCCC0000CCCCULL;
    x = x ^ a ^ (a << 14);
    a = (x ^ (x >> 28)) & 0x00000000F0F0F0F0ULL;
    return x = x ^ a ^ (a << 28);
}

inline uint64_t black_line_mirror(uint64_t x){
    uint64_t a = (x ^ (x >> 9)) & 0x0055005500550055ULL;
    x = x ^ a ^ (a << 9);
    a = (x ^ (x >> 18)) & 0x0000333300003333ULL;
    x = x ^ a ^ (a << 18);
    a = (x ^ (x >> 36)) & 0x000000000F0F0F0FULL;
    return x = x ^ a ^ (a << 36);
}

inline uint64_t vertical_mirror(uint64_t x){
    x = ((x >> 8) & 0x00FF00FF00FF00FFULL) | ((x << 8) & 0xFF00FF00FF00FF00ULL);
    x = ((x >> 16) & 0x0000FFFF0000FFFFULL) | ((x << 16) & 0xFFFF0000FFFF0000ULL);
    return ((x >> 32) & 0x00000000FFFFFFFFULL) | ((x << 32) & 0xFFFFFFFF00000000ULL);
}

inline uint64_t horizontal_mirror(uint64_t x){
    x = ((x >> 1) & 0x5555555555555555ULL) | ((x << 1) & 0xAAAAAAAAAAAAAAAAULL);
    x = ((x >> 2) & 0x3333333333333333ULL) | ((x << 2) & 0xCCCCCCCCCCCCCCCCULL);
    return ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((x << 4) & 0xF0F0F0F0F0F0F0F0ULL);
}

// direction is couner clockwise
inline uint64_t rotate_90(uint64_t x){
    return vertical_mirror(white_line_mirror(x));
}

// direction is couner clockwise
inline uint64_t rotate_270(uint64_t x){
    return vertical_mirror(black_line_mirror(x));
}

inline uint64_t rotate_180(uint64_t x){
    x = ((x & 0x5555555555555555ULL) << 1) | ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
    x = ((x & 0x3333333333333333ULL) << 2) | ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
    x = ((x & 0x0F0F0F0F0F0F0F0FULL) << 4) | ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
    x = ((x & 0x00FF00FF00FF00FFULL) << 8) | ((x & 0xFF00FF00FF00FF00ULL) >> 8);
    x = ((x & 0x0000FFFF0000FFFFULL) << 16) | ((x & 0xFFFF0000FFFF0000ULL) >> 16);
    return ((x & 0x00000000FFFFFFFFULL) << 32) | ((x & 0xFFFFFFFF00000000ULL) >> 32);
}

// rotate 45 degrees counter clockwise
/*
      8  7  6  5  4  3  2  1
      9  8  7  6  5  4  3  2
     10  9  8  7  6  5  4  3
     11 10  9  8  7  6  5  4
     12 11 10  9  8  7  6  5
     13 12 11 10  9  8  7  6
     14 13 12 11 10  9  8  7
     15 14 13 12 11 10  9  8
    
    to

     14 14  6  6  6  6  6  6
     13 13 13  5  5  5  5  5
     12 12 12 12  4  4  4  4
     11 11 11 11 11  3  3  3
     10 10 10 10 10 10  2  2
      9  9  9  9  9  9  9  1
      8  8  8  8  8  8  8  8
     15  7  7  7  7  7  7  7
*/
inline uint64_t rotate_45(uint64_t x){
    uint64_t a = (x ^ (x >> 8)) & 0x0055005500550055ULL;
    x = x ^ a ^ (a << 8);
    a = (x ^ (x >> 16)) & 0x0000CC660000CC66ULL;
    x = x ^ a ^ (a << 16);
    a = (x ^ (x >> 32)) & 0x00000000C3E1F078ULL;
    return x ^ a ^ (a << 32);
}

// unrotate 45 degrees counter clockwise
inline uint64_t unrotate_45(uint64_t x){
    uint64_t a = (x ^ (x >> 32)) & 0x00000000C3E1F078ULL;
    x = x ^ a ^ (a << 32);
    a = (x ^ (x >> 16)) & 0x0000CC660000CC66ULL;
    x = x ^ a ^ (a << 16);
    a = (x ^ (x >> 8)) & 0x0055005500550055ULL;
    return x ^ a ^ (a << 8);
}

inline uint64_t rotate_225(uint64_t x){
    return rotate_45(rotate_180(x));
}

inline uint64_t unrotate_225(uint64_t x){
    return rotate_180(unrotate_45(x));
}

inline uint64_t rotate_135(uint64_t x){
    return rotate_45(rotate_90(x));
}

inline uint64_t unrotate_135(uint64_t x){
    return rotate_270(unrotate_45(x));
}

inline uint64_t rotate_315(uint64_t x){
    return rotate_45(rotate_270(x));
}

inline uint64_t unrotate_315(uint64_t x){
    return rotate_90(unrotate_45(x));
}

constexpr uint8_t d7_mask[HW2] = {
    0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111,
    0b11000000, 0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111,
    0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111,
    0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111,
    0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111,
    0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111, 0b00000111,
    0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111, 0b00000111, 0b00000011,
    0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111, 0b00000111, 0b00000011, 0b00000001
};

constexpr uint8_t d9_mask[HW2] = {
    0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111, 0b00000111, 0b00000011, 0b00000001,
    0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111, 0b00000111, 0b00000011,
    0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111, 0b00000111,
    0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111, 0b00001111,
    0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111, 0b00011111,
    0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111, 0b00111111,
    0b11000000, 0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b01111111,
    0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111
};

inline uint8_t join_v_line(uint64_t x, int c){
    x = (x >> c) & 0b0000000100000001000000010000000100000001000000010000000100000001ULL;
    return (x * 0b0000000100000010000001000000100000010000001000000100000010000000ULL) >> 56;
}

inline uint64_t split_v_line(uint8_t x, int c){
    uint64_t res = 0;
    uint64_t a = x & 0b00001111;
    uint64_t b = x & 0b11110000;
    res = a | (b << 28);
    a = res & 0b0000000000000000000000000000001100000000000000000000000000000011ULL;
    b = res & 0b0000000000000000000000000000110000000000000000000000000000001100ULL;
    res = a | (b << 14);
    a = res & 0b0000000000000001000000000000000100000000000000010000000000000001ULL;
    b = res & 0b0000000000000010000000000000001000000000000000100000000000000010ULL;
    res = a | (b << 7);
    return res << c;
}

inline uint8_t join_d7_line(uint64_t x, const int t){
    x = (x >> t) & 0b0000000000000010000001000000100000010000001000000100000010000001ULL;
    return (x * 0b1000000010000000100000001000000010000000100000001000000010000000ULL) >> 56;
}

inline uint64_t split_d7_line(uint8_t x, int t){
    uint8_t c = x & 0b01010101;
    uint8_t d = x & 0b10101010;
    x = (c << 1) | (d >> 1);
    c = x & 0b00110011;
    d = x & 0b11001100;
    x = (c << 2) | (d >> 2);
    c = x & 0b00001111;
    d = x & 0b11110000;
    x = (c << 4) | (d >> 4);
    uint64_t a = x & 0b00001111;
    uint64_t b = x & 0b11110000;
    uint64_t res = a | (b << 24);
    a = res & 0b0000000000000000000000000000000000110000000000000000000000000011ULL;
    b = res & 0b0000000000000000000000000000000011000000000000000000000000001100ULL;
    res = a | (b << 12);
    a = res & 0b0000000100000000000001000000000000010000000000000100000000000001ULL;
    b = res & 0b0000001000000000000010000000000000100000000000001000000000000010ULL;
    res = a | (b << 6);
    return res << t;
}

inline uint8_t join_d9_line(uint64_t x, int t){
    if (t > 0)
        x >>= t;
    else if (t < 0)
        x <<= (-t);
    x &= 0b1000000001000000001000000001000000001000000001000000001000000001ULL;
    return (x * 0b0000000100000001000000010000000100000001000000010000000100000001ULL) >> 56;
}

inline uint64_t split_d9_line(uint8_t x, int t){
    uint64_t a = x & 0b00001111;
    uint64_t b = x & 0b11110000;
    uint64_t res = a | (b << 32);
    a = res & 0b0000000000000000000000000011000000000000000000000000000000000011ULL;
    b = res & 0b0000000000000000000000001100000000000000000000000000000000001100ULL;
    res = a | (b << 16);
    a = res & 0b0000000001000000000000000001000000000000000001000000000000000001ULL;
    b = res & 0b0000000010000000000000000010000000000000000010000000000000000010ULL;
    res = a | (b << 8);
    if (t > 0)
        return res << t;
    return res >> (-t);
}

inline uint8_t join_d7_line_2(const uint64_t x){
    return ((x & 0b00000000'00000000'00000000'00000000'00000000'00000001'00000010'00000100ULL) * 
                0b00100000'00100000'00100000'00000000'00000000'00000000'00000000'00000000ULL) >> 56;
}

inline uint8_t join_d7_line_3(const uint64_t x){
    return ((x & 0b00000000'00000000'00000000'00000000'00000001'00000010'00000100'00001000ULL) * 
                0b00010000'00010000'00010000'00010000'00000000'00000000'00000000'00000000ULL) >> 56;
}

inline uint8_t join_d7_line_4(const uint64_t x){
    return ((x & 0b00000000'00000000'00000000'00000001'00000010'00000100'00001000'00010000ULL) * 
            0b00001000'00001000'00001000'00001000'00001000'00000000'00000000'00000000ULL) >> 56;
}

inline uint8_t join_d7_line_5(const uint64_t x){
    return ((x & 0b00000000'00000000'00000001'00000010'00000100'00001000'00010000'00100000ULL) * 
            0b00000100'00000100'00000100'00000100'00000100'00000100'00000000'00000000ULL) >> 56;
}

inline uint8_t join_d7_line_6(const uint64_t x){
    return ((x & 0b00000000'00000001'00000010'00000100'00001000'00010000'00100000'01000000ULL) * 
            0b00000010'00000010'00000010'00000010'00000010'00000010'00000010'00000000ULL) >> 56;
}

inline uint8_t join_d7_line_7(const uint64_t x){
    return ((x & 0b00000001'00000010'00000100'00001000'00010000'00100000'01000000'10000000ULL) * 
            0b00000001'00000001'00000001'00000001'00000001'00000001'00000001'00000001ULL) >> 56;
}

inline uint8_t join_d7_line_8(const uint64_t x){
    return ((x & 0b00000010'00000100'00001000'00010000'00100000'01000000'10000000'00000000ULL) * 
            0b00000000'00000001'00000001'00000001'00000001'00000001'00000001'00000001ULL) >> 57;
}

inline uint8_t join_d7_line_9(const uint64_t x){
    return ((x & 0b00000100'00001000'00010000'00100000'01000000'10000000'00000000'00000000ULL) * 
            0b00000000'00000000'00000001'00000001'00000001'00000001'00000001'00000001ULL) >> 58;
}

inline uint8_t join_d7_line_10(const uint64_t x){
    return ((x & 0b00001000'00010000'00100000'01000000'10000000'00000000'00000000'00000000ULL) * 
            0b00000000'00000000'00000000'00000001'00000001'00000001'00000001'00000001ULL) >> 59;
}

inline uint8_t join_d7_line_11(const uint64_t x){
    return ((x & 0b00010000'00100000'01000000'10000000'00000000'00000000'00000000'00000000ULL) * 
            0b00000000'00000000'00000000'00000000'00000001'00000001'00000001'00000001ULL) >> 60;
}

inline uint8_t join_d7_line_12(const uint64_t x){
    return ((x & 0b00100000'01000000'10000000'00000000'00000000'00000000'00000000'00000000ULL) * 
            0b00000000'00000000'00000000'00000000'00000000'00000001'00000001'00000001ULL) >> 61;
}

uint8_t (*join_d7_lines[])(const uint64_t) = {
    join_d7_line_2, join_d7_line_3, join_d7_line_4, join_d7_line_5, 
    join_d7_line_6, join_d7_line_7, join_d7_line_8, join_d7_line_9, 
    join_d7_line_10, join_d7_line_11, join_d7_line_12, 
};

inline uint8_t join_d9_line_m5(const uint64_t x){
    return ((x & 0b00000100'00000010'00000001'00000000'00000000'00000000'00000000'00000000ULL) * 
            0b00000000'00000000'00000000'00000000'00000000'00100000'00100000'00100000ULL) >> 56;
}

inline uint8_t join_d9_line_m4(const uint64_t x){
    return ((x & 0b00001000'00000100'00000010'00000001'00000000'00000000'00000000'00000000ULL) * 
            0b00000000'00000000'00000000'00000000'00010000'00010000'00010000'00010000ULL) >> 56;
}

inline uint8_t join_d9_line_m3(const uint64_t x){
    return ((x & 0b00010000'00001000'00000100'00000010'00000001'00000000'00000000'00000000ULL) * 
            0b00000000'00000000'00000000'00001000'00001000'00001000'00001000'00001000ULL) >> 56;
}

inline uint8_t join_d9_line_m2(const uint64_t x){
    return ((x & 0b00100000'00010000'00001000'00000100'00000010'00000001'00000000'00000000ULL) * 
            0b00000000'00000000'00000100'00000100'00000100'00000100'00000100'00000100ULL) >> 56;
}

inline uint8_t join_d9_line_m1(const uint64_t x){
    return ((x & 0b01000000'00100000'00010000'00001000'00000100'00000010'00000001'00000000ULL) * 
            0b00000000'00000010'00000010'00000010'00000010'00000010'00000010'00000010ULL) >> 56;
}

inline uint8_t join_d9_line_0(const uint64_t x){
    return ((x & 0b10000000'01000000'00100000'00010000'00001000'00000100'00000010'00000001ULL) * 
            0b00000001'00000001'00000001'00000001'00000001'00000001'00000001'00000001ULL) >> 56;
}

inline uint8_t join_d9_line_1(const uint64_t x){
    return ((x & 0b0000000'10000000'01000000'00100000'00010000'00001000'00000100'00000010ULL) * 
            0b00000000'10000000'10000000'10000000'10000000'10000000'10000000'10000000ULL) >> 56;
}

inline uint8_t join_d9_line_2(const uint64_t x){
    return ((x & 0b0000000'0000000'10000000'01000000'00100000'00010000'00001000'00000100ULL) * 
            0b00000000'01000000'01000000'01000000'01000000'01000000'01000000'00000000ULL) >> 56;
}

inline uint8_t join_d9_line_3(const uint64_t x){
    return ((x & 0b0000000'0000000'0000000'10000000'01000000'00100000'00010000'00001000ULL) * 
            0b00000000'00100000'00100000'00100000'00100000'00100000'00000000'00000000ULL) >> 56;
}

inline uint8_t join_d9_line_4(const uint64_t x){
    return ((x & 0b0000000'0000000'0000000'0000000'10000000'01000000'00100000'00010000ULL) * 
            0b00000000'00010000'00010000'00010000'00010000'00000000'00000000'00000000ULL) >> 56;
}

inline uint8_t join_d9_line_5(const uint64_t x){
    return ((x & 0b0000000'0000000'0000000'0000000'0000000'10000000'01000000'00100000ULL) * 
            0b00000000'00001000'00001000'00001000'00000000'00000000'00000000'00000000ULL) >> 56;
}

uint8_t (*join_d9_lines[])(const uint64_t) = {
    join_d9_line_m5, join_d9_line_m4, join_d9_line_m3, join_d9_line_m2, 
    join_d9_line_m1, join_d9_line_0, join_d9_line_1, join_d9_line_2, 
    join_d9_line_3, join_d9_line_4, join_d9_line_5 
};

uint64_t split_d7_lines[N_8BIT][N_DIAG_LINE];
uint64_t split_d9_lines[N_8BIT][N_DIAG_LINE];

void line_to_board_init(){
    int i, t;
    for (i = 0; i < N_8BIT; ++i){
        for (t = 0; t < N_DIAG_LINE; ++t){
            split_d7_lines[i][t] = split_d7_line(i, t + 2);
            split_d9_lines[i][t] = split_d9_line(i, t - 5);
        }
    }
}