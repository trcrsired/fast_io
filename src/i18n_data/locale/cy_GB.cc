﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification = {.name = tsc("cy_GB"),
                       .encoding = tsc(FAST_IO_LOCALE_ENCODING),
                       .title = tsc("Welsh language locale for Great Britain"),
                       .source = tsc("thanks to Dafydd Tomos (dafydd@imaginet.co.uk)\t\t;\t\tfast_io"),
                       .address =
                           tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc("Pablo Saratxaga\t\t;\t\tfast_io"),
                       .email = tsc("pablo@mandrakesoft.com;euloanty@live.com"),
                       .tel = tsc(""),
                       .fax = tsc(""),
                       .language = tsc("Welsh"),
                       .territory = tsc("United Kingdom"),
                       .revision = tsc("0.9"),
                       .date = tsc("2004-09-27")},
    .monetary = {.int_curr_symbol = tsc("GBP "),
                 .currency_symbol = tsc("£"),
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
    .time = {.abday = {tsc("Sul"), tsc("Llu"), tsc("Maw"), tsc("Mer"), tsc("Iau"), tsc("Gwe"), tsc("Sad")},
             .day = {tsc("Sul"), tsc("Llun"), tsc("Mawrth"), tsc("Mercher"), tsc("Iau"), tsc("Gwener"), tsc("Sadwrn")},
             .abmon = {tsc("Ion"), tsc("Chw"), tsc("Maw"), tsc("Ebr"), tsc("Mai"), tsc("Meh"), tsc("Gor"), tsc("Aws"),
                       tsc("Med"), tsc("Hyd"), tsc("Tach"), tsc("Rha")},
             .mon = {tsc("Ionawr"), tsc("Chwefror"), tsc("Mawrth"), tsc("Ebrill"), tsc("Mai"), tsc("Mehefin"),
                     tsc("Gorffennaf"), tsc("Awst"), tsc("Medi"), tsc("Hydref"), tsc("Tachwedd"), tsc("Rhagfyr")},
             .d_t_fmt = tsc("Dydd %A %d mis %B %Y %T %Z"),
             .d_fmt = tsc("%d.%m.%y"),
             .t_fmt = tsc("%T"),
             .t_fmt_ampm = tsc("%l:%M:%S %P %Z"),
             .date_fmt = tsc("%a %e %b %H:%M:%S %Z %Y"),
             .am_pm = {tsc("am"), tsc("pm")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc("^[+1iItTyY]"), .noexpr = tsc("^[-0nN]"), .yesstr = tsc("ie"), .nostr = tsc("na")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("+%c %a %l"),
                  .tel_dom_fmt = tsc("%A %l"),
                  .int_select = tsc("00"),
                  .int_prefix = tsc("44")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f"),
             .name_miss = tsc("Miss."),
             .name_mr = tsc("Mr."),
             .name_mrs = tsc("Mrs."),
             .name_ms = tsc("Ms.")},
    .address = {.postal_fmt = tsc("%d%N%f%N%d%N%b%N%s %h 5e %r%N%C%z %T%N%c%N"),
                .country_name = tsc("Y Deyrnas Unedig"),
                .country_post = tsc("GB"),
                .country_ab2 = tsc("GB"),
                .country_ab3 = tsc("GBR"),
                .country_num = 826,
                .country_car = tsc("GB"),
                .country_isbn = tsc("0"),
                .lang_name = tsc("Cymraeg"),
                .lang_ab = tsc("cy"),
                .lang_term = tsc("cym"),
                .lang_lib = tsc("wel")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification = {.name = tsc(L"cy_GB"),
                       .encoding = tsc(FAST_IO_LOCALE_LENCODING),
                       .title = tsc(L"Welsh language locale for Great Britain"),
                       .source = tsc(L"thanks to Dafydd Tomos (dafydd@imaginet.co.uk)\t\t;\t\tfast_io"),
                       .address =
                           tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(L"Pablo Saratxaga\t\t;\t\tfast_io"),
                       .email = tsc(L"pablo@mandrakesoft.com;euloanty@live.com"),
                       .tel = tsc(L""),
                       .fax = tsc(L""),
                       .language = tsc(L"Welsh"),
                       .territory = tsc(L"United Kingdom"),
                       .revision = tsc(L"0.9"),
                       .date = tsc(L"2004-09-27")},
    .monetary = {.int_curr_symbol = tsc(L"GBP "),
                 .currency_symbol = tsc(L"£"),
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
    .time = {.abday = {tsc(L"Sul"), tsc(L"Llu"), tsc(L"Maw"), tsc(L"Mer"), tsc(L"Iau"), tsc(L"Gwe"), tsc(L"Sad")},
             .day = {tsc(L"Sul"), tsc(L"Llun"), tsc(L"Mawrth"), tsc(L"Mercher"), tsc(L"Iau"), tsc(L"Gwener"),
                     tsc(L"Sadwrn")},
             .abmon = {tsc(L"Ion"), tsc(L"Chw"), tsc(L"Maw"), tsc(L"Ebr"), tsc(L"Mai"), tsc(L"Meh"), tsc(L"Gor"),
                       tsc(L"Aws"), tsc(L"Med"), tsc(L"Hyd"), tsc(L"Tach"), tsc(L"Rha")},
             .mon = {tsc(L"Ionawr"), tsc(L"Chwefror"), tsc(L"Mawrth"), tsc(L"Ebrill"), tsc(L"Mai"), tsc(L"Mehefin"),
                     tsc(L"Gorffennaf"), tsc(L"Awst"), tsc(L"Medi"), tsc(L"Hydref"), tsc(L"Tachwedd"), tsc(L"Rhagfyr")},
             .d_t_fmt = tsc(L"Dydd %A %d mis %B %Y %T %Z"),
             .d_fmt = tsc(L"%d.%m.%y"),
             .t_fmt = tsc(L"%T"),
             .t_fmt_ampm = tsc(L"%l:%M:%S %P %Z"),
             .date_fmt = tsc(L"%a %e %b %H:%M:%S %Z %Y"),
             .am_pm = {tsc(L"am"), tsc(L"pm")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(L"^[+1iItTyY]"), .noexpr = tsc(L"^[-0nN]"), .yesstr = tsc(L"ie"), .nostr = tsc(L"na")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"+%c %a %l"),
                  .tel_dom_fmt = tsc(L"%A %l"),
                  .int_select = tsc(L"00"),
                  .int_prefix = tsc(L"44")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f"),
             .name_miss = tsc(L"Miss."),
             .name_mr = tsc(L"Mr."),
             .name_mrs = tsc(L"Mrs."),
             .name_ms = tsc(L"Ms.")},
    .address = {.postal_fmt = tsc(L"%d%N%f%N%d%N%b%N%s %h 5e %r%N%C%z %T%N%c%N"),
                .country_name = tsc(L"Y Deyrnas Unedig"),
                .country_post = tsc(L"GB"),
                .country_ab2 = tsc(L"GB"),
                .country_ab3 = tsc(L"GBR"),
                .country_num = 826,
                .country_car = tsc(L"GB"),
                .country_isbn = tsc(L"0"),
                .lang_name = tsc(L"Cymraeg"),
                .lang_ab = tsc(L"cy"),
                .lang_term = tsc(L"cym"),
                .lang_lib = tsc(L"wel")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification = {.name = tsc(u8"cy_GB"),
                       .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
                       .title = tsc(u8"Welsh language locale for Great Britain"),
                       .source = tsc(u8"thanks to Dafydd Tomos (dafydd@imaginet.co.uk)\t\t;\t\tfast_io"),
                       .address =
                           tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u8"Pablo Saratxaga\t\t;\t\tfast_io"),
                       .email = tsc(u8"pablo@mandrakesoft.com;euloanty@live.com"),
                       .tel = tsc(u8""),
                       .fax = tsc(u8""),
                       .language = tsc(u8"Welsh"),
                       .territory = tsc(u8"United Kingdom"),
                       .revision = tsc(u8"0.9"),
                       .date = tsc(u8"2004-09-27")},
    .monetary = {.int_curr_symbol = tsc(u8"GBP "),
                 .currency_symbol = tsc(u8"£"),
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
    .time = {.abday = {tsc(u8"Sul"), tsc(u8"Llu"), tsc(u8"Maw"), tsc(u8"Mer"), tsc(u8"Iau"), tsc(u8"Gwe"),
                       tsc(u8"Sad")},
             .day = {tsc(u8"Sul"), tsc(u8"Llun"), tsc(u8"Mawrth"), tsc(u8"Mercher"), tsc(u8"Iau"), tsc(u8"Gwener"),
                     tsc(u8"Sadwrn")},
             .abmon = {tsc(u8"Ion"), tsc(u8"Chw"), tsc(u8"Maw"), tsc(u8"Ebr"), tsc(u8"Mai"), tsc(u8"Meh"), tsc(u8"Gor"),
                       tsc(u8"Aws"), tsc(u8"Med"), tsc(u8"Hyd"), tsc(u8"Tach"), tsc(u8"Rha")},
             .mon = {tsc(u8"Ionawr"), tsc(u8"Chwefror"), tsc(u8"Mawrth"), tsc(u8"Ebrill"), tsc(u8"Mai"),
                     tsc(u8"Mehefin"), tsc(u8"Gorffennaf"), tsc(u8"Awst"), tsc(u8"Medi"), tsc(u8"Hydref"),
                     tsc(u8"Tachwedd"), tsc(u8"Rhagfyr")},
             .d_t_fmt = tsc(u8"Dydd %A %d mis %B %Y %T %Z"),
             .d_fmt = tsc(u8"%d.%m.%y"),
             .t_fmt = tsc(u8"%T"),
             .t_fmt_ampm = tsc(u8"%l:%M:%S %P %Z"),
             .date_fmt = tsc(u8"%a %e %b %H:%M:%S %Z %Y"),
             .am_pm = {tsc(u8"am"), tsc(u8"pm")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u8"^[+1iItTyY]"),
                 .noexpr = tsc(u8"^[-0nN]"),
                 .yesstr = tsc(u8"ie"),
                 .nostr = tsc(u8"na")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"+%c %a %l"),
                  .tel_dom_fmt = tsc(u8"%A %l"),
                  .int_select = tsc(u8"00"),
                  .int_prefix = tsc(u8"44")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f"),
             .name_miss = tsc(u8"Miss."),
             .name_mr = tsc(u8"Mr."),
             .name_mrs = tsc(u8"Mrs."),
             .name_ms = tsc(u8"Ms.")},
    .address = {.postal_fmt = tsc(u8"%d%N%f%N%d%N%b%N%s %h 5e %r%N%C%z %T%N%c%N"),
                .country_name = tsc(u8"Y Deyrnas Unedig"),
                .country_post = tsc(u8"GB"),
                .country_ab2 = tsc(u8"GB"),
                .country_ab3 = tsc(u8"GBR"),
                .country_num = 826,
                .country_car = tsc(u8"GB"),
                .country_isbn = tsc(u8"0"),
                .lang_name = tsc(u8"Cymraeg"),
                .lang_ab = tsc(u8"cy"),
                .lang_term = tsc(u8"cym"),
                .lang_lib = tsc(u8"wel")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification = {.name = tsc(u"cy_GB"),
                       .encoding = tsc(FAST_IO_LOCALE_uENCODING),
                       .title = tsc(u"Welsh language locale for Great Britain"),
                       .source = tsc(u"thanks to Dafydd Tomos (dafydd@imaginet.co.uk)\t\t;\t\tfast_io"),
                       .address =
                           tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(u"Pablo Saratxaga\t\t;\t\tfast_io"),
                       .email = tsc(u"pablo@mandrakesoft.com;euloanty@live.com"),
                       .tel = tsc(u""),
                       .fax = tsc(u""),
                       .language = tsc(u"Welsh"),
                       .territory = tsc(u"United Kingdom"),
                       .revision = tsc(u"0.9"),
                       .date = tsc(u"2004-09-27")},
    .monetary = {.int_curr_symbol = tsc(u"GBP "),
                 .currency_symbol = tsc(u"£"),
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
    .time = {.abday = {tsc(u"Sul"), tsc(u"Llu"), tsc(u"Maw"), tsc(u"Mer"), tsc(u"Iau"), tsc(u"Gwe"), tsc(u"Sad")},
             .day = {tsc(u"Sul"), tsc(u"Llun"), tsc(u"Mawrth"), tsc(u"Mercher"), tsc(u"Iau"), tsc(u"Gwener"),
                     tsc(u"Sadwrn")},
             .abmon = {tsc(u"Ion"), tsc(u"Chw"), tsc(u"Maw"), tsc(u"Ebr"), tsc(u"Mai"), tsc(u"Meh"), tsc(u"Gor"),
                       tsc(u"Aws"), tsc(u"Med"), tsc(u"Hyd"), tsc(u"Tach"), tsc(u"Rha")},
             .mon = {tsc(u"Ionawr"), tsc(u"Chwefror"), tsc(u"Mawrth"), tsc(u"Ebrill"), tsc(u"Mai"), tsc(u"Mehefin"),
                     tsc(u"Gorffennaf"), tsc(u"Awst"), tsc(u"Medi"), tsc(u"Hydref"), tsc(u"Tachwedd"), tsc(u"Rhagfyr")},
             .d_t_fmt = tsc(u"Dydd %A %d mis %B %Y %T %Z"),
             .d_fmt = tsc(u"%d.%m.%y"),
             .t_fmt = tsc(u"%T"),
             .t_fmt_ampm = tsc(u"%l:%M:%S %P %Z"),
             .date_fmt = tsc(u"%a %e %b %H:%M:%S %Z %Y"),
             .am_pm = {tsc(u"am"), tsc(u"pm")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(u"^[+1iItTyY]"), .noexpr = tsc(u"^[-0nN]"), .yesstr = tsc(u"ie"), .nostr = tsc(u"na")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"+%c %a %l"),
                  .tel_dom_fmt = tsc(u"%A %l"),
                  .int_select = tsc(u"00"),
                  .int_prefix = tsc(u"44")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f"),
             .name_miss = tsc(u"Miss."),
             .name_mr = tsc(u"Mr."),
             .name_mrs = tsc(u"Mrs."),
             .name_ms = tsc(u"Ms.")},
    .address = {.postal_fmt = tsc(u"%d%N%f%N%d%N%b%N%s %h 5e %r%N%C%z %T%N%c%N"),
                .country_name = tsc(u"Y Deyrnas Unedig"),
                .country_post = tsc(u"GB"),
                .country_ab2 = tsc(u"GB"),
                .country_ab3 = tsc(u"GBR"),
                .country_num = 826,
                .country_car = tsc(u"GB"),
                .country_isbn = tsc(u"0"),
                .lang_name = tsc(u"Cymraeg"),
                .lang_ab = tsc(u"cy"),
                .lang_term = tsc(u"cym"),
                .lang_lib = tsc(u"wel")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification = {.name = tsc(U"cy_GB"),
                       .encoding = tsc(FAST_IO_LOCALE_UENCODING),
                       .title = tsc(U"Welsh language locale for Great Britain"),
                       .source = tsc(U"thanks to Dafydd Tomos (dafydd@imaginet.co.uk)\t\t;\t\tfast_io"),
                       .address =
                           tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
                       .contact = tsc(U"Pablo Saratxaga\t\t;\t\tfast_io"),
                       .email = tsc(U"pablo@mandrakesoft.com;euloanty@live.com"),
                       .tel = tsc(U""),
                       .fax = tsc(U""),
                       .language = tsc(U"Welsh"),
                       .territory = tsc(U"United Kingdom"),
                       .revision = tsc(U"0.9"),
                       .date = tsc(U"2004-09-27")},
    .monetary = {.int_curr_symbol = tsc(U"GBP "),
                 .currency_symbol = tsc(U"£"),
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
    .time = {.abday = {tsc(U"Sul"), tsc(U"Llu"), tsc(U"Maw"), tsc(U"Mer"), tsc(U"Iau"), tsc(U"Gwe"), tsc(U"Sad")},
             .day = {tsc(U"Sul"), tsc(U"Llun"), tsc(U"Mawrth"), tsc(U"Mercher"), tsc(U"Iau"), tsc(U"Gwener"),
                     tsc(U"Sadwrn")},
             .abmon = {tsc(U"Ion"), tsc(U"Chw"), tsc(U"Maw"), tsc(U"Ebr"), tsc(U"Mai"), tsc(U"Meh"), tsc(U"Gor"),
                       tsc(U"Aws"), tsc(U"Med"), tsc(U"Hyd"), tsc(U"Tach"), tsc(U"Rha")},
             .mon = {tsc(U"Ionawr"), tsc(U"Chwefror"), tsc(U"Mawrth"), tsc(U"Ebrill"), tsc(U"Mai"), tsc(U"Mehefin"),
                     tsc(U"Gorffennaf"), tsc(U"Awst"), tsc(U"Medi"), tsc(U"Hydref"), tsc(U"Tachwedd"), tsc(U"Rhagfyr")},
             .d_t_fmt = tsc(U"Dydd %A %d mis %B %Y %T %Z"),
             .d_fmt = tsc(U"%d.%m.%y"),
             .t_fmt = tsc(U"%T"),
             .t_fmt_ampm = tsc(U"%l:%M:%S %P %Z"),
             .date_fmt = tsc(U"%a %e %b %H:%M:%S %Z %Y"),
             .am_pm = {tsc(U"am"), tsc(U"pm")},
             .week = {7, 19971130, 4},
             .first_weekday = 2},
    .messages = {.yesexpr = tsc(U"^[+1iItTyY]"), .noexpr = tsc(U"^[-0nN]"), .yesstr = tsc(U"ie"), .nostr = tsc(U"na")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"+%c %a %l"),
                  .tel_dom_fmt = tsc(U"%A %l"),
                  .int_select = tsc(U"00"),
                  .int_prefix = tsc(U"44")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f"),
             .name_miss = tsc(U"Miss."),
             .name_mr = tsc(U"Mr."),
             .name_mrs = tsc(U"Mrs."),
             .name_ms = tsc(U"Ms.")},
    .address = {.postal_fmt = tsc(U"%d%N%f%N%d%N%b%N%s %h 5e %r%N%C%z %T%N%c%N"),
                .country_name = tsc(U"Y Deyrnas Unedig"),
                .country_post = tsc(U"GB"),
                .country_ab2 = tsc(U"GB"),
                .country_ab3 = tsc(U"GBR"),
                .country_num = 826,
                .country_car = tsc(U"GB"),
                .country_isbn = tsc(U"0"),
                .lang_name = tsc(U"Cymraeg"),
                .lang_ab = tsc(U"cy"),
                .lang_term = tsc(U"cym"),
                .lang_lib = tsc(U"wel")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"