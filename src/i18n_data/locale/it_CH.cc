﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification = {.name = tsc("it_CH"),
                       .encoding = tsc(FAST_IO_LOCALE_ENCODING),
                       .title = tsc("Italian locale for Switzerland"),
                       .source = tsc("fast_io"),
                       .address =
                           tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc("fast_io"),
                       .email = tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(""),
                       .fax = tsc(""),
                       .language = tsc("Italian"),
                       .territory = tsc("Switzerland"),
                       .revision = tsc("1.0"),
                       .date = tsc("2000-06-29")},
    .monetary = {.int_curr_symbol = tsc("CHF "),
                 .currency_symbol = tsc("CHF"),
                 .mon_decimal_point = tsc("."),
                 .mon_thousands_sep = tsc("’"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(""),
                 .negative_sign = tsc("-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 4,
                 .n_sign_posn = 4},
    .numeric = {.decimal_point = tsc("."), .thousands_sep = tsc("’"), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc("dom"), tsc("lun"), tsc("mar"), tsc("mer"), tsc("gio"), tsc("ven"), tsc("sab")},
             .day = {tsc("domenica"), tsc("lunedì"), tsc("martedì"), tsc("mercoledì"), tsc("giovedì"), tsc("venerdì"),
                     tsc("sabato")},
             .abmon = {tsc("gen"), tsc("feb"), tsc("mar"), tsc("apr"), tsc("mag"), tsc("giu"), tsc("lug"), tsc("ago"),
                       tsc("set"), tsc("ott"), tsc("nov"), tsc("dic")},
             .mon = {tsc("gennaio"), tsc("febbraio"), tsc("marzo"), tsc("aprile"), tsc("maggio"), tsc("giugno"),
                     tsc("luglio"), tsc("agosto"), tsc("settembre"), tsc("ottobre"), tsc("novembre"), tsc("dicembre")},
             .d_t_fmt = tsc("%a %-d %b %Y, %T"),
             .d_fmt = tsc("%d.%m.%Y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%a %-d %b %Y, %T, %Z"),
             .am_pm = {tsc(""), tsc("")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1sSjJoOyY]"), .noexpr = tsc("^[-0nN]"), .yesstr = tsc("sì"), .nostr = tsc("no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"), .int_select = tsc("00"), .int_prefix = tsc("41")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("Svizzera"),
                .country_ab2 = tsc("CH"),
                .country_ab3 = tsc("CHE"),
                .country_num = 756,
                .country_car = tsc("CH"),
                .lang_name = tsc("italiano"),
                .lang_ab = tsc("it"),
                .lang_term = tsc("ita"),
                .lang_lib = tsc("ita")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification = {.name = tsc(L"it_CH"),
                       .encoding = tsc(FAST_IO_LOCALE_LENCODING),
                       .title = tsc(L"Italian locale for Switzerland"),
                       .source = tsc(L"fast_io"),
                       .address =
                           tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(L"fast_io"),
                       .email = tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(L""),
                       .fax = tsc(L""),
                       .language = tsc(L"Italian"),
                       .territory = tsc(L"Switzerland"),
                       .revision = tsc(L"1.0"),
                       .date = tsc(L"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(L"CHF "),
                 .currency_symbol = tsc(L"CHF"),
                 .mon_decimal_point = tsc(L"."),
                 .mon_thousands_sep = tsc(L"’"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(L""),
                 .negative_sign = tsc(L"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 4,
                 .n_sign_posn = 4},
    .numeric = {.decimal_point = tsc(L"."), .thousands_sep = tsc(L"’"), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(L"dom"), tsc(L"lun"), tsc(L"mar"), tsc(L"mer"), tsc(L"gio"), tsc(L"ven"), tsc(L"sab")},
             .day = {tsc(L"domenica"), tsc(L"lunedì"), tsc(L"martedì"), tsc(L"mercoledì"), tsc(L"giovedì"),
                     tsc(L"venerdì"), tsc(L"sabato")},
             .abmon = {tsc(L"gen"), tsc(L"feb"), tsc(L"mar"), tsc(L"apr"), tsc(L"mag"), tsc(L"giu"), tsc(L"lug"),
                       tsc(L"ago"), tsc(L"set"), tsc(L"ott"), tsc(L"nov"), tsc(L"dic")},
             .mon = {tsc(L"gennaio"), tsc(L"febbraio"), tsc(L"marzo"), tsc(L"aprile"), tsc(L"maggio"), tsc(L"giugno"),
                     tsc(L"luglio"), tsc(L"agosto"), tsc(L"settembre"), tsc(L"ottobre"), tsc(L"novembre"),
                     tsc(L"dicembre")},
             .d_t_fmt = tsc(L"%a %-d %b %Y, %T"),
             .d_fmt = tsc(L"%d.%m.%Y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%a %-d %b %Y, %T, %Z"),
             .am_pm = {tsc(L""), tsc(L"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1sSjJoOyY]"),
                 .noexpr = tsc(L"^[-0nN]"),
                 .yesstr = tsc(L"sì"),
                 .nostr = tsc(L"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"), .int_select = tsc(L"00"), .int_prefix = tsc(L"41")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"Svizzera"),
                .country_ab2 = tsc(L"CH"),
                .country_ab3 = tsc(L"CHE"),
                .country_num = 756,
                .country_car = tsc(L"CH"),
                .lang_name = tsc(L"italiano"),
                .lang_ab = tsc(L"it"),
                .lang_term = tsc(L"ita"),
                .lang_lib = tsc(L"ita")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification = {.name = tsc(u8"it_CH"),
                       .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
                       .title = tsc(u8"Italian locale for Switzerland"),
                       .source = tsc(u8"fast_io"),
                       .address =
                           tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u8"fast_io"),
                       .email = tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(u8""),
                       .fax = tsc(u8""),
                       .language = tsc(u8"Italian"),
                       .territory = tsc(u8"Switzerland"),
                       .revision = tsc(u8"1.0"),
                       .date = tsc(u8"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(u8"CHF "),
                 .currency_symbol = tsc(u8"CHF"),
                 .mon_decimal_point = tsc(u8"."),
                 .mon_thousands_sep = tsc(u8"’"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u8""),
                 .negative_sign = tsc(u8"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 4,
                 .n_sign_posn = 4},
    .numeric = {.decimal_point = tsc(u8"."), .thousands_sep = tsc(u8"’"), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u8"dom"), tsc(u8"lun"), tsc(u8"mar"), tsc(u8"mer"), tsc(u8"gio"), tsc(u8"ven"),
                       tsc(u8"sab")},
             .day = {tsc(u8"domenica"), tsc(u8"lunedì"), tsc(u8"martedì"), tsc(u8"mercoledì"), tsc(u8"giovedì"),
                     tsc(u8"venerdì"), tsc(u8"sabato")},
             .abmon = {tsc(u8"gen"), tsc(u8"feb"), tsc(u8"mar"), tsc(u8"apr"), tsc(u8"mag"), tsc(u8"giu"), tsc(u8"lug"),
                       tsc(u8"ago"), tsc(u8"set"), tsc(u8"ott"), tsc(u8"nov"), tsc(u8"dic")},
             .mon = {tsc(u8"gennaio"), tsc(u8"febbraio"), tsc(u8"marzo"), tsc(u8"aprile"), tsc(u8"maggio"),
                     tsc(u8"giugno"), tsc(u8"luglio"), tsc(u8"agosto"), tsc(u8"settembre"), tsc(u8"ottobre"),
                     tsc(u8"novembre"), tsc(u8"dicembre")},
             .d_t_fmt = tsc(u8"%a %-d %b %Y, %T"),
             .d_fmt = tsc(u8"%d.%m.%Y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%a %-d %b %Y, %T, %Z"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1sSjJoOyY]"),
                 .noexpr = tsc(u8"^[-0nN]"),
                 .yesstr = tsc(u8"sì"),
                 .nostr = tsc(u8"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"), .int_select = tsc(u8"00"), .int_prefix = tsc(u8"41")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"Svizzera"),
                .country_ab2 = tsc(u8"CH"),
                .country_ab3 = tsc(u8"CHE"),
                .country_num = 756,
                .country_car = tsc(u8"CH"),
                .lang_name = tsc(u8"italiano"),
                .lang_ab = tsc(u8"it"),
                .lang_term = tsc(u8"ita"),
                .lang_lib = tsc(u8"ita")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification = {.name = tsc(u"it_CH"),
                       .encoding = tsc(FAST_IO_LOCALE_uENCODING),
                       .title = tsc(u"Italian locale for Switzerland"),
                       .source = tsc(u"fast_io"),
                       .address =
                           tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u"fast_io"),
                       .email = tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(u""),
                       .fax = tsc(u""),
                       .language = tsc(u"Italian"),
                       .territory = tsc(u"Switzerland"),
                       .revision = tsc(u"1.0"),
                       .date = tsc(u"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(u"CHF "),
                 .currency_symbol = tsc(u"CHF"),
                 .mon_decimal_point = tsc(u"."),
                 .mon_thousands_sep = tsc(u"’"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u""),
                 .negative_sign = tsc(u"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 4,
                 .n_sign_posn = 4},
    .numeric = {.decimal_point = tsc(u"."), .thousands_sep = tsc(u"’"), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u"dom"), tsc(u"lun"), tsc(u"mar"), tsc(u"mer"), tsc(u"gio"), tsc(u"ven"), tsc(u"sab")},
             .day = {tsc(u"domenica"), tsc(u"lunedì"), tsc(u"martedì"), tsc(u"mercoledì"), tsc(u"giovedì"),
                     tsc(u"venerdì"), tsc(u"sabato")},
             .abmon = {tsc(u"gen"), tsc(u"feb"), tsc(u"mar"), tsc(u"apr"), tsc(u"mag"), tsc(u"giu"), tsc(u"lug"),
                       tsc(u"ago"), tsc(u"set"), tsc(u"ott"), tsc(u"nov"), tsc(u"dic")},
             .mon = {tsc(u"gennaio"), tsc(u"febbraio"), tsc(u"marzo"), tsc(u"aprile"), tsc(u"maggio"), tsc(u"giugno"),
                     tsc(u"luglio"), tsc(u"agosto"), tsc(u"settembre"), tsc(u"ottobre"), tsc(u"novembre"),
                     tsc(u"dicembre")},
             .d_t_fmt = tsc(u"%a %-d %b %Y, %T"),
             .d_fmt = tsc(u"%d.%m.%Y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%a %-d %b %Y, %T, %Z"),
             .am_pm = {tsc(u""), tsc(u"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1sSjJoOyY]"),
                 .noexpr = tsc(u"^[-0nN]"),
                 .yesstr = tsc(u"sì"),
                 .nostr = tsc(u"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"), .int_select = tsc(u"00"), .int_prefix = tsc(u"41")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"Svizzera"),
                .country_ab2 = tsc(u"CH"),
                .country_ab3 = tsc(u"CHE"),
                .country_num = 756,
                .country_car = tsc(u"CH"),
                .lang_name = tsc(u"italiano"),
                .lang_ab = tsc(u"it"),
                .lang_term = tsc(u"ita"),
                .lang_lib = tsc(u"ita")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification = {.name = tsc(U"it_CH"),
                       .encoding = tsc(FAST_IO_LOCALE_UENCODING),
                       .title = tsc(U"Italian locale for Switzerland"),
                       .source = tsc(U"fast_io"),
                       .address =
                           tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(U"fast_io"),
                       .email = tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),
                       .tel = tsc(U""),
                       .fax = tsc(U""),
                       .language = tsc(U"Italian"),
                       .territory = tsc(U"Switzerland"),
                       .revision = tsc(U"1.0"),
                       .date = tsc(U"2000-06-29")},
    .monetary = {.int_curr_symbol = tsc(U"CHF "),
                 .currency_symbol = tsc(U"CHF"),
                 .mon_decimal_point = tsc(U"."),
                 .mon_thousands_sep = tsc(U"’"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(U""),
                 .negative_sign = tsc(U"-"),
                 .int_frac_digits = 2,
                 .frac_digits = 2,
                 .p_cs_precedes = 1,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 1,
                 .n_sep_by_space = 1,
                 .p_sign_posn = 4,
                 .n_sign_posn = 4},
    .numeric = {.decimal_point = tsc(U"."), .thousands_sep = tsc(U"’"), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(U"dom"), tsc(U"lun"), tsc(U"mar"), tsc(U"mer"), tsc(U"gio"), tsc(U"ven"), tsc(U"sab")},
             .day = {tsc(U"domenica"), tsc(U"lunedì"), tsc(U"martedì"), tsc(U"mercoledì"), tsc(U"giovedì"),
                     tsc(U"venerdì"), tsc(U"sabato")},
             .abmon = {tsc(U"gen"), tsc(U"feb"), tsc(U"mar"), tsc(U"apr"), tsc(U"mag"), tsc(U"giu"), tsc(U"lug"),
                       tsc(U"ago"), tsc(U"set"), tsc(U"ott"), tsc(U"nov"), tsc(U"dic")},
             .mon = {tsc(U"gennaio"), tsc(U"febbraio"), tsc(U"marzo"), tsc(U"aprile"), tsc(U"maggio"), tsc(U"giugno"),
                     tsc(U"luglio"), tsc(U"agosto"), tsc(U"settembre"), tsc(U"ottobre"), tsc(U"novembre"),
                     tsc(U"dicembre")},
             .d_t_fmt = tsc(U"%a %-d %b %Y, %T"),
             .d_fmt = tsc(U"%d.%m.%Y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%a %-d %b %Y, %T, %Z"),
             .am_pm = {tsc(U""), tsc(U"")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1sSjJoOyY]"),
                 .noexpr = tsc(U"^[-0nN]"),
                 .yesstr = tsc(U"sì"),
                 .nostr = tsc(U"no")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"), .int_select = tsc(U"00"), .int_prefix = tsc(U"41")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"Svizzera"),
                .country_ab2 = tsc(U"CH"),
                .country_ab3 = tsc(U"CHE"),
                .country_num = 756,
                .country_car = tsc(U"CH"),
                .lang_name = tsc(U"italiano"),
                .lang_ab = tsc(U"it"),
                .lang_term = tsc(U"ita"),
                .lang_lib = tsc(U"ita")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"