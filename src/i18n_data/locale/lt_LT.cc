﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("lt_LT"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Lithuanian locale for Lithuania"),
         .source = tsc("Lithuanian Computer Society and\t\t;\t\tfast_io"),
         .address = tsc(
             "P.O Box 1147, Donelaicio 60, 3000 Kaunas, "
             "Lithuania\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("fast_io"),
         .email = tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Lithuanian"),
         .territory = tsc("Lithuania"),
         .revision = tsc("1.0"),
         .date = tsc("2000-06-29")},
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
    .time = {.abday = {tsc("Sk"), tsc("Pr"), tsc("An"), tsc("Tr"), tsc("Kt"), tsc("Pn"), tsc("Št")},
             .day = {tsc("Sekmadienis"), tsc("Pirmadienis"), tsc("Antradienis"), tsc("Trečiadienis"),
                     tsc("Ketvirtadienis"), tsc("Penktadienis"), tsc("Šeštadienis")},
             .abmon = {tsc("saus."), tsc("vas."), tsc("kov."), tsc("bal."), tsc("geg."), tsc("birž."), tsc("liep."),
                       tsc("rugp."), tsc("rugs."), tsc("spal."), tsc("lapkr."), tsc("gruod.")},
             .mon = {tsc("sausio"), tsc("vasario"), tsc("kovo"), tsc("balandžio"), tsc("gegužės"), tsc("birželio"),
                     tsc("liepos"), tsc("rugpjūčio"), tsc("rugsėjo"), tsc("spalio"), tsc("lapkričio"), tsc("gruodžio")},
             .d_t_fmt = tsc("%Y m. %B %d d. %T"),
             .d_fmt = tsc("%Y-%m-%d"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%Y m. %B %d d. %T %Z"),
             .am_pm = {tsc(""), tsc("")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1TtYy]"), .noexpr = tsc("^[-0Nn]"), .yesstr = tsc("taip"), .nostr = tsc("ne")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"), .int_select = tsc("00"), .int_prefix = tsc("370")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Lietuva"),
                .country_ab2 = tsc("LT"),
                .country_ab3 = tsc("LTU"),
                .country_num = 440,
                .country_car = tsc("LT"),
                .lang_name = tsc("lietuvių"),
                .lang_ab = tsc("lt"),
                .lang_term = tsc("lit"),
                .lang_lib = tsc("lit")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"lt_LT"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Lithuanian locale for Lithuania"),
         .source = tsc(L"Lithuanian Computer Society and\t\t;\t\tfast_io"),
         .address = tsc(
             L"P.O Box 1147, Donelaicio 60, 3000 Kaunas, "
             L"Lithuania\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"fast_io"),
         .email = tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Lithuanian"),
         .territory = tsc(L"Lithuania"),
         .revision = tsc(L"1.0"),
         .date = tsc(L"2000-06-29")},
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
    .time = {.abday = {tsc(L"Sk"), tsc(L"Pr"), tsc(L"An"), tsc(L"Tr"), tsc(L"Kt"), tsc(L"Pn"), tsc(L"Št")},
             .day = {tsc(L"Sekmadienis"), tsc(L"Pirmadienis"), tsc(L"Antradienis"), tsc(L"Trečiadienis"),
                     tsc(L"Ketvirtadienis"), tsc(L"Penktadienis"), tsc(L"Šeštadienis")},
             .abmon = {tsc(L"saus."), tsc(L"vas."), tsc(L"kov."), tsc(L"bal."), tsc(L"geg."), tsc(L"birž."),
                       tsc(L"liep."), tsc(L"rugp."), tsc(L"rugs."), tsc(L"spal."), tsc(L"lapkr."), tsc(L"gruod.")},
             .mon = {tsc(L"sausio"), tsc(L"vasario"), tsc(L"kovo"), tsc(L"balandžio"), tsc(L"gegužės"),
                     tsc(L"birželio"), tsc(L"liepos"), tsc(L"rugpjūčio"), tsc(L"rugsėjo"), tsc(L"spalio"),
                     tsc(L"lapkričio"), tsc(L"gruodžio")},
             .d_t_fmt = tsc(L"%Y m. %B %d d. %T"),
             .d_fmt = tsc(L"%Y-%m-%d"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%Y m. %B %d d. %T %Z"),
             .am_pm = {tsc(L""), tsc(L"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1TtYy]"), .noexpr = tsc(L"^[-0Nn]"), .yesstr = tsc(L"taip"), .nostr = tsc(L"ne")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"), .int_select = tsc(L"00"), .int_prefix = tsc(L"370")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Lietuva"),
                .country_ab2 = tsc(L"LT"),
                .country_ab3 = tsc(L"LTU"),
                .country_num = 440,
                .country_car = tsc(L"LT"),
                .lang_name = tsc(L"lietuvių"),
                .lang_ab = tsc(L"lt"),
                .lang_term = tsc(L"lit"),
                .lang_lib = tsc(L"lit")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"lt_LT"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Lithuanian locale for Lithuania"),
         .source = tsc(u8"Lithuanian Computer Society and\t\t;\t\tfast_io"),
         .address = tsc(
             u8"P.O Box 1147, Donelaicio 60, 3000 Kaunas, "
             u8"Lithuania\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"fast_io"),
         .email = tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Lithuanian"),
         .territory = tsc(u8"Lithuania"),
         .revision = tsc(u8"1.0"),
         .date = tsc(u8"2000-06-29")},
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
    .time = {.abday = {tsc(u8"Sk"), tsc(u8"Pr"), tsc(u8"An"), tsc(u8"Tr"), tsc(u8"Kt"), tsc(u8"Pn"), tsc(u8"Št")},
             .day = {tsc(u8"Sekmadienis"), tsc(u8"Pirmadienis"), tsc(u8"Antradienis"), tsc(u8"Trečiadienis"),
                     tsc(u8"Ketvirtadienis"), tsc(u8"Penktadienis"), tsc(u8"Šeštadienis")},
             .abmon = {tsc(u8"saus."), tsc(u8"vas."), tsc(u8"kov."), tsc(u8"bal."), tsc(u8"geg."), tsc(u8"birž."),
                       tsc(u8"liep."), tsc(u8"rugp."), tsc(u8"rugs."), tsc(u8"spal."), tsc(u8"lapkr."),
                       tsc(u8"gruod.")},
             .mon = {tsc(u8"sausio"), tsc(u8"vasario"), tsc(u8"kovo"), tsc(u8"balandžio"), tsc(u8"gegužės"),
                     tsc(u8"birželio"), tsc(u8"liepos"), tsc(u8"rugpjūčio"), tsc(u8"rugsėjo"), tsc(u8"spalio"),
                     tsc(u8"lapkričio"), tsc(u8"gruodžio")},
             .d_t_fmt = tsc(u8"%Y m. %B %d d. %T"),
             .d_fmt = tsc(u8"%Y-%m-%d"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%Y m. %B %d d. %T %Z"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1TtYy]"),
                 .noexpr = tsc(u8"^[-0Nn]"),
                 .yesstr = tsc(u8"taip"),
                 .nostr = tsc(u8"ne")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"), .int_select = tsc(u8"00"), .int_prefix = tsc(u8"370")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Lietuva"),
                .country_ab2 = tsc(u8"LT"),
                .country_ab3 = tsc(u8"LTU"),
                .country_num = 440,
                .country_car = tsc(u8"LT"),
                .lang_name = tsc(u8"lietuvių"),
                .lang_ab = tsc(u8"lt"),
                .lang_term = tsc(u8"lit"),
                .lang_lib = tsc(u8"lit")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"lt_LT"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Lithuanian locale for Lithuania"),
         .source = tsc(u"Lithuanian Computer Society and\t\t;\t\tfast_io"),
         .address = tsc(
             u"P.O Box 1147, Donelaicio 60, 3000 Kaunas, "
             u"Lithuania\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"fast_io"),
         .email = tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Lithuanian"),
         .territory = tsc(u"Lithuania"),
         .revision = tsc(u"1.0"),
         .date = tsc(u"2000-06-29")},
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
    .time = {.abday = {tsc(u"Sk"), tsc(u"Pr"), tsc(u"An"), tsc(u"Tr"), tsc(u"Kt"), tsc(u"Pn"), tsc(u"Št")},
             .day = {tsc(u"Sekmadienis"), tsc(u"Pirmadienis"), tsc(u"Antradienis"), tsc(u"Trečiadienis"),
                     tsc(u"Ketvirtadienis"), tsc(u"Penktadienis"), tsc(u"Šeštadienis")},
             .abmon = {tsc(u"saus."), tsc(u"vas."), tsc(u"kov."), tsc(u"bal."), tsc(u"geg."), tsc(u"birž."),
                       tsc(u"liep."), tsc(u"rugp."), tsc(u"rugs."), tsc(u"spal."), tsc(u"lapkr."), tsc(u"gruod.")},
             .mon = {tsc(u"sausio"), tsc(u"vasario"), tsc(u"kovo"), tsc(u"balandžio"), tsc(u"gegužės"),
                     tsc(u"birželio"), tsc(u"liepos"), tsc(u"rugpjūčio"), tsc(u"rugsėjo"), tsc(u"spalio"),
                     tsc(u"lapkričio"), tsc(u"gruodžio")},
             .d_t_fmt = tsc(u"%Y m. %B %d d. %T"),
             .d_fmt = tsc(u"%Y-%m-%d"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%Y m. %B %d d. %T %Z"),
             .am_pm = {tsc(u""), tsc(u"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1TtYy]"), .noexpr = tsc(u"^[-0Nn]"), .yesstr = tsc(u"taip"), .nostr = tsc(u"ne")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"), .int_select = tsc(u"00"), .int_prefix = tsc(u"370")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Lietuva"),
                .country_ab2 = tsc(u"LT"),
                .country_ab3 = tsc(u"LTU"),
                .country_num = 440,
                .country_car = tsc(u"LT"),
                .lang_name = tsc(u"lietuvių"),
                .lang_ab = tsc(u"lt"),
                .lang_term = tsc(u"lit"),
                .lang_lib = tsc(u"lit")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"lt_LT"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Lithuanian locale for Lithuania"),
         .source = tsc(U"Lithuanian Computer Society and\t\t;\t\tfast_io"),
         .address = tsc(
             U"P.O Box 1147, Donelaicio 60, 3000 Kaunas, "
             U"Lithuania\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"fast_io"),
         .email = tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Lithuanian"),
         .territory = tsc(U"Lithuania"),
         .revision = tsc(U"1.0"),
         .date = tsc(U"2000-06-29")},
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
    .time = {.abday = {tsc(U"Sk"), tsc(U"Pr"), tsc(U"An"), tsc(U"Tr"), tsc(U"Kt"), tsc(U"Pn"), tsc(U"Št")},
             .day = {tsc(U"Sekmadienis"), tsc(U"Pirmadienis"), tsc(U"Antradienis"), tsc(U"Trečiadienis"),
                     tsc(U"Ketvirtadienis"), tsc(U"Penktadienis"), tsc(U"Šeštadienis")},
             .abmon = {tsc(U"saus."), tsc(U"vas."), tsc(U"kov."), tsc(U"bal."), tsc(U"geg."), tsc(U"birž."),
                       tsc(U"liep."), tsc(U"rugp."), tsc(U"rugs."), tsc(U"spal."), tsc(U"lapkr."), tsc(U"gruod.")},
             .mon = {tsc(U"sausio"), tsc(U"vasario"), tsc(U"kovo"), tsc(U"balandžio"), tsc(U"gegužės"),
                     tsc(U"birželio"), tsc(U"liepos"), tsc(U"rugpjūčio"), tsc(U"rugsėjo"), tsc(U"spalio"),
                     tsc(U"lapkričio"), tsc(U"gruodžio")},
             .d_t_fmt = tsc(U"%Y m. %B %d d. %T"),
             .d_fmt = tsc(U"%Y-%m-%d"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%Y m. %B %d d. %T %Z"),
             .am_pm = {tsc(U""), tsc(U"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1TtYy]"), .noexpr = tsc(U"^[-0Nn]"), .yesstr = tsc(U"taip"), .nostr = tsc(U"ne")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"), .int_select = tsc(U"00"), .int_prefix = tsc(U"370")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Lietuva"),
                .country_ab2 = tsc(U"LT"),
                .country_ab3 = tsc(U"LTU"),
                .country_num = 440,
                .country_car = tsc(U"LT"),
                .lang_name = tsc(U"lietuvių"),
                .lang_ab = tsc(U"lt"),
                .lang_term = tsc(U"lit"),
                .lang_lib = tsc(U"lit")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"