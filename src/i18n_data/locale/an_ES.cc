﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification = {.name = tsc("an_ES"),
                       .encoding = tsc(FAST_IO_LOCALE_ENCODING),
                       .title = tsc("Aragonese locale for Spain"),
                       .source = tsc("Softaragones\t\t;\t\tfast_io"),
                       .address =
                           tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc("Jordi Mallach Pérez, Juan Pablo Martínez\t\t;\t\tfast_io"),
                       .email = tsc("bug-glibc-locales@gnu.org, softaragones@softaragones.org;euloanty@live.com"),
                       .tel = tsc(""),
                       .fax = tsc(""),
                       .language = tsc("Aragonese"),
                       .territory = tsc("Spain"),
                       .revision = tsc("1.2"),
                       .date = tsc("2018-02-05")},
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
    .time = {.abday = {tsc("dom"), tsc("lun"), tsc("mar"), tsc("mie"), tsc("chu"), tsc("vie"), tsc("sab")},
             .day = {tsc("domingo"), tsc("luns"), tsc("martes"), tsc("mierques"), tsc("chueves"), tsc("viernes"),
                     tsc("sabado")},
             .abmon = {tsc("chi"), tsc("feb"), tsc("mar"), tsc("abr"), tsc("may"), tsc("chn"), tsc("chl"), tsc("ago"),
                       tsc("set"), tsc("oct"), tsc("nov"), tsc("avi")},
             .mon = {tsc("chinero"), tsc("febrero"), tsc("marzo"), tsc("abril"), tsc("mayo"), tsc("chunyo"),
                     tsc("chuliol"), tsc("agosto"), tsc("setiembre"), tsc("octubre"), tsc("noviembre"), tsc("aviento")},
             .d_t_fmt = tsc("%a %d %b %Y %T"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%a %d %b %Y %T %Z"),
             .am_pm = {tsc(""), tsc("")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1sSyY]"), .noexpr = tsc("^[-0nN]"), .yesstr = tsc("sí"), .nostr = tsc("no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"), .int_select = tsc("00"), .int_prefix = tsc("34")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%a%N%f%N%d%N%b%N%s %h %e %r%N%z %T, %S%N%c%N"),
                .country_name = tsc("Espanya"),
                .country_post = tsc("E"),
                .country_ab2 = tsc("ES"),
                .country_ab3 = tsc("ESP"),
                .country_num = 724,
                .country_car = tsc("E"),
                .country_isbn = tsc("978-84"),
                .lang_name = tsc("aragonés"),
                .lang_ab = tsc("an"),
                .lang_term = tsc("arg"),
                .lang_lib = tsc("arg")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification = {.name = tsc(L"an_ES"),
                       .encoding = tsc(FAST_IO_LOCALE_LENCODING),
                       .title = tsc(L"Aragonese locale for Spain"),
                       .source = tsc(L"Softaragones\t\t;\t\tfast_io"),
                       .address =
                           tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(L"Jordi Mallach Pérez, Juan Pablo Martínez\t\t;\t\tfast_io"),
                       .email = tsc(L"bug-glibc-locales@gnu.org, softaragones@softaragones.org;euloanty@live.com"),
                       .tel = tsc(L""),
                       .fax = tsc(L""),
                       .language = tsc(L"Aragonese"),
                       .territory = tsc(L"Spain"),
                       .revision = tsc(L"1.2"),
                       .date = tsc(L"2018-02-05")},
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
    .time = {.abday = {tsc(L"dom"), tsc(L"lun"), tsc(L"mar"), tsc(L"mie"), tsc(L"chu"), tsc(L"vie"), tsc(L"sab")},
             .day = {tsc(L"domingo"), tsc(L"luns"), tsc(L"martes"), tsc(L"mierques"), tsc(L"chueves"), tsc(L"viernes"),
                     tsc(L"sabado")},
             .abmon = {tsc(L"chi"), tsc(L"feb"), tsc(L"mar"), tsc(L"abr"), tsc(L"may"), tsc(L"chn"), tsc(L"chl"),
                       tsc(L"ago"), tsc(L"set"), tsc(L"oct"), tsc(L"nov"), tsc(L"avi")},
             .mon = {tsc(L"chinero"), tsc(L"febrero"), tsc(L"marzo"), tsc(L"abril"), tsc(L"mayo"), tsc(L"chunyo"),
                     tsc(L"chuliol"), tsc(L"agosto"), tsc(L"setiembre"), tsc(L"octubre"), tsc(L"noviembre"),
                     tsc(L"aviento")},
             .d_t_fmt = tsc(L"%a %d %b %Y %T"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(L""), tsc(L"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1sSyY]"), .noexpr = tsc(L"^[-0nN]"), .yesstr = tsc(L"sí"), .nostr = tsc(L"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"), .int_select = tsc(L"00"), .int_prefix = tsc(L"34")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%a%N%f%N%d%N%b%N%s %h %e %r%N%z %T, %S%N%c%N"),
                .country_name = tsc(L"Espanya"),
                .country_post = tsc(L"E"),
                .country_ab2 = tsc(L"ES"),
                .country_ab3 = tsc(L"ESP"),
                .country_num = 724,
                .country_car = tsc(L"E"),
                .country_isbn = tsc(L"978-84"),
                .lang_name = tsc(L"aragonés"),
                .lang_ab = tsc(L"an"),
                .lang_term = tsc(L"arg"),
                .lang_lib = tsc(L"arg")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification = {.name = tsc(u8"an_ES"),
                       .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
                       .title = tsc(u8"Aragonese locale for Spain"),
                       .source = tsc(u8"Softaragones\t\t;\t\tfast_io"),
                       .address =
                           tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u8"Jordi Mallach Pérez, Juan Pablo Martínez\t\t;\t\tfast_io"),
                       .email = tsc(u8"bug-glibc-locales@gnu.org, softaragones@softaragones.org;euloanty@live.com"),
                       .tel = tsc(u8""),
                       .fax = tsc(u8""),
                       .language = tsc(u8"Aragonese"),
                       .territory = tsc(u8"Spain"),
                       .revision = tsc(u8"1.2"),
                       .date = tsc(u8"2018-02-05")},
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
    .time = {.abday = {tsc(u8"dom"), tsc(u8"lun"), tsc(u8"mar"), tsc(u8"mie"), tsc(u8"chu"), tsc(u8"vie"),
                       tsc(u8"sab")},
             .day = {tsc(u8"domingo"), tsc(u8"luns"), tsc(u8"martes"), tsc(u8"mierques"), tsc(u8"chueves"),
                     tsc(u8"viernes"), tsc(u8"sabado")},
             .abmon = {tsc(u8"chi"), tsc(u8"feb"), tsc(u8"mar"), tsc(u8"abr"), tsc(u8"may"), tsc(u8"chn"), tsc(u8"chl"),
                       tsc(u8"ago"), tsc(u8"set"), tsc(u8"oct"), tsc(u8"nov"), tsc(u8"avi")},
             .mon = {tsc(u8"chinero"), tsc(u8"febrero"), tsc(u8"marzo"), tsc(u8"abril"), tsc(u8"mayo"), tsc(u8"chunyo"),
                     tsc(u8"chuliol"), tsc(u8"agosto"), tsc(u8"setiembre"), tsc(u8"octubre"), tsc(u8"noviembre"),
                     tsc(u8"aviento")},
             .d_t_fmt = tsc(u8"%a %d %b %Y %T"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1sSyY]"),
                 .noexpr = tsc(u8"^[-0nN]"),
                 .yesstr = tsc(u8"sí"),
                 .nostr = tsc(u8"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"), .int_select = tsc(u8"00"), .int_prefix = tsc(u8"34")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%a%N%f%N%d%N%b%N%s %h %e %r%N%z %T, %S%N%c%N"),
                .country_name = tsc(u8"Espanya"),
                .country_post = tsc(u8"E"),
                .country_ab2 = tsc(u8"ES"),
                .country_ab3 = tsc(u8"ESP"),
                .country_num = 724,
                .country_car = tsc(u8"E"),
                .country_isbn = tsc(u8"978-84"),
                .lang_name = tsc(u8"aragonés"),
                .lang_ab = tsc(u8"an"),
                .lang_term = tsc(u8"arg"),
                .lang_lib = tsc(u8"arg")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification = {.name = tsc(u"an_ES"),
                       .encoding = tsc(FAST_IO_LOCALE_uENCODING),
                       .title = tsc(u"Aragonese locale for Spain"),
                       .source = tsc(u"Softaragones\t\t;\t\tfast_io"),
                       .address =
                           tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u"Jordi Mallach Pérez, Juan Pablo Martínez\t\t;\t\tfast_io"),
                       .email = tsc(u"bug-glibc-locales@gnu.org, softaragones@softaragones.org;euloanty@live.com"),
                       .tel = tsc(u""),
                       .fax = tsc(u""),
                       .language = tsc(u"Aragonese"),
                       .territory = tsc(u"Spain"),
                       .revision = tsc(u"1.2"),
                       .date = tsc(u"2018-02-05")},
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
    .time = {.abday = {tsc(u"dom"), tsc(u"lun"), tsc(u"mar"), tsc(u"mie"), tsc(u"chu"), tsc(u"vie"), tsc(u"sab")},
             .day = {tsc(u"domingo"), tsc(u"luns"), tsc(u"martes"), tsc(u"mierques"), tsc(u"chueves"), tsc(u"viernes"),
                     tsc(u"sabado")},
             .abmon = {tsc(u"chi"), tsc(u"feb"), tsc(u"mar"), tsc(u"abr"), tsc(u"may"), tsc(u"chn"), tsc(u"chl"),
                       tsc(u"ago"), tsc(u"set"), tsc(u"oct"), tsc(u"nov"), tsc(u"avi")},
             .mon = {tsc(u"chinero"), tsc(u"febrero"), tsc(u"marzo"), tsc(u"abril"), tsc(u"mayo"), tsc(u"chunyo"),
                     tsc(u"chuliol"), tsc(u"agosto"), tsc(u"setiembre"), tsc(u"octubre"), tsc(u"noviembre"),
                     tsc(u"aviento")},
             .d_t_fmt = tsc(u"%a %d %b %Y %T"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u""), tsc(u"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1sSyY]"), .noexpr = tsc(u"^[-0nN]"), .yesstr = tsc(u"sí"), .nostr = tsc(u"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"), .int_select = tsc(u"00"), .int_prefix = tsc(u"34")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%a%N%f%N%d%N%b%N%s %h %e %r%N%z %T, %S%N%c%N"),
                .country_name = tsc(u"Espanya"),
                .country_post = tsc(u"E"),
                .country_ab2 = tsc(u"ES"),
                .country_ab3 = tsc(u"ESP"),
                .country_num = 724,
                .country_car = tsc(u"E"),
                .country_isbn = tsc(u"978-84"),
                .lang_name = tsc(u"aragonés"),
                .lang_ab = tsc(u"an"),
                .lang_term = tsc(u"arg"),
                .lang_lib = tsc(u"arg")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification = {.name = tsc(U"an_ES"),
                       .encoding = tsc(FAST_IO_LOCALE_UENCODING),
                       .title = tsc(U"Aragonese locale for Spain"),
                       .source = tsc(U"Softaragones\t\t;\t\tfast_io"),
                       .address =
                           tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(U"Jordi Mallach Pérez, Juan Pablo Martínez\t\t;\t\tfast_io"),
                       .email = tsc(U"bug-glibc-locales@gnu.org, softaragones@softaragones.org;euloanty@live.com"),
                       .tel = tsc(U""),
                       .fax = tsc(U""),
                       .language = tsc(U"Aragonese"),
                       .territory = tsc(U"Spain"),
                       .revision = tsc(U"1.2"),
                       .date = tsc(U"2018-02-05")},
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
    .time = {.abday = {tsc(U"dom"), tsc(U"lun"), tsc(U"mar"), tsc(U"mie"), tsc(U"chu"), tsc(U"vie"), tsc(U"sab")},
             .day = {tsc(U"domingo"), tsc(U"luns"), tsc(U"martes"), tsc(U"mierques"), tsc(U"chueves"), tsc(U"viernes"),
                     tsc(U"sabado")},
             .abmon = {tsc(U"chi"), tsc(U"feb"), tsc(U"mar"), tsc(U"abr"), tsc(U"may"), tsc(U"chn"), tsc(U"chl"),
                       tsc(U"ago"), tsc(U"set"), tsc(U"oct"), tsc(U"nov"), tsc(U"avi")},
             .mon = {tsc(U"chinero"), tsc(U"febrero"), tsc(U"marzo"), tsc(U"abril"), tsc(U"mayo"), tsc(U"chunyo"),
                     tsc(U"chuliol"), tsc(U"agosto"), tsc(U"setiembre"), tsc(U"octubre"), tsc(U"noviembre"),
                     tsc(U"aviento")},
             .d_t_fmt = tsc(U"%a %d %b %Y %T"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(U""), tsc(U"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1sSyY]"), .noexpr = tsc(U"^[-0nN]"), .yesstr = tsc(U"sí"), .nostr = tsc(U"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"), .int_select = tsc(U"00"), .int_prefix = tsc(U"34")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%a%N%f%N%d%N%b%N%s %h %e %r%N%z %T, %S%N%c%N"),
                .country_name = tsc(U"Espanya"),
                .country_post = tsc(U"E"),
                .country_ab2 = tsc(U"ES"),
                .country_ab3 = tsc(U"ESP"),
                .country_num = 724,
                .country_car = tsc(U"E"),
                .country_isbn = tsc(U"978-84"),
                .lang_name = tsc(U"aragonés"),
                .lang_ab = tsc(U"an"),
                .lang_term = tsc(U"arg"),
                .lang_lib = tsc(U"arg")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"