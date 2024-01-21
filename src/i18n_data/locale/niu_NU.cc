﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("niu_NU"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Niuean (Vagahau Niue) locale for Niue"),
         .source = tsc("RockET Systems\t\t;\t\tfast_io"),
         .address =
             tsc("PO Box 229, Fatalupe, Alofi, "
                 "Niue\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("Emani Fakaotimanava-Lui\t\t;\t\tfast_io"),
         .email = tsc("emani@niue.nu;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Niuean"),
         .territory = tsc("Niue"),
         .revision = tsc("1.0"),
         .date = tsc("2012-07-18")},
    .monetary = {.int_curr_symbol = tsc("NZD "),
                 .currency_symbol = tsc("$"),
                 .mon_decimal_point = tsc("."),
                 .mon_thousands_sep = tsc(","),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(""),
                 .negative_sign = tsc("-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 0,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 0,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc("."), .thousands_sep = tsc(","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc("Tapu"), tsc("Gofua"), tsc("Ua"), tsc("Lotu"), tsc("Tuloto"), tsc("Falaile"), tsc("Faiumu")},
             .day = {tsc("Aho Tapu"), tsc("Aho Gofua"), tsc("Aho Ua"), tsc("Aho Lotu"), tsc("Aho Tuloto"),
                     tsc("Aho Falaile"), tsc("Aho Faiumu")},
             .abmon = {tsc("Ian"), tsc("Fep"), tsc("Mas"), tsc("Ape"), tsc("Me"), tsc("Iun"), tsc("Iul"), tsc("Aok"),
                       tsc("Sep"), tsc("Oke"), tsc("Nov"), tsc("Tes")},
             .mon = {tsc("Ianuali"), tsc("Fepuali"), tsc("Masi"), tsc("Apelila"), tsc("Me"), tsc("Iuni"), tsc("Iulai"),
                     tsc("Aokuso"), tsc("Sepetema"), tsc("Oketopa"), tsc("Novema"), tsc("Tesemo")},
             .d_t_fmt = tsc("%a %d %b %Y %T"),
             .d_fmt = tsc("%d//%m//%y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc("%I:%M:%S %p"),
             .date_fmt = tsc("%a %d %b %Y %T %Z"),
             .am_pm = {tsc("AM"), tsc("PM")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1yYēĒ]"), .noexpr = tsc("^[-0nN]"), .yesstr = tsc("Talia"), .nostr = tsc("Nakai")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"), .int_select = tsc("00"), .int_prefix = tsc("683")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Niuē"),
                .country_post = tsc("NU"),
                .country_ab2 = tsc("NU"),
                .country_ab3 = tsc("NIU"),
                .country_num = 570,
                .lang_name = tsc("ko e vagahau Niuē"),
                .lang_term = tsc("niu"),
                .lang_lib = tsc("niu")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"niu_NU"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Niuean (Vagahau Niue) locale for Niue"),
         .source = tsc(L"RockET Systems\t\t;\t\tfast_io"),
         .address =
             tsc(L"PO Box 229, Fatalupe, Alofi, "
                 L"Niue\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"Emani Fakaotimanava-Lui\t\t;\t\tfast_io"),
         .email = tsc(L"emani@niue.nu;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Niuean"),
         .territory = tsc(L"Niue"),
         .revision = tsc(L"1.0"),
         .date = tsc(L"2012-07-18")},
    .monetary = {.int_curr_symbol = tsc(L"NZD "),
                 .currency_symbol = tsc(L"$"),
                 .mon_decimal_point = tsc(L"."),
                 .mon_thousands_sep = tsc(L","),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(L""),
                 .negative_sign = tsc(L"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 0,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 0,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(L"."), .thousands_sep = tsc(L","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(L"Tapu"), tsc(L"Gofua"), tsc(L"Ua"), tsc(L"Lotu"), tsc(L"Tuloto"), tsc(L"Falaile"),
                       tsc(L"Faiumu")},
             .day = {tsc(L"Aho Tapu"), tsc(L"Aho Gofua"), tsc(L"Aho Ua"), tsc(L"Aho Lotu"), tsc(L"Aho Tuloto"),
                     tsc(L"Aho Falaile"), tsc(L"Aho Faiumu")},
             .abmon = {tsc(L"Ian"), tsc(L"Fep"), tsc(L"Mas"), tsc(L"Ape"), tsc(L"Me"), tsc(L"Iun"), tsc(L"Iul"),
                       tsc(L"Aok"), tsc(L"Sep"), tsc(L"Oke"), tsc(L"Nov"), tsc(L"Tes")},
             .mon = {tsc(L"Ianuali"), tsc(L"Fepuali"), tsc(L"Masi"), tsc(L"Apelila"), tsc(L"Me"), tsc(L"Iuni"),
                     tsc(L"Iulai"), tsc(L"Aokuso"), tsc(L"Sepetema"), tsc(L"Oketopa"), tsc(L"Novema"), tsc(L"Tesemo")},
             .d_t_fmt = tsc(L"%a %d %b %Y %T"),
             .d_fmt = tsc(L"%d//%m//%y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L"%I:%M:%S %p"),
             .date_fmt = tsc(L"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(L"AM"), tsc(L"PM")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1yYēĒ]"),
                 .noexpr = tsc(L"^[-0nN]"),
                 .yesstr = tsc(L"Talia"),
                 .nostr = tsc(L"Nakai")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"), .int_select = tsc(L"00"), .int_prefix = tsc(L"683")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Niuē"),
                .country_post = tsc(L"NU"),
                .country_ab2 = tsc(L"NU"),
                .country_ab3 = tsc(L"NIU"),
                .country_num = 570,
                .lang_name = tsc(L"ko e vagahau Niuē"),
                .lang_term = tsc(L"niu"),
                .lang_lib = tsc(L"niu")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"niu_NU"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Niuean (Vagahau Niue) locale for Niue"),
         .source = tsc(u8"RockET Systems\t\t;\t\tfast_io"),
         .address =
             tsc(u8"PO Box 229, Fatalupe, Alofi, "
                 u8"Niue\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"Emani Fakaotimanava-Lui\t\t;\t\tfast_io"),
         .email = tsc(u8"emani@niue.nu;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Niuean"),
         .territory = tsc(u8"Niue"),
         .revision = tsc(u8"1.0"),
         .date = tsc(u8"2012-07-18")},
    .monetary = {.int_curr_symbol = tsc(u8"NZD "),
                 .currency_symbol = tsc(u8"$"),
                 .mon_decimal_point = tsc(u8"."),
                 .mon_thousands_sep = tsc(u8","),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u8""),
                 .negative_sign = tsc(u8"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 0,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 0,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u8"."), .thousands_sep = tsc(u8","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u8"Tapu"), tsc(u8"Gofua"), tsc(u8"Ua"), tsc(u8"Lotu"), tsc(u8"Tuloto"), tsc(u8"Falaile"),
                       tsc(u8"Faiumu")},
             .day = {tsc(u8"Aho Tapu"), tsc(u8"Aho Gofua"), tsc(u8"Aho Ua"), tsc(u8"Aho Lotu"), tsc(u8"Aho Tuloto"),
                     tsc(u8"Aho Falaile"), tsc(u8"Aho Faiumu")},
             .abmon = {tsc(u8"Ian"), tsc(u8"Fep"), tsc(u8"Mas"), tsc(u8"Ape"), tsc(u8"Me"), tsc(u8"Iun"), tsc(u8"Iul"),
                       tsc(u8"Aok"), tsc(u8"Sep"), tsc(u8"Oke"), tsc(u8"Nov"), tsc(u8"Tes")},
             .mon = {tsc(u8"Ianuali"), tsc(u8"Fepuali"), tsc(u8"Masi"), tsc(u8"Apelila"), tsc(u8"Me"), tsc(u8"Iuni"),
                     tsc(u8"Iulai"), tsc(u8"Aokuso"), tsc(u8"Sepetema"), tsc(u8"Oketopa"), tsc(u8"Novema"),
                     tsc(u8"Tesemo")},
             .d_t_fmt = tsc(u8"%a %d %b %Y %T"),
             .d_fmt = tsc(u8"%d//%m//%y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8"%I:%M:%S %p"),
             .date_fmt = tsc(u8"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u8"AM"), tsc(u8"PM")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1yYēĒ]"),
                 .noexpr = tsc(u8"^[-0nN]"),
                 .yesstr = tsc(u8"Talia"),
                 .nostr = tsc(u8"Nakai")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"), .int_select = tsc(u8"00"), .int_prefix = tsc(u8"683")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Niuē"),
                .country_post = tsc(u8"NU"),
                .country_ab2 = tsc(u8"NU"),
                .country_ab3 = tsc(u8"NIU"),
                .country_num = 570,
                .lang_name = tsc(u8"ko e vagahau Niuē"),
                .lang_term = tsc(u8"niu"),
                .lang_lib = tsc(u8"niu")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"niu_NU"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Niuean (Vagahau Niue) locale for Niue"),
         .source = tsc(u"RockET Systems\t\t;\t\tfast_io"),
         .address =
             tsc(u"PO Box 229, Fatalupe, Alofi, "
                 u"Niue\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"Emani Fakaotimanava-Lui\t\t;\t\tfast_io"),
         .email = tsc(u"emani@niue.nu;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Niuean"),
         .territory = tsc(u"Niue"),
         .revision = tsc(u"1.0"),
         .date = tsc(u"2012-07-18")},
    .monetary = {.int_curr_symbol = tsc(u"NZD "),
                 .currency_symbol = tsc(u"$"),
                 .mon_decimal_point = tsc(u"."),
                 .mon_thousands_sep = tsc(u","),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u""),
                 .negative_sign = tsc(u"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 0,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 0,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u"."), .thousands_sep = tsc(u","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u"Tapu"), tsc(u"Gofua"), tsc(u"Ua"), tsc(u"Lotu"), tsc(u"Tuloto"), tsc(u"Falaile"),
                       tsc(u"Faiumu")},
             .day = {tsc(u"Aho Tapu"), tsc(u"Aho Gofua"), tsc(u"Aho Ua"), tsc(u"Aho Lotu"), tsc(u"Aho Tuloto"),
                     tsc(u"Aho Falaile"), tsc(u"Aho Faiumu")},
             .abmon = {tsc(u"Ian"), tsc(u"Fep"), tsc(u"Mas"), tsc(u"Ape"), tsc(u"Me"), tsc(u"Iun"), tsc(u"Iul"),
                       tsc(u"Aok"), tsc(u"Sep"), tsc(u"Oke"), tsc(u"Nov"), tsc(u"Tes")},
             .mon = {tsc(u"Ianuali"), tsc(u"Fepuali"), tsc(u"Masi"), tsc(u"Apelila"), tsc(u"Me"), tsc(u"Iuni"),
                     tsc(u"Iulai"), tsc(u"Aokuso"), tsc(u"Sepetema"), tsc(u"Oketopa"), tsc(u"Novema"), tsc(u"Tesemo")},
             .d_t_fmt = tsc(u"%a %d %b %Y %T"),
             .d_fmt = tsc(u"%d//%m//%y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u"%I:%M:%S %p"),
             .date_fmt = tsc(u"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u"AM"), tsc(u"PM")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1yYēĒ]"),
                 .noexpr = tsc(u"^[-0nN]"),
                 .yesstr = tsc(u"Talia"),
                 .nostr = tsc(u"Nakai")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"), .int_select = tsc(u"00"), .int_prefix = tsc(u"683")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Niuē"),
                .country_post = tsc(u"NU"),
                .country_ab2 = tsc(u"NU"),
                .country_ab3 = tsc(u"NIU"),
                .country_num = 570,
                .lang_name = tsc(u"ko e vagahau Niuē"),
                .lang_term = tsc(u"niu"),
                .lang_lib = tsc(u"niu")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"niu_NU"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Niuean (Vagahau Niue) locale for Niue"),
         .source = tsc(U"RockET Systems\t\t;\t\tfast_io"),
         .address =
             tsc(U"PO Box 229, Fatalupe, Alofi, "
                 U"Niue\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"Emani Fakaotimanava-Lui\t\t;\t\tfast_io"),
         .email = tsc(U"emani@niue.nu;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Niuean"),
         .territory = tsc(U"Niue"),
         .revision = tsc(U"1.0"),
         .date = tsc(U"2012-07-18")},
    .monetary = {.int_curr_symbol = tsc(U"NZD "),
                 .currency_symbol = tsc(U"$"),
                 .mon_decimal_point = tsc(U"."),
                 .mon_thousands_sep = tsc(U","),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(U""),
                 .negative_sign = tsc(U"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 0,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 0,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(U"."), .thousands_sep = tsc(U","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(U"Tapu"), tsc(U"Gofua"), tsc(U"Ua"), tsc(U"Lotu"), tsc(U"Tuloto"), tsc(U"Falaile"),
                       tsc(U"Faiumu")},
             .day = {tsc(U"Aho Tapu"), tsc(U"Aho Gofua"), tsc(U"Aho Ua"), tsc(U"Aho Lotu"), tsc(U"Aho Tuloto"),
                     tsc(U"Aho Falaile"), tsc(U"Aho Faiumu")},
             .abmon = {tsc(U"Ian"), tsc(U"Fep"), tsc(U"Mas"), tsc(U"Ape"), tsc(U"Me"), tsc(U"Iun"), tsc(U"Iul"),
                       tsc(U"Aok"), tsc(U"Sep"), tsc(U"Oke"), tsc(U"Nov"), tsc(U"Tes")},
             .mon = {tsc(U"Ianuali"), tsc(U"Fepuali"), tsc(U"Masi"), tsc(U"Apelila"), tsc(U"Me"), tsc(U"Iuni"),
                     tsc(U"Iulai"), tsc(U"Aokuso"), tsc(U"Sepetema"), tsc(U"Oketopa"), tsc(U"Novema"), tsc(U"Tesemo")},
             .d_t_fmt = tsc(U"%a %d %b %Y %T"),
             .d_fmt = tsc(U"%d//%m//%y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U"%I:%M:%S %p"),
             .date_fmt = tsc(U"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(U"AM"), tsc(U"PM")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1yYēĒ]"),
                 .noexpr = tsc(U"^[-0nN]"),
                 .yesstr = tsc(U"Talia"),
                 .nostr = tsc(U"Nakai")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"), .int_select = tsc(U"00"), .int_prefix = tsc(U"683")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Niuē"),
                .country_post = tsc(U"NU"),
                .country_ab2 = tsc(U"NU"),
                .country_ab3 = tsc(U"NIU"),
                .country_num = 570,
                .lang_name = tsc(U"ko e vagahau Niuē"),
                .lang_term = tsc(U"niu"),
                .lang_lib = tsc(U"niu")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"