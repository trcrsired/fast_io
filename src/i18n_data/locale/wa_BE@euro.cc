﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification = {.name = tsc("wa_BE@euro"),
                       .encoding = tsc(FAST_IO_LOCALE_ENCODING),
                       .title = tsc("Walloon locale for Belgium with Euro"),
                       .source = tsc("Free Software Foundation, Inc.\t\t;\t\tfast_io"),
                       .address = tsc("https://www.gnu.org/software/libc/\t\t;\t\thttps://gitee.com/qabeowjbtkwb/"
                                      "fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc("fast_io"),
                       .email = tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(""),
                       .fax = tsc(""),
                       .language = tsc("Walloon"),
                       .territory = tsc("Belgium"),
                       .revision = tsc("1.0"),
                       .date = tsc("2002-02-23")},
    .monetary = {.int_curr_symbol = tsc("EUR "),
                 .currency_symbol = tsc("€"),
                 .mon_decimal_point = tsc(","),
                 .mon_thousands_sep = tsc("."),
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
    .numeric = {.decimal_point = tsc(","), .thousands_sep = tsc("."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc("dim"), tsc("lon"), tsc("mår"), tsc("mie"), tsc("dju"), tsc("vén"), tsc("sem")},
             .day = {tsc("dimegne"), tsc("londi"), tsc("mårdi"), tsc("mierkidi"), tsc("djudi"), tsc("vénrdi"),
                     tsc("semdi")},
             .abmon = {tsc("dja"), tsc("fev"), tsc("mås"), tsc("avr"), tsc("may"), tsc("djn"), tsc("djl"), tsc("awo"),
                       tsc("set"), tsc("oct"), tsc("nôv"), tsc("dec")},
             .mon = {tsc("di djanvî"), tsc("di fevrî"), tsc("di måss"), tsc("d’ avri"), tsc("di may"), tsc("di djun"),
                     tsc("di djulete"), tsc("d’ awousse"), tsc("di setimbe"), tsc("d’ octôbe"), tsc("di nôvimbe"),
                     tsc("di decimbe")},
             .d_t_fmt = tsc("Li %A %d %B %Y %T"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%H:%M:%S"),
             .t_fmt_ampm = tsc("%I:%M:%S %p"),
             .date_fmt = tsc("Li %A %d %B %Y %T %Z"),
             .am_pm = {tsc("AM"), tsc("PM")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1oOyYaAwW]"), .noexpr = tsc("^[-0nN]"), .yesstr = tsc("Oyi"), .nostr = tsc("Neni")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"), .int_select = tsc("00"), .int_prefix = tsc("32")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f"),
             .name_gen = tsc(""),
             .name_miss = tsc("Mle"),
             .name_mr = tsc("M."),
             .name_mrs = tsc("Mme"),
             .name_ms = tsc("Mme")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Beldjike"),
                .country_post = tsc("B"),
                .country_ab2 = tsc("BE"),
                .country_ab3 = tsc("BEL"),
                .country_num = 56,
                .country_car = tsc("B"),
                .country_isbn = tsc("2"),
                .lang_name = tsc("walon"),
                .lang_ab = tsc("wa"),
                .lang_term = tsc("wln"),
                .lang_lib = tsc("wln")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification = {.name = tsc(L"wa_BE@euro"),
                       .encoding = tsc(FAST_IO_LOCALE_LENCODING),
                       .title = tsc(L"Walloon locale for Belgium with Euro"),
                       .source = tsc(L"Free Software Foundation, Inc.\t\t;\t\tfast_io"),
                       .address = tsc(L"https://www.gnu.org/software/libc/\t\t;\t\thttps://gitee.com/qabeowjbtkwb/"
                                      L"fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(L"fast_io"),
                       .email = tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(L""),
                       .fax = tsc(L""),
                       .language = tsc(L"Walloon"),
                       .territory = tsc(L"Belgium"),
                       .revision = tsc(L"1.0"),
                       .date = tsc(L"2002-02-23")},
    .monetary = {.int_curr_symbol = tsc(L"EUR "),
                 .currency_symbol = tsc(L"€"),
                 .mon_decimal_point = tsc(L","),
                 .mon_thousands_sep = tsc(L"."),
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
    .numeric = {.decimal_point = tsc(L","), .thousands_sep = tsc(L"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(L"dim"), tsc(L"lon"), tsc(L"mår"), tsc(L"mie"), tsc(L"dju"), tsc(L"vén"), tsc(L"sem")},
             .day = {tsc(L"dimegne"), tsc(L"londi"), tsc(L"mårdi"), tsc(L"mierkidi"), tsc(L"djudi"), tsc(L"vénrdi"),
                     tsc(L"semdi")},
             .abmon = {tsc(L"dja"), tsc(L"fev"), tsc(L"mås"), tsc(L"avr"), tsc(L"may"), tsc(L"djn"), tsc(L"djl"),
                       tsc(L"awo"), tsc(L"set"), tsc(L"oct"), tsc(L"nôv"), tsc(L"dec")},
             .mon = {tsc(L"di djanvî"), tsc(L"di fevrî"), tsc(L"di måss"), tsc(L"d’ avri"), tsc(L"di may"),
                     tsc(L"di djun"), tsc(L"di djulete"), tsc(L"d’ awousse"), tsc(L"di setimbe"), tsc(L"d’ octôbe"),
                     tsc(L"di nôvimbe"), tsc(L"di decimbe")},
             .d_t_fmt = tsc(L"Li %A %d %B %Y %T"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%H:%M:%S"),
             .t_fmt_ampm = tsc(L"%I:%M:%S %p"),
             .date_fmt = tsc(L"Li %A %d %B %Y %T %Z"),
             .am_pm = {tsc(L"AM"), tsc(L"PM")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1oOyYaAwW]"),
                 .noexpr = tsc(L"^[-0nN]"),
                 .yesstr = tsc(L"Oyi"),
                 .nostr = tsc(L"Neni")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"), .int_select = tsc(L"00"), .int_prefix = tsc(L"32")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f"),
             .name_gen = tsc(L""),
             .name_miss = tsc(L"Mle"),
             .name_mr = tsc(L"M."),
             .name_mrs = tsc(L"Mme"),
             .name_ms = tsc(L"Mme")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Beldjike"),
                .country_post = tsc(L"B"),
                .country_ab2 = tsc(L"BE"),
                .country_ab3 = tsc(L"BEL"),
                .country_num = 56,
                .country_car = tsc(L"B"),
                .country_isbn = tsc(L"2"),
                .lang_name = tsc(L"walon"),
                .lang_ab = tsc(L"wa"),
                .lang_term = tsc(L"wln"),
                .lang_lib = tsc(L"wln")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification = {.name = tsc(u8"wa_BE@euro"),
                       .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
                       .title = tsc(u8"Walloon locale for Belgium with Euro"),
                       .source = tsc(u8"Free Software Foundation, Inc.\t\t;\t\tfast_io"),
                       .address = tsc(u8"https://www.gnu.org/software/libc/\t\t;\t\thttps://gitee.com/qabeowjbtkwb/"
                                      u8"fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u8"fast_io"),
                       .email = tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(u8""),
                       .fax = tsc(u8""),
                       .language = tsc(u8"Walloon"),
                       .territory = tsc(u8"Belgium"),
                       .revision = tsc(u8"1.0"),
                       .date = tsc(u8"2002-02-23")},
    .monetary = {.int_curr_symbol = tsc(u8"EUR "),
                 .currency_symbol = tsc(u8"€"),
                 .mon_decimal_point = tsc(u8","),
                 .mon_thousands_sep = tsc(u8"."),
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
    .numeric = {.decimal_point = tsc(u8","), .thousands_sep = tsc(u8"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u8"dim"), tsc(u8"lon"), tsc(u8"mår"), tsc(u8"mie"), tsc(u8"dju"), tsc(u8"vén"),
                       tsc(u8"sem")},
             .day = {tsc(u8"dimegne"), tsc(u8"londi"), tsc(u8"mårdi"), tsc(u8"mierkidi"), tsc(u8"djudi"),
                     tsc(u8"vénrdi"), tsc(u8"semdi")},
             .abmon = {tsc(u8"dja"), tsc(u8"fev"), tsc(u8"mås"), tsc(u8"avr"), tsc(u8"may"), tsc(u8"djn"), tsc(u8"djl"),
                       tsc(u8"awo"), tsc(u8"set"), tsc(u8"oct"), tsc(u8"nôv"), tsc(u8"dec")},
             .mon = {tsc(u8"di djanvî"), tsc(u8"di fevrî"), tsc(u8"di måss"), tsc(u8"d’ avri"), tsc(u8"di may"),
                     tsc(u8"di djun"), tsc(u8"di djulete"), tsc(u8"d’ awousse"), tsc(u8"di setimbe"),
                     tsc(u8"d’ octôbe"), tsc(u8"di nôvimbe"), tsc(u8"di decimbe")},
             .d_t_fmt = tsc(u8"Li %A %d %B %Y %T"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%H:%M:%S"),
             .t_fmt_ampm = tsc(u8"%I:%M:%S %p"),
             .date_fmt = tsc(u8"Li %A %d %B %Y %T %Z"),
             .am_pm = {tsc(u8"AM"), tsc(u8"PM")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1oOyYaAwW]"),
                 .noexpr = tsc(u8"^[-0nN]"),
                 .yesstr = tsc(u8"Oyi"),
                 .nostr = tsc(u8"Neni")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"), .int_select = tsc(u8"00"), .int_prefix = tsc(u8"32")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u8""),
             .name_miss = tsc(u8"Mle"),
             .name_mr = tsc(u8"M."),
             .name_mrs = tsc(u8"Mme"),
             .name_ms = tsc(u8"Mme")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Beldjike"),
                .country_post = tsc(u8"B"),
                .country_ab2 = tsc(u8"BE"),
                .country_ab3 = tsc(u8"BEL"),
                .country_num = 56,
                .country_car = tsc(u8"B"),
                .country_isbn = tsc(u8"2"),
                .lang_name = tsc(u8"walon"),
                .lang_ab = tsc(u8"wa"),
                .lang_term = tsc(u8"wln"),
                .lang_lib = tsc(u8"wln")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification = {.name = tsc(u"wa_BE@euro"),
                       .encoding = tsc(FAST_IO_LOCALE_uENCODING),
                       .title = tsc(u"Walloon locale for Belgium with Euro"),
                       .source = tsc(u"Free Software Foundation, Inc.\t\t;\t\tfast_io"),
                       .address = tsc(u"https://www.gnu.org/software/libc/\t\t;\t\thttps://gitee.com/qabeowjbtkwb/"
                                      u"fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u"fast_io"),
                       .email = tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(u""),
                       .fax = tsc(u""),
                       .language = tsc(u"Walloon"),
                       .territory = tsc(u"Belgium"),
                       .revision = tsc(u"1.0"),
                       .date = tsc(u"2002-02-23")},
    .monetary = {.int_curr_symbol = tsc(u"EUR "),
                 .currency_symbol = tsc(u"€"),
                 .mon_decimal_point = tsc(u","),
                 .mon_thousands_sep = tsc(u"."),
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
    .numeric = {.decimal_point = tsc(u","), .thousands_sep = tsc(u"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u"dim"), tsc(u"lon"), tsc(u"mår"), tsc(u"mie"), tsc(u"dju"), tsc(u"vén"), tsc(u"sem")},
             .day = {tsc(u"dimegne"), tsc(u"londi"), tsc(u"mårdi"), tsc(u"mierkidi"), tsc(u"djudi"), tsc(u"vénrdi"),
                     tsc(u"semdi")},
             .abmon = {tsc(u"dja"), tsc(u"fev"), tsc(u"mås"), tsc(u"avr"), tsc(u"may"), tsc(u"djn"), tsc(u"djl"),
                       tsc(u"awo"), tsc(u"set"), tsc(u"oct"), tsc(u"nôv"), tsc(u"dec")},
             .mon = {tsc(u"di djanvî"), tsc(u"di fevrî"), tsc(u"di måss"), tsc(u"d’ avri"), tsc(u"di may"),
                     tsc(u"di djun"), tsc(u"di djulete"), tsc(u"d’ awousse"), tsc(u"di setimbe"), tsc(u"d’ octôbe"),
                     tsc(u"di nôvimbe"), tsc(u"di decimbe")},
             .d_t_fmt = tsc(u"Li %A %d %B %Y %T"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%H:%M:%S"),
             .t_fmt_ampm = tsc(u"%I:%M:%S %p"),
             .date_fmt = tsc(u"Li %A %d %B %Y %T %Z"),
             .am_pm = {tsc(u"AM"), tsc(u"PM")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1oOyYaAwW]"),
                 .noexpr = tsc(u"^[-0nN]"),
                 .yesstr = tsc(u"Oyi"),
                 .nostr = tsc(u"Neni")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"), .int_select = tsc(u"00"), .int_prefix = tsc(u"32")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u""),
             .name_miss = tsc(u"Mle"),
             .name_mr = tsc(u"M."),
             .name_mrs = tsc(u"Mme"),
             .name_ms = tsc(u"Mme")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Beldjike"),
                .country_post = tsc(u"B"),
                .country_ab2 = tsc(u"BE"),
                .country_ab3 = tsc(u"BEL"),
                .country_num = 56,
                .country_car = tsc(u"B"),
                .country_isbn = tsc(u"2"),
                .lang_name = tsc(u"walon"),
                .lang_ab = tsc(u"wa"),
                .lang_term = tsc(u"wln"),
                .lang_lib = tsc(u"wln")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification = {.name = tsc(U"wa_BE@euro"),
                       .encoding = tsc(FAST_IO_LOCALE_UENCODING),
                       .title = tsc(U"Walloon locale for Belgium with Euro"),
                       .source = tsc(U"Free Software Foundation, Inc.\t\t;\t\tfast_io"),
                       .address = tsc(U"https://www.gnu.org/software/libc/\t\t;\t\thttps://gitee.com/qabeowjbtkwb/"
                                      U"fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(U"fast_io"),
                       .email = tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(U""),
                       .fax = tsc(U""),
                       .language = tsc(U"Walloon"),
                       .territory = tsc(U"Belgium"),
                       .revision = tsc(U"1.0"),
                       .date = tsc(U"2002-02-23")},
    .monetary = {.int_curr_symbol = tsc(U"EUR "),
                 .currency_symbol = tsc(U"€"),
                 .mon_decimal_point = tsc(U","),
                 .mon_thousands_sep = tsc(U"."),
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
    .numeric = {.decimal_point = tsc(U","), .thousands_sep = tsc(U"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(U"dim"), tsc(U"lon"), tsc(U"mår"), tsc(U"mie"), tsc(U"dju"), tsc(U"vén"), tsc(U"sem")},
             .day = {tsc(U"dimegne"), tsc(U"londi"), tsc(U"mårdi"), tsc(U"mierkidi"), tsc(U"djudi"), tsc(U"vénrdi"),
                     tsc(U"semdi")},
             .abmon = {tsc(U"dja"), tsc(U"fev"), tsc(U"mås"), tsc(U"avr"), tsc(U"may"), tsc(U"djn"), tsc(U"djl"),
                       tsc(U"awo"), tsc(U"set"), tsc(U"oct"), tsc(U"nôv"), tsc(U"dec")},
             .mon = {tsc(U"di djanvî"), tsc(U"di fevrî"), tsc(U"di måss"), tsc(U"d’ avri"), tsc(U"di may"),
                     tsc(U"di djun"), tsc(U"di djulete"), tsc(U"d’ awousse"), tsc(U"di setimbe"), tsc(U"d’ octôbe"),
                     tsc(U"di nôvimbe"), tsc(U"di decimbe")},
             .d_t_fmt = tsc(U"Li %A %d %B %Y %T"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%H:%M:%S"),
             .t_fmt_ampm = tsc(U"%I:%M:%S %p"),
             .date_fmt = tsc(U"Li %A %d %B %Y %T %Z"),
             .am_pm = {tsc(U"AM"), tsc(U"PM")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1oOyYaAwW]"),
                 .noexpr = tsc(U"^[-0nN]"),
                 .yesstr = tsc(U"Oyi"),
                 .nostr = tsc(U"Neni")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"), .int_select = tsc(U"00"), .int_prefix = tsc(U"32")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f"),
             .name_gen = tsc(U""),
             .name_miss = tsc(U"Mle"),
             .name_mr = tsc(U"M."),
             .name_mrs = tsc(U"Mme"),
             .name_ms = tsc(U"Mme")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Beldjike"),
                .country_post = tsc(U"B"),
                .country_ab2 = tsc(U"BE"),
                .country_ab3 = tsc(U"BEL"),
                .country_num = 56,
                .country_car = tsc(U"B"),
                .country_isbn = tsc(U"2"),
                .lang_name = tsc(U"walon"),
                .lang_ab = tsc(U"wa"),
                .lang_term = tsc(U"wln"),
                .lang_lib = tsc(U"wln")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"