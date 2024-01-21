﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification = {.name = tsc("kab_DZ"),
                       .encoding = tsc(FAST_IO_LOCALE_ENCODING),
                       .title = tsc("Kabyle language locale for Algeria"),
                       .source = tsc("fast_io"),
                       .contact = tsc("fast_io"),
                       .email = tsc("belkacem77@gmail.com;euloanty@live.com"),
                       .language = tsc("Kabyle"),
                       .territory = tsc("Algeria"),
                       .revision = tsc("1.0"),
                       .date = tsc("2016-07-06")},
    .monetary = {.int_curr_symbol = tsc("DZD "),
                 .currency_symbol = tsc("DA"),
                 .mon_decimal_point = tsc(","),
                 .mon_thousands_sep = tsc(""),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(""),
                 .negative_sign = tsc("-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(","), .thousands_sep = tsc(""), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc("Ace"), tsc("Ari"), tsc("Ara"), tsc("Aha"), tsc("Amh"), tsc("Sem"), tsc("Sed")},
             .day = {tsc("Acer"), tsc("Arim"), tsc("Aram"), tsc("Ahad"), tsc("Amhad"), tsc("Sem"), tsc("Sed")},
             .abmon = {tsc("Yen"), tsc("Fur"), tsc("Meɣ"), tsc("Yeb"), tsc("May"), tsc("Yun"), tsc("Yul"), tsc("ɣuc"),
                       tsc("Cte"), tsc("Tub"), tsc("Wam"), tsc("Duj")},
             .mon = {tsc("Yennayer"), tsc("Fuṛar"), tsc("Meɣres"), tsc("Yebrir"), tsc("Mayyu"), tsc("Yunyu"),
                     tsc("Yulyu"), tsc("ɣuct"), tsc("Ctembeṛ"), tsc("Tubeṛ"), tsc("Wambeṛ"), tsc("Dujembeṛ")},
             .d_t_fmt = tsc("%a %d %b %Y %T"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc("%I:%M:%S %p"),
             .date_fmt = tsc("%a %d %b %Y %T %Z"),
             .am_pm = {tsc("FT"), tsc("MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 7,
             .first_workday = 1},
    .messages = {.yesexpr = tsc("^[+1yYiI]"), .noexpr = tsc("^[-0nNuUaA]"), .yesstr = tsc("Ih"), .nostr = tsc("Uhu")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c ;%a ;%l"), .int_select = tsc("00"), .int_prefix = tsc("213")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f"),
             .name_miss = tsc("Tamsiwt"),
             .name_mr = tsc("Mas"),
             .name_mrs = tsc("Massa"),
             .name_ms = tsc("Massa")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Lezzayer"),
                .country_ab2 = tsc("DZ"),
                .country_ab3 = tsc("DZA"),
                .country_num = 12,
                .country_car = tsc("DZ"),
                .lang_name = tsc("Taqbaylit"),
                .lang_term = tsc("kab"),
                .lang_lib = tsc("kab")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification = {.name = tsc(L"kab_DZ"),
                       .encoding = tsc(FAST_IO_LOCALE_LENCODING),
                       .title = tsc(L"Kabyle language locale for Algeria"),
                       .source = tsc(L"fast_io"),
                       .contact = tsc(L"fast_io"),
                       .email = tsc(L"belkacem77@gmail.com;euloanty@live.com"),
                       .language = tsc(L"Kabyle"),
                       .territory = tsc(L"Algeria"),
                       .revision = tsc(L"1.0"),
                       .date = tsc(L"2016-07-06")},
    .monetary = {.int_curr_symbol = tsc(L"DZD "),
                 .currency_symbol = tsc(L"DA"),
                 .mon_decimal_point = tsc(L","),
                 .mon_thousands_sep = tsc(L""),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(L""),
                 .negative_sign = tsc(L"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(L","), .thousands_sep = tsc(L""), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(L"Ace"), tsc(L"Ari"), tsc(L"Ara"), tsc(L"Aha"), tsc(L"Amh"), tsc(L"Sem"), tsc(L"Sed")},
             .day = {tsc(L"Acer"), tsc(L"Arim"), tsc(L"Aram"), tsc(L"Ahad"), tsc(L"Amhad"), tsc(L"Sem"), tsc(L"Sed")},
             .abmon = {tsc(L"Yen"), tsc(L"Fur"), tsc(L"Meɣ"), tsc(L"Yeb"), tsc(L"May"), tsc(L"Yun"), tsc(L"Yul"),
                       tsc(L"ɣuc"), tsc(L"Cte"), tsc(L"Tub"), tsc(L"Wam"), tsc(L"Duj")},
             .mon = {tsc(L"Yennayer"), tsc(L"Fuṛar"), tsc(L"Meɣres"), tsc(L"Yebrir"), tsc(L"Mayyu"), tsc(L"Yunyu"),
                     tsc(L"Yulyu"), tsc(L"ɣuct"), tsc(L"Ctembeṛ"), tsc(L"Tubeṛ"), tsc(L"Wambeṛ"), tsc(L"Dujembeṛ")},
             .d_t_fmt = tsc(L"%a %d %b %Y %T"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L"%I:%M:%S %p"),
             .date_fmt = tsc(L"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(L"FT"), tsc(L"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 7,
             .first_workday = 1},
    .messages = {.yesexpr = tsc(L"^[+1yYiI]"),
                 .noexpr = tsc(L"^[-0nNuUaA]"),
                 .yesstr = tsc(L"Ih"),
                 .nostr = tsc(L"Uhu")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c ;%a ;%l"), .int_select = tsc(L"00"), .int_prefix = tsc(L"213")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f"),
             .name_miss = tsc(L"Tamsiwt"),
             .name_mr = tsc(L"Mas"),
             .name_mrs = tsc(L"Massa"),
             .name_ms = tsc(L"Massa")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Lezzayer"),
                .country_ab2 = tsc(L"DZ"),
                .country_ab3 = tsc(L"DZA"),
                .country_num = 12,
                .country_car = tsc(L"DZ"),
                .lang_name = tsc(L"Taqbaylit"),
                .lang_term = tsc(L"kab"),
                .lang_lib = tsc(L"kab")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification = {.name = tsc(u8"kab_DZ"),
                       .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
                       .title = tsc(u8"Kabyle language locale for Algeria"),
                       .source = tsc(u8"fast_io"),
                       .contact = tsc(u8"fast_io"),
                       .email = tsc(u8"belkacem77@gmail.com;euloanty@live.com"),
                       .language = tsc(u8"Kabyle"),
                       .territory = tsc(u8"Algeria"),
                       .revision = tsc(u8"1.0"),
                       .date = tsc(u8"2016-07-06")},
    .monetary = {.int_curr_symbol = tsc(u8"DZD "),
                 .currency_symbol = tsc(u8"DA"),
                 .mon_decimal_point = tsc(u8","),
                 .mon_thousands_sep = tsc(u8""),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u8""),
                 .negative_sign = tsc(u8"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u8","), .thousands_sep = tsc(u8""), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u8"Ace"), tsc(u8"Ari"), tsc(u8"Ara"), tsc(u8"Aha"), tsc(u8"Amh"), tsc(u8"Sem"),
                       tsc(u8"Sed")},
             .day = {tsc(u8"Acer"), tsc(u8"Arim"), tsc(u8"Aram"), tsc(u8"Ahad"), tsc(u8"Amhad"), tsc(u8"Sem"),
                     tsc(u8"Sed")},
             .abmon = {tsc(u8"Yen"), tsc(u8"Fur"), tsc(u8"Meɣ"), tsc(u8"Yeb"), tsc(u8"May"), tsc(u8"Yun"), tsc(u8"Yul"),
                       tsc(u8"ɣuc"), tsc(u8"Cte"), tsc(u8"Tub"), tsc(u8"Wam"), tsc(u8"Duj")},
             .mon = {tsc(u8"Yennayer"), tsc(u8"Fuṛar"), tsc(u8"Meɣres"), tsc(u8"Yebrir"), tsc(u8"Mayyu"),
                     tsc(u8"Yunyu"), tsc(u8"Yulyu"), tsc(u8"ɣuct"), tsc(u8"Ctembeṛ"), tsc(u8"Tubeṛ"), tsc(u8"Wambeṛ"),
                     tsc(u8"Dujembeṛ")},
             .d_t_fmt = tsc(u8"%a %d %b %Y %T"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8"%I:%M:%S %p"),
             .date_fmt = tsc(u8"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u8"FT"), tsc(u8"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 7,
             .first_workday = 1},
    .messages = {.yesexpr = tsc(u8"^[+1yYiI]"),
                 .noexpr = tsc(u8"^[-0nNuUaA]"),
                 .yesstr = tsc(u8"Ih"),
                 .nostr = tsc(u8"Uhu")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c ;%a ;%l"), .int_select = tsc(u8"00"), .int_prefix = tsc(u8"213")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f"),
             .name_miss = tsc(u8"Tamsiwt"),
             .name_mr = tsc(u8"Mas"),
             .name_mrs = tsc(u8"Massa"),
             .name_ms = tsc(u8"Massa")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Lezzayer"),
                .country_ab2 = tsc(u8"DZ"),
                .country_ab3 = tsc(u8"DZA"),
                .country_num = 12,
                .country_car = tsc(u8"DZ"),
                .lang_name = tsc(u8"Taqbaylit"),
                .lang_term = tsc(u8"kab"),
                .lang_lib = tsc(u8"kab")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification = {.name = tsc(u"kab_DZ"),
                       .encoding = tsc(FAST_IO_LOCALE_uENCODING),
                       .title = tsc(u"Kabyle language locale for Algeria"),
                       .source = tsc(u"fast_io"),
                       .contact = tsc(u"fast_io"),
                       .email = tsc(u"belkacem77@gmail.com;euloanty@live.com"),
                       .language = tsc(u"Kabyle"),
                       .territory = tsc(u"Algeria"),
                       .revision = tsc(u"1.0"),
                       .date = tsc(u"2016-07-06")},
    .monetary = {.int_curr_symbol = tsc(u"DZD "),
                 .currency_symbol = tsc(u"DA"),
                 .mon_decimal_point = tsc(u","),
                 .mon_thousands_sep = tsc(u""),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u""),
                 .negative_sign = tsc(u"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u","), .thousands_sep = tsc(u""), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u"Ace"), tsc(u"Ari"), tsc(u"Ara"), tsc(u"Aha"), tsc(u"Amh"), tsc(u"Sem"), tsc(u"Sed")},
             .day = {tsc(u"Acer"), tsc(u"Arim"), tsc(u"Aram"), tsc(u"Ahad"), tsc(u"Amhad"), tsc(u"Sem"), tsc(u"Sed")},
             .abmon = {tsc(u"Yen"), tsc(u"Fur"), tsc(u"Meɣ"), tsc(u"Yeb"), tsc(u"May"), tsc(u"Yun"), tsc(u"Yul"),
                       tsc(u"ɣuc"), tsc(u"Cte"), tsc(u"Tub"), tsc(u"Wam"), tsc(u"Duj")},
             .mon = {tsc(u"Yennayer"), tsc(u"Fuṛar"), tsc(u"Meɣres"), tsc(u"Yebrir"), tsc(u"Mayyu"), tsc(u"Yunyu"),
                     tsc(u"Yulyu"), tsc(u"ɣuct"), tsc(u"Ctembeṛ"), tsc(u"Tubeṛ"), tsc(u"Wambeṛ"), tsc(u"Dujembeṛ")},
             .d_t_fmt = tsc(u"%a %d %b %Y %T"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u"%I:%M:%S %p"),
             .date_fmt = tsc(u"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u"FT"), tsc(u"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 7,
             .first_workday = 1},
    .messages = {.yesexpr = tsc(u"^[+1yYiI]"),
                 .noexpr = tsc(u"^[-0nNuUaA]"),
                 .yesstr = tsc(u"Ih"),
                 .nostr = tsc(u"Uhu")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c ;%a ;%l"), .int_select = tsc(u"00"), .int_prefix = tsc(u"213")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f"),
             .name_miss = tsc(u"Tamsiwt"),
             .name_mr = tsc(u"Mas"),
             .name_mrs = tsc(u"Massa"),
             .name_ms = tsc(u"Massa")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Lezzayer"),
                .country_ab2 = tsc(u"DZ"),
                .country_ab3 = tsc(u"DZA"),
                .country_num = 12,
                .country_car = tsc(u"DZ"),
                .lang_name = tsc(u"Taqbaylit"),
                .lang_term = tsc(u"kab"),
                .lang_lib = tsc(u"kab")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification = {.name = tsc(U"kab_DZ"),
                       .encoding = tsc(FAST_IO_LOCALE_UENCODING),
                       .title = tsc(U"Kabyle language locale for Algeria"),
                       .source = tsc(U"fast_io"),
                       .contact = tsc(U"fast_io"),
                       .email = tsc(U"belkacem77@gmail.com;euloanty@live.com"),
                       .language = tsc(U"Kabyle"),
                       .territory = tsc(U"Algeria"),
                       .revision = tsc(U"1.0"),
                       .date = tsc(U"2016-07-06")},
    .monetary = {.int_curr_symbol = tsc(U"DZD "),
                 .currency_symbol = tsc(U"DA"),
                 .mon_decimal_point = tsc(U","),
                 .mon_thousands_sep = tsc(U""),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(U""),
                 .negative_sign = tsc(U"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(U","), .thousands_sep = tsc(U""), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(U"Ace"), tsc(U"Ari"), tsc(U"Ara"), tsc(U"Aha"), tsc(U"Amh"), tsc(U"Sem"), tsc(U"Sed")},
             .day = {tsc(U"Acer"), tsc(U"Arim"), tsc(U"Aram"), tsc(U"Ahad"), tsc(U"Amhad"), tsc(U"Sem"), tsc(U"Sed")},
             .abmon = {tsc(U"Yen"), tsc(U"Fur"), tsc(U"Meɣ"), tsc(U"Yeb"), tsc(U"May"), tsc(U"Yun"), tsc(U"Yul"),
                       tsc(U"ɣuc"), tsc(U"Cte"), tsc(U"Tub"), tsc(U"Wam"), tsc(U"Duj")},
             .mon = {tsc(U"Yennayer"), tsc(U"Fuṛar"), tsc(U"Meɣres"), tsc(U"Yebrir"), tsc(U"Mayyu"), tsc(U"Yunyu"),
                     tsc(U"Yulyu"), tsc(U"ɣuct"), tsc(U"Ctembeṛ"), tsc(U"Tubeṛ"), tsc(U"Wambeṛ"), tsc(U"Dujembeṛ")},
             .d_t_fmt = tsc(U"%a %d %b %Y %T"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U"%I:%M:%S %p"),
             .date_fmt = tsc(U"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(U"FT"), tsc(U"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 7,
             .first_workday = 1},
    .messages = {.yesexpr = tsc(U"^[+1yYiI]"),
                 .noexpr = tsc(U"^[-0nNuUaA]"),
                 .yesstr = tsc(U"Ih"),
                 .nostr = tsc(U"Uhu")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c ;%a ;%l"), .int_select = tsc(U"00"), .int_prefix = tsc(U"213")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f"),
             .name_miss = tsc(U"Tamsiwt"),
             .name_mr = tsc(U"Mas"),
             .name_mrs = tsc(U"Massa"),
             .name_ms = tsc(U"Massa")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Lezzayer"),
                .country_ab2 = tsc(U"DZ"),
                .country_ab3 = tsc(U"DZA"),
                .country_num = 12,
                .country_car = tsc(U"DZ"),
                .lang_name = tsc(U"Taqbaylit"),
                .lang_term = tsc(U"kab"),
                .lang_lib = tsc(U"kab")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"