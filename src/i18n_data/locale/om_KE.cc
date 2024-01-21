﻿#include "../localedef.h"

namespace fast_io_i18n {
namespace {

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{
    .identification =
        {.name = tsc("om_KE"),
         .encoding = tsc(FAST_IO_LOCALE_ENCODING),
         .title = tsc("Oromo language locale for Kenya."),
         .source = tsc("Ge'ez Frontier Foundation & Sagalee Oromoo Publishing Co. Inc.\t\t;\t\tfast_io"),
         .address =
             tsc("7802 Solomon Seal Dr., Springfield, VA 22152, "
                 "USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc("fast_io"),
         .email = tsc("locales@geez.org;euloanty@live.com"),
         .tel = tsc(""),
         .fax = tsc(""),
         .language = tsc("Oromo"),
         .territory = tsc("Kenya"),
         .revision = tsc("0.20"),
         .date = tsc("2003-07-05")},
    .monetary = {.int_curr_symbol = tsc("KES "),
                 .currency_symbol = tsc("Ksh"),
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
    .time = {.abday = {tsc("Dil"), tsc("Wix"), tsc("Qib"), tsc("Rob"), tsc("Kam"), tsc("Jim"), tsc("San")},
             .day = {tsc("Dilbata"), tsc("Wiixata"), tsc("Qibxata"), tsc("Roobii"), tsc("Kamiisa"), tsc("Jimaata"),
                     tsc("Sanbata")},
             .abmon = {tsc("Ama"), tsc("Gur"), tsc("Bit"), tsc("Elb"), tsc("Cam"), tsc("Wax"), tsc("Ado"), tsc("Hag"),
                       tsc("Ful"), tsc("Onk"), tsc("Sad"), tsc("Mud")},
             .mon = {tsc("Amajjii"), tsc("Guraandhala"), tsc("Bitooteessa"), tsc("Elba"), tsc("Caamsa"),
                     tsc("Waxabajjii"), tsc("Adooleessa"), tsc("Hagayya"), tsc("Fuulbana"), tsc("Onkololeessa"),
                     tsc("Sadaasa"), tsc("Muddee")},
             .d_t_fmt = tsc("%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc("%d//%m//%Y"),
             .t_fmt = tsc("%l:%M:%S %p"),
             .t_fmt_ampm = tsc("%l:%M:%S %p"),
             .date_fmt = tsc("%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc("WD"), tsc("WB")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc("^[+1eEyY]"),
                 .noexpr = tsc("^[-0mMnN]"),
                 .yesstr = tsc("eeyyee"),
                 .nostr = tsc("miti")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc("%c-%a-%l"),
                  .tel_dom_fmt = tsc("%a-%l"),
                  .int_select = tsc("000"),
                  .int_prefix = tsc("254")},
    .name = {.name_fmt = tsc("%d%t%g%t%m%t%f"),
             .name_gen = tsc(""),
             .name_miss = tsc("Du"),
             .name_mr = tsc("Ob"),
             .name_mrs = tsc("Ad"),
             .name_ms = tsc("")},
    .address = {.postal_fmt = tsc("%z%c%T%s%b%e%r"),
                .country_name = tsc("Keeniyaa"),
                .country_post = tsc("KEN"),
                .country_ab2 = tsc("KE"),
                .country_ab3 = tsc("KEN"),
                .country_num = 404,
                .country_car = tsc("EAK"),
                .lang_name = tsc("Oromoo"),
                .lang_ab = tsc("om"),
                .lang_term = tsc("orm"),
                .lang_lib = tsc("orm")},
    .measurement = {.measurement = 1}};

inline constexpr wlc_all wlc_all_global{
    .identification =
        {.name = tsc(L"om_KE"),
         .encoding = tsc(FAST_IO_LOCALE_LENCODING),
         .title = tsc(L"Oromo language locale for Kenya."),
         .source = tsc(L"Ge'ez Frontier Foundation & Sagalee Oromoo Publishing Co. Inc.\t\t;\t\tfast_io"),
         .address =
             tsc(L"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 L"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(L"fast_io"),
         .email = tsc(L"locales@geez.org;euloanty@live.com"),
         .tel = tsc(L""),
         .fax = tsc(L""),
         .language = tsc(L"Oromo"),
         .territory = tsc(L"Kenya"),
         .revision = tsc(L"0.20"),
         .date = tsc(L"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(L"KES "),
                 .currency_symbol = tsc(L"Ksh"),
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
    .time = {.abday = {tsc(L"Dil"), tsc(L"Wix"), tsc(L"Qib"), tsc(L"Rob"), tsc(L"Kam"), tsc(L"Jim"), tsc(L"San")},
             .day = {tsc(L"Dilbata"), tsc(L"Wiixata"), tsc(L"Qibxata"), tsc(L"Roobii"), tsc(L"Kamiisa"),
                     tsc(L"Jimaata"), tsc(L"Sanbata")},
             .abmon = {tsc(L"Ama"), tsc(L"Gur"), tsc(L"Bit"), tsc(L"Elb"), tsc(L"Cam"), tsc(L"Wax"), tsc(L"Ado"),
                       tsc(L"Hag"), tsc(L"Ful"), tsc(L"Onk"), tsc(L"Sad"), tsc(L"Mud")},
             .mon = {tsc(L"Amajjii"), tsc(L"Guraandhala"), tsc(L"Bitooteessa"), tsc(L"Elba"), tsc(L"Caamsa"),
                     tsc(L"Waxabajjii"), tsc(L"Adooleessa"), tsc(L"Hagayya"), tsc(L"Fuulbana"), tsc(L"Onkololeessa"),
                     tsc(L"Sadaasa"), tsc(L"Muddee")},
             .d_t_fmt = tsc(L"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(L"%d//%m//%Y"),
             .t_fmt = tsc(L"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(L"%l:%M:%S %p"),
             .date_fmt = tsc(L"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(L"WD"), tsc(L"WB")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(L"^[+1eEyY]"),
                 .noexpr = tsc(L"^[-0mMnN]"),
                 .yesstr = tsc(L"eeyyee"),
                 .nostr = tsc(L"miti")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(L"%c-%a-%l"),
                  .tel_dom_fmt = tsc(L"%a-%l"),
                  .int_select = tsc(L"000"),
                  .int_prefix = tsc(L"254")},
    .name = {.name_fmt = tsc(L"%d%t%g%t%m%t%f"),
             .name_gen = tsc(L""),
             .name_miss = tsc(L"Du"),
             .name_mr = tsc(L"Ob"),
             .name_mrs = tsc(L"Ad"),
             .name_ms = tsc(L"")},
    .address = {.postal_fmt = tsc(L"%z%c%T%s%b%e%r"),
                .country_name = tsc(L"Keeniyaa"),
                .country_post = tsc(L"KEN"),
                .country_ab2 = tsc(L"KE"),
                .country_ab3 = tsc(L"KEN"),
                .country_num = 404,
                .country_car = tsc(L"EAK"),
                .lang_name = tsc(L"Oromoo"),
                .lang_ab = tsc(L"om"),
                .lang_term = tsc(L"orm"),
                .lang_lib = tsc(L"orm")},
    .measurement = {.measurement = 1}};

inline constexpr u8lc_all u8lc_all_global{
    .identification =
        {.name = tsc(u8"om_KE"),
         .encoding = tsc(FAST_IO_LOCALE_u8ENCODING),
         .title = tsc(u8"Oromo language locale for Kenya."),
         .source = tsc(u8"Ge'ez Frontier Foundation & Sagalee Oromoo Publishing Co. Inc.\t\t;\t\tfast_io"),
         .address =
             tsc(u8"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 u8"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u8"fast_io"),
         .email = tsc(u8"locales@geez.org;euloanty@live.com"),
         .tel = tsc(u8""),
         .fax = tsc(u8""),
         .language = tsc(u8"Oromo"),
         .territory = tsc(u8"Kenya"),
         .revision = tsc(u8"0.20"),
         .date = tsc(u8"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(u8"KES "),
                 .currency_symbol = tsc(u8"Ksh"),
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
    .time = {.abday = {tsc(u8"Dil"), tsc(u8"Wix"), tsc(u8"Qib"), tsc(u8"Rob"), tsc(u8"Kam"), tsc(u8"Jim"),
                       tsc(u8"San")},
             .day = {tsc(u8"Dilbata"), tsc(u8"Wiixata"), tsc(u8"Qibxata"), tsc(u8"Roobii"), tsc(u8"Kamiisa"),
                     tsc(u8"Jimaata"), tsc(u8"Sanbata")},
             .abmon = {tsc(u8"Ama"), tsc(u8"Gur"), tsc(u8"Bit"), tsc(u8"Elb"), tsc(u8"Cam"), tsc(u8"Wax"), tsc(u8"Ado"),
                       tsc(u8"Hag"), tsc(u8"Ful"), tsc(u8"Onk"), tsc(u8"Sad"), tsc(u8"Mud")},
             .mon = {tsc(u8"Amajjii"), tsc(u8"Guraandhala"), tsc(u8"Bitooteessa"), tsc(u8"Elba"), tsc(u8"Caamsa"),
                     tsc(u8"Waxabajjii"), tsc(u8"Adooleessa"), tsc(u8"Hagayya"), tsc(u8"Fuulbana"),
                     tsc(u8"Onkololeessa"), tsc(u8"Sadaasa"), tsc(u8"Muddee")},
             .d_t_fmt = tsc(u8"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(u8"%d//%m//%Y"),
             .t_fmt = tsc(u8"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(u8"%l:%M:%S %p"),
             .date_fmt = tsc(u8"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(u8"WD"), tsc(u8"WB")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u8"^[+1eEyY]"),
                 .noexpr = tsc(u8"^[-0mMnN]"),
                 .yesstr = tsc(u8"eeyyee"),
                 .nostr = tsc(u8"miti")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u8"%c-%a-%l"),
                  .tel_dom_fmt = tsc(u8"%a-%l"),
                  .int_select = tsc(u8"000"),
                  .int_prefix = tsc(u8"254")},
    .name = {.name_fmt = tsc(u8"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u8""),
             .name_miss = tsc(u8"Du"),
             .name_mr = tsc(u8"Ob"),
             .name_mrs = tsc(u8"Ad"),
             .name_ms = tsc(u8"")},
    .address = {.postal_fmt = tsc(u8"%z%c%T%s%b%e%r"),
                .country_name = tsc(u8"Keeniyaa"),
                .country_post = tsc(u8"KEN"),
                .country_ab2 = tsc(u8"KE"),
                .country_ab3 = tsc(u8"KEN"),
                .country_num = 404,
                .country_car = tsc(u8"EAK"),
                .lang_name = tsc(u8"Oromoo"),
                .lang_ab = tsc(u8"om"),
                .lang_term = tsc(u8"orm"),
                .lang_lib = tsc(u8"orm")},
    .measurement = {.measurement = 1}};

inline constexpr u16lc_all u16lc_all_global{
    .identification =
        {.name = tsc(u"om_KE"),
         .encoding = tsc(FAST_IO_LOCALE_uENCODING),
         .title = tsc(u"Oromo language locale for Kenya."),
         .source = tsc(u"Ge'ez Frontier Foundation & Sagalee Oromoo Publishing Co. Inc.\t\t;\t\tfast_io"),
         .address =
             tsc(u"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 u"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(u"fast_io"),
         .email = tsc(u"locales@geez.org;euloanty@live.com"),
         .tel = tsc(u""),
         .fax = tsc(u""),
         .language = tsc(u"Oromo"),
         .territory = tsc(u"Kenya"),
         .revision = tsc(u"0.20"),
         .date = tsc(u"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(u"KES "),
                 .currency_symbol = tsc(u"Ksh"),
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
    .time = {.abday = {tsc(u"Dil"), tsc(u"Wix"), tsc(u"Qib"), tsc(u"Rob"), tsc(u"Kam"), tsc(u"Jim"), tsc(u"San")},
             .day = {tsc(u"Dilbata"), tsc(u"Wiixata"), tsc(u"Qibxata"), tsc(u"Roobii"), tsc(u"Kamiisa"),
                     tsc(u"Jimaata"), tsc(u"Sanbata")},
             .abmon = {tsc(u"Ama"), tsc(u"Gur"), tsc(u"Bit"), tsc(u"Elb"), tsc(u"Cam"), tsc(u"Wax"), tsc(u"Ado"),
                       tsc(u"Hag"), tsc(u"Ful"), tsc(u"Onk"), tsc(u"Sad"), tsc(u"Mud")},
             .mon = {tsc(u"Amajjii"), tsc(u"Guraandhala"), tsc(u"Bitooteessa"), tsc(u"Elba"), tsc(u"Caamsa"),
                     tsc(u"Waxabajjii"), tsc(u"Adooleessa"), tsc(u"Hagayya"), tsc(u"Fuulbana"), tsc(u"Onkololeessa"),
                     tsc(u"Sadaasa"), tsc(u"Muddee")},
             .d_t_fmt = tsc(u"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(u"%d//%m//%Y"),
             .t_fmt = tsc(u"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(u"%l:%M:%S %p"),
             .date_fmt = tsc(u"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(u"WD"), tsc(u"WB")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(u"^[+1eEyY]"),
                 .noexpr = tsc(u"^[-0mMnN]"),
                 .yesstr = tsc(u"eeyyee"),
                 .nostr = tsc(u"miti")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(u"%c-%a-%l"),
                  .tel_dom_fmt = tsc(u"%a-%l"),
                  .int_select = tsc(u"000"),
                  .int_prefix = tsc(u"254")},
    .name = {.name_fmt = tsc(u"%d%t%g%t%m%t%f"),
             .name_gen = tsc(u""),
             .name_miss = tsc(u"Du"),
             .name_mr = tsc(u"Ob"),
             .name_mrs = tsc(u"Ad"),
             .name_ms = tsc(u"")},
    .address = {.postal_fmt = tsc(u"%z%c%T%s%b%e%r"),
                .country_name = tsc(u"Keeniyaa"),
                .country_post = tsc(u"KEN"),
                .country_ab2 = tsc(u"KE"),
                .country_ab3 = tsc(u"KEN"),
                .country_num = 404,
                .country_car = tsc(u"EAK"),
                .lang_name = tsc(u"Oromoo"),
                .lang_ab = tsc(u"om"),
                .lang_term = tsc(u"orm"),
                .lang_lib = tsc(u"orm")},
    .measurement = {.measurement = 1}};

inline constexpr u32lc_all u32lc_all_global{
    .identification =
        {.name = tsc(U"om_KE"),
         .encoding = tsc(FAST_IO_LOCALE_UENCODING),
         .title = tsc(U"Oromo language locale for Kenya."),
         .source = tsc(U"Ge'ez Frontier Foundation & Sagalee Oromoo Publishing Co. Inc.\t\t;\t\tfast_io"),
         .address =
             tsc(U"7802 Solomon Seal Dr., Springfield, VA 22152, "
                 U"USA\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),
         .contact = tsc(U"fast_io"),
         .email = tsc(U"locales@geez.org;euloanty@live.com"),
         .tel = tsc(U""),
         .fax = tsc(U""),
         .language = tsc(U"Oromo"),
         .territory = tsc(U"Kenya"),
         .revision = tsc(U"0.20"),
         .date = tsc(U"2003-07-05")},
    .monetary = {.int_curr_symbol = tsc(U"KES "),
                 .currency_symbol = tsc(U"Ksh"),
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
    .time = {.abday = {tsc(U"Dil"), tsc(U"Wix"), tsc(U"Qib"), tsc(U"Rob"), tsc(U"Kam"), tsc(U"Jim"), tsc(U"San")},
             .day = {tsc(U"Dilbata"), tsc(U"Wiixata"), tsc(U"Qibxata"), tsc(U"Roobii"), tsc(U"Kamiisa"),
                     tsc(U"Jimaata"), tsc(U"Sanbata")},
             .abmon = {tsc(U"Ama"), tsc(U"Gur"), tsc(U"Bit"), tsc(U"Elb"), tsc(U"Cam"), tsc(U"Wax"), tsc(U"Ado"),
                       tsc(U"Hag"), tsc(U"Ful"), tsc(U"Onk"), tsc(U"Sad"), tsc(U"Mud")},
             .mon = {tsc(U"Amajjii"), tsc(U"Guraandhala"), tsc(U"Bitooteessa"), tsc(U"Elba"), tsc(U"Caamsa"),
                     tsc(U"Waxabajjii"), tsc(U"Adooleessa"), tsc(U"Hagayya"), tsc(U"Fuulbana"), tsc(U"Onkololeessa"),
                     tsc(U"Sadaasa"), tsc(U"Muddee")},
             .d_t_fmt = tsc(U"%A, %B %e, %Y %r %Z"),
             .d_fmt = tsc(U"%d//%m//%Y"),
             .t_fmt = tsc(U"%l:%M:%S %p"),
             .t_fmt_ampm = tsc(U"%l:%M:%S %p"),
             .date_fmt = tsc(U"%A, %B %e, %r %Z %Y"),
             .am_pm = {tsc(U"WD"), tsc(U"WB")},
             .week = {7, 19971130, 1}},
    .messages = {.yesexpr = tsc(U"^[+1eEyY]"),
                 .noexpr = tsc(U"^[-0mMnN]"),
                 .yesstr = tsc(U"eeyyee"),
                 .nostr = tsc(U"miti")},
    .paper = {.width = 210, .height = 297},
    .telephone = {.tel_int_fmt = tsc(U"%c-%a-%l"),
                  .tel_dom_fmt = tsc(U"%a-%l"),
                  .int_select = tsc(U"000"),
                  .int_prefix = tsc(U"254")},
    .name = {.name_fmt = tsc(U"%d%t%g%t%m%t%f"),
             .name_gen = tsc(U""),
             .name_miss = tsc(U"Du"),
             .name_mr = tsc(U"Ob"),
             .name_mrs = tsc(U"Ad"),
             .name_ms = tsc(U"")},
    .address = {.postal_fmt = tsc(U"%z%c%T%s%b%e%r"),
                .country_name = tsc(U"Keeniyaa"),
                .country_post = tsc(U"KEN"),
                .country_ab2 = tsc(U"KE"),
                .country_ab3 = tsc(U"KEN"),
                .country_num = 404,
                .country_car = tsc(U"EAK"),
                .lang_name = tsc(U"Oromoo"),
                .lang_ab = tsc(U"om"),
                .lang_term = tsc(U"orm"),
                .lang_lib = tsc(U"orm")},
    .measurement = {.measurement = 1}};

} // namespace
} // namespace fast_io_i18n

#include "../main.h"