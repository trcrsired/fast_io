﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t monetary_mon_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("aa_ER@saaho"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Afar language locale for Eritrea (Saaho Dialect)."),
         .source = tsc("Ge'ez Frontier Foundation\t\t;\t\tfast_io"),
         .address =
             tsc("7802 Solomon Seal Dr., Springfield, VA 22152, "
                 "USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("fast_io"),
         .email = tsc("locales@geez.org;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Afar"),
         .territory = tsc("Eritrea"),
         .revision = tsc("0.20"),
         .date = tsc("2003-07-05")},
    .monetary = {.int_curr_symbol = tsc("ERN "),
                 .currency_symbol = tsc("Nfk"),
                 .mon_decimal_point = tsc("."),
                 .mon_thousands_sep = tsc(","),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
                 .positive_sign = tsc(""),
                 .negative_sign = tsc("-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc("."), .thousands_sep = tsc("")},
    .time = {.abday = {tsc("Nab"), tsc("San"), tsc("Sal"), tsc("Rab"), tsc("Cam"), tsc("Jum"), tsc("Qun")},
             .day = {tsc("Naba Sambat"), tsc("Sani"), tsc("Salus"), tsc("Rabuq"), tsc("Camus"), tsc("Jumqata"),
                     tsc("Qunxa Sambat")},
             .abmon = {tsc("Qun"), tsc("Nah"), tsc("Cig"), tsc("Agd"), tsc("Cax"), tsc("Qas"), tsc("Qad"), tsc("Leq"),
                       tsc("Way"), tsc("Dit"), tsc("Xim"), tsc("Kax")},
             .mon = {tsc("Qunxa Garablu"), tsc("Naharsi Kudo"), tsc("Ciggilta Kudo"), tsc("Agda Baxis"),
                     tsc("Caxah Alsa"), tsc("Qasa Dirri"), tsc("Qado Dirri"), tsc("Liiqen"), tsc("Waysu"),
                     tsc("Diteli"), tsc("Ximoli"), tsc("Kaxxa Garablu")},
             .d_t_fmt = tsc("%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%l:%M:%S %p"),
             .t_fmt_ampm = tsc("%l:%M:%S %p"),
             .date_fmt = tsc("%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc("saaku"), tsc("carra")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1yY]"), .noexpr = tsc("^[-0mnMN]"), .yesstr = tsc("Yeey"), .nostr = tsc("Maleey")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"),
                  .tel_dom_fmt = tsc("%a-%l"),
                  .int_select = tsc("00"),
                  .int_prefix = tsc("291")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f"),
             .name_gen = tsc(""),
             .name_miss = tsc("Qunxa Awka"),
             .name_mr = tsc("Saqal"),
             .name_mrs = tsc("Numa"),
             .name_ms = tsc("")},
    .address = {.postal_fmt = tsc("%z%c%T%s%b%e%r"),
                .country_name = tsc("Eretria"),
                .country_post = tsc("ERI"),
                .country_ab2 = tsc("ER"),
                .country_ab3 = tsc("ERI"),
                .country_num = 232,
                .country_car = tsc("ER"),
                .lang_name = tsc("Qafar"),
                .lang_ab = tsc("aa"),
                .lang_term = tsc("aar"),
                .lang_lib = tsc("aar")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"aa_ER@saaho"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Afar language locale for Eritrea (Saaho Dialect)."),
         .source = tsc(L"Ge'ez Frontier Foundation\t\t;\t\tfast_io"),
         .address =
             tsc(L"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 L"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"fast_io"),
         .email = tsc(L"locales@geez.org;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Afar"),
         .territory = tsc(L"Eritrea"),
         .revision = tsc(L"0.20"),
         .date = tsc(L"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(L"ERN "),
                 .currency_symbol = tsc(L"Nfk"),
                 .mon_decimal_point = tsc(L"."),
                 .mon_thousands_sep = tsc(L","),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
                 .positive_sign = tsc(L""),
                 .negative_sign = tsc(L"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(L"."), .thousands_sep = tsc(L"")},
    .time = {.abday = {tsc(L"Nab"), tsc(L"San"), tsc(L"Sal"), tsc(L"Rab"), tsc(L"Cam"), tsc(L"Jum"), tsc(L"Qun")},
             .day = {tsc(L"Naba Sambat"), tsc(L"Sani"), tsc(L"Salus"), tsc(L"Rabuq"), tsc(L"Camus"), tsc(L"Jumqata"),
                     tsc(L"Qunxa Sambat")},
             .abmon = {tsc(L"Qun"), tsc(L"Nah"), tsc(L"Cig"), tsc(L"Agd"), tsc(L"Cax"), tsc(L"Qas"), tsc(L"Qad"),
                       tsc(L"Leq"), tsc(L"Way"), tsc(L"Dit"), tsc(L"Xim"), tsc(L"Kax")},
             .mon = {tsc(L"Qunxa Garablu"), tsc(L"Naharsi Kudo"), tsc(L"Ciggilta Kudo"), tsc(L"Agda Baxis"),
                     tsc(L"Caxah Alsa"), tsc(L"Qasa Dirri"), tsc(L"Qado Dirri"), tsc(L"Liiqen"), tsc(L"Waysu"),
                     tsc(L"Diteli"), tsc(L"Ximoli"), tsc(L"Kaxxa Garablu")},
             .d_t_fmt = tsc(L"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(L"%l:%M:%S %p"),
             .date_fmt = tsc(L"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(L"saaku"), tsc(L"carra")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1yY]"),
                 .noexpr = tsc(L"^[-0mnMN]"),
                 .yesstr = tsc(L"Yeey"),
                 .nostr = tsc(L"Maleey")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"),
                  .tel_dom_fmt = tsc(L"%a-%l"),
                  .int_select = tsc(L"00"),
                  .int_prefix = tsc(L"291")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f"),
             .name_gen = tsc(L""),
             .name_miss = tsc(L"Qunxa Awka"),
             .name_mr = tsc(L"Saqal"),
             .name_mrs = tsc(L"Numa"),
             .name_ms = tsc(L"")},
    .address = {.postal_fmt = tsc(L"%z%c%T%s%b%e%r"),
                .country_name = tsc(L"Eretria"),
                .country_post = tsc(L"ERI"),
                .country_ab2 = tsc(L"ER"),
                .country_ab3 = tsc(L"ERI"),
                .country_num = 232,
                .country_car = tsc(L"ER"),
                .lang_name = tsc(L"Qafar"),
                .lang_ab = tsc(L"aa"),
                .lang_term = tsc(L"aar"),
                .lang_lib = tsc(L"aar")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"aa_ER@saaho"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Afar language locale for Eritrea (Saaho Dialect)."),
         .source = tsc(u8"Ge'ez Frontier Foundation\t\t;\t\tfast_io"),
         .address =
             tsc(u8"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 u8"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"fast_io"),
         .email = tsc(u8"locales@geez.org;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Afar"),
         .territory = tsc(u8"Eritrea"),
         .revision = tsc(u8"0.20"),
         .date = tsc(u8"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(u8"ERN "),
                 .currency_symbol = tsc(u8"Nfk"),
                 .mon_decimal_point = tsc(u8"."),
                 .mon_thousands_sep = tsc(u8","),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
                 .positive_sign = tsc(u8""),
                 .negative_sign = tsc(u8"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u8"."), .thousands_sep = tsc(u8"")},
    .time = {.abday = {tsc(u8"Nab"), tsc(u8"San"), tsc(u8"Sal"), tsc(u8"Rab"), tsc(u8"Cam"), tsc(u8"Jum"),
                       tsc(u8"Qun")},
             .day = {tsc(u8"Naba Sambat"), tsc(u8"Sani"), tsc(u8"Salus"), tsc(u8"Rabuq"), tsc(u8"Camus"),
                     tsc(u8"Jumqata"), tsc(u8"Qunxa Sambat")},
             .abmon = {tsc(u8"Qun"), tsc(u8"Nah"), tsc(u8"Cig"), tsc(u8"Agd"), tsc(u8"Cax"), tsc(u8"Qas"), tsc(u8"Qad"),
                       tsc(u8"Leq"), tsc(u8"Way"), tsc(u8"Dit"), tsc(u8"Xim"), tsc(u8"Kax")},
             .mon = {tsc(u8"Qunxa Garablu"), tsc(u8"Naharsi Kudo"), tsc(u8"Ciggilta Kudo"), tsc(u8"Agda Baxis"),
                     tsc(u8"Caxah Alsa"), tsc(u8"Qasa Dirri"), tsc(u8"Qado Dirri"), tsc(u8"Liiqen"), tsc(u8"Waysu"),
                     tsc(u8"Diteli"), tsc(u8"Ximoli"), tsc(u8"Kaxxa Garablu")},
             .d_t_fmt = tsc(u8"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(u8"%l:%M:%S %p"),
             .date_fmt = tsc(u8"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(u8"saaku"), tsc(u8"carra")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1yY]"),
                 .noexpr = tsc(u8"^[-0mnMN]"),
                 .yesstr = tsc(u8"Yeey"),
                 .nostr = tsc(u8"Maleey")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"),
                  .tel_dom_fmt = tsc(u8"%a-%l"),
                  .int_select = tsc(u8"00"),
                  .int_prefix = tsc(u8"291")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u8""),
             .name_miss = tsc(u8"Qunxa Awka"),
             .name_mr = tsc(u8"Saqal"),
             .name_mrs = tsc(u8"Numa"),
             .name_ms = tsc(u8"")},
    .address = {.postal_fmt = tsc(u8"%z%c%T%s%b%e%r"),
                .country_name = tsc(u8"Eretria"),
                .country_post = tsc(u8"ERI"),
                .country_ab2 = tsc(u8"ER"),
                .country_ab3 = tsc(u8"ERI"),
                .country_num = 232,
                .country_car = tsc(u8"ER"),
                .lang_name = tsc(u8"Qafar"),
                .lang_ab = tsc(u8"aa"),
                .lang_term = tsc(u8"aar"),
                .lang_lib = tsc(u8"aar")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"aa_ER@saaho"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Afar language locale for Eritrea (Saaho Dialect)."),
         .source = tsc(u"Ge'ez Frontier Foundation\t\t;\t\tfast_io"),
         .address =
             tsc(u"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 u"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"fast_io"),
         .email = tsc(u"locales@geez.org;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Afar"),
         .territory = tsc(u"Eritrea"),
         .revision = tsc(u"0.20"),
         .date = tsc(u"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(u"ERN "),
                 .currency_symbol = tsc(u"Nfk"),
                 .mon_decimal_point = tsc(u"."),
                 .mon_thousands_sep = tsc(u","),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
                 .positive_sign = tsc(u""),
                 .negative_sign = tsc(u"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u"."), .thousands_sep = tsc(u"")},
    .time = {.abday = {tsc(u"Nab"), tsc(u"San"), tsc(u"Sal"), tsc(u"Rab"), tsc(u"Cam"), tsc(u"Jum"), tsc(u"Qun")},
             .day = {tsc(u"Naba Sambat"), tsc(u"Sani"), tsc(u"Salus"), tsc(u"Rabuq"), tsc(u"Camus"), tsc(u"Jumqata"),
                     tsc(u"Qunxa Sambat")},
             .abmon = {tsc(u"Qun"), tsc(u"Nah"), tsc(u"Cig"), tsc(u"Agd"), tsc(u"Cax"), tsc(u"Qas"), tsc(u"Qad"),
                       tsc(u"Leq"), tsc(u"Way"), tsc(u"Dit"), tsc(u"Xim"), tsc(u"Kax")},
             .mon = {tsc(u"Qunxa Garablu"), tsc(u"Naharsi Kudo"), tsc(u"Ciggilta Kudo"), tsc(u"Agda Baxis"),
                     tsc(u"Caxah Alsa"), tsc(u"Qasa Dirri"), tsc(u"Qado Dirri"), tsc(u"Liiqen"), tsc(u"Waysu"),
                     tsc(u"Diteli"), tsc(u"Ximoli"), tsc(u"Kaxxa Garablu")},
             .d_t_fmt = tsc(u"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(u"%l:%M:%S %p"),
             .date_fmt = tsc(u"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(u"saaku"), tsc(u"carra")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1yY]"),
                 .noexpr = tsc(u"^[-0mnMN]"),
                 .yesstr = tsc(u"Yeey"),
                 .nostr = tsc(u"Maleey")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"),
                  .tel_dom_fmt = tsc(u"%a-%l"),
                  .int_select = tsc(u"00"),
                  .int_prefix = tsc(u"291")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u""),
             .name_miss = tsc(u"Qunxa Awka"),
             .name_mr = tsc(u"Saqal"),
             .name_mrs = tsc(u"Numa"),
             .name_ms = tsc(u"")},
    .address = {.postal_fmt = tsc(u"%z%c%T%s%b%e%r"),
                .country_name = tsc(u"Eretria"),
                .country_post = tsc(u"ERI"),
                .country_ab2 = tsc(u"ER"),
                .country_ab3 = tsc(u"ERI"),
                .country_num = 232,
                .country_car = tsc(u"ER"),
                .lang_name = tsc(u"Qafar"),
                .lang_ab = tsc(u"aa"),
                .lang_term = tsc(u"aar"),
                .lang_lib = tsc(u"aar")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"aa_ER@saaho"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Afar language locale for Eritrea (Saaho Dialect)."),
         .source = tsc(U"Ge'ez Frontier Foundation\t\t;\t\tfast_io"),
         .address =
             tsc(U"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 U"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"fast_io"),
         .email = tsc(U"locales@geez.org;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Afar"),
         .territory = tsc(U"Eritrea"),
         .revision = tsc(U"0.20"),
         .date = tsc(U"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(U"ERN "),
                 .currency_symbol = tsc(U"Nfk"),
                 .mon_decimal_point = tsc(U"."),
                 .mon_thousands_sep = tsc(U","),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
                 .positive_sign = tsc(U""),
                 .negative_sign = tsc(U"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(U"."), .thousands_sep = tsc(U"")},
    .time = {.abday = {tsc(U"Nab"), tsc(U"San"), tsc(U"Sal"), tsc(U"Rab"), tsc(U"Cam"), tsc(U"Jum"), tsc(U"Qun")},
             .day = {tsc(U"Naba Sambat"), tsc(U"Sani"), tsc(U"Salus"), tsc(U"Rabuq"), tsc(U"Camus"), tsc(U"Jumqata"),
                     tsc(U"Qunxa Sambat")},
             .abmon = {tsc(U"Qun"), tsc(U"Nah"), tsc(U"Cig"), tsc(U"Agd"), tsc(U"Cax"), tsc(U"Qas"), tsc(U"Qad"),
                       tsc(U"Leq"), tsc(U"Way"), tsc(U"Dit"), tsc(U"Xim"), tsc(U"Kax")},
             .mon = {tsc(U"Qunxa Garablu"), tsc(U"Naharsi Kudo"), tsc(U"Ciggilta Kudo"), tsc(U"Agda Baxis"),
                     tsc(U"Caxah Alsa"), tsc(U"Qasa Dirri"), tsc(U"Qado Dirri"), tsc(U"Liiqen"), tsc(U"Waysu"),
                     tsc(U"Diteli"), tsc(U"Ximoli"), tsc(U"Kaxxa Garablu")},
             .d_t_fmt = tsc(U"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(U"%l:%M:%S %p"),
             .date_fmt = tsc(U"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(U"saaku"), tsc(U"carra")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1yY]"),
                 .noexpr = tsc(U"^[-0mnMN]"),
                 .yesstr = tsc(U"Yeey"),
                 .nostr = tsc(U"Maleey")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"),
                  .tel_dom_fmt = tsc(U"%a-%l"),
                  .int_select = tsc(U"00"),
                  .int_prefix = tsc(U"291")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f"),
             .name_gen = tsc(U""),
             .name_miss = tsc(U"Qunxa Awka"),
             .name_mr = tsc(U"Saqal"),
             .name_mrs = tsc(U"Numa"),
             .name_ms = tsc(U"")},
    .address = {.postal_fmt = tsc(U"%z%c%T%s%b%e%r"),
                .country_name = tsc(U"Eretria"),
                .country_post = tsc(U"ERI"),
                .country_ab2 = tsc(U"ER"),
                .country_ab3 = tsc(U"ERI"),
                .country_num = 232,
                .country_car = tsc(U"ER"),
                .lang_name = tsc(U"Qafar"),
                .lang_ab = tsc(U"aa"),
                .lang_term = tsc(U"aar"),
                .lang_lib = tsc(U"aar")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"