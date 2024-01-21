﻿#pragma once

namespace fast_io::details::codecvt {
inline constexpr char8_t first_unit_info[256][2]{
    {0, 0},    {1, 0},    {2, 0},    {3, 0},    {4, 0},    {5, 0},    {6, 0},    {7, 0},    {8, 0},    {9, 0},
    {10, 0},   {11, 0},   {12, 0},   {13, 0},   {14, 0},   {15, 0},   {16, 0},   {17, 0},   {18, 0},   {19, 0},
    {20, 0},   {21, 0},   {22, 0},   {23, 0},   {24, 0},   {25, 0},   {26, 0},   {27, 0},   {28, 0},   {29, 0},
    {30, 0},   {31, 0},   {32, 0},   {33, 0},   {34, 0},   {35, 0},   {36, 0},   {37, 0},   {38, 0},   {39, 0},
    {40, 0},   {41, 0},   {42, 0},   {43, 0},   {44, 0},   {45, 0},   {46, 0},   {47, 0},   {48, 0},   {49, 0},
    {50, 0},   {51, 0},   {52, 0},   {53, 0},   {54, 0},   {55, 0},   {56, 0},   {57, 0},   {58, 0},   {59, 0},
    {60, 0},   {61, 0},   {62, 0},   {63, 0},   {64, 0},   {65, 0},   {66, 0},   {67, 0},   {68, 0},   {69, 0},
    {70, 0},   {71, 0},   {72, 0},   {73, 0},   {74, 0},   {75, 0},   {76, 0},   {77, 0},   {78, 0},   {79, 0},
    {80, 0},   {81, 0},   {82, 0},   {83, 0},   {84, 0},   {85, 0},   {86, 0},   {87, 0},   {88, 0},   {89, 0},
    {90, 0},   {91, 0},   {92, 0},   {93, 0},   {94, 0},   {95, 0},   {96, 0},   {97, 0},   {98, 0},   {99, 0},
    {100, 0},  {101, 0},  {102, 0},  {103, 0},  {104, 0},  {105, 0},  {106, 0},  {107, 0},  {108, 0},  {109, 0},
    {110, 0},  {111, 0},  {112, 0},  {113, 0},  {114, 0},  {115, 0},  {116, 0},  {117, 0},  {118, 0},  {119, 0},
    {120, 0},  {121, 0},  {122, 0},  {123, 0},  {124, 0},  {125, 0},  {126, 0},  {127, 0},  {0, 12},   {1, 12},
    {2, 12},   {3, 12},   {4, 12},   {5, 12},   {6, 12},   {7, 12},   {8, 12},   {9, 12},   {10, 12},  {11, 12},
    {12, 12},  {13, 12},  {14, 12},  {15, 12},  {16, 12},  {17, 12},  {18, 12},  {19, 12},  {20, 12},  {21, 12},
    {22, 12},  {23, 12},  {24, 12},  {25, 12},  {26, 12},  {27, 12},  {28, 12},  {29, 12},  {30, 12},  {31, 12},
    {32, 12},  {33, 12},  {34, 12},  {35, 12},  {36, 12},  {37, 12},  {38, 12},  {39, 12},  {40, 12},  {41, 12},
    {42, 12},  {43, 12},  {44, 12},  {45, 12},  {46, 12},  {47, 12},  {48, 12},  {49, 12},  {50, 12},  {51, 12},
    {52, 12},  {53, 12},  {54, 12},  {55, 12},  {56, 12},  {57, 12},  {58, 12},  {59, 12},  {60, 12},  {61, 12},
    {62, 12},  {63, 12},  {192, 12}, {193, 12}, {2, 24},   {3, 24},   {4, 24},   {5, 24},   {6, 24},   {7, 24},
    {8, 24},   {9, 24},   {10, 24},  {11, 24},  {12, 24},  {13, 24},  {14, 24},  {15, 24},  {16, 24},  {17, 24},
    {18, 24},  {19, 24},  {20, 24},  {21, 24},  {22, 24},  {23, 24},  {24, 24},  {25, 24},  {26, 24},  {27, 24},
    {28, 24},  {29, 24},  {30, 24},  {31, 24},  {0, 60},   {1, 36},   {2, 36},   {3, 36},   {4, 36},   {5, 36},
    {6, 36},   {7, 36},   {8, 36},   {9, 36},   {10, 36},  {11, 36},  {12, 36},  {13, 72},  {14, 36},  {15, 36},
    {0, 84},   {1, 48},   {2, 48},   {3, 48},   {4, 96},   {245, 12}, {246, 12}, {247, 12}, {248, 12}, {249, 12},
    {250, 12}, {251, 12}, {252, 12}, {253, 12}, {254, 12}, {255, 12}};
inline constexpr char8_t octet_category[256]{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,  4,  4,  4,  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7, 9, 10, 10, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr char8_t transitions[108]{
    12, 0,  12, 12, 12, 24, 60, 36, 72, 84, 48, 96, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 0,
    0,  0,  12, 12, 12, 12, 12, 12, 12, 12, 12, 24, 24, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 36, 36, 36, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 24, 24, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 36, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12};

} // namespace fast_io::details::codecvt