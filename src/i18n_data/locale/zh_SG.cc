﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("zh_SG"),.encoding=tsc(FAST_IO_LOCALE_ENCODING),.title=tsc("Chinese language locale for Singapore"),.source=tsc("IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc("1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("fast_io"),.email=tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Chinese"),.territory=tsc("Singapore"),.revision=tsc("1.0"),.date=tsc("2000,October,27 (XML source:2000,July,20)")},.monetary={.int_curr_symbol=tsc("SGD "),.currency_symbol=tsc("$"),.mon_decimal_point=tsc("."),.mon_thousands_sep=tsc(","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc("."),.thousands_sep=tsc(","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("日"),tsc("一"),tsc("二"),tsc("三"),tsc("四"),tsc("五"),tsc("六")},.day={tsc("星期日"),tsc("星期一"),tsc("星期二"),tsc("星期三"),tsc("星期四"),tsc("星期五"),tsc("星期六")},.abmon={tsc("一月"),tsc("二月"),tsc("三月"),tsc("四月"),tsc("五月"),tsc("六月"),tsc("七月"),tsc("八月"),tsc("九月"),tsc("十月"),tsc("十一月"),tsc("十二月")},.mon={tsc("一月"),tsc("二月"),tsc("三月"),tsc("四月"),tsc("五月"),tsc("六月"),tsc("七月"),tsc("八月"),tsc("九月"),tsc("十月"),tsc("十一月"),tsc("十二月")},.d_t_fmt=tsc("%Y年%m月%d日 %H时%M分%S秒"),.d_fmt=tsc("%Y年%m月%d日"),.t_fmt=tsc("%H时%M分%S秒 %Z"),.t_fmt_ampm=tsc(""),.date_fmt=tsc("%Y年%m月%d日 %H时%M分%S秒 %Z"),.am_pm={tsc("上午"),tsc("下午")},.week={7,19971130,1}},.messages={.yesexpr=tsc("^[+1yYｙＹ是]"),.noexpr=tsc("^[-0nNｎＮ不否]"),.yesstr=tsc("是"),.nostr=tsc("不是")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c ;%a ;%l"),.int_select=tsc("001"),.int_prefix=tsc("65")},.name={.name_fmt=tsc("%p%t%f%t%g"),.name_gen=tsc(""),.name_miss=tsc("Miss."),.name_mr=tsc("Mr."),.name_mrs=tsc("Mrs."),.name_ms=tsc("Ms.")},.address={.postal_fmt=tsc("%z%c%T%s%b%e%r"),.country_name=tsc("新加坡"),.country_ab2=tsc("SG"),.country_ab3=tsc("SGP"),.country_num=702,.country_car=tsc("SGP"),.lang_name=tsc("简体中文"),.lang_ab=tsc("zh"),.lang_term=tsc("zho"),.lang_lib=tsc("chi")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"zh_SG"),.encoding=tsc(FAST_IO_LOCALE_LENCODING),.title=tsc(L"Chinese language locale for Singapore"),.source=tsc(L"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(L"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"fast_io"),.email=tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Chinese"),.territory=tsc(L"Singapore"),.revision=tsc(L"1.0"),.date=tsc(L"2000,October,27 (XML source:2000,July,20)")},.monetary={.int_curr_symbol=tsc(L"SGD "),.currency_symbol=tsc(L"$"),.mon_decimal_point=tsc(L"."),.mon_thousands_sep=tsc(L","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(L"."),.thousands_sep=tsc(L","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"日"),tsc(L"一"),tsc(L"二"),tsc(L"三"),tsc(L"四"),tsc(L"五"),tsc(L"六")},.day={tsc(L"星期日"),tsc(L"星期一"),tsc(L"星期二"),tsc(L"星期三"),tsc(L"星期四"),tsc(L"星期五"),tsc(L"星期六")},.abmon={tsc(L"一月"),tsc(L"二月"),tsc(L"三月"),tsc(L"四月"),tsc(L"五月"),tsc(L"六月"),tsc(L"七月"),tsc(L"八月"),tsc(L"九月"),tsc(L"十月"),tsc(L"十一月"),tsc(L"十二月")},.mon={tsc(L"一月"),tsc(L"二月"),tsc(L"三月"),tsc(L"四月"),tsc(L"五月"),tsc(L"六月"),tsc(L"七月"),tsc(L"八月"),tsc(L"九月"),tsc(L"十月"),tsc(L"十一月"),tsc(L"十二月")},.d_t_fmt=tsc(L"%Y年%m月%d日 %H时%M分%S秒"),.d_fmt=tsc(L"%Y年%m月%d日"),.t_fmt=tsc(L"%H时%M分%S秒 %Z"),.t_fmt_ampm=tsc(L""),.date_fmt=tsc(L"%Y年%m月%d日 %H时%M分%S秒 %Z"),.am_pm={tsc(L"上午"),tsc(L"下午")},.week={7,19971130,1}},.messages={.yesexpr=tsc(L"^[+1yYｙＹ是]"),.noexpr=tsc(L"^[-0nNｎＮ不否]"),.yesstr=tsc(L"是"),.nostr=tsc(L"不是")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c ;%a ;%l"),.int_select=tsc(L"001"),.int_prefix=tsc(L"65")},.name={.name_fmt=tsc(L"%p%t%f%t%g"),.name_gen=tsc(L""),.name_miss=tsc(L"Miss."),.name_mr=tsc(L"Mr."),.name_mrs=tsc(L"Mrs."),.name_ms=tsc(L"Ms.")},.address={.postal_fmt=tsc(L"%z%c%T%s%b%e%r"),.country_name=tsc(L"新加坡"),.country_ab2=tsc(L"SG"),.country_ab3=tsc(L"SGP"),.country_num=702,.country_car=tsc(L"SGP"),.lang_name=tsc(L"简体中文"),.lang_ab=tsc(L"zh"),.lang_term=tsc(L"zho"),.lang_lib=tsc(L"chi")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"zh_SG"),.encoding=tsc(FAST_IO_LOCALE_u8ENCODING),.title=tsc(u8"Chinese language locale for Singapore"),.source=tsc(u8"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u8"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"fast_io"),.email=tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Chinese"),.territory=tsc(u8"Singapore"),.revision=tsc(u8"1.0"),.date=tsc(u8"2000,October,27 (XML source:2000,July,20)")},.monetary={.int_curr_symbol=tsc(u8"SGD "),.currency_symbol=tsc(u8"$"),.mon_decimal_point=tsc(u8"."),.mon_thousands_sep=tsc(u8","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(u8"."),.thousands_sep=tsc(u8","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"日"),tsc(u8"一"),tsc(u8"二"),tsc(u8"三"),tsc(u8"四"),tsc(u8"五"),tsc(u8"六")},.day={tsc(u8"星期日"),tsc(u8"星期一"),tsc(u8"星期二"),tsc(u8"星期三"),tsc(u8"星期四"),tsc(u8"星期五"),tsc(u8"星期六")},.abmon={tsc(u8"一月"),tsc(u8"二月"),tsc(u8"三月"),tsc(u8"四月"),tsc(u8"五月"),tsc(u8"六月"),tsc(u8"七月"),tsc(u8"八月"),tsc(u8"九月"),tsc(u8"十月"),tsc(u8"十一月"),tsc(u8"十二月")},.mon={tsc(u8"一月"),tsc(u8"二月"),tsc(u8"三月"),tsc(u8"四月"),tsc(u8"五月"),tsc(u8"六月"),tsc(u8"七月"),tsc(u8"八月"),tsc(u8"九月"),tsc(u8"十月"),tsc(u8"十一月"),tsc(u8"十二月")},.d_t_fmt=tsc(u8"%Y年%m月%d日 %H时%M分%S秒"),.d_fmt=tsc(u8"%Y年%m月%d日"),.t_fmt=tsc(u8"%H时%M分%S秒 %Z"),.t_fmt_ampm=tsc(u8""),.date_fmt=tsc(u8"%Y年%m月%d日 %H时%M分%S秒 %Z"),.am_pm={tsc(u8"上午"),tsc(u8"下午")},.week={7,19971130,1}},.messages={.yesexpr=tsc(u8"^[+1yYｙＹ是]"),.noexpr=tsc(u8"^[-0nNｎＮ不否]"),.yesstr=tsc(u8"是"),.nostr=tsc(u8"不是")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c ;%a ;%l"),.int_select=tsc(u8"001"),.int_prefix=tsc(u8"65")},.name={.name_fmt=tsc(u8"%p%t%f%t%g"),.name_gen=tsc(u8""),.name_miss=tsc(u8"Miss."),.name_mr=tsc(u8"Mr."),.name_mrs=tsc(u8"Mrs."),.name_ms=tsc(u8"Ms.")},.address={.postal_fmt=tsc(u8"%z%c%T%s%b%e%r"),.country_name=tsc(u8"新加坡"),.country_ab2=tsc(u8"SG"),.country_ab3=tsc(u8"SGP"),.country_num=702,.country_car=tsc(u8"SGP"),.lang_name=tsc(u8"简体中文"),.lang_ab=tsc(u8"zh"),.lang_term=tsc(u8"zho"),.lang_lib=tsc(u8"chi")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"zh_SG"),.encoding=tsc(FAST_IO_LOCALE_uENCODING),.title=tsc(u"Chinese language locale for Singapore"),.source=tsc(u"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"fast_io"),.email=tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Chinese"),.territory=tsc(u"Singapore"),.revision=tsc(u"1.0"),.date=tsc(u"2000,October,27 (XML source:2000,July,20)")},.monetary={.int_curr_symbol=tsc(u"SGD "),.currency_symbol=tsc(u"$"),.mon_decimal_point=tsc(u"."),.mon_thousands_sep=tsc(u","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(u"."),.thousands_sep=tsc(u","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"日"),tsc(u"一"),tsc(u"二"),tsc(u"三"),tsc(u"四"),tsc(u"五"),tsc(u"六")},.day={tsc(u"星期日"),tsc(u"星期一"),tsc(u"星期二"),tsc(u"星期三"),tsc(u"星期四"),tsc(u"星期五"),tsc(u"星期六")},.abmon={tsc(u"一月"),tsc(u"二月"),tsc(u"三月"),tsc(u"四月"),tsc(u"五月"),tsc(u"六月"),tsc(u"七月"),tsc(u"八月"),tsc(u"九月"),tsc(u"十月"),tsc(u"十一月"),tsc(u"十二月")},.mon={tsc(u"一月"),tsc(u"二月"),tsc(u"三月"),tsc(u"四月"),tsc(u"五月"),tsc(u"六月"),tsc(u"七月"),tsc(u"八月"),tsc(u"九月"),tsc(u"十月"),tsc(u"十一月"),tsc(u"十二月")},.d_t_fmt=tsc(u"%Y年%m月%d日 %H时%M分%S秒"),.d_fmt=tsc(u"%Y年%m月%d日"),.t_fmt=tsc(u"%H时%M分%S秒 %Z"),.t_fmt_ampm=tsc(u""),.date_fmt=tsc(u"%Y年%m月%d日 %H时%M分%S秒 %Z"),.am_pm={tsc(u"上午"),tsc(u"下午")},.week={7,19971130,1}},.messages={.yesexpr=tsc(u"^[+1yYｙＹ是]"),.noexpr=tsc(u"^[-0nNｎＮ不否]"),.yesstr=tsc(u"是"),.nostr=tsc(u"不是")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c ;%a ;%l"),.int_select=tsc(u"001"),.int_prefix=tsc(u"65")},.name={.name_fmt=tsc(u"%p%t%f%t%g"),.name_gen=tsc(u""),.name_miss=tsc(u"Miss."),.name_mr=tsc(u"Mr."),.name_mrs=tsc(u"Mrs."),.name_ms=tsc(u"Ms.")},.address={.postal_fmt=tsc(u"%z%c%T%s%b%e%r"),.country_name=tsc(u"新加坡"),.country_ab2=tsc(u"SG"),.country_ab3=tsc(u"SGP"),.country_num=702,.country_car=tsc(u"SGP"),.lang_name=tsc(u"简体中文"),.lang_ab=tsc(u"zh"),.lang_term=tsc(u"zho"),.lang_lib=tsc(u"chi")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"zh_SG"),.encoding=tsc(FAST_IO_LOCALE_UENCODING),.title=tsc(U"Chinese language locale for Singapore"),.source=tsc(U"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(U"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"fast_io"),.email=tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Chinese"),.territory=tsc(U"Singapore"),.revision=tsc(U"1.0"),.date=tsc(U"2000,October,27 (XML source:2000,July,20)")},.monetary={.int_curr_symbol=tsc(U"SGD "),.currency_symbol=tsc(U"$"),.mon_decimal_point=tsc(U"."),.mon_thousands_sep=tsc(U","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(U"."),.thousands_sep=tsc(U","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"日"),tsc(U"一"),tsc(U"二"),tsc(U"三"),tsc(U"四"),tsc(U"五"),tsc(U"六")},.day={tsc(U"星期日"),tsc(U"星期一"),tsc(U"星期二"),tsc(U"星期三"),tsc(U"星期四"),tsc(U"星期五"),tsc(U"星期六")},.abmon={tsc(U"一月"),tsc(U"二月"),tsc(U"三月"),tsc(U"四月"),tsc(U"五月"),tsc(U"六月"),tsc(U"七月"),tsc(U"八月"),tsc(U"九月"),tsc(U"十月"),tsc(U"十一月"),tsc(U"十二月")},.mon={tsc(U"一月"),tsc(U"二月"),tsc(U"三月"),tsc(U"四月"),tsc(U"五月"),tsc(U"六月"),tsc(U"七月"),tsc(U"八月"),tsc(U"九月"),tsc(U"十月"),tsc(U"十一月"),tsc(U"十二月")},.d_t_fmt=tsc(U"%Y年%m月%d日 %H时%M分%S秒"),.d_fmt=tsc(U"%Y年%m月%d日"),.t_fmt=tsc(U"%H时%M分%S秒 %Z"),.t_fmt_ampm=tsc(U""),.date_fmt=tsc(U"%Y年%m月%d日 %H时%M分%S秒 %Z"),.am_pm={tsc(U"上午"),tsc(U"下午")},.week={7,19971130,1}},.messages={.yesexpr=tsc(U"^[+1yYｙＹ是]"),.noexpr=tsc(U"^[-0nNｎＮ不否]"),.yesstr=tsc(U"是"),.nostr=tsc(U"不是")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c ;%a ;%l"),.int_select=tsc(U"001"),.int_prefix=tsc(U"65")},.name={.name_fmt=tsc(U"%p%t%f%t%g"),.name_gen=tsc(U""),.name_miss=tsc(U"Miss."),.name_mr=tsc(U"Mr."),.name_mrs=tsc(U"Mrs."),.name_ms=tsc(U"Ms.")},.address={.postal_fmt=tsc(U"%z%c%T%s%b%e%r"),.country_name=tsc(U"新加坡"),.country_ab2=tsc(U"SG"),.country_ab3=tsc(U"SGP"),.country_num=702,.country_car=tsc(U"SGP"),.lang_name=tsc(U"简体中文"),.lang_ab=tsc(U"zh"),.lang_term=tsc(U"zho"),.lang_lib=tsc(U"chi")},.measurement={.measurement=1}};


}
}

#include"../main.h"