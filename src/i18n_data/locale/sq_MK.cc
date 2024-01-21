﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("sq_MK"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Albanian language locale for Macedonia"),
         .source = tsc("Albanian Language Team\t\t;\t\tfast_io"),
         .address =
             tsc("465 4th St. 2nd Floor, Palisades Park, NJ 07650, "
                 "U.S.A.\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("fast_io"),
         .email = tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Albanian"),
         .territory = tsc("Macedonia"),
         .revision = tsc("0.01"),
         .date = tsc("2010-04-06")},
    .monetary = {.int_curr_symbol = tsc("MKD "),
                 .currency_symbol = tsc("den"),
                 .mon_decimal_point = tsc(","),
                 .mon_thousands_sep = tsc(" "),
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
    .time = {.abday = {tsc("Die"), tsc("Hën"), tsc("Mar"), tsc("Mër"), tsc("Enj"), tsc("Pre"), tsc("Sht")},
             .day = {tsc("e diel"), tsc("e hënë"), tsc("e martë"), tsc("e mërkurë"), tsc("e enjte"), tsc("e premte"),
                     tsc("e shtunë")},
             .abmon = {tsc("Jan"), tsc("Shk"), tsc("Mar"), tsc("Pri"), tsc("Maj"), tsc("Qer"), tsc("Kor"), tsc("Gsh"),
                       tsc("Sht"), tsc("Tet"), tsc("Nën"), tsc("Dhj")},
             .mon = {tsc("janar"), tsc("shkurt"), tsc("mars"), tsc("prill"), tsc("maj"), tsc("qershor"), tsc("korrik"),
                     tsc("gusht"), tsc("shtator"), tsc("tetor"), tsc("nëntor"), tsc("dhjetor")},
             .d_t_fmt = tsc("%a %-d %b %Y %I:%M:%S.%p"),
             .d_fmt = tsc("%-d.%-m.%y"),
             .t_fmt = tsc("%I:%M:%S.%p %Z"),
             .t_fmt_ampm = tsc("%I:%M:%S.%p %Z"),
             .date_fmt = tsc("%a %-d %b %Y %I:%M:%S.%p %Z"),
             .am_pm = {tsc("PD"), tsc("MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1yYpP]"), .noexpr = tsc("^[-0nNjJ]"), .yesstr = tsc("po"), .nostr = tsc("jo")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"),
                  .tel_dom_fmt = tsc("%A %l"),
                  .int_select = tsc("00"),
                  .int_prefix = tsc("389")},
    .name = {.name_fmt = tsc("%p%t%f%t%g"),
             .name_gen = tsc(""),
             .name_miss = tsc("Zsh."),
             .name_mr = tsc("Z."),
             .name_mrs = tsc("Znj."),
             .name_ms = tsc("Znj.")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Maqedoni"),
                .country_post = tsc("MK"),
                .country_ab2 = tsc("MK"),
                .country_ab3 = tsc("MKD"),
                .country_num = 807,
                .country_car = tsc("MK"),
                .country_isbn = tsc("9989"),
                .lang_name = tsc("shqip"),
                .lang_ab = tsc("sq"),
                .lang_term = tsc("sqi"),
                .lang_lib = tsc("alb")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"sq_MK"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Albanian language locale for Macedonia"),
         .source = tsc(L"Albanian Language Team\t\t;\t\tfast_io"),
         .address =
             tsc(L"465 4th St. 2nd Floor, Palisades Park, NJ 07650, "
                 L"U.S.A.\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"fast_io"),
         .email = tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Albanian"),
         .territory = tsc(L"Macedonia"),
         .revision = tsc(L"0.01"),
         .date = tsc(L"2010-04-06")},
    .monetary = {.int_curr_symbol = tsc(L"MKD "),
                 .currency_symbol = tsc(L"den"),
                 .mon_decimal_point = tsc(L","),
                 .mon_thousands_sep = tsc(L" "),
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
    .time = {.abday = {tsc(L"Die"), tsc(L"Hën"), tsc(L"Mar"), tsc(L"Mër"), tsc(L"Enj"), tsc(L"Pre"), tsc(L"Sht")},
             .day = {tsc(L"e diel"), tsc(L"e hënë"), tsc(L"e martë"), tsc(L"e mërkurë"), tsc(L"e enjte"),
                     tsc(L"e premte"), tsc(L"e shtunë")},
             .abmon = {tsc(L"Jan"), tsc(L"Shk"), tsc(L"Mar"), tsc(L"Pri"), tsc(L"Maj"), tsc(L"Qer"), tsc(L"Kor"),
                       tsc(L"Gsh"), tsc(L"Sht"), tsc(L"Tet"), tsc(L"Nën"), tsc(L"Dhj")},
             .mon = {tsc(L"janar"), tsc(L"shkurt"), tsc(L"mars"), tsc(L"prill"), tsc(L"maj"), tsc(L"qershor"),
                     tsc(L"korrik"), tsc(L"gusht"), tsc(L"shtator"), tsc(L"tetor"), tsc(L"nëntor"), tsc(L"dhjetor")},
             .d_t_fmt = tsc(L"%a %-d %b %Y %I:%M:%S.%p"),
             .d_fmt = tsc(L"%-d.%-m.%y"),
             .t_fmt = tsc(L"%I:%M:%S.%p %Z"),
             .t_fmt_ampm = tsc(L"%I:%M:%S.%p %Z"),
             .date_fmt = tsc(L"%a %-d %b %Y %I:%M:%S.%p %Z"),
             .am_pm = {tsc(L"PD"), tsc(L"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1yYpP]"), .noexpr = tsc(L"^[-0nNjJ]"), .yesstr = tsc(L"po"), .nostr = tsc(L"jo")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"),
                  .tel_dom_fmt = tsc(L"%A %l"),
                  .int_select = tsc(L"00"),
                  .int_prefix = tsc(L"389")},
    .name = {.name_fmt = tsc(L"%p%t%f%t%g"),
             .name_gen = tsc(L""),
             .name_miss = tsc(L"Zsh."),
             .name_mr = tsc(L"Z."),
             .name_mrs = tsc(L"Znj."),
             .name_ms = tsc(L"Znj.")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Maqedoni"),
                .country_post = tsc(L"MK"),
                .country_ab2 = tsc(L"MK"),
                .country_ab3 = tsc(L"MKD"),
                .country_num = 807,
                .country_car = tsc(L"MK"),
                .country_isbn = tsc(L"9989"),
                .lang_name = tsc(L"shqip"),
                .lang_ab = tsc(L"sq"),
                .lang_term = tsc(L"sqi"),
                .lang_lib = tsc(L"alb")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"sq_MK"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Albanian language locale for Macedonia"),
         .source = tsc(u8"Albanian Language Team\t\t;\t\tfast_io"),
         .address = tsc(
             u8"465 4th St. 2nd Floor, Palisades Park, NJ 07650, "
             u8"U.S.A.\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"fast_io"),
         .email = tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Albanian"),
         .territory = tsc(u8"Macedonia"),
         .revision = tsc(u8"0.01"),
         .date = tsc(u8"2010-04-06")},
    .monetary = {.int_curr_symbol = tsc(u8"MKD "),
                 .currency_symbol = tsc(u8"den"),
                 .mon_decimal_point = tsc(u8","),
                 .mon_thousands_sep = tsc(u8" "),
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
    .time = {.abday = {tsc(u8"Die"), tsc(u8"Hën"), tsc(u8"Mar"), tsc(u8"Mër"), tsc(u8"Enj"), tsc(u8"Pre"),
                       tsc(u8"Sht")},
             .day = {tsc(u8"e diel"), tsc(u8"e hënë"), tsc(u8"e martë"), tsc(u8"e mërkurë"), tsc(u8"e enjte"),
                     tsc(u8"e premte"), tsc(u8"e shtunë")},
             .abmon = {tsc(u8"Jan"), tsc(u8"Shk"), tsc(u8"Mar"), tsc(u8"Pri"), tsc(u8"Maj"), tsc(u8"Qer"), tsc(u8"Kor"),
                       tsc(u8"Gsh"), tsc(u8"Sht"), tsc(u8"Tet"), tsc(u8"Nën"), tsc(u8"Dhj")},
             .mon = {tsc(u8"janar"), tsc(u8"shkurt"), tsc(u8"mars"), tsc(u8"prill"), tsc(u8"maj"), tsc(u8"qershor"),
                     tsc(u8"korrik"), tsc(u8"gusht"), tsc(u8"shtator"), tsc(u8"tetor"), tsc(u8"nëntor"),
                     tsc(u8"dhjetor")},
             .d_t_fmt = tsc(u8"%a %-d %b %Y %I:%M:%S.%p"),
             .d_fmt = tsc(u8"%-d.%-m.%y"),
             .t_fmt = tsc(u8"%I:%M:%S.%p %Z"),
             .t_fmt_ampm = tsc(u8"%I:%M:%S.%p %Z"),
             .date_fmt = tsc(u8"%a %-d %b %Y %I:%M:%S.%p %Z"),
             .am_pm = {tsc(u8"PD"), tsc(u8"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1yYpP]"),
                 .noexpr = tsc(u8"^[-0nNjJ]"),
                 .yesstr = tsc(u8"po"),
                 .nostr = tsc(u8"jo")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"),
                  .tel_dom_fmt = tsc(u8"%A %l"),
                  .int_select = tsc(u8"00"),
                  .int_prefix = tsc(u8"389")},
    .name = {.name_fmt = tsc(u8"%p%t%f%t%g"),
             .name_gen = tsc(u8""),
             .name_miss = tsc(u8"Zsh."),
             .name_mr = tsc(u8"Z."),
             .name_mrs = tsc(u8"Znj."),
             .name_ms = tsc(u8"Znj.")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Maqedoni"),
                .country_post = tsc(u8"MK"),
                .country_ab2 = tsc(u8"MK"),
                .country_ab3 = tsc(u8"MKD"),
                .country_num = 807,
                .country_car = tsc(u8"MK"),
                .country_isbn = tsc(u8"9989"),
                .lang_name = tsc(u8"shqip"),
                .lang_ab = tsc(u8"sq"),
                .lang_term = tsc(u8"sqi"),
                .lang_lib = tsc(u8"alb")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"sq_MK"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Albanian language locale for Macedonia"),
         .source = tsc(u"Albanian Language Team\t\t;\t\tfast_io"),
         .address =
             tsc(u"465 4th St. 2nd Floor, Palisades Park, NJ 07650, "
                 u"U.S.A.\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"fast_io"),
         .email = tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Albanian"),
         .territory = tsc(u"Macedonia"),
         .revision = tsc(u"0.01"),
         .date = tsc(u"2010-04-06")},
    .monetary = {.int_curr_symbol = tsc(u"MKD "),
                 .currency_symbol = tsc(u"den"),
                 .mon_decimal_point = tsc(u","),
                 .mon_thousands_sep = tsc(u" "),
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
    .time = {.abday = {tsc(u"Die"), tsc(u"Hën"), tsc(u"Mar"), tsc(u"Mër"), tsc(u"Enj"), tsc(u"Pre"), tsc(u"Sht")},
             .day = {tsc(u"e diel"), tsc(u"e hënë"), tsc(u"e martë"), tsc(u"e mërkurë"), tsc(u"e enjte"),
                     tsc(u"e premte"), tsc(u"e shtunë")},
             .abmon = {tsc(u"Jan"), tsc(u"Shk"), tsc(u"Mar"), tsc(u"Pri"), tsc(u"Maj"), tsc(u"Qer"), tsc(u"Kor"),
                       tsc(u"Gsh"), tsc(u"Sht"), tsc(u"Tet"), tsc(u"Nën"), tsc(u"Dhj")},
             .mon = {tsc(u"janar"), tsc(u"shkurt"), tsc(u"mars"), tsc(u"prill"), tsc(u"maj"), tsc(u"qershor"),
                     tsc(u"korrik"), tsc(u"gusht"), tsc(u"shtator"), tsc(u"tetor"), tsc(u"nëntor"), tsc(u"dhjetor")},
             .d_t_fmt = tsc(u"%a %-d %b %Y %I:%M:%S.%p"),
             .d_fmt = tsc(u"%-d.%-m.%y"),
             .t_fmt = tsc(u"%I:%M:%S.%p %Z"),
             .t_fmt_ampm = tsc(u"%I:%M:%S.%p %Z"),
             .date_fmt = tsc(u"%a %-d %b %Y %I:%M:%S.%p %Z"),
             .am_pm = {tsc(u"PD"), tsc(u"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1yYpP]"), .noexpr = tsc(u"^[-0nNjJ]"), .yesstr = tsc(u"po"), .nostr = tsc(u"jo")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"),
                  .tel_dom_fmt = tsc(u"%A %l"),
                  .int_select = tsc(u"00"),
                  .int_prefix = tsc(u"389")},
    .name = {.name_fmt = tsc(u"%p%t%f%t%g"),
             .name_gen = tsc(u""),
             .name_miss = tsc(u"Zsh."),
             .name_mr = tsc(u"Z."),
             .name_mrs = tsc(u"Znj."),
             .name_ms = tsc(u"Znj.")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Maqedoni"),
                .country_post = tsc(u"MK"),
                .country_ab2 = tsc(u"MK"),
                .country_ab3 = tsc(u"MKD"),
                .country_num = 807,
                .country_car = tsc(u"MK"),
                .country_isbn = tsc(u"9989"),
                .lang_name = tsc(u"shqip"),
                .lang_ab = tsc(u"sq"),
                .lang_term = tsc(u"sqi"),
                .lang_lib = tsc(u"alb")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"sq_MK"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Albanian language locale for Macedonia"),
         .source = tsc(U"Albanian Language Team\t\t;\t\tfast_io"),
         .address =
             tsc(U"465 4th St. 2nd Floor, Palisades Park, NJ 07650, "
                 U"U.S.A.\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"fast_io"),
         .email = tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Albanian"),
         .territory = tsc(U"Macedonia"),
         .revision = tsc(U"0.01"),
         .date = tsc(U"2010-04-06")},
    .monetary = {.int_curr_symbol = tsc(U"MKD "),
                 .currency_symbol = tsc(U"den"),
                 .mon_decimal_point = tsc(U","),
                 .mon_thousands_sep = tsc(U" "),
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
    .time = {.abday = {tsc(U"Die"), tsc(U"Hën"), tsc(U"Mar"), tsc(U"Mër"), tsc(U"Enj"), tsc(U"Pre"), tsc(U"Sht")},
             .day = {tsc(U"e diel"), tsc(U"e hënë"), tsc(U"e martë"), tsc(U"e mërkurë"), tsc(U"e enjte"),
                     tsc(U"e premte"), tsc(U"e shtunë")},
             .abmon = {tsc(U"Jan"), tsc(U"Shk"), tsc(U"Mar"), tsc(U"Pri"), tsc(U"Maj"), tsc(U"Qer"), tsc(U"Kor"),
                       tsc(U"Gsh"), tsc(U"Sht"), tsc(U"Tet"), tsc(U"Nën"), tsc(U"Dhj")},
             .mon = {tsc(U"janar"), tsc(U"shkurt"), tsc(U"mars"), tsc(U"prill"), tsc(U"maj"), tsc(U"qershor"),
                     tsc(U"korrik"), tsc(U"gusht"), tsc(U"shtator"), tsc(U"tetor"), tsc(U"nëntor"), tsc(U"dhjetor")},
             .d_t_fmt = tsc(U"%a %-d %b %Y %I:%M:%S.%p"),
             .d_fmt = tsc(U"%-d.%-m.%y"),
             .t_fmt = tsc(U"%I:%M:%S.%p %Z"),
             .t_fmt_ampm = tsc(U"%I:%M:%S.%p %Z"),
             .date_fmt = tsc(U"%a %-d %b %Y %I:%M:%S.%p %Z"),
             .am_pm = {tsc(U"PD"), tsc(U"MD")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1yYpP]"), .noexpr = tsc(U"^[-0nNjJ]"), .yesstr = tsc(U"po"), .nostr = tsc(U"jo")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"),
                  .tel_dom_fmt = tsc(U"%A %l"),
                  .int_select = tsc(U"00"),
                  .int_prefix = tsc(U"389")},
    .name = {.name_fmt = tsc(U"%p%t%f%t%g"),
             .name_gen = tsc(U""),
             .name_miss = tsc(U"Zsh."),
             .name_mr = tsc(U"Z."),
             .name_mrs = tsc(U"Znj."),
             .name_ms = tsc(U"Znj.")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Maqedoni"),
                .country_post = tsc(U"MK"),
                .country_ab2 = tsc(U"MK"),
                .country_ab3 = tsc(U"MKD"),
                .country_num = 807,
                .country_car = tsc(U"MK"),
                .country_isbn = tsc(U"9989"),
                .lang_name = tsc(U"shqip"),
                .lang_ab = tsc(U"sq"),
                .lang_term = tsc(U"sqi"),
                .lang_lib = tsc(U"alb")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"