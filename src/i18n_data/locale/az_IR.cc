﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr basic_io_scatter_t<char> time_alt_digits_storage[]{
    tsc("۰۰"), tsc("۰۱"), tsc("۰۲"), tsc("۰۳"), tsc("۰۴"), tsc("۰۵"), tsc("۰۶"), tsc("۰۷"), tsc("۰۸"), tsc("۰۹"),
    tsc("۱۰"), tsc("۱۱"), tsc("۱۲"), tsc("۱۳"), tsc("۱۴"), tsc("۱۵"), tsc("۱۶"), tsc("۱۷"), tsc("۱۸"), tsc("۱۹"),
    tsc("۲۰"), tsc("۲۱"), tsc("۲۲"), tsc("۲۳"), tsc("۲۴"), tsc("۲۵"), tsc("۲۶"), tsc("۲۷"), tsc("۲۸"), tsc("۲۹"),
    tsc("۳۰"), tsc("۳۱"), tsc("۳۲"), tsc("۳۳"), tsc("۳۴"), tsc("۳۵"), tsc("۳۶"), tsc("۳۷"), tsc("۳۸"), tsc("۳۹"),
    tsc("۴۰"), tsc("۴۱"), tsc("۴۲"), tsc("۴۳"), tsc("۴۴"), tsc("۴۵"), tsc("۴۶"), tsc("۴۷"), tsc("۴۸"), tsc("۴۹"),
    tsc("۵۰"), tsc("۵۱"), tsc("۵۲"), tsc("۵۳"), tsc("۵۴"), tsc("۵۵"), tsc("۵۶"), tsc("۵۷"), tsc("۵۸"), tsc("۵۹"),
    tsc("۶۰"), tsc("۶۱"), tsc("۶۲"), tsc("۶۳"), tsc("۶۴"), tsc("۶۵"), tsc("۶۶"), tsc("۶۷"), tsc("۶۸"), tsc("۶۹"),
    tsc("۷۰"), tsc("۷۱"), tsc("۷۲"), tsc("۷۳"), tsc("۷۴"), tsc("۷۵"), tsc("۷۶"), tsc("۷۷"), tsc("۷۸"), tsc("۷۹"),
    tsc("۸۰"), tsc("۸۱"), tsc("۸۲"), tsc("۸۳"), tsc("۸۴"), tsc("۸۵"), tsc("۸۶"), tsc("۸۷"), tsc("۸۸"), tsc("۸۹"),
    tsc("۹۰"), tsc("۹۱"), tsc("۹۲"), tsc("۹۳"), tsc("۹۴"), tsc("۹۵"), tsc("۹۶"), tsc("۹۷"), tsc("۹۸"), tsc("۹۹")};

inline constexpr lc_all lc_all_global{
    .identification = {.name = tsc("az_IR"),
                       .encoding = tsc(FAST_IO_LOCALE_ENCODING),
                       .title = tsc("South Azerbaijani language locale for Iran"),
                       .source = tsc("fast_io"),
                       .address =
                           tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc("Mousa Moradi\t\t;\t\tfast_io"),
                       .email = tsc("mousamk@gmail.com;euloanty@live.com"),
                       .tel = tsc("(+98) 914 492 1058"),
                       .fax = tsc(""),
                       .language = tsc("South Azerbaijani"),
                       .territory = tsc("Iran"),
                       .revision = tsc("0.2"),
                       .date = tsc("2014-05-01")},
    .monetary = {.int_curr_symbol = tsc("IRR "),
                 .currency_symbol = tsc("ریال"),
                 .mon_decimal_point = tsc("٫"),
                 .mon_thousands_sep = tsc("٬"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(""),
                 .negative_sign = tsc("-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .int_p_cs_precedes = 0,
                 .int_p_sep_by_space = 1,
                 .int_n_cs_precedes = 0,
                 .int_n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1,
                 .int_p_sign_posn = 1,
                 .int_n_sign_posn = 1},
    .numeric = {.decimal_point = tsc("."), .thousands_sep = tsc(","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc("یکشنبه"), tsc("دوشنبه"), tsc("سه‌شنبه"), tsc("چارشنبه"), tsc("جۆمعه آخشامی"),
                       tsc("جۆمعه"), tsc("شنبه")},
             .day = {tsc("یکشنبه"), tsc("دوشنبه"), tsc("سه‌شنبه"), tsc("چارشنبه"), tsc("جۆمعه آخشامی"),
                     tsc("جۆمعه"), tsc("شنبه")},
             .abmon = {tsc("ژانویه"), tsc("فوریه"), tsc("مارس"), tsc("آوریل"), tsc("مئی"), tsc("ژوئن"), tsc("جولای"),
                       tsc("آقۇست"), tsc("سپتامبر"), tsc("اوْکتوْبر"), tsc("نوْوامبر"), tsc("دسامبر")},
             .mon = {tsc("ژانویه"), tsc("فوریه"), tsc("مارس"), tsc("آوریل"), tsc("مئی"), tsc("ژوئن"), tsc("جولای"),
                     tsc("آقۇست"), tsc("سپتامبر"), tsc("اوْکتوْبر"), tsc("نوْوامبر"), tsc("دسامبر")},
             .d_t_fmt = tsc("‫%A %Oe %B %Oy، %OH:%OM:%OS‬"),
             .d_fmt = tsc("%Oy//%Om//%Od"),
             .t_fmt = tsc("%OH:%OM:%OS"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("‫%A %Oe %B %Oy، ساعات %OH:%OM:%OS (%Z)‬"),
             .am_pm = {tsc(""), tsc("")},
             .alt_digits = {time_alt_digits_storage, 100},
             .first_weekday = 7,
             .first_workday = 7,
             .cal_direction = 3},
    .messages = {.yesexpr = tsc("^[+1yYهب]"), .noexpr = tsc("^[-0nNیخ]"), .yesstr = tsc("هن"), .nostr = tsc("یوْخ")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("‪+%c %a %l‬"),
                  .tel_dom_fmt = tsc("‪%A‒%l‬"),
                  .int_select = tsc("00"),
                  .int_prefix = tsc("98")},
    .name = {.name_fmt = tsc("%d%t%s%t%g%t%m%t%f"),
             .name_gen = tsc(""),
             .name_miss = tsc("خانیم"),
             .name_mr = tsc("آغا"),
             .name_mrs = tsc("خانیم"),
             .name_ms = tsc("خانیم")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc("ایران"),
                .country_ab2 = tsc("IR"),
                .country_ab3 = tsc("IRN"),
                .country_num = 364,
                .country_car = tsc("IR"),
                .country_isbn = tsc("964"),
                .lang_name = tsc("تۆرکجه"),
                .lang_ab = tsc("az"),
                .lang_term = tsc("azb"),
                .lang_lib = tsc("aze")},
    .measurement = {.measurement = 1}};

inline constexpr basic_io_scatter_t<wchar_t> wtime_alt_digits_storage[]{
    tsc(L"۰۰"), tsc(L"۰۱"), tsc(L"۰۲"), tsc(L"۰۳"), tsc(L"۰۴"), tsc(L"۰۵"), tsc(L"۰۶"), tsc(L"۰۷"), tsc(L"۰۸"),
    tsc(L"۰۹"), tsc(L"۱۰"), tsc(L"۱۱"), tsc(L"۱۲"), tsc(L"۱۳"), tsc(L"۱۴"), tsc(L"۱۵"), tsc(L"۱۶"), tsc(L"۱۷"),
    tsc(L"۱۸"), tsc(L"۱۹"), tsc(L"۲۰"), tsc(L"۲۱"), tsc(L"۲۲"), tsc(L"۲۳"), tsc(L"۲۴"), tsc(L"۲۵"), tsc(L"۲۶"),
    tsc(L"۲۷"), tsc(L"۲۸"), tsc(L"۲۹"), tsc(L"۳۰"), tsc(L"۳۱"), tsc(L"۳۲"), tsc(L"۳۳"), tsc(L"۳۴"), tsc(L"۳۵"),
    tsc(L"۳۶"), tsc(L"۳۷"), tsc(L"۳۸"), tsc(L"۳۹"), tsc(L"۴۰"), tsc(L"۴۱"), tsc(L"۴۲"), tsc(L"۴۳"), tsc(L"۴۴"),
    tsc(L"۴۵"), tsc(L"۴۶"), tsc(L"۴۷"), tsc(L"۴۸"), tsc(L"۴۹"), tsc(L"۵۰"), tsc(L"۵۱"), tsc(L"۵۲"), tsc(L"۵۳"),
    tsc(L"۵۴"), tsc(L"۵۵"), tsc(L"۵۶"), tsc(L"۵۷"), tsc(L"۵۸"), tsc(L"۵۹"), tsc(L"۶۰"), tsc(L"۶۱"), tsc(L"۶۲"),
    tsc(L"۶۳"), tsc(L"۶۴"), tsc(L"۶۵"), tsc(L"۶۶"), tsc(L"۶۷"), tsc(L"۶۸"), tsc(L"۶۹"), tsc(L"۷۰"), tsc(L"۷۱"),
    tsc(L"۷۲"), tsc(L"۷۳"), tsc(L"۷۴"), tsc(L"۷۵"), tsc(L"۷۶"), tsc(L"۷۷"), tsc(L"۷۸"), tsc(L"۷۹"), tsc(L"۸۰"),
    tsc(L"۸۱"), tsc(L"۸۲"), tsc(L"۸۳"), tsc(L"۸۴"), tsc(L"۸۵"), tsc(L"۸۶"), tsc(L"۸۷"), tsc(L"۸۸"), tsc(L"۸۹"),
    tsc(L"۹۰"), tsc(L"۹۱"), tsc(L"۹۲"), tsc(L"۹۳"), tsc(L"۹۴"), tsc(L"۹۵"), tsc(L"۹۶"), tsc(L"۹۷"), tsc(L"۹۸"),
    tsc(L"۹۹")};

inline constexpr wlc_all wlc_all_global{
    .identification = {.name = tsc(L"az_IR"),
                       .encoding = tsc(FAST_IO_LOCALE_LENCODING),
                       .title = tsc(L"South Azerbaijani language locale for Iran"),
                       .source = tsc(L"fast_io"),
                       .address =
                           tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(L"Mousa Moradi\t\t;\t\tfast_io"),
                       .email = tsc(L"mousamk@gmail.com;euloanty@live.com"),
                       .tel = tsc(L"(+98) 914 492 1058"),
                       .fax = tsc(L""),
                       .language = tsc(L"South Azerbaijani"),
                       .territory = tsc(L"Iran"),
                       .revision = tsc(L"0.2"),
                       .date = tsc(L"2014-05-01")},
    .monetary = {.int_curr_symbol = tsc(L"IRR "),
                 .currency_symbol = tsc(L"ریال"),
                 .mon_decimal_point = tsc(L"٫"),
                 .mon_thousands_sep = tsc(L"٬"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(L""),
                 .negative_sign = tsc(L"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .int_p_cs_precedes = 0,
                 .int_p_sep_by_space = 1,
                 .int_n_cs_precedes = 0,
                 .int_n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1,
                 .int_p_sign_posn = 1,
                 .int_n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(L"."), .thousands_sep = tsc(L","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(L"یکشنبه"), tsc(L"دوشنبه"), tsc(L"سه‌شنبه"), tsc(L"چارشنبه"), tsc(L"جۆمعه آخشامی"),
                       tsc(L"جۆمعه"), tsc(L"شنبه")},
             .day = {tsc(L"یکشنبه"), tsc(L"دوشنبه"), tsc(L"سه‌شنبه"), tsc(L"چارشنبه"), tsc(L"جۆمعه آخشامی"),
                     tsc(L"جۆمعه"), tsc(L"شنبه")},
             .abmon = {tsc(L"ژانویه"), tsc(L"فوریه"), tsc(L"مارس"), tsc(L"آوریل"), tsc(L"مئی"), tsc(L"ژوئن"),
                       tsc(L"جولای"), tsc(L"آقۇست"), tsc(L"سپتامبر"), tsc(L"اوْکتوْبر"), tsc(L"نوْوامبر"), tsc(L"دسامبر")},
             .mon = {tsc(L"ژانویه"), tsc(L"فوریه"), tsc(L"مارس"), tsc(L"آوریل"), tsc(L"مئی"), tsc(L"ژوئن"),
                     tsc(L"جولای"), tsc(L"آقۇست"), tsc(L"سپتامبر"), tsc(L"اوْکتوْبر"), tsc(L"نوْوامبر"), tsc(L"دسامبر")},
             .d_t_fmt = tsc(L"‫%A %Oe %B %Oy، %OH:%OM:%OS‬"),
             .d_fmt = tsc(L"%Oy//%Om//%Od"),
             .t_fmt = tsc(L"%OH:%OM:%OS"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"‫%A %Oe %B %Oy، ساعات %OH:%OM:%OS (%Z)‬"),
             .am_pm = {tsc(L""), tsc(L"")},
             .alt_digits = {wtime_alt_digits_storage, 100},
             .first_weekday = 7,
             .first_workday = 7,
             .cal_direction = 3},
    .messages = {.yesexpr = tsc(L"^[+1yYهب]"), .noexpr = tsc(L"^[-0nNیخ]"), .yesstr = tsc(L"هن"), .nostr = tsc(L"یوْخ")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"‪+%c %a %l‬"),
                  .tel_dom_fmt = tsc(L"‪%A‒%l‬"),
                  .int_select = tsc(L"00"),
                  .int_prefix = tsc(L"98")},
    .name = {.name_fmt = tsc(L"%d%t%s%t%g%t%m%t%f"),
             .name_gen = tsc(L""),
             .name_miss = tsc(L"خانیم"),
             .name_mr = tsc(L"آغا"),
             .name_mrs = tsc(L"خانیم"),
             .name_ms = tsc(L"خانیم")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(L"ایران"),
                .country_ab2 = tsc(L"IR"),
                .country_ab3 = tsc(L"IRN"),
                .country_num = 364,
                .country_car = tsc(L"IR"),
                .country_isbn = tsc(L"964"),
                .lang_name = tsc(L"تۆرکجه"),
                .lang_ab = tsc(L"az"),
                .lang_term = tsc(L"azb"),
                .lang_lib = tsc(L"aze")},
    .measurement = {.measurement = 1}};

inline constexpr basic_io_scatter_t<char8_t> u8time_alt_digits_storage[]{
    tsc(u8"۰۰"), tsc(u8"۰۱"), tsc(u8"۰۲"), tsc(u8"۰۳"), tsc(u8"۰۴"), tsc(u8"۰۵"), tsc(u8"۰۶"), tsc(u8"۰۷"), tsc(u8"۰۸"),
    tsc(u8"۰۹"), tsc(u8"۱۰"), tsc(u8"۱۱"), tsc(u8"۱۲"), tsc(u8"۱۳"), tsc(u8"۱۴"), tsc(u8"۱۵"), tsc(u8"۱۶"), tsc(u8"۱۷"),
    tsc(u8"۱۸"), tsc(u8"۱۹"), tsc(u8"۲۰"), tsc(u8"۲۱"), tsc(u8"۲۲"), tsc(u8"۲۳"), tsc(u8"۲۴"), tsc(u8"۲۵"), tsc(u8"۲۶"),
    tsc(u8"۲۷"), tsc(u8"۲۸"), tsc(u8"۲۹"), tsc(u8"۳۰"), tsc(u8"۳۱"), tsc(u8"۳۲"), tsc(u8"۳۳"), tsc(u8"۳۴"), tsc(u8"۳۵"),
    tsc(u8"۳۶"), tsc(u8"۳۷"), tsc(u8"۳۸"), tsc(u8"۳۹"), tsc(u8"۴۰"), tsc(u8"۴۱"), tsc(u8"۴۲"), tsc(u8"۴۳"), tsc(u8"۴۴"),
    tsc(u8"۴۵"), tsc(u8"۴۶"), tsc(u8"۴۷"), tsc(u8"۴۸"), tsc(u8"۴۹"), tsc(u8"۵۰"), tsc(u8"۵۱"), tsc(u8"۵۲"), tsc(u8"۵۳"),
    tsc(u8"۵۴"), tsc(u8"۵۵"), tsc(u8"۵۶"), tsc(u8"۵۷"), tsc(u8"۵۸"), tsc(u8"۵۹"), tsc(u8"۶۰"), tsc(u8"۶۱"), tsc(u8"۶۲"),
    tsc(u8"۶۳"), tsc(u8"۶۴"), tsc(u8"۶۵"), tsc(u8"۶۶"), tsc(u8"۶۷"), tsc(u8"۶۸"), tsc(u8"۶۹"), tsc(u8"۷۰"), tsc(u8"۷۱"),
    tsc(u8"۷۲"), tsc(u8"۷۳"), tsc(u8"۷۴"), tsc(u8"۷۵"), tsc(u8"۷۶"), tsc(u8"۷۷"), tsc(u8"۷۸"), tsc(u8"۷۹"), tsc(u8"۸۰"),
    tsc(u8"۸۱"), tsc(u8"۸۲"), tsc(u8"۸۳"), tsc(u8"۸۴"), tsc(u8"۸۵"), tsc(u8"۸۶"), tsc(u8"۸۷"), tsc(u8"۸۸"), tsc(u8"۸۹"),
    tsc(u8"۹۰"), tsc(u8"۹۱"), tsc(u8"۹۲"), tsc(u8"۹۳"), tsc(u8"۹۴"), tsc(u8"۹۵"), tsc(u8"۹۶"), tsc(u8"۹۷"), tsc(u8"۹۸"),
    tsc(u8"۹۹")};

inline constexpr u8lc_all u8lc_all_global{
    .identification = {.name = tsc(u8"az_IR"),
                       .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
                       .title = tsc(u8"South Azerbaijani language locale for Iran"),
                       .source = tsc(u8"fast_io"),
                       .address =
                           tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u8"Mousa Moradi\t\t;\t\tfast_io"),
                       .email = tsc(u8"mousamk@gmail.com;euloanty@live.com"),
                       .tel = tsc(u8"(+98) 914 492 1058"),
                       .fax = tsc(u8""),
                       .language = tsc(u8"South Azerbaijani"),
                       .territory = tsc(u8"Iran"),
                       .revision = tsc(u8"0.2"),
                       .date = tsc(u8"2014-05-01")},
    .monetary = {.int_curr_symbol = tsc(u8"IRR "),
                 .currency_symbol = tsc(u8"ریال"),
                 .mon_decimal_point = tsc(u8"٫"),
                 .mon_thousands_sep = tsc(u8"٬"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u8""),
                 .negative_sign = tsc(u8"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .int_p_cs_precedes = 0,
                 .int_p_sep_by_space = 1,
                 .int_n_cs_precedes = 0,
                 .int_n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1,
                 .int_p_sign_posn = 1,
                 .int_n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u8"."), .thousands_sep = tsc(u8","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u8"یکشنبه"), tsc(u8"دوشنبه"), tsc(u8"سه‌شنبه"), tsc(u8"چارشنبه"),
                       tsc(u8"جۆمعه آخشامی"), tsc(u8"جۆمعه"), tsc(u8"شنبه")},
             .day = {tsc(u8"یکشنبه"), tsc(u8"دوشنبه"), tsc(u8"سه‌شنبه"), tsc(u8"چارشنبه"),
                     tsc(u8"جۆمعه آخشامی"), tsc(u8"جۆمعه"), tsc(u8"شنبه")},
             .abmon = {tsc(u8"ژانویه"), tsc(u8"فوریه"), tsc(u8"مارس"), tsc(u8"آوریل"), tsc(u8"مئی"), tsc(u8"ژوئن"),
                       tsc(u8"جولای"), tsc(u8"آقۇست"), tsc(u8"سپتامبر"), tsc(u8"اوْکتوْبر"), tsc(u8"نوْوامبر"),
                       tsc(u8"دسامبر")},
             .mon = {tsc(u8"ژانویه"), tsc(u8"فوریه"), tsc(u8"مارس"), tsc(u8"آوریل"), tsc(u8"مئی"), tsc(u8"ژوئن"),
                     tsc(u8"جولای"), tsc(u8"آقۇست"), tsc(u8"سپتامبر"), tsc(u8"اوْکتوْبر"), tsc(u8"نوْوامبر"),
                     tsc(u8"دسامبر")},
             .d_t_fmt = tsc(u8"‫%A %Oe %B %Oy، %OH:%OM:%OS‬"),
             .d_fmt = tsc(u8"%Oy//%Om//%Od"),
             .t_fmt = tsc(u8"%OH:%OM:%OS"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"‫%A %Oe %B %Oy، ساعات %OH:%OM:%OS (%Z)‬"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .alt_digits = {u8time_alt_digits_storage, 100},
             .first_weekday = 7,
             .first_workday = 7,
             .cal_direction = 3},
    .messages = {.yesexpr = tsc(u8"^[+1yYهب]"),
                 .noexpr = tsc(u8"^[-0nNیخ]"),
                 .yesstr = tsc(u8"هن"),
                 .nostr = tsc(u8"یوْخ")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"‪+%c %a %l‬"),
                  .tel_dom_fmt = tsc(u8"‪%A‒%l‬"),
                  .int_select = tsc(u8"00"),
                  .int_prefix = tsc(u8"98")},
    .name = {.name_fmt = tsc(u8"%d%t%s%t%g%t%m%t%f"),
             .name_gen = tsc(u8""),
             .name_miss = tsc(u8"خانیم"),
             .name_mr = tsc(u8"آغا"),
             .name_mrs = tsc(u8"خانیم"),
             .name_ms = tsc(u8"خانیم")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u8"ایران"),
                .country_ab2 = tsc(u8"IR"),
                .country_ab3 = tsc(u8"IRN"),
                .country_num = 364,
                .country_car = tsc(u8"IR"),
                .country_isbn = tsc(u8"964"),
                .lang_name = tsc(u8"تۆرکجه"),
                .lang_ab = tsc(u8"az"),
                .lang_term = tsc(u8"azb"),
                .lang_lib = tsc(u8"aze")},
    .measurement = {.measurement = 1}};

inline constexpr basic_io_scatter_t<char16_t> u16time_alt_digits_storage[]{
    tsc(u"۰۰"), tsc(u"۰۱"), tsc(u"۰۲"), tsc(u"۰۳"), tsc(u"۰۴"), tsc(u"۰۵"), tsc(u"۰۶"), tsc(u"۰۷"), tsc(u"۰۸"),
    tsc(u"۰۹"), tsc(u"۱۰"), tsc(u"۱۱"), tsc(u"۱۲"), tsc(u"۱۳"), tsc(u"۱۴"), tsc(u"۱۵"), tsc(u"۱۶"), tsc(u"۱۷"),
    tsc(u"۱۸"), tsc(u"۱۹"), tsc(u"۲۰"), tsc(u"۲۱"), tsc(u"۲۲"), tsc(u"۲۳"), tsc(u"۲۴"), tsc(u"۲۵"), tsc(u"۲۶"),
    tsc(u"۲۷"), tsc(u"۲۸"), tsc(u"۲۹"), tsc(u"۳۰"), tsc(u"۳۱"), tsc(u"۳۲"), tsc(u"۳۳"), tsc(u"۳۴"), tsc(u"۳۵"),
    tsc(u"۳۶"), tsc(u"۳۷"), tsc(u"۳۸"), tsc(u"۳۹"), tsc(u"۴۰"), tsc(u"۴۱"), tsc(u"۴۲"), tsc(u"۴۳"), tsc(u"۴۴"),
    tsc(u"۴۵"), tsc(u"۴۶"), tsc(u"۴۷"), tsc(u"۴۸"), tsc(u"۴۹"), tsc(u"۵۰"), tsc(u"۵۱"), tsc(u"۵۲"), tsc(u"۵۳"),
    tsc(u"۵۴"), tsc(u"۵۵"), tsc(u"۵۶"), tsc(u"۵۷"), tsc(u"۵۸"), tsc(u"۵۹"), tsc(u"۶۰"), tsc(u"۶۱"), tsc(u"۶۲"),
    tsc(u"۶۳"), tsc(u"۶۴"), tsc(u"۶۵"), tsc(u"۶۶"), tsc(u"۶۷"), tsc(u"۶۸"), tsc(u"۶۹"), tsc(u"۷۰"), tsc(u"۷۱"),
    tsc(u"۷۲"), tsc(u"۷۳"), tsc(u"۷۴"), tsc(u"۷۵"), tsc(u"۷۶"), tsc(u"۷۷"), tsc(u"۷۸"), tsc(u"۷۹"), tsc(u"۸۰"),
    tsc(u"۸۱"), tsc(u"۸۲"), tsc(u"۸۳"), tsc(u"۸۴"), tsc(u"۸۵"), tsc(u"۸۶"), tsc(u"۸۷"), tsc(u"۸۸"), tsc(u"۸۹"),
    tsc(u"۹۰"), tsc(u"۹۱"), tsc(u"۹۲"), tsc(u"۹۳"), tsc(u"۹۴"), tsc(u"۹۵"), tsc(u"۹۶"), tsc(u"۹۷"), tsc(u"۹۸"),
    tsc(u"۹۹")};

inline constexpr u16lc_all u16lc_all_global{
    .identification = {.name = tsc(u"az_IR"),
                       .encoding = tsc(FAST_IO_LOCALE_uENCODING),
                       .title = tsc(u"South Azerbaijani language locale for Iran"),
                       .source = tsc(u"fast_io"),
                       .address =
                           tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u"Mousa Moradi\t\t;\t\tfast_io"),
                       .email = tsc(u"mousamk@gmail.com;euloanty@live.com"),
                       .tel = tsc(u"(+98) 914 492 1058"),
                       .fax = tsc(u""),
                       .language = tsc(u"South Azerbaijani"),
                       .territory = tsc(u"Iran"),
                       .revision = tsc(u"0.2"),
                       .date = tsc(u"2014-05-01")},
    .monetary = {.int_curr_symbol = tsc(u"IRR "),
                 .currency_symbol = tsc(u"ریال"),
                 .mon_decimal_point = tsc(u"٫"),
                 .mon_thousands_sep = tsc(u"٬"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(u""),
                 .negative_sign = tsc(u"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .int_p_cs_precedes = 0,
                 .int_p_sep_by_space = 1,
                 .int_n_cs_precedes = 0,
                 .int_n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1,
                 .int_p_sign_posn = 1,
                 .int_n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(u"."), .thousands_sep = tsc(u","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u"یکشنبه"), tsc(u"دوشنبه"), tsc(u"سه‌شنبه"), tsc(u"چارشنبه"), tsc(u"جۆمعه آخشامی"),
                       tsc(u"جۆمعه"), tsc(u"شنبه")},
             .day = {tsc(u"یکشنبه"), tsc(u"دوشنبه"), tsc(u"سه‌شنبه"), tsc(u"چارشنبه"), tsc(u"جۆمعه آخشامی"),
                     tsc(u"جۆمعه"), tsc(u"شنبه")},
             .abmon = {tsc(u"ژانویه"), tsc(u"فوریه"), tsc(u"مارس"), tsc(u"آوریل"), tsc(u"مئی"), tsc(u"ژوئن"),
                       tsc(u"جولای"), tsc(u"آقۇست"), tsc(u"سپتامبر"), tsc(u"اوْکتوْبر"), tsc(u"نوْوامبر"), tsc(u"دسامبر")},
             .mon = {tsc(u"ژانویه"), tsc(u"فوریه"), tsc(u"مارس"), tsc(u"آوریل"), tsc(u"مئی"), tsc(u"ژوئن"),
                     tsc(u"جولای"), tsc(u"آقۇست"), tsc(u"سپتامبر"), tsc(u"اوْکتوْبر"), tsc(u"نوْوامبر"), tsc(u"دسامبر")},
             .d_t_fmt = tsc(u"‫%A %Oe %B %Oy، %OH:%OM:%OS‬"),
             .d_fmt = tsc(u"%Oy//%Om//%Od"),
             .t_fmt = tsc(u"%OH:%OM:%OS"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"‫%A %Oe %B %Oy، ساعات %OH:%OM:%OS (%Z)‬"),
             .am_pm = {tsc(u""), tsc(u"")},
             .alt_digits = {u16time_alt_digits_storage, 100},
             .first_weekday = 7,
             .first_workday = 7,
             .cal_direction = 3},
    .messages = {.yesexpr = tsc(u"^[+1yYهب]"), .noexpr = tsc(u"^[-0nNیخ]"), .yesstr = tsc(u"هن"), .nostr = tsc(u"یوْخ")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"‪+%c %a %l‬"),
                  .tel_dom_fmt = tsc(u"‪%A‒%l‬"),
                  .int_select = tsc(u"00"),
                  .int_prefix = tsc(u"98")},
    .name = {.name_fmt = tsc(u"%d%t%s%t%g%t%m%t%f"),
             .name_gen = tsc(u""),
             .name_miss = tsc(u"خانیم"),
             .name_mr = tsc(u"آغا"),
             .name_mrs = tsc(u"خانیم"),
             .name_ms = tsc(u"خانیم")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(u"ایران"),
                .country_ab2 = tsc(u"IR"),
                .country_ab3 = tsc(u"IRN"),
                .country_num = 364,
                .country_car = tsc(u"IR"),
                .country_isbn = tsc(u"964"),
                .lang_name = tsc(u"تۆرکجه"),
                .lang_ab = tsc(u"az"),
                .lang_term = tsc(u"azb"),
                .lang_lib = tsc(u"aze")},
    .measurement = {.measurement = 1}};

inline constexpr basic_io_scatter_t<char32_t> u32time_alt_digits_storage[]{
    tsc(U"۰۰"), tsc(U"۰۱"), tsc(U"۰۲"), tsc(U"۰۳"), tsc(U"۰۴"), tsc(U"۰۵"), tsc(U"۰۶"), tsc(U"۰۷"), tsc(U"۰۸"),
    tsc(U"۰۹"), tsc(U"۱۰"), tsc(U"۱۱"), tsc(U"۱۲"), tsc(U"۱۳"), tsc(U"۱۴"), tsc(U"۱۵"), tsc(U"۱۶"), tsc(U"۱۷"),
    tsc(U"۱۸"), tsc(U"۱۹"), tsc(U"۲۰"), tsc(U"۲۱"), tsc(U"۲۲"), tsc(U"۲۳"), tsc(U"۲۴"), tsc(U"۲۵"), tsc(U"۲۶"),
    tsc(U"۲۷"), tsc(U"۲۸"), tsc(U"۲۹"), tsc(U"۳۰"), tsc(U"۳۱"), tsc(U"۳۲"), tsc(U"۳۳"), tsc(U"۳۴"), tsc(U"۳۵"),
    tsc(U"۳۶"), tsc(U"۳۷"), tsc(U"۳۸"), tsc(U"۳۹"), tsc(U"۴۰"), tsc(U"۴۱"), tsc(U"۴۲"), tsc(U"۴۳"), tsc(U"۴۴"),
    tsc(U"۴۵"), tsc(U"۴۶"), tsc(U"۴۷"), tsc(U"۴۸"), tsc(U"۴۹"), tsc(U"۵۰"), tsc(U"۵۱"), tsc(U"۵۲"), tsc(U"۵۳"),
    tsc(U"۵۴"), tsc(U"۵۵"), tsc(U"۵۶"), tsc(U"۵۷"), tsc(U"۵۸"), tsc(U"۵۹"), tsc(U"۶۰"), tsc(U"۶۱"), tsc(U"۶۲"),
    tsc(U"۶۳"), tsc(U"۶۴"), tsc(U"۶۵"), tsc(U"۶۶"), tsc(U"۶۷"), tsc(U"۶۸"), tsc(U"۶۹"), tsc(U"۷۰"), tsc(U"۷۱"),
    tsc(U"۷۲"), tsc(U"۷۳"), tsc(U"۷۴"), tsc(U"۷۵"), tsc(U"۷۶"), tsc(U"۷۷"), tsc(U"۷۸"), tsc(U"۷۹"), tsc(U"۸۰"),
    tsc(U"۸۱"), tsc(U"۸۲"), tsc(U"۸۳"), tsc(U"۸۴"), tsc(U"۸۵"), tsc(U"۸۶"), tsc(U"۸۷"), tsc(U"۸۸"), tsc(U"۸۹"),
    tsc(U"۹۰"), tsc(U"۹۱"), tsc(U"۹۲"), tsc(U"۹۳"), tsc(U"۹۴"), tsc(U"۹۵"), tsc(U"۹۶"), tsc(U"۹۷"), tsc(U"۹۸"),
    tsc(U"۹۹")};

inline constexpr u32lc_all u32lc_all_global{
    .identification = {.name = tsc(U"az_IR"),
                       .encoding = tsc(FAST_IO_LOCALE_UENCODING),
                       .title = tsc(U"South Azerbaijani language locale for Iran"),
                       .source = tsc(U"fast_io"),
                       .address =
                           tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(U"Mousa Moradi\t\t;\t\tfast_io"),
                       .email = tsc(U"mousamk@gmail.com;euloanty@live.com"),
                       .tel = tsc(U"(+98) 914 492 1058"),
                       .fax = tsc(U""),
                       .language = tsc(U"South Azerbaijani"),
                       .territory = tsc(U"Iran"),
                       .revision = tsc(U"0.2"),
                       .date = tsc(U"2014-05-01")},
    .monetary = {.int_curr_symbol = tsc(U"IRR "),
                 .currency_symbol = tsc(U"ریال"),
                 .mon_decimal_point = tsc(U"٫"),
                 .mon_thousands_sep = tsc(U"٬"),
                 .mon_grouping = {numeric_grouping_storage, 1},
                 .positive_sign = tsc(U""),
                 .negative_sign = tsc(U"-"),
                 .int_frac_digits = 0,
                 .frac_digits = 0,
                 .p_cs_precedes = 0,
                 .p_sep_by_space = 1,
                 .n_cs_precedes = 0,
                 .n_sep_by_space = 1,
                 .int_p_cs_precedes = 0,
                 .int_p_sep_by_space = 1,
                 .int_n_cs_precedes = 0,
                 .int_n_sep_by_space = 1,
                 .p_sign_posn = 1,
                 .n_sign_posn = 1,
                 .int_p_sign_posn = 1,
                 .int_n_sign_posn = 1},
    .numeric = {.decimal_point = tsc(U"."), .thousands_sep = tsc(U","), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(U"یکشنبه"), tsc(U"دوشنبه"), tsc(U"سه‌شنبه"), tsc(U"چارشنبه"), tsc(U"جۆمعه آخشامی"),
                       tsc(U"جۆمعه"), tsc(U"شنبه")},
             .day = {tsc(U"یکشنبه"), tsc(U"دوشنبه"), tsc(U"سه‌شنبه"), tsc(U"چارشنبه"), tsc(U"جۆمعه آخشامی"),
                     tsc(U"جۆمعه"), tsc(U"شنبه")},
             .abmon = {tsc(U"ژانویه"), tsc(U"فوریه"), tsc(U"مارس"), tsc(U"آوریل"), tsc(U"مئی"), tsc(U"ژوئن"),
                       tsc(U"جولای"), tsc(U"آقۇست"), tsc(U"سپتامبر"), tsc(U"اوْکتوْبر"), tsc(U"نوْوامبر"), tsc(U"دسامبر")},
             .mon = {tsc(U"ژانویه"), tsc(U"فوریه"), tsc(U"مارس"), tsc(U"آوریل"), tsc(U"مئی"), tsc(U"ژوئن"),
                     tsc(U"جولای"), tsc(U"آقۇست"), tsc(U"سپتامبر"), tsc(U"اوْکتوْبر"), tsc(U"نوْوامبر"), tsc(U"دسامبر")},
             .d_t_fmt = tsc(U"‫%A %Oe %B %Oy، %OH:%OM:%OS‬"),
             .d_fmt = tsc(U"%Oy//%Om//%Od"),
             .t_fmt = tsc(U"%OH:%OM:%OS"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"‫%A %Oe %B %Oy، ساعات %OH:%OM:%OS (%Z)‬"),
             .am_pm = {tsc(U""), tsc(U"")},
             .alt_digits = {u32time_alt_digits_storage, 100},
             .first_weekday = 7,
             .first_workday = 7,
             .cal_direction = 3},
    .messages = {.yesexpr = tsc(U"^[+1yYهب]"), .noexpr = tsc(U"^[-0nNیخ]"), .yesstr = tsc(U"هن"), .nostr = tsc(U"یوْخ")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"‪+%c %a %l‬"),
                  .tel_dom_fmt = tsc(U"‪%A‒%l‬"),
                  .int_select = tsc(U"00"),
                  .int_prefix = tsc(U"98")},
    .name = {.name_fmt = tsc(U"%d%t%s%t%g%t%m%t%f"),
             .name_gen = tsc(U""),
             .name_miss = tsc(U"خانیم"),
             .name_mr = tsc(U"آغا"),
             .name_mrs = tsc(U"خانیم"),
             .name_ms = tsc(U"خانیم")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_name = tsc(U"ایران"),
                .country_ab2 = tsc(U"IR"),
                .country_ab3 = tsc(U"IRN"),
                .country_num = 364,
                .country_car = tsc(U"IR"),
                .country_isbn = tsc(U"964"),
                .lang_name = tsc(U"تۆرکجه"),
                .lang_ab = tsc(U"az"),
                .lang_term = tsc(U"azb"),
                .lang_lib = tsc(U"aze")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"