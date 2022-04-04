﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("yuw_PG"),.encoding=tsc(FAST_IO_LOCALE_ENCODING),.title=tsc("Yau/Nungon locale for Papua New Guinea"),.source=tsc("Information from native speakers\t\t;\t\tfast_io"),.address=tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("Hannah Sarvasy\t\t;\t\tfast_io"),.email=tsc("nungon.localization@gmail.com;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Yau"),.territory=tsc("Papua New Guinea"),.revision=tsc("1.0"),.date=tsc("2016-12-07")},.monetary={.int_curr_symbol=tsc("PGK "),.currency_symbol=tsc("K"),.mon_decimal_point=tsc("."),.mon_thousands_sep=tsc(","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc("."),.thousands_sep=tsc(","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("sön"),tsc("mön"),tsc("sin"),tsc("mit"),tsc("soi"),tsc("nen"),tsc("sab")},.day={tsc("sönda"),tsc("mönda"),tsc("sinda"),tsc("mitiwö"),tsc("sogipbono"),tsc("nenggo"),tsc("söndanggie")},.abmon={tsc("jen"),tsc("feb"),tsc("mas"),tsc("epr"),tsc("mei"),tsc("jun"),tsc("jul"),tsc("ögu"),tsc("sep"),tsc("ökt"),tsc("nöw"),tsc("dis")},.mon={tsc("jenuari"),tsc("febuari"),tsc("mas"),tsc("epril"),tsc("mei"),tsc("jun"),tsc("julai"),tsc("ögus"),tsc("septemba"),tsc("öktoba"),tsc("nöwemba"),tsc("diksemba")},.d_t_fmt=tsc("%a %d %b %Y %T"),.d_fmt=tsc("%d//%m//%y"),.t_fmt=tsc("%T"),.t_fmt_ampm=tsc("%I:%M:%S %p"),.date_fmt=tsc("%a %d %b %Y %T %Z"),.am_pm={tsc("AM"),tsc("PM")},.week={7,19971130,1}},.messages={.yesexpr=tsc("^[+1yYöÖ]"),.noexpr=tsc("^[-0nNmM]"),.yesstr=tsc("öö"),.nostr=tsc("muuno")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c %a %l"),.int_select=tsc("00"),.int_prefix=tsc("675")},.name={.name_fmt=tsc("%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc("Papua New Guinea"),.country_post=tsc(""),.country_ab2=tsc("PG"),.country_ab3=tsc("PNG"),.country_num=598,.country_car=tsc("PNG"),.lang_name=tsc("Uruwa"),.lang_ab=tsc(""),.lang_term=tsc("yuw"),.lang_lib=tsc("yuw")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"yuw_PG"),.encoding=tsc(FAST_IO_LOCALE_LENCODING),.title=tsc(L"Yau/Nungon locale for Papua New Guinea"),.source=tsc(L"Information from native speakers\t\t;\t\tfast_io"),.address=tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"Hannah Sarvasy\t\t;\t\tfast_io"),.email=tsc(L"nungon.localization@gmail.com;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Yau"),.territory=tsc(L"Papua New Guinea"),.revision=tsc(L"1.0"),.date=tsc(L"2016-12-07")},.monetary={.int_curr_symbol=tsc(L"PGK "),.currency_symbol=tsc(L"K"),.mon_decimal_point=tsc(L"."),.mon_thousands_sep=tsc(L","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(L"."),.thousands_sep=tsc(L","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"sön"),tsc(L"mön"),tsc(L"sin"),tsc(L"mit"),tsc(L"soi"),tsc(L"nen"),tsc(L"sab")},.day={tsc(L"sönda"),tsc(L"mönda"),tsc(L"sinda"),tsc(L"mitiwö"),tsc(L"sogipbono"),tsc(L"nenggo"),tsc(L"söndanggie")},.abmon={tsc(L"jen"),tsc(L"feb"),tsc(L"mas"),tsc(L"epr"),tsc(L"mei"),tsc(L"jun"),tsc(L"jul"),tsc(L"ögu"),tsc(L"sep"),tsc(L"ökt"),tsc(L"nöw"),tsc(L"dis")},.mon={tsc(L"jenuari"),tsc(L"febuari"),tsc(L"mas"),tsc(L"epril"),tsc(L"mei"),tsc(L"jun"),tsc(L"julai"),tsc(L"ögus"),tsc(L"septemba"),tsc(L"öktoba"),tsc(L"nöwemba"),tsc(L"diksemba")},.d_t_fmt=tsc(L"%a %d %b %Y %T"),.d_fmt=tsc(L"%d//%m//%y"),.t_fmt=tsc(L"%T"),.t_fmt_ampm=tsc(L"%I:%M:%S %p"),.date_fmt=tsc(L"%a %d %b %Y %T %Z"),.am_pm={tsc(L"AM"),tsc(L"PM")},.week={7,19971130,1}},.messages={.yesexpr=tsc(L"^[+1yYöÖ]"),.noexpr=tsc(L"^[-0nNmM]"),.yesstr=tsc(L"öö"),.nostr=tsc(L"muuno")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c %a %l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"675")},.name={.name_fmt=tsc(L"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(L"Papua New Guinea"),.country_post=tsc(L""),.country_ab2=tsc(L"PG"),.country_ab3=tsc(L"PNG"),.country_num=598,.country_car=tsc(L"PNG"),.lang_name=tsc(L"Uruwa"),.lang_ab=tsc(L""),.lang_term=tsc(L"yuw"),.lang_lib=tsc(L"yuw")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"yuw_PG"),.encoding=tsc(FAST_IO_LOCALE_u8ENCODING),.title=tsc(u8"Yau/Nungon locale for Papua New Guinea"),.source=tsc(u8"Information from native speakers\t\t;\t\tfast_io"),.address=tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"Hannah Sarvasy\t\t;\t\tfast_io"),.email=tsc(u8"nungon.localization@gmail.com;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Yau"),.territory=tsc(u8"Papua New Guinea"),.revision=tsc(u8"1.0"),.date=tsc(u8"2016-12-07")},.monetary={.int_curr_symbol=tsc(u8"PGK "),.currency_symbol=tsc(u8"K"),.mon_decimal_point=tsc(u8"."),.mon_thousands_sep=tsc(u8","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u8"."),.thousands_sep=tsc(u8","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"sön"),tsc(u8"mön"),tsc(u8"sin"),tsc(u8"mit"),tsc(u8"soi"),tsc(u8"nen"),tsc(u8"sab")},.day={tsc(u8"sönda"),tsc(u8"mönda"),tsc(u8"sinda"),tsc(u8"mitiwö"),tsc(u8"sogipbono"),tsc(u8"nenggo"),tsc(u8"söndanggie")},.abmon={tsc(u8"jen"),tsc(u8"feb"),tsc(u8"mas"),tsc(u8"epr"),tsc(u8"mei"),tsc(u8"jun"),tsc(u8"jul"),tsc(u8"ögu"),tsc(u8"sep"),tsc(u8"ökt"),tsc(u8"nöw"),tsc(u8"dis")},.mon={tsc(u8"jenuari"),tsc(u8"febuari"),tsc(u8"mas"),tsc(u8"epril"),tsc(u8"mei"),tsc(u8"jun"),tsc(u8"julai"),tsc(u8"ögus"),tsc(u8"septemba"),tsc(u8"öktoba"),tsc(u8"nöwemba"),tsc(u8"diksemba")},.d_t_fmt=tsc(u8"%a %d %b %Y %T"),.d_fmt=tsc(u8"%d//%m//%y"),.t_fmt=tsc(u8"%T"),.t_fmt_ampm=tsc(u8"%I:%M:%S %p"),.date_fmt=tsc(u8"%a %d %b %Y %T %Z"),.am_pm={tsc(u8"AM"),tsc(u8"PM")},.week={7,19971130,1}},.messages={.yesexpr=tsc(u8"^[+1yYöÖ]"),.noexpr=tsc(u8"^[-0nNmM]"),.yesstr=tsc(u8"öö"),.nostr=tsc(u8"muuno")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c %a %l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"675")},.name={.name_fmt=tsc(u8"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u8"Papua New Guinea"),.country_post=tsc(u8""),.country_ab2=tsc(u8"PG"),.country_ab3=tsc(u8"PNG"),.country_num=598,.country_car=tsc(u8"PNG"),.lang_name=tsc(u8"Uruwa"),.lang_ab=tsc(u8""),.lang_term=tsc(u8"yuw"),.lang_lib=tsc(u8"yuw")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"yuw_PG"),.encoding=tsc(FAST_IO_LOCALE_uENCODING),.title=tsc(u"Yau/Nungon locale for Papua New Guinea"),.source=tsc(u"Information from native speakers\t\t;\t\tfast_io"),.address=tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"Hannah Sarvasy\t\t;\t\tfast_io"),.email=tsc(u"nungon.localization@gmail.com;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Yau"),.territory=tsc(u"Papua New Guinea"),.revision=tsc(u"1.0"),.date=tsc(u"2016-12-07")},.monetary={.int_curr_symbol=tsc(u"PGK "),.currency_symbol=tsc(u"K"),.mon_decimal_point=tsc(u"."),.mon_thousands_sep=tsc(u","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u"."),.thousands_sep=tsc(u","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"sön"),tsc(u"mön"),tsc(u"sin"),tsc(u"mit"),tsc(u"soi"),tsc(u"nen"),tsc(u"sab")},.day={tsc(u"sönda"),tsc(u"mönda"),tsc(u"sinda"),tsc(u"mitiwö"),tsc(u"sogipbono"),tsc(u"nenggo"),tsc(u"söndanggie")},.abmon={tsc(u"jen"),tsc(u"feb"),tsc(u"mas"),tsc(u"epr"),tsc(u"mei"),tsc(u"jun"),tsc(u"jul"),tsc(u"ögu"),tsc(u"sep"),tsc(u"ökt"),tsc(u"nöw"),tsc(u"dis")},.mon={tsc(u"jenuari"),tsc(u"febuari"),tsc(u"mas"),tsc(u"epril"),tsc(u"mei"),tsc(u"jun"),tsc(u"julai"),tsc(u"ögus"),tsc(u"septemba"),tsc(u"öktoba"),tsc(u"nöwemba"),tsc(u"diksemba")},.d_t_fmt=tsc(u"%a %d %b %Y %T"),.d_fmt=tsc(u"%d//%m//%y"),.t_fmt=tsc(u"%T"),.t_fmt_ampm=tsc(u"%I:%M:%S %p"),.date_fmt=tsc(u"%a %d %b %Y %T %Z"),.am_pm={tsc(u"AM"),tsc(u"PM")},.week={7,19971130,1}},.messages={.yesexpr=tsc(u"^[+1yYöÖ]"),.noexpr=tsc(u"^[-0nNmM]"),.yesstr=tsc(u"öö"),.nostr=tsc(u"muuno")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c %a %l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"675")},.name={.name_fmt=tsc(u"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u"Papua New Guinea"),.country_post=tsc(u""),.country_ab2=tsc(u"PG"),.country_ab3=tsc(u"PNG"),.country_num=598,.country_car=tsc(u"PNG"),.lang_name=tsc(u"Uruwa"),.lang_ab=tsc(u""),.lang_term=tsc(u"yuw"),.lang_lib=tsc(u"yuw")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"yuw_PG"),.encoding=tsc(FAST_IO_LOCALE_UENCODING),.title=tsc(U"Yau/Nungon locale for Papua New Guinea"),.source=tsc(U"Information from native speakers\t\t;\t\tfast_io"),.address=tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"Hannah Sarvasy\t\t;\t\tfast_io"),.email=tsc(U"nungon.localization@gmail.com;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Yau"),.territory=tsc(U"Papua New Guinea"),.revision=tsc(U"1.0"),.date=tsc(U"2016-12-07")},.monetary={.int_curr_symbol=tsc(U"PGK "),.currency_symbol=tsc(U"K"),.mon_decimal_point=tsc(U"."),.mon_thousands_sep=tsc(U","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(U"."),.thousands_sep=tsc(U","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"sön"),tsc(U"mön"),tsc(U"sin"),tsc(U"mit"),tsc(U"soi"),tsc(U"nen"),tsc(U"sab")},.day={tsc(U"sönda"),tsc(U"mönda"),tsc(U"sinda"),tsc(U"mitiwö"),tsc(U"sogipbono"),tsc(U"nenggo"),tsc(U"söndanggie")},.abmon={tsc(U"jen"),tsc(U"feb"),tsc(U"mas"),tsc(U"epr"),tsc(U"mei"),tsc(U"jun"),tsc(U"jul"),tsc(U"ögu"),tsc(U"sep"),tsc(U"ökt"),tsc(U"nöw"),tsc(U"dis")},.mon={tsc(U"jenuari"),tsc(U"febuari"),tsc(U"mas"),tsc(U"epril"),tsc(U"mei"),tsc(U"jun"),tsc(U"julai"),tsc(U"ögus"),tsc(U"septemba"),tsc(U"öktoba"),tsc(U"nöwemba"),tsc(U"diksemba")},.d_t_fmt=tsc(U"%a %d %b %Y %T"),.d_fmt=tsc(U"%d//%m//%y"),.t_fmt=tsc(U"%T"),.t_fmt_ampm=tsc(U"%I:%M:%S %p"),.date_fmt=tsc(U"%a %d %b %Y %T %Z"),.am_pm={tsc(U"AM"),tsc(U"PM")},.week={7,19971130,1}},.messages={.yesexpr=tsc(U"^[+1yYöÖ]"),.noexpr=tsc(U"^[-0nNmM]"),.yesstr=tsc(U"öö"),.nostr=tsc(U"muuno")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c %a %l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"675")},.name={.name_fmt=tsc(U"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(U"Papua New Guinea"),.country_post=tsc(U""),.country_ab2=tsc(U"PG"),.country_ab3=tsc(U"PNG"),.country_num=598,.country_car=tsc(U"PNG"),.lang_name=tsc(U"Uruwa"),.lang_ab=tsc(U""),.lang_term=tsc(U"yuw"),.lang_lib=tsc(U"yuw")},.measurement={.measurement=1}};


}
}

#include"../main.h"