﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t monetary_mon_grouping_storage[]{3};

inline constexpr std::size_t numeric_grouping_storage[]{2, 2, 2, 3};

inline constexpr lc_all lc_all_global{
    .identification = {.name = tsc("unm_US"),
                       .encoding = tsc(FAST_IO_LOCALE_ENCODING),
                       .title = tsc("Unami Delaware locale for the USA"),
                       .source = tsc("fast_io"),
                       .address =
                           tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc("fast_io"),
                       .email = tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(""),
                       .fax = tsc(""),
                       .language = tsc("Unami Delaware"),
                       .territory = tsc("United States"),
                       .revision = tsc("1.0"),
                       .date = tsc("2011-10-27")},
    .monetary = {.int_curr_symbol = tsc("USD "),
                 .currency_symbol = tsc("$"),
                 .mon_decimal_point = tsc("."),
                 .mon_thousands_sep = tsc(" "),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
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
    .numeric = {.decimal_point = tsc("."), .thousands_sep = tsc(" "), .grouping = {numeric_grouping_storage, 4}},
    .time = {.abday = {tsc("ken"), tsc("man"), tsc("tus"), tsc("lel"), tsc("tas"), tsc("pel"), tsc("sat")},
             .day = {tsc("kentuwei"), tsc("manteke"), tsc("tusteke"), tsc("lelai"), tsc("tasteke"), tsc("pelaiteke"),
                     tsc("sateteke")},
             .abmon = {tsc("eni"), tsc("chk"), tsc("xam"), tsc("kwe"), tsc("tai"), tsc("nip"), tsc("lai"), tsc("win"),
                       tsc("tah"), tsc("puk"), tsc("kun"), tsc("mux")},
             .mon = {tsc("enikwsi"), tsc("chkwali"), tsc("xamokhwite"), tsc("kwetayoxe"), tsc("tainipen"),
                     tsc("kichinipen"), tsc("lainipen"), tsc("winaminke"), tsc("kichitahkok"), tsc("puksit"),
                     tsc("wini"), tsc("muxkotae")},
             .d_t_fmt = tsc("%a %d %b %Y %T"),
             .d_fmt = tsc("%d//%m//%y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%a %d %b %Y %T %Z"),
             .am_pm = {tsc(""), tsc("")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc("^[+1yY]"), .noexpr = tsc("^[-0kKmM]")},
    .paper = {.width = 216, .height = 279},
    .telephone = {.tel_int_fmt = tsc("+%c (%a) %l"),
                  .tel_dom_fmt = tsc("(%a) %l"),
                  .int_select = tsc("11"),
                  .int_prefix = tsc("1")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%a%N%f%N%d%N%b%N%h %s %e %r%N%T, %S %z%N%c%N"),
                .country_name = tsc("USA"),
                .country_post = tsc("USA"),
                .country_ab2 = tsc("US"),
                .country_ab3 = tsc("USA"),
                .country_num = 840,
                .country_car = tsc("USA"),
                .country_isbn = tsc("0"),
                .lang_name = tsc("Lenape"),
                .lang_term = tsc("unm"),
                .lang_lib = tsc("unm")},
    .measurement = {.measurement = 2}};

inline constexpr wlc_all wlc_all_global{
    .identification = {.name = tsc(L"unm_US"),
                       .encoding = tsc(FAST_IO_LOCALE_LENCODING),
                       .title = tsc(L"Unami Delaware locale for the USA"),
                       .source = tsc(L"fast_io"),
                       .address =
                           tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(L"fast_io"),
                       .email = tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(L""),
                       .fax = tsc(L""),
                       .language = tsc(L"Unami Delaware"),
                       .territory = tsc(L"United States"),
                       .revision = tsc(L"1.0"),
                       .date = tsc(L"2011-10-27")},
    .monetary = {.int_curr_symbol = tsc(L"USD "),
                 .currency_symbol = tsc(L"$"),
                 .mon_decimal_point = tsc(L"."),
                 .mon_thousands_sep = tsc(L" "),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
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
    .numeric = {.decimal_point = tsc(L"."), .thousands_sep = tsc(L" "), .grouping = {numeric_grouping_storage, 4}},
    .time = {.abday = {tsc(L"ken"), tsc(L"man"), tsc(L"tus"), tsc(L"lel"), tsc(L"tas"), tsc(L"pel"), tsc(L"sat")},
             .day = {tsc(L"kentuwei"), tsc(L"manteke"), tsc(L"tusteke"), tsc(L"lelai"), tsc(L"tasteke"),
                     tsc(L"pelaiteke"), tsc(L"sateteke")},
             .abmon = {tsc(L"eni"), tsc(L"chk"), tsc(L"xam"), tsc(L"kwe"), tsc(L"tai"), tsc(L"nip"), tsc(L"lai"),
                       tsc(L"win"), tsc(L"tah"), tsc(L"puk"), tsc(L"kun"), tsc(L"mux")},
             .mon = {tsc(L"enikwsi"), tsc(L"chkwali"), tsc(L"xamokhwite"), tsc(L"kwetayoxe"), tsc(L"tainipen"),
                     tsc(L"kichinipen"), tsc(L"lainipen"), tsc(L"winaminke"), tsc(L"kichitahkok"), tsc(L"puksit"),
                     tsc(L"wini"), tsc(L"muxkotae")},
             .d_t_fmt = tsc(L"%a %d %b %Y %T"),
             .d_fmt = tsc(L"%d//%m//%y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(L""), tsc(L"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(L"^[+1yY]"), .noexpr = tsc(L"^[-0kKmM]")},
    .paper = {.width = 216, .height = 279},
    .telephone = {.tel_int_fmt = tsc(L"+%c (%a) %l"),
                  .tel_dom_fmt = tsc(L"(%a) %l"),
                  .int_select = tsc(L"11"),
                  .int_prefix = tsc(L"1")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%a%N%f%N%d%N%b%N%h %s %e %r%N%T, %S %z%N%c%N"),
                .country_name = tsc(L"USA"),
                .country_post = tsc(L"USA"),
                .country_ab2 = tsc(L"US"),
                .country_ab3 = tsc(L"USA"),
                .country_num = 840,
                .country_car = tsc(L"USA"),
                .country_isbn = tsc(L"0"),
                .lang_name = tsc(L"Lenape"),
                .lang_term = tsc(L"unm"),
                .lang_lib = tsc(L"unm")},
    .measurement = {.measurement = 2}};

inline constexpr u8lc_all u8lc_all_global{
    .identification = {.name = tsc(u8"unm_US"),
                       .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
                       .title = tsc(u8"Unami Delaware locale for the USA"),
                       .source = tsc(u8"fast_io"),
                       .address =
                           tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u8"fast_io"),
                       .email = tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(u8""),
                       .fax = tsc(u8""),
                       .language = tsc(u8"Unami Delaware"),
                       .territory = tsc(u8"United States"),
                       .revision = tsc(u8"1.0"),
                       .date = tsc(u8"2011-10-27")},
    .monetary = {.int_curr_symbol = tsc(u8"USD "),
                 .currency_symbol = tsc(u8"$"),
                 .mon_decimal_point = tsc(u8"."),
                 .mon_thousands_sep = tsc(u8" "),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
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
    .numeric = {.decimal_point = tsc(u8"."), .thousands_sep = tsc(u8" "), .grouping = {numeric_grouping_storage, 4}},
    .time = {.abday = {tsc(u8"ken"), tsc(u8"man"), tsc(u8"tus"), tsc(u8"lel"), tsc(u8"tas"), tsc(u8"pel"),
                       tsc(u8"sat")},
             .day = {tsc(u8"kentuwei"), tsc(u8"manteke"), tsc(u8"tusteke"), tsc(u8"lelai"), tsc(u8"tasteke"),
                     tsc(u8"pelaiteke"), tsc(u8"sateteke")},
             .abmon = {tsc(u8"eni"), tsc(u8"chk"), tsc(u8"xam"), tsc(u8"kwe"), tsc(u8"tai"), tsc(u8"nip"), tsc(u8"lai"),
                       tsc(u8"win"), tsc(u8"tah"), tsc(u8"puk"), tsc(u8"kun"), tsc(u8"mux")},
             .mon = {tsc(u8"enikwsi"), tsc(u8"chkwali"), tsc(u8"xamokhwite"), tsc(u8"kwetayoxe"), tsc(u8"tainipen"),
                     tsc(u8"kichinipen"), tsc(u8"lainipen"), tsc(u8"winaminke"), tsc(u8"kichitahkok"), tsc(u8"puksit"),
                     tsc(u8"wini"), tsc(u8"muxkotae")},
             .d_t_fmt = tsc(u8"%a %d %b %Y %T"),
             .d_fmt = tsc(u8"%d//%m//%y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u8"^[+1yY]"), .noexpr = tsc(u8"^[-0kKmM]")},
    .paper = {.width = 216, .height = 279},
    .telephone = {.tel_int_fmt = tsc(u8"+%c (%a) %l"),
                  .tel_dom_fmt = tsc(u8"(%a) %l"),
                  .int_select = tsc(u8"11"),
                  .int_prefix = tsc(u8"1")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%a%N%f%N%d%N%b%N%h %s %e %r%N%T, %S %z%N%c%N"),
                .country_name = tsc(u8"USA"),
                .country_post = tsc(u8"USA"),
                .country_ab2 = tsc(u8"US"),
                .country_ab3 = tsc(u8"USA"),
                .country_num = 840,
                .country_car = tsc(u8"USA"),
                .country_isbn = tsc(u8"0"),
                .lang_name = tsc(u8"Lenape"),
                .lang_term = tsc(u8"unm"),
                .lang_lib = tsc(u8"unm")},
    .measurement = {.measurement = 2}};

inline constexpr u16lc_all u16lc_all_global{
    .identification = {.name = tsc(u"unm_US"),
                       .encoding = tsc(FAST_IO_LOCALE_uENCODING),
                       .title = tsc(u"Unami Delaware locale for the USA"),
                       .source = tsc(u"fast_io"),
                       .address =
                           tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u"fast_io"),
                       .email = tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(u""),
                       .fax = tsc(u""),
                       .language = tsc(u"Unami Delaware"),
                       .territory = tsc(u"United States"),
                       .revision = tsc(u"1.0"),
                       .date = tsc(u"2011-10-27")},
    .monetary = {.int_curr_symbol = tsc(u"USD "),
                 .currency_symbol = tsc(u"$"),
                 .mon_decimal_point = tsc(u"."),
                 .mon_thousands_sep = tsc(u" "),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
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
    .numeric = {.decimal_point = tsc(u"."), .thousands_sep = tsc(u" "), .grouping = {numeric_grouping_storage, 4}},
    .time = {.abday = {tsc(u"ken"), tsc(u"man"), tsc(u"tus"), tsc(u"lel"), tsc(u"tas"), tsc(u"pel"), tsc(u"sat")},
             .day = {tsc(u"kentuwei"), tsc(u"manteke"), tsc(u"tusteke"), tsc(u"lelai"), tsc(u"tasteke"),
                     tsc(u"pelaiteke"), tsc(u"sateteke")},
             .abmon = {tsc(u"eni"), tsc(u"chk"), tsc(u"xam"), tsc(u"kwe"), tsc(u"tai"), tsc(u"nip"), tsc(u"lai"),
                       tsc(u"win"), tsc(u"tah"), tsc(u"puk"), tsc(u"kun"), tsc(u"mux")},
             .mon = {tsc(u"enikwsi"), tsc(u"chkwali"), tsc(u"xamokhwite"), tsc(u"kwetayoxe"), tsc(u"tainipen"),
                     tsc(u"kichinipen"), tsc(u"lainipen"), tsc(u"winaminke"), tsc(u"kichitahkok"), tsc(u"puksit"),
                     tsc(u"wini"), tsc(u"muxkotae")},
             .d_t_fmt = tsc(u"%a %d %b %Y %T"),
             .d_fmt = tsc(u"%d//%m//%y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u""), tsc(u"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u"^[+1yY]"), .noexpr = tsc(u"^[-0kKmM]")},
    .paper = {.width = 216, .height = 279},
    .telephone = {.tel_int_fmt = tsc(u"+%c (%a) %l"),
                  .tel_dom_fmt = tsc(u"(%a) %l"),
                  .int_select = tsc(u"11"),
                  .int_prefix = tsc(u"1")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%a%N%f%N%d%N%b%N%h %s %e %r%N%T, %S %z%N%c%N"),
                .country_name = tsc(u"USA"),
                .country_post = tsc(u"USA"),
                .country_ab2 = tsc(u"US"),
                .country_ab3 = tsc(u"USA"),
                .country_num = 840,
                .country_car = tsc(u"USA"),
                .country_isbn = tsc(u"0"),
                .lang_name = tsc(u"Lenape"),
                .lang_term = tsc(u"unm"),
                .lang_lib = tsc(u"unm")},
    .measurement = {.measurement = 2}};

inline constexpr u32lc_all u32lc_all_global{
    .identification = {.name = tsc(U"unm_US"),
                       .encoding = tsc(FAST_IO_LOCALE_UENCODING),
                       .title = tsc(U"Unami Delaware locale for the USA"),
                       .source = tsc(U"fast_io"),
                       .address =
                           tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(U"fast_io"),
                       .email = tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(U""),
                       .fax = tsc(U""),
                       .language = tsc(U"Unami Delaware"),
                       .territory = tsc(U"United States"),
                       .revision = tsc(U"1.0"),
                       .date = tsc(U"2011-10-27")},
    .monetary = {.int_curr_symbol = tsc(U"USD "),
                 .currency_symbol = tsc(U"$"),
                 .mon_decimal_point = tsc(U"."),
                 .mon_thousands_sep = tsc(U" "),
                 .mon_grouping = {monetary_mon_grouping_storage, 1},
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
    .numeric = {.decimal_point = tsc(U"."), .thousands_sep = tsc(U" "), .grouping = {numeric_grouping_storage, 4}},
    .time = {.abday = {tsc(U"ken"), tsc(U"man"), tsc(U"tus"), tsc(U"lel"), tsc(U"tas"), tsc(U"pel"), tsc(U"sat")},
             .day = {tsc(U"kentuwei"), tsc(U"manteke"), tsc(U"tusteke"), tsc(U"lelai"), tsc(U"tasteke"),
                     tsc(U"pelaiteke"), tsc(U"sateteke")},
             .abmon = {tsc(U"eni"), tsc(U"chk"), tsc(U"xam"), tsc(U"kwe"), tsc(U"tai"), tsc(U"nip"), tsc(U"lai"),
                       tsc(U"win"), tsc(U"tah"), tsc(U"puk"), tsc(U"kun"), tsc(U"mux")},
             .mon = {tsc(U"enikwsi"), tsc(U"chkwali"), tsc(U"xamokhwite"), tsc(U"kwetayoxe"), tsc(U"tainipen"),
                     tsc(U"kichinipen"), tsc(U"lainipen"), tsc(U"winaminke"), tsc(U"kichitahkok"), tsc(U"puksit"),
                     tsc(U"wini"), tsc(U"muxkotae")},
             .d_t_fmt = tsc(U"%a %d %b %Y %T"),
             .d_fmt = tsc(U"%d//%m//%y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(U""), tsc(U"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(U"^[+1yY]"), .noexpr = tsc(U"^[-0kKmM]")},
    .paper = {.width = 216, .height = 279},
    .telephone = {.tel_int_fmt = tsc(U"+%c (%a) %l"),
                  .tel_dom_fmt = tsc(U"(%a) %l"),
                  .int_select = tsc(U"11"),
                  .int_prefix = tsc(U"1")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%a%N%f%N%d%N%b%N%h %s %e %r%N%T, %S %z%N%c%N"),
                .country_name = tsc(U"USA"),
                .country_post = tsc(U"USA"),
                .country_ab2 = tsc(U"US"),
                .country_ab3 = tsc(U"USA"),
                .country_num = 840,
                .country_car = tsc(U"USA"),
                .country_isbn = tsc(U"0"),
                .lang_name = tsc(U"Lenape"),
                .lang_term = tsc(U"unm"),
                .lang_lib = tsc(U"unm")},
    .measurement = {.measurement = 2}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"