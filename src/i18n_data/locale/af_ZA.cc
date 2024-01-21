﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("af_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Afrikaans locale for South Africa"),
         .source = tsc("Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc("Box 28364, Sunnyside, 0132, South "
                 "Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc("dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Afrikaans"),
         .territory = tsc("South Africa"),
         .revision = tsc("1.2.1"),
         .date = tsc("2005-10-13")},
    .monetary = {.int_curr_symbol = tsc("ZAR "),
                 .currency_symbol = tsc("R"),
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
    .time = {.abday = {tsc("So"), tsc("Ma"), tsc("Di"), tsc("Wo"), tsc("Do"), tsc("Vr"), tsc("Sa")},
             .day = {tsc("Sondag"), tsc("Maandag"), tsc("Dinsdag"), tsc("Woensdag"), tsc("Donderdag"), tsc("Vrydag"),
                     tsc("Saterdag")},
             .abmon = {tsc("Jan"), tsc("Feb"), tsc("Mrt"), tsc("Apr"), tsc("Mei"), tsc("Jun"), tsc("Jul"), tsc("Aug"),
                       tsc("Sep"), tsc("Okt"), tsc("Nov"), tsc("Des")},
             .mon = {tsc("Januarie"), tsc("Februarie"), tsc("Maart"), tsc("April"), tsc("Mei"), tsc("Junie"),
                     tsc("Julie"), tsc("Augustus"), tsc("September"), tsc("Oktober"), tsc("November"), tsc("Desember")},
             .d_t_fmt = tsc("%a %d %b %Y %T"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%a %d %b %Y %T %Z"),
             .am_pm = {tsc("VM"), tsc("NM")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc("^[+1jJyY]"), .noexpr = tsc("^[-0nN]"), .yesstr = tsc("ja"), .nostr = tsc("nee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"),
                  .tel_dom_fmt = tsc("(%A) %l"),
                  .int_select = tsc("00"),
                  .int_prefix = tsc("27")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f"),
             .name_gen = tsc(""),
             .name_miss = tsc("mej"),
             .name_mr = tsc("mnr"),
             .name_mrs = tsc("mev"),
             .name_ms = tsc("me")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Suid-Afrika"),
                .country_post = tsc("ZA"),
                .country_ab2 = tsc("ZA"),
                .country_ab3 = tsc("ZAF"),
                .country_num = 710,
                .country_car = tsc("ZA"),
                .country_isbn = tsc("0"),
                .lang_name = tsc("Afrikaans"),
                .lang_ab = tsc("af"),
                .lang_term = tsc("afr"),
                .lang_lib = tsc("afr")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"af_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Afrikaans locale for South Africa"),
         .source = tsc(L"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc(L"Box 28364, Sunnyside, 0132, South "
                 L"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(L"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Afrikaans"),
         .territory = tsc(L"South Africa"),
         .revision = tsc(L"1.2.1"),
         .date = tsc(L"2005-10-13")},
    .monetary = {.int_curr_symbol = tsc(L"ZAR "),
                 .currency_symbol = tsc(L"R"),
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
    .time = {.abday = {tsc(L"So"), tsc(L"Ma"), tsc(L"Di"), tsc(L"Wo"), tsc(L"Do"), tsc(L"Vr"), tsc(L"Sa")},
             .day = {tsc(L"Sondag"), tsc(L"Maandag"), tsc(L"Dinsdag"), tsc(L"Woensdag"), tsc(L"Donderdag"),
                     tsc(L"Vrydag"), tsc(L"Saterdag")},
             .abmon = {tsc(L"Jan"), tsc(L"Feb"), tsc(L"Mrt"), tsc(L"Apr"), tsc(L"Mei"), tsc(L"Jun"), tsc(L"Jul"),
                       tsc(L"Aug"), tsc(L"Sep"), tsc(L"Okt"), tsc(L"Nov"), tsc(L"Des")},
             .mon = {tsc(L"Januarie"), tsc(L"Februarie"), tsc(L"Maart"), tsc(L"April"), tsc(L"Mei"), tsc(L"Junie"),
                     tsc(L"Julie"), tsc(L"Augustus"), tsc(L"September"), tsc(L"Oktober"), tsc(L"November"),
                     tsc(L"Desember")},
             .d_t_fmt = tsc(L"%a %d %b %Y %T"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(L"VM"), tsc(L"NM")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(L"^[+1jJyY]"), .noexpr = tsc(L"^[-0nN]"), .yesstr = tsc(L"ja"), .nostr = tsc(L"nee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"),
                  .tel_dom_fmt = tsc(L"(%A) %l"),
                  .int_select = tsc(L"00"),
                  .int_prefix = tsc(L"27")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f"),
             .name_gen = tsc(L""),
             .name_miss = tsc(L"mej"),
             .name_mr = tsc(L"mnr"),
             .name_mrs = tsc(L"mev"),
             .name_ms = tsc(L"me")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Suid-Afrika"),
                .country_post = tsc(L"ZA"),
                .country_ab2 = tsc(L"ZA"),
                .country_ab3 = tsc(L"ZAF"),
                .country_num = 710,
                .country_car = tsc(L"ZA"),
                .country_isbn = tsc(L"0"),
                .lang_name = tsc(L"Afrikaans"),
                .lang_ab = tsc(L"af"),
                .lang_term = tsc(L"afr"),
                .lang_lib = tsc(L"afr")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"af_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Afrikaans locale for South Africa"),
         .source = tsc(u8"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address = tsc(
             u8"Box 28364, Sunnyside, 0132, South "
             u8"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(u8"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Afrikaans"),
         .territory = tsc(u8"South Africa"),
         .revision = tsc(u8"1.2.1"),
         .date = tsc(u8"2005-10-13")},
    .monetary = {.int_curr_symbol = tsc(u8"ZAR "),
                 .currency_symbol = tsc(u8"R"),
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
    .time = {.abday = {tsc(u8"So"), tsc(u8"Ma"), tsc(u8"Di"), tsc(u8"Wo"), tsc(u8"Do"), tsc(u8"Vr"), tsc(u8"Sa")},
             .day = {tsc(u8"Sondag"), tsc(u8"Maandag"), tsc(u8"Dinsdag"), tsc(u8"Woensdag"), tsc(u8"Donderdag"),
                     tsc(u8"Vrydag"), tsc(u8"Saterdag")},
             .abmon = {tsc(u8"Jan"), tsc(u8"Feb"), tsc(u8"Mrt"), tsc(u8"Apr"), tsc(u8"Mei"), tsc(u8"Jun"), tsc(u8"Jul"),
                       tsc(u8"Aug"), tsc(u8"Sep"), tsc(u8"Okt"), tsc(u8"Nov"), tsc(u8"Des")},
             .mon = {tsc(u8"Januarie"), tsc(u8"Februarie"), tsc(u8"Maart"), tsc(u8"April"), tsc(u8"Mei"),
                     tsc(u8"Junie"), tsc(u8"Julie"), tsc(u8"Augustus"), tsc(u8"September"), tsc(u8"Oktober"),
                     tsc(u8"November"), tsc(u8"Desember")},
             .d_t_fmt = tsc(u8"%a %d %b %Y %T"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u8"VM"), tsc(u8"NM")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u8"^[+1jJyY]"),
                 .noexpr = tsc(u8"^[-0nN]"),
                 .yesstr = tsc(u8"ja"),
                 .nostr = tsc(u8"nee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"),
                  .tel_dom_fmt = tsc(u8"(%A) %l"),
                  .int_select = tsc(u8"00"),
                  .int_prefix = tsc(u8"27")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u8""),
             .name_miss = tsc(u8"mej"),
             .name_mr = tsc(u8"mnr"),
             .name_mrs = tsc(u8"mev"),
             .name_ms = tsc(u8"me")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Suid-Afrika"),
                .country_post = tsc(u8"ZA"),
                .country_ab2 = tsc(u8"ZA"),
                .country_ab3 = tsc(u8"ZAF"),
                .country_num = 710,
                .country_car = tsc(u8"ZA"),
                .country_isbn = tsc(u8"0"),
                .lang_name = tsc(u8"Afrikaans"),
                .lang_ab = tsc(u8"af"),
                .lang_term = tsc(u8"afr"),
                .lang_lib = tsc(u8"afr")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"af_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Afrikaans locale for South Africa"),
         .source = tsc(u"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc(u"Box 28364, Sunnyside, 0132, South "
                 u"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(u"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Afrikaans"),
         .territory = tsc(u"South Africa"),
         .revision = tsc(u"1.2.1"),
         .date = tsc(u"2005-10-13")},
    .monetary = {.int_curr_symbol = tsc(u"ZAR "),
                 .currency_symbol = tsc(u"R"),
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
    .time = {.abday = {tsc(u"So"), tsc(u"Ma"), tsc(u"Di"), tsc(u"Wo"), tsc(u"Do"), tsc(u"Vr"), tsc(u"Sa")},
             .day = {tsc(u"Sondag"), tsc(u"Maandag"), tsc(u"Dinsdag"), tsc(u"Woensdag"), tsc(u"Donderdag"),
                     tsc(u"Vrydag"), tsc(u"Saterdag")},
             .abmon = {tsc(u"Jan"), tsc(u"Feb"), tsc(u"Mrt"), tsc(u"Apr"), tsc(u"Mei"), tsc(u"Jun"), tsc(u"Jul"),
                       tsc(u"Aug"), tsc(u"Sep"), tsc(u"Okt"), tsc(u"Nov"), tsc(u"Des")},
             .mon = {tsc(u"Januarie"), tsc(u"Februarie"), tsc(u"Maart"), tsc(u"April"), tsc(u"Mei"), tsc(u"Junie"),
                     tsc(u"Julie"), tsc(u"Augustus"), tsc(u"September"), tsc(u"Oktober"), tsc(u"November"),
                     tsc(u"Desember")},
             .d_t_fmt = tsc(u"%a %d %b %Y %T"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u"VM"), tsc(u"NM")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u"^[+1jJyY]"), .noexpr = tsc(u"^[-0nN]"), .yesstr = tsc(u"ja"), .nostr = tsc(u"nee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"),
                  .tel_dom_fmt = tsc(u"(%A) %l"),
                  .int_select = tsc(u"00"),
                  .int_prefix = tsc(u"27")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u""),
             .name_miss = tsc(u"mej"),
             .name_mr = tsc(u"mnr"),
             .name_mrs = tsc(u"mev"),
             .name_ms = tsc(u"me")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Suid-Afrika"),
                .country_post = tsc(u"ZA"),
                .country_ab2 = tsc(u"ZA"),
                .country_ab3 = tsc(u"ZAF"),
                .country_num = 710,
                .country_car = tsc(u"ZA"),
                .country_isbn = tsc(u"0"),
                .lang_name = tsc(u"Afrikaans"),
                .lang_ab = tsc(u"af"),
                .lang_term = tsc(u"afr"),
                .lang_lib = tsc(u"afr")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"af_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Afrikaans locale for South Africa"),
         .source = tsc(U"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc(U"Box 28364, Sunnyside, 0132, South "
                 U"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(U"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Afrikaans"),
         .territory = tsc(U"South Africa"),
         .revision = tsc(U"1.2.1"),
         .date = tsc(U"2005-10-13")},
    .monetary = {.int_curr_symbol = tsc(U"ZAR "),
                 .currency_symbol = tsc(U"R"),
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
    .time = {.abday = {tsc(U"So"), tsc(U"Ma"), tsc(U"Di"), tsc(U"Wo"), tsc(U"Do"), tsc(U"Vr"), tsc(U"Sa")},
             .day = {tsc(U"Sondag"), tsc(U"Maandag"), tsc(U"Dinsdag"), tsc(U"Woensdag"), tsc(U"Donderdag"),
                     tsc(U"Vrydag"), tsc(U"Saterdag")},
             .abmon = {tsc(U"Jan"), tsc(U"Feb"), tsc(U"Mrt"), tsc(U"Apr"), tsc(U"Mei"), tsc(U"Jun"), tsc(U"Jul"),
                       tsc(U"Aug"), tsc(U"Sep"), tsc(U"Okt"), tsc(U"Nov"), tsc(U"Des")},
             .mon = {tsc(U"Januarie"), tsc(U"Februarie"), tsc(U"Maart"), tsc(U"April"), tsc(U"Mei"), tsc(U"Junie"),
                     tsc(U"Julie"), tsc(U"Augustus"), tsc(U"September"), tsc(U"Oktober"), tsc(U"November"),
                     tsc(U"Desember")},
             .d_t_fmt = tsc(U"%a %d %b %Y %T"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(U"VM"), tsc(U"NM")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(U"^[+1jJyY]"), .noexpr = tsc(U"^[-0nN]"), .yesstr = tsc(U"ja"), .nostr = tsc(U"nee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"),
                  .tel_dom_fmt = tsc(U"(%A) %l"),
                  .int_select = tsc(U"00"),
                  .int_prefix = tsc(U"27")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f"),
             .name_gen = tsc(U""),
             .name_miss = tsc(U"mej"),
             .name_mr = tsc(U"mnr"),
             .name_mrs = tsc(U"mev"),
             .name_ms = tsc(U"me")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Suid-Afrika"),
                .country_post = tsc(U"ZA"),
                .country_ab2 = tsc(U"ZA"),
                .country_ab3 = tsc(U"ZAF"),
                .country_num = 710,
                .country_car = tsc(U"ZA"),
                .country_isbn = tsc(U"0"),
                .lang_name = tsc(U"Afrikaans"),
                .lang_ab = tsc(U"af"),
                .lang_term = tsc(U"afr"),
                .lang_lib = tsc(U"afr")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"