﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("ts_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Tsonga locale for South Africa"),
         .source = tsc("Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc("PO Box 28364, Sunnyside, 0132, South "
                 "Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc("dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Tsonga"),
         .territory = tsc("South Africa"),
         .revision = tsc("0.3"),
         .date = tsc("2005-10-12")},
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
    .time = {.abday = {tsc("Son"), tsc("Mus"), tsc("Bir"), tsc("Har"), tsc("Ne"), tsc("Tlh"), tsc("Mug")},
             .day = {tsc("Sonto"), tsc("Musumbhunuku"), tsc("Ravumbirhi"), tsc("Ravunharhu"), tsc("Ravumune"),
                     tsc("Ravuntlhanu"), tsc("Mugqivela")},
             .abmon = {tsc("Sun"), tsc("Yan"), tsc("Kul"), tsc("Dzi"), tsc("Mud"), tsc("Kho"), tsc("Maw"), tsc("Mha"),
                       tsc("Ndz"), tsc("Nhl"), tsc("Huk"), tsc("N'w")},
             .mon = {tsc("Sunguti"), tsc("Nyenyenyani"), tsc("Nyenyankulu"), tsc("Dzivamisoko"), tsc("Mudyaxihi"),
                     tsc("Khotavuxika"), tsc("Mawuwani"), tsc("Mhawuri"), tsc("Ndzhati"), tsc("Nhlangula"),
                     tsc("Hukuri"), tsc("N'wendzamhala")},
             .d_t_fmt = tsc("%a %-e %b %Y %T %Z"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%a %b %-e %H:%M:%S %Z %Y"),
             .am_pm = {tsc(""), tsc("")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc("^[+1iIyY]"), .noexpr = tsc("^[-0eEnN]"), .yesstr = tsc("Ina"), .nostr = tsc("Ee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"),
                  .tel_dom_fmt = tsc("(%A) %l"),
                  .int_select = tsc("00"),
                  .int_prefix = tsc("27")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Afrika Dzonga"),
                .country_post = tsc("ZA"),
                .country_ab2 = tsc("ZA"),
                .country_ab3 = tsc("ZAF"),
                .country_num = 710,
                .country_car = tsc("ZA"),
                .lang_name = tsc("Xitsonga"),
                .lang_ab = tsc("ts"),
                .lang_term = tsc("tso"),
                .lang_lib = tsc("tso")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"ts_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Tsonga locale for South Africa"),
         .source = tsc(L"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc(L"PO Box 28364, Sunnyside, 0132, South "
                 L"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(L"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Tsonga"),
         .territory = tsc(L"South Africa"),
         .revision = tsc(L"0.3"),
         .date = tsc(L"2005-10-12")},
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
    .time = {.abday = {tsc(L"Son"), tsc(L"Mus"), tsc(L"Bir"), tsc(L"Har"), tsc(L"Ne"), tsc(L"Tlh"), tsc(L"Mug")},
             .day = {tsc(L"Sonto"), tsc(L"Musumbhunuku"), tsc(L"Ravumbirhi"), tsc(L"Ravunharhu"), tsc(L"Ravumune"),
                     tsc(L"Ravuntlhanu"), tsc(L"Mugqivela")},
             .abmon = {tsc(L"Sun"), tsc(L"Yan"), tsc(L"Kul"), tsc(L"Dzi"), tsc(L"Mud"), tsc(L"Kho"), tsc(L"Maw"),
                       tsc(L"Mha"), tsc(L"Ndz"), tsc(L"Nhl"), tsc(L"Huk"), tsc(L"N'w")},
             .mon = {tsc(L"Sunguti"), tsc(L"Nyenyenyani"), tsc(L"Nyenyankulu"), tsc(L"Dzivamisoko"), tsc(L"Mudyaxihi"),
                     tsc(L"Khotavuxika"), tsc(L"Mawuwani"), tsc(L"Mhawuri"), tsc(L"Ndzhati"), tsc(L"Nhlangula"),
                     tsc(L"Hukuri"), tsc(L"N'wendzamhala")},
             .d_t_fmt = tsc(L"%a %-e %b %Y %T %Z"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%a %b %-e %H:%M:%S %Z %Y"),
             .am_pm = {tsc(L""), tsc(L"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(L"^[+1iIyY]"), .noexpr = tsc(L"^[-0eEnN]"), .yesstr = tsc(L"Ina"), .nostr = tsc(L"Ee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"),
                  .tel_dom_fmt = tsc(L"(%A) %l"),
                  .int_select = tsc(L"00"),
                  .int_prefix = tsc(L"27")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Afrika Dzonga"),
                .country_post = tsc(L"ZA"),
                .country_ab2 = tsc(L"ZA"),
                .country_ab3 = tsc(L"ZAF"),
                .country_num = 710,
                .country_car = tsc(L"ZA"),
                .lang_name = tsc(L"Xitsonga"),
                .lang_ab = tsc(L"ts"),
                .lang_term = tsc(L"tso"),
                .lang_lib = tsc(L"tso")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"ts_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Tsonga locale for South Africa"),
         .source = tsc(u8"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address = tsc(
             u8"PO Box 28364, Sunnyside, 0132, South "
             u8"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(u8"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Tsonga"),
         .territory = tsc(u8"South Africa"),
         .revision = tsc(u8"0.3"),
         .date = tsc(u8"2005-10-12")},
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
    .time = {.abday = {tsc(u8"Son"), tsc(u8"Mus"), tsc(u8"Bir"), tsc(u8"Har"), tsc(u8"Ne"), tsc(u8"Tlh"), tsc(u8"Mug")},
             .day = {tsc(u8"Sonto"), tsc(u8"Musumbhunuku"), tsc(u8"Ravumbirhi"), tsc(u8"Ravunharhu"), tsc(u8"Ravumune"),
                     tsc(u8"Ravuntlhanu"), tsc(u8"Mugqivela")},
             .abmon = {tsc(u8"Sun"), tsc(u8"Yan"), tsc(u8"Kul"), tsc(u8"Dzi"), tsc(u8"Mud"), tsc(u8"Kho"), tsc(u8"Maw"),
                       tsc(u8"Mha"), tsc(u8"Ndz"), tsc(u8"Nhl"), tsc(u8"Huk"), tsc(u8"N'w")},
             .mon = {tsc(u8"Sunguti"), tsc(u8"Nyenyenyani"), tsc(u8"Nyenyankulu"), tsc(u8"Dzivamisoko"),
                     tsc(u8"Mudyaxihi"), tsc(u8"Khotavuxika"), tsc(u8"Mawuwani"), tsc(u8"Mhawuri"), tsc(u8"Ndzhati"),
                     tsc(u8"Nhlangula"), tsc(u8"Hukuri"), tsc(u8"N'wendzamhala")},
             .d_t_fmt = tsc(u8"%a %-e %b %Y %T %Z"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%a %b %-e %H:%M:%S %Z %Y"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u8"^[+1iIyY]"),
                 .noexpr = tsc(u8"^[-0eEnN]"),
                 .yesstr = tsc(u8"Ina"),
                 .nostr = tsc(u8"Ee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"),
                  .tel_dom_fmt = tsc(u8"(%A) %l"),
                  .int_select = tsc(u8"00"),
                  .int_prefix = tsc(u8"27")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Afrika Dzonga"),
                .country_post = tsc(u8"ZA"),
                .country_ab2 = tsc(u8"ZA"),
                .country_ab3 = tsc(u8"ZAF"),
                .country_num = 710,
                .country_car = tsc(u8"ZA"),
                .lang_name = tsc(u8"Xitsonga"),
                .lang_ab = tsc(u8"ts"),
                .lang_term = tsc(u8"tso"),
                .lang_lib = tsc(u8"tso")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"ts_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Tsonga locale for South Africa"),
         .source = tsc(u"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc(u"PO Box 28364, Sunnyside, 0132, South "
                 u"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(u"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Tsonga"),
         .territory = tsc(u"South Africa"),
         .revision = tsc(u"0.3"),
         .date = tsc(u"2005-10-12")},
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
    .time = {.abday = {tsc(u"Son"), tsc(u"Mus"), tsc(u"Bir"), tsc(u"Har"), tsc(u"Ne"), tsc(u"Tlh"), tsc(u"Mug")},
             .day = {tsc(u"Sonto"), tsc(u"Musumbhunuku"), tsc(u"Ravumbirhi"), tsc(u"Ravunharhu"), tsc(u"Ravumune"),
                     tsc(u"Ravuntlhanu"), tsc(u"Mugqivela")},
             .abmon = {tsc(u"Sun"), tsc(u"Yan"), tsc(u"Kul"), tsc(u"Dzi"), tsc(u"Mud"), tsc(u"Kho"), tsc(u"Maw"),
                       tsc(u"Mha"), tsc(u"Ndz"), tsc(u"Nhl"), tsc(u"Huk"), tsc(u"N'w")},
             .mon = {tsc(u"Sunguti"), tsc(u"Nyenyenyani"), tsc(u"Nyenyankulu"), tsc(u"Dzivamisoko"), tsc(u"Mudyaxihi"),
                     tsc(u"Khotavuxika"), tsc(u"Mawuwani"), tsc(u"Mhawuri"), tsc(u"Ndzhati"), tsc(u"Nhlangula"),
                     tsc(u"Hukuri"), tsc(u"N'wendzamhala")},
             .d_t_fmt = tsc(u"%a %-e %b %Y %T %Z"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%a %b %-e %H:%M:%S %Z %Y"),
             .am_pm = {tsc(u""), tsc(u"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u"^[+1iIyY]"), .noexpr = tsc(u"^[-0eEnN]"), .yesstr = tsc(u"Ina"), .nostr = tsc(u"Ee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"),
                  .tel_dom_fmt = tsc(u"(%A) %l"),
                  .int_select = tsc(u"00"),
                  .int_prefix = tsc(u"27")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Afrika Dzonga"),
                .country_post = tsc(u"ZA"),
                .country_ab2 = tsc(u"ZA"),
                .country_ab3 = tsc(u"ZAF"),
                .country_num = 710,
                .country_car = tsc(u"ZA"),
                .lang_name = tsc(u"Xitsonga"),
                .lang_ab = tsc(u"ts"),
                .lang_term = tsc(u"tso"),
                .lang_lib = tsc(u"tso")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"ts_ZA"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Tsonga locale for South Africa"),
         .source = tsc(U"Zuza Software Foundation (Translate.org.za)\t\t;\t\tfast_io"),
         .address =
             tsc(U"PO Box 28364, Sunnyside, 0132, South "
                 U"Africa\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"Dwayne Bailey\t\t;\t\tfast_io"),
         .email = tsc(U"dwayne@translate.org.za;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Tsonga"),
         .territory = tsc(U"South Africa"),
         .revision = tsc(U"0.3"),
         .date = tsc(U"2005-10-12")},
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
    .time = {.abday = {tsc(U"Son"), tsc(U"Mus"), tsc(U"Bir"), tsc(U"Har"), tsc(U"Ne"), tsc(U"Tlh"), tsc(U"Mug")},
             .day = {tsc(U"Sonto"), tsc(U"Musumbhunuku"), tsc(U"Ravumbirhi"), tsc(U"Ravunharhu"), tsc(U"Ravumune"),
                     tsc(U"Ravuntlhanu"), tsc(U"Mugqivela")},
             .abmon = {tsc(U"Sun"), tsc(U"Yan"), tsc(U"Kul"), tsc(U"Dzi"), tsc(U"Mud"), tsc(U"Kho"), tsc(U"Maw"),
                       tsc(U"Mha"), tsc(U"Ndz"), tsc(U"Nhl"), tsc(U"Huk"), tsc(U"N'w")},
             .mon = {tsc(U"Sunguti"), tsc(U"Nyenyenyani"), tsc(U"Nyenyankulu"), tsc(U"Dzivamisoko"), tsc(U"Mudyaxihi"),
                     tsc(U"Khotavuxika"), tsc(U"Mawuwani"), tsc(U"Mhawuri"), tsc(U"Ndzhati"), tsc(U"Nhlangula"),
                     tsc(U"Hukuri"), tsc(U"N'wendzamhala")},
             .d_t_fmt = tsc(U"%a %-e %b %Y %T %Z"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%a %b %-e %H:%M:%S %Z %Y"),
             .am_pm = {tsc(U""), tsc(U"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(U"^[+1iIyY]"), .noexpr = tsc(U"^[-0eEnN]"), .yesstr = tsc(U"Ina"), .nostr = tsc(U"Ee")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"),
                  .tel_dom_fmt = tsc(U"(%A) %l"),
                  .int_select = tsc(U"00"),
                  .int_prefix = tsc(U"27")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Afrika Dzonga"),
                .country_post = tsc(U"ZA"),
                .country_ab2 = tsc(U"ZA"),
                .country_ab3 = tsc(U"ZAF"),
                .country_num = 710,
                .country_car = tsc(U"ZA"),
                .lang_name = tsc(U"Xitsonga"),
                .lang_ab = tsc(U"ts"),
                .lang_term = tsc(U"tso"),
                .lang_lib = tsc(U"tso")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"