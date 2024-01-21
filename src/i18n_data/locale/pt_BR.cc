﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("pt_BR"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Portuguese locale for Brasil"),
         .source = tsc("RAP\t\t;\t\tfast_io"),
         .address =
             tsc("Sankt Jørgens Alle 8, DK-1615 København V, "
                 "Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("fast_io"),
         .email = tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Brazilian Portuguese"),
         .territory = tsc("Brazil"),
         .revision = tsc("1.0"),
         .date = tsc("2000-06-29")},
    .monetary = {.int_curr_symbol = tsc("BRL "),
                 .currency_symbol = tsc("R$"),
                 .mon_decimal_point = tsc(","),
                 .mon_thousands_sep = tsc("."),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(""),
                 .negative_sign = tsc("-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(","), .thousands_sep = tsc("."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc("dom"), tsc("seg"), tsc("ter"), tsc("qua"), tsc("qui"), tsc("sex"), tsc("sáb")},
             .day = {tsc("domingo"), tsc("segunda"), tsc("terça"), tsc("quarta"), tsc("quinta"), tsc("sexta"),
                     tsc("sábado")},
             .abmon = {tsc("jan"), tsc("fev"), tsc("mar"), tsc("abr"), tsc("mai"), tsc("jun"), tsc("jul"), tsc("ago"),
                       tsc("set"), tsc("out"), tsc("nov"), tsc("dez")},
             .mon = {tsc("janeiro"), tsc("fevereiro"), tsc("março"), tsc("abril"), tsc("maio"), tsc("junho"),
                     tsc("julho"), tsc("agosto"), tsc("setembro"), tsc("outubro"), tsc("novembro"), tsc("dezembro")},
             .d_t_fmt = tsc("%a %d %b %Y %T"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%a %d %b %Y %T %Z"),
             .am_pm = {tsc(""), tsc("")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc("^[+1SsyY]"), .noexpr = tsc("^[-0nN]"), .yesstr = tsc("sim"), .nostr = tsc("não")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"), .int_select = tsc("00"), .int_prefix = tsc("55")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Brasil"),
                .country_ab2 = tsc("BR"),
                .country_ab3 = tsc("BRA"),
                .country_num = 76,
                .country_car = tsc("BR"),
                .lang_name = tsc("português"),
                .lang_ab = tsc("pt"),
                .lang_term = tsc("por"),
                .lang_lib = tsc("por")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"pt_BR"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Portuguese locale for Brasil"),
         .source = tsc(L"RAP\t\t;\t\tfast_io"),
         .address = tsc(
             L"Sankt Jørgens Alle 8, DK-1615 København V, "
             L"Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"fast_io"),
         .email = tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Brazilian Portuguese"),
         .territory = tsc(L"Brazil"),
         .revision = tsc(L"1.0"),
         .date = tsc(L"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(L"BRL "),
                 .currency_symbol = tsc(L"R$"),
                 .mon_decimal_point = tsc(L","),
                 .mon_thousands_sep = tsc(L"."),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(L""),
                 .negative_sign = tsc(L"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(L","), .thousands_sep = tsc(L"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(L"dom"), tsc(L"seg"), tsc(L"ter"), tsc(L"qua"), tsc(L"qui"), tsc(L"sex"), tsc(L"sáb")},
             .day = {tsc(L"domingo"), tsc(L"segunda"), tsc(L"terça"), tsc(L"quarta"), tsc(L"quinta"), tsc(L"sexta"),
                     tsc(L"sábado")},
             .abmon = {tsc(L"jan"), tsc(L"fev"), tsc(L"mar"), tsc(L"abr"), tsc(L"mai"), tsc(L"jun"), tsc(L"jul"),
                       tsc(L"ago"), tsc(L"set"), tsc(L"out"), tsc(L"nov"), tsc(L"dez")},
             .mon = {tsc(L"janeiro"), tsc(L"fevereiro"), tsc(L"março"), tsc(L"abril"), tsc(L"maio"), tsc(L"junho"),
                     tsc(L"julho"), tsc(L"agosto"), tsc(L"setembro"), tsc(L"outubro"), tsc(L"novembro"),
                     tsc(L"dezembro")},
             .d_t_fmt = tsc(L"%a %d %b %Y %T"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(L""), tsc(L"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(L"^[+1SsyY]"), .noexpr = tsc(L"^[-0nN]"), .yesstr = tsc(L"sim"), .nostr = tsc(L"não")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"), .int_select = tsc(L"00"), .int_prefix = tsc(L"55")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Brasil"),
                .country_ab2 = tsc(L"BR"),
                .country_ab3 = tsc(L"BRA"),
                .country_num = 76,
                .country_car = tsc(L"BR"),
                .lang_name = tsc(L"português"),
                .lang_ab = tsc(L"pt"),
                .lang_term = tsc(L"por"),
                .lang_lib = tsc(L"por")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"pt_BR"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Portuguese locale for Brasil"),
         .source = tsc(u8"RAP\t\t;\t\tfast_io"),
         .address = tsc(
             u8"Sankt Jørgens Alle 8, DK-1615 København V, "
             u8"Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"fast_io"),
         .email = tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Brazilian Portuguese"),
         .territory = tsc(u8"Brazil"),
         .revision = tsc(u8"1.0"),
         .date = tsc(u8"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(u8"BRL "),
                 .currency_symbol = tsc(u8"R$"),
                 .mon_decimal_point = tsc(u8","),
                 .mon_thousands_sep = tsc(u8"."),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u8""),
                 .negative_sign = tsc(u8"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u8","), .thousands_sep = tsc(u8"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u8"dom"), tsc(u8"seg"), tsc(u8"ter"), tsc(u8"qua"), tsc(u8"qui"), tsc(u8"sex"),
                       tsc(u8"sáb")},
             .day = {tsc(u8"domingo"), tsc(u8"segunda"), tsc(u8"terça"), tsc(u8"quarta"), tsc(u8"quinta"),
                     tsc(u8"sexta"), tsc(u8"sábado")},
             .abmon = {tsc(u8"jan"), tsc(u8"fev"), tsc(u8"mar"), tsc(u8"abr"), tsc(u8"mai"), tsc(u8"jun"), tsc(u8"jul"),
                       tsc(u8"ago"), tsc(u8"set"), tsc(u8"out"), tsc(u8"nov"), tsc(u8"dez")},
             .mon = {tsc(u8"janeiro"), tsc(u8"fevereiro"), tsc(u8"março"), tsc(u8"abril"), tsc(u8"maio"),
                     tsc(u8"junho"), tsc(u8"julho"), tsc(u8"agosto"), tsc(u8"setembro"), tsc(u8"outubro"),
                     tsc(u8"novembro"), tsc(u8"dezembro")},
             .d_t_fmt = tsc(u8"%a %d %b %Y %T"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u8"^[+1SsyY]"),
                 .noexpr = tsc(u8"^[-0nN]"),
                 .yesstr = tsc(u8"sim"),
                 .nostr = tsc(u8"não")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"), .int_select = tsc(u8"00"), .int_prefix = tsc(u8"55")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Brasil"),
                .country_ab2 = tsc(u8"BR"),
                .country_ab3 = tsc(u8"BRA"),
                .country_num = 76,
                .country_car = tsc(u8"BR"),
                .lang_name = tsc(u8"português"),
                .lang_ab = tsc(u8"pt"),
                .lang_term = tsc(u8"por"),
                .lang_lib = tsc(u8"por")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"pt_BR"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Portuguese locale for Brasil"),
         .source = tsc(u"RAP\t\t;\t\tfast_io"),
         .address = tsc(
             u"Sankt Jørgens Alle 8, DK-1615 København V, "
             u"Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"fast_io"),
         .email = tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Brazilian Portuguese"),
         .territory = tsc(u"Brazil"),
         .revision = tsc(u"1.0"),
         .date = tsc(u"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(u"BRL "),
                 .currency_symbol = tsc(u"R$"),
                 .mon_decimal_point = tsc(u","),
                 .mon_thousands_sep = tsc(u"."),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u""),
                 .negative_sign = tsc(u"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u","), .thousands_sep = tsc(u"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u"dom"), tsc(u"seg"), tsc(u"ter"), tsc(u"qua"), tsc(u"qui"), tsc(u"sex"), tsc(u"sáb")},
             .day = {tsc(u"domingo"), tsc(u"segunda"), tsc(u"terça"), tsc(u"quarta"), tsc(u"quinta"), tsc(u"sexta"),
                     tsc(u"sábado")},
             .abmon = {tsc(u"jan"), tsc(u"fev"), tsc(u"mar"), tsc(u"abr"), tsc(u"mai"), tsc(u"jun"), tsc(u"jul"),
                       tsc(u"ago"), tsc(u"set"), tsc(u"out"), tsc(u"nov"), tsc(u"dez")},
             .mon = {tsc(u"janeiro"), tsc(u"fevereiro"), tsc(u"março"), tsc(u"abril"), tsc(u"maio"), tsc(u"junho"),
                     tsc(u"julho"), tsc(u"agosto"), tsc(u"setembro"), tsc(u"outubro"), tsc(u"novembro"),
                     tsc(u"dezembro")},
             .d_t_fmt = tsc(u"%a %d %b %Y %T"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(u""), tsc(u"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u"^[+1SsyY]"), .noexpr = tsc(u"^[-0nN]"), .yesstr = tsc(u"sim"), .nostr = tsc(u"não")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"), .int_select = tsc(u"00"), .int_prefix = tsc(u"55")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Brasil"),
                .country_ab2 = tsc(u"BR"),
                .country_ab3 = tsc(u"BRA"),
                .country_num = 76,
                .country_car = tsc(u"BR"),
                .lang_name = tsc(u"português"),
                .lang_ab = tsc(u"pt"),
                .lang_term = tsc(u"por"),
                .lang_lib = tsc(u"por")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"pt_BR"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Portuguese locale for Brasil"),
         .source = tsc(U"RAP\t\t;\t\tfast_io"),
         .address = tsc(
             U"Sankt Jørgens Alle 8, DK-1615 København V, "
             U"Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"fast_io"),
         .email = tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Brazilian Portuguese"),
         .territory = tsc(U"Brazil"),
         .revision = tsc(U"1.0"),
         .date = tsc(U"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(U"BRL "),
                 .currency_symbol = tsc(U"R$"),
                 .mon_decimal_point = tsc(U","),
                 .mon_thousands_sep = tsc(U"."),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(U""),
                 .negative_sign = tsc(U"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(U","), .thousands_sep = tsc(U"."), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(U"dom"), tsc(U"seg"), tsc(U"ter"), tsc(U"qua"), tsc(U"qui"), tsc(U"sex"), tsc(U"sáb")},
             .day = {tsc(U"domingo"), tsc(U"segunda"), tsc(U"terça"), tsc(U"quarta"), tsc(U"quinta"), tsc(U"sexta"),
                     tsc(U"sábado")},
             .abmon = {tsc(U"jan"), tsc(U"fev"), tsc(U"mar"), tsc(U"abr"), tsc(U"mai"), tsc(U"jun"), tsc(U"jul"),
                       tsc(U"ago"), tsc(U"set"), tsc(U"out"), tsc(U"nov"), tsc(U"dez")},
             .mon = {tsc(U"janeiro"), tsc(U"fevereiro"), tsc(U"março"), tsc(U"abril"), tsc(U"maio"), tsc(U"junho"),
                     tsc(U"julho"), tsc(U"agosto"), tsc(U"setembro"), tsc(U"outubro"), tsc(U"novembro"),
                     tsc(U"dezembro")},
             .d_t_fmt = tsc(U"%a %d %b %Y %T"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%a %d %b %Y %T %Z"),
             .am_pm = {tsc(U""), tsc(U"")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(U"^[+1SsyY]"), .noexpr = tsc(U"^[-0nN]"), .yesstr = tsc(U"sim"), .nostr = tsc(U"não")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"), .int_select = tsc(U"00"), .int_prefix = tsc(U"55")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Brasil"),
                .country_ab2 = tsc(U"BR"),
                .country_ab3 = tsc(U"BRA"),
                .country_num = 76,
                .country_car = tsc(U"BR"),
                .lang_name = tsc(U"português"),
                .lang_ab = tsc(U"pt"),
                .lang_term = tsc(U"por"),
                .lang_lib = tsc(U"por")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"