﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("mhr_RU"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Mari locale for Russia"),
         .source = tsc("PeshSajSoft Ltd.\t\t;\t\tfast_io"),
         .address =
             tsc("112, Komsomolskaya Street, Yoshkar-Ola, Mari El Republic, "
                 "Russia\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("Vyacheslav Kileev\t\t;\t\tfast_io"),
         .email = tsc("slavakileev@yandex.ru;euloanty@live.com"),
         .tel = tsc(""),
         .language = tsc("Meadow Mari"),
         .territory = tsc("Russia"),
         .revision = tsc("0.2"),
         .date = tsc("2011-04-18")},
    .monetary = {.int_curr_symbol = tsc("RUB "),
                 .currency_symbol = tsc("₽"),
                 .mon_decimal_point = tsc("."),
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
    .numeric = {.decimal_point = tsc(","), .thousands_sep = tsc(" "), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc("Ршр"), tsc("Шчм"), tsc("Кжм"), tsc("Вгч"), tsc("Изр"), tsc("Кгр"), tsc("Шмт")},
             .day = {tsc("Рушарня"), tsc("Шочмо"), tsc("Кушкыжмо"), tsc("Вӱргече"), tsc("Изарня"), tsc("Кугарня"),
                     tsc("Шуматкече")},
             .abmon = {tsc("Шрк"), tsc("Пгж"), tsc("Ӱрн"), tsc("Вшр"), tsc("Ага"), tsc("Пдш"), tsc("Срм"), tsc("Срл"),
                       tsc("Идм"), tsc("Шыж"), tsc("Клм"), tsc("Тел")},
             .mon = {tsc("Шорыкйол"), tsc("Пургыж"), tsc("Ӱярня"), tsc("Вӱдшор"), tsc("Ага"), tsc("Пеледыш"),
                     tsc("Сӱрем"), tsc("Сорла"), tsc("Идым"), tsc("Шыжа"), tsc("Кылме"), tsc("Теле")},
             .d_t_fmt = tsc("%A %Y %B %d %T"),
             .d_fmt = tsc("%Y.%m.%d"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc(""),
             .date_fmt = tsc("%A %Y %B %d %H:%M:%S %Z"),
             .am_pm = {tsc(""), tsc("")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1yYТт]"), .noexpr = tsc("^[-0nNУу]")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"), .int_select = tsc("0~10"), .int_prefix = tsc("7")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_ab2 = tsc("RU"),
                .country_ab3 = tsc("RUS"),
                .country_num = 643,
                .country_car = tsc("RUS"),
                .lang_name = tsc("марий йылме"),
                .lang_term = tsc("mhr"),
                .lang_lib = tsc("mhr")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"mhr_RU"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Mari locale for Russia"),
         .source = tsc(L"PeshSajSoft Ltd.\t\t;\t\tfast_io"),
         .address =
             tsc(L"112, Komsomolskaya Street, Yoshkar-Ola, Mari El Republic, "
                 L"Russia\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"Vyacheslav Kileev\t\t;\t\tfast_io"),
         .email = tsc(L"slavakileev@yandex.ru;euloanty@live.com"),
         .tel = tsc(L""),
         .language = tsc(L"Meadow Mari"),
         .territory = tsc(L"Russia"),
         .revision = tsc(L"0.2"),
         .date = tsc(L"2011-04-18")},
    .monetary = {.int_curr_symbol = tsc(L"RUB "),
                 .currency_symbol = tsc(L"₽"),
                 .mon_decimal_point = tsc(L"."),
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
    .numeric = {.decimal_point = tsc(L","), .thousands_sep = tsc(L" "), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(L"Ршр"), tsc(L"Шчм"), tsc(L"Кжм"), tsc(L"Вгч"), tsc(L"Изр"), tsc(L"Кгр"), tsc(L"Шмт")},
             .day = {tsc(L"Рушарня"), tsc(L"Шочмо"), tsc(L"Кушкыжмо"), tsc(L"Вӱргече"), tsc(L"Изарня"), tsc(L"Кугарня"),
                     tsc(L"Шуматкече")},
             .abmon = {tsc(L"Шрк"), tsc(L"Пгж"), tsc(L"Ӱрн"), tsc(L"Вшр"), tsc(L"Ага"), tsc(L"Пдш"), tsc(L"Срм"),
                       tsc(L"Срл"), tsc(L"Идм"), tsc(L"Шыж"), tsc(L"Клм"), tsc(L"Тел")},
             .mon = {tsc(L"Шорыкйол"), tsc(L"Пургыж"), tsc(L"Ӱярня"), tsc(L"Вӱдшор"), tsc(L"Ага"), tsc(L"Пеледыш"),
                     tsc(L"Сӱрем"), tsc(L"Сорла"), tsc(L"Идым"), tsc(L"Шыжа"), tsc(L"Кылме"), tsc(L"Теле")},
             .d_t_fmt = tsc(L"%A %Y %B %d %T"),
             .d_fmt = tsc(L"%Y.%m.%d"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L""),
             .date_fmt = tsc(L"%A %Y %B %d %H:%M:%S %Z"),
             .am_pm = {tsc(L""), tsc(L"")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1yYТт]"), .noexpr = tsc(L"^[-0nNУу]")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"), .int_select = tsc(L"0~10"), .int_prefix = tsc(L"7")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_ab2 = tsc(L"RU"),
                .country_ab3 = tsc(L"RUS"),
                .country_num = 643,
                .country_car = tsc(L"RUS"),
                .lang_name = tsc(L"марий йылме"),
                .lang_term = tsc(L"mhr"),
                .lang_lib = tsc(L"mhr")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"mhr_RU"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Mari locale for Russia"),
         .source = tsc(u8"PeshSajSoft Ltd.\t\t;\t\tfast_io"),
         .address = tsc(
             u8"112, Komsomolskaya Street, Yoshkar-Ola, Mari El Republic, "
             u8"Russia\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"Vyacheslav Kileev\t\t;\t\tfast_io"),
         .email = tsc(u8"slavakileev@yandex.ru;euloanty@live.com"),
         .tel = tsc(u8""),
         .language = tsc(u8"Meadow Mari"),
         .territory = tsc(u8"Russia"),
         .revision = tsc(u8"0.2"),
         .date = tsc(u8"2011-04-18")},
    .monetary = {.int_curr_symbol = tsc(u8"RUB "),
                 .currency_symbol = tsc(u8"₽"),
                 .mon_decimal_point = tsc(u8"."),
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
    .numeric = {.decimal_point = tsc(u8","), .thousands_sep = tsc(u8" "), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u8"Ршр"), tsc(u8"Шчм"), tsc(u8"Кжм"), tsc(u8"Вгч"), tsc(u8"Изр"), tsc(u8"Кгр"),
                       tsc(u8"Шмт")},
             .day = {tsc(u8"Рушарня"), tsc(u8"Шочмо"), tsc(u8"Кушкыжмо"), tsc(u8"Вӱргече"), tsc(u8"Изарня"),
                     tsc(u8"Кугарня"), tsc(u8"Шуматкече")},
             .abmon = {tsc(u8"Шрк"), tsc(u8"Пгж"), tsc(u8"Ӱрн"), tsc(u8"Вшр"), tsc(u8"Ага"), tsc(u8"Пдш"), tsc(u8"Срм"),
                       tsc(u8"Срл"), tsc(u8"Идм"), tsc(u8"Шыж"), tsc(u8"Клм"), tsc(u8"Тел")},
             .mon = {tsc(u8"Шорыкйол"), tsc(u8"Пургыж"), tsc(u8"Ӱярня"), tsc(u8"Вӱдшор"), tsc(u8"Ага"),
                     tsc(u8"Пеледыш"), tsc(u8"Сӱрем"), tsc(u8"Сорла"), tsc(u8"Идым"), tsc(u8"Шыжа"), tsc(u8"Кылме"),
                     tsc(u8"Теле")},
             .d_t_fmt = tsc(u8"%A %Y %B %d %T"),
             .d_fmt = tsc(u8"%Y.%m.%d"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8""),
             .date_fmt = tsc(u8"%A %Y %B %d %H:%M:%S %Z"),
             .am_pm = {tsc(u8""), tsc(u8"")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1yYТт]"), .noexpr = tsc(u8"^[-0nNУу]")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"), .int_select = tsc(u8"0~10"), .int_prefix = tsc(u8"7")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_ab2 = tsc(u8"RU"),
                .country_ab3 = tsc(u8"RUS"),
                .country_num = 643,
                .country_car = tsc(u8"RUS"),
                .lang_name = tsc(u8"марий йылме"),
                .lang_term = tsc(u8"mhr"),
                .lang_lib = tsc(u8"mhr")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"mhr_RU"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Mari locale for Russia"),
         .source = tsc(u"PeshSajSoft Ltd.\t\t;\t\tfast_io"),
         .address =
             tsc(u"112, Komsomolskaya Street, Yoshkar-Ola, Mari El Republic, "
                 u"Russia\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"Vyacheslav Kileev\t\t;\t\tfast_io"),
         .email = tsc(u"slavakileev@yandex.ru;euloanty@live.com"),
         .tel = tsc(u""),
         .language = tsc(u"Meadow Mari"),
         .territory = tsc(u"Russia"),
         .revision = tsc(u"0.2"),
         .date = tsc(u"2011-04-18")},
    .monetary = {.int_curr_symbol = tsc(u"RUB "),
                 .currency_symbol = tsc(u"₽"),
                 .mon_decimal_point = tsc(u"."),
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
    .numeric = {.decimal_point = tsc(u","), .thousands_sep = tsc(u" "), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(u"Ршр"), tsc(u"Шчм"), tsc(u"Кжм"), tsc(u"Вгч"), tsc(u"Изр"), tsc(u"Кгр"), tsc(u"Шмт")},
             .day = {tsc(u"Рушарня"), tsc(u"Шочмо"), tsc(u"Кушкыжмо"), tsc(u"Вӱргече"), tsc(u"Изарня"), tsc(u"Кугарня"),
                     tsc(u"Шуматкече")},
             .abmon = {tsc(u"Шрк"), tsc(u"Пгж"), tsc(u"Ӱрн"), tsc(u"Вшр"), tsc(u"Ага"), tsc(u"Пдш"), tsc(u"Срм"),
                       tsc(u"Срл"), tsc(u"Идм"), tsc(u"Шыж"), tsc(u"Клм"), tsc(u"Тел")},
             .mon = {tsc(u"Шорыкйол"), tsc(u"Пургыж"), tsc(u"Ӱярня"), tsc(u"Вӱдшор"), tsc(u"Ага"), tsc(u"Пеледыш"),
                     tsc(u"Сӱрем"), tsc(u"Сорла"), tsc(u"Идым"), tsc(u"Шыжа"), tsc(u"Кылме"), tsc(u"Теле")},
             .d_t_fmt = tsc(u"%A %Y %B %d %T"),
             .d_fmt = tsc(u"%Y.%m.%d"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u""),
             .date_fmt = tsc(u"%A %Y %B %d %H:%M:%S %Z"),
             .am_pm = {tsc(u""), tsc(u"")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1yYТт]"), .noexpr = tsc(u"^[-0nNУу]")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"), .int_select = tsc(u"0~10"), .int_prefix = tsc(u"7")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_ab2 = tsc(u"RU"),
                .country_ab3 = tsc(u"RUS"),
                .country_num = 643,
                .country_car = tsc(u"RUS"),
                .lang_name = tsc(u"марий йылме"),
                .lang_term = tsc(u"mhr"),
                .lang_lib = tsc(u"mhr")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"mhr_RU"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Mari locale for Russia"),
         .source = tsc(U"PeshSajSoft Ltd.\t\t;\t\tfast_io"),
         .address =
             tsc(U"112, Komsomolskaya Street, Yoshkar-Ola, Mari El Republic, "
                 U"Russia\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"Vyacheslav Kileev\t\t;\t\tfast_io"),
         .email = tsc(U"slavakileev@yandex.ru;euloanty@live.com"),
         .tel = tsc(U""),
         .language = tsc(U"Meadow Mari"),
         .territory = tsc(U"Russia"),
         .revision = tsc(U"0.2"),
         .date = tsc(U"2011-04-18")},
    .monetary = {.int_curr_symbol = tsc(U"RUB "),
                 .currency_symbol = tsc(U"₽"),
                 .mon_decimal_point = tsc(U"."),
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
    .numeric = {.decimal_point = tsc(U","), .thousands_sep = tsc(U" "), .grouping = {numeric_grouping_storage, 1}},
    .time = {.abday = {tsc(U"Ршр"), tsc(U"Шчм"), tsc(U"Кжм"), tsc(U"Вгч"), tsc(U"Изр"), tsc(U"Кгр"), tsc(U"Шмт")},
             .day = {tsc(U"Рушарня"), tsc(U"Шочмо"), tsc(U"Кушкыжмо"), tsc(U"Вӱргече"), tsc(U"Изарня"), tsc(U"Кугарня"),
                     tsc(U"Шуматкече")},
             .abmon = {tsc(U"Шрк"), tsc(U"Пгж"), tsc(U"Ӱрн"), tsc(U"Вшр"), tsc(U"Ага"), tsc(U"Пдш"), tsc(U"Срм"),
                       tsc(U"Срл"), tsc(U"Идм"), tsc(U"Шыж"), tsc(U"Клм"), tsc(U"Тел")},
             .mon = {tsc(U"Шорыкйол"), tsc(U"Пургыж"), tsc(U"Ӱярня"), tsc(U"Вӱдшор"), tsc(U"Ага"), tsc(U"Пеледыш"),
                     tsc(U"Сӱрем"), tsc(U"Сорла"), tsc(U"Идым"), tsc(U"Шыжа"), tsc(U"Кылме"), tsc(U"Теле")},
             .d_t_fmt = tsc(U"%A %Y %B %d %T"),
             .d_fmt = tsc(U"%Y.%m.%d"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U""),
             .date_fmt = tsc(U"%A %Y %B %d %H:%M:%S %Z"),
             .am_pm = {tsc(U""), tsc(U"")},
             .week = {7, 19971130, 1},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1yYТт]"), .noexpr = tsc(U"^[-0nNУу]")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"), .int_select = tsc(U"0~10"), .int_prefix = tsc(U"7")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f")},
    .address = {.postal_fmt = tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),
                .country_ab2 = tsc(U"RU"),
                .country_ab3 = tsc(U"RUS"),
                .country_num = 643,
                .country_car = tsc(U"RUS"),
                .lang_name = tsc(U"марий йылме"),
                .lang_term = tsc(U"mhr"),
                .lang_lib = tsc(U"mhr")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"