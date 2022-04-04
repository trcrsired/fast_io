﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3,2};

inline constexpr basic_io_scatter_t<char> time_alt_digits_storage[]{tsc("୦"),tsc("୧"),tsc("୨"),tsc("୩"),tsc("୪"),tsc("୫"),tsc("୬"),tsc("୭"),tsc("୮"),tsc("୯"),tsc("୧୦"),tsc("୧୧"),tsc("୧୨"),tsc("୧୩"),tsc("୧୪"),tsc("୧୫"),tsc("୧୬"),tsc("୧୭"),tsc("୧୮"),tsc("୧୯"),tsc("୨୦"),tsc("୨୧"),tsc("୨୨"),tsc("୨୩"),tsc("୨୪"),tsc("୨୫"),tsc("୨୬"),tsc("୨୭"),tsc("୨୮"),tsc("୨୯"),tsc("୩୦"),tsc("୩୧"),tsc("୩୨"),tsc("୩୩"),tsc("୩୪"),tsc("୩୫"),tsc("୩୬"),tsc("୩୭"),tsc("୩୮"),tsc("୩୯"),tsc("୪୦"),tsc("୪୧"),tsc("୪୨"),tsc("୪୩"),tsc("୪୪"),tsc("୪୫"),tsc("୪୬"),tsc("୪୭"),tsc("୪୮"),tsc("୪୯"),tsc("୫୦"),tsc("୫୧"),tsc("୫୨"),tsc("୫୩"),tsc("୫୪"),tsc("୫୫"),tsc("୫୬"),tsc("୫୭"),tsc("୫୮"),tsc("୫୯"),tsc("୬୦"),tsc("୬୧"),tsc("୬୨"),tsc("୬୩"),tsc("୬୪"),tsc("୬୫"),tsc("୬୬"),tsc("୬୭"),tsc("୬୮"),tsc("୬୯"),tsc("୭୦"),tsc("୭୧"),tsc("୭୨"),tsc("୭୩"),tsc("୭୪"),tsc("୭୫"),tsc("୭୬"),tsc("୭୭"),tsc("୭୮"),tsc("୭୯"),tsc("୮୦"),tsc("୮୧"),tsc("୮୨"),tsc("୮୩"),tsc("୮୪"),tsc("୮୫"),tsc("୮୬"),tsc("୮୭"),tsc("୮୮"),tsc("୮୯"),tsc("୯୦"),tsc("୯୧"),tsc("୯୨"),tsc("୯୩"),tsc("୯୪"),tsc("୯୫"),tsc("୯୬"),tsc("୯୭"),tsc("୯୮"),tsc("୯୯")};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("or_IN"),.encoding=tsc(FAST_IO_LOCALE_ENCODING),.title=tsc("Odia language locale for India"),.source=tsc("IBM AP Linux Technology Center, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc("1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("fast_io"),.email=tsc("bug-glibc@gnu.org;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Odia"),.territory=tsc("India"),.revision=tsc("1.0"),.date=tsc("2006-05-25")},.monetary={.int_curr_symbol=tsc("INR "),.currency_symbol=tsc("₹"),.mon_decimal_point=tsc("."),.mon_thousands_sep=tsc(","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc("."),.thousands_sep=tsc(","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc("ରବି"),tsc("ସୋମ"),tsc("ମଙ୍ଗଳ"),tsc("ବୁଧ"),tsc("ଗୁରୁ"),tsc("ଶୁକ୍ର"),tsc("ଶନି")},.day={tsc("ରବିବାର"),tsc("ସୋମବାର"),tsc("ମଙ୍ଗଳବାର"),tsc("ବୁଧବାର"),tsc("ଗୁରୁବାର"),tsc("ଶୁକ୍ରବାର"),tsc("ଶନିବାର")},.abmon={tsc("ଜାନୁଆରୀ"),tsc("ଫେବୃଆରୀ"),tsc("ମାର୍ଚ୍ଚ"),tsc("ଅପ୍ରେଲ"),tsc("ମଇ"),tsc("ଜୁନ"),tsc("ଜୁଲାଇ"),tsc("ଅଗଷ୍ଟ"),tsc("ସେପ୍ଟେମ୍ବର"),tsc("ଅକ୍ଟୋବର"),tsc("ନଭେମ୍ବର"),tsc("ଡିସେମ୍ବର")},.mon={tsc("ଜାନୁଆରୀ"),tsc("ଫେବୃଆରୀ"),tsc("ମାର୍ଚ୍ଚ"),tsc("ଅପ୍ରେଲ"),tsc("ମଇ"),tsc("ଜୁନ"),tsc("ଜୁଲାଇ"),tsc("ଅଗଷ୍ଟ"),tsc("ସେପ୍ଟେମ୍ବର"),tsc("ଅକ୍ଟୋବର"),tsc("ନଭେମ୍ବର"),tsc("ଡିସେମ୍ବର")},.d_t_fmt=tsc("%Oe %B %Oy %OI:%OM:%OS %p"),.d_fmt=tsc("%Od-%Om-%Oy"),.t_fmt=tsc("%OI:%OM:%OS %p"),.t_fmt_ampm=tsc("%OI:%OM:%OS %p"),.date_fmt=tsc("%Oe %B %Oy %OI:%OM:%OS %p %Z"),.am_pm={tsc("AM"),tsc("PM")},.alt_digits={time_alt_digits_storage,100},.week={7,19971130,1}},.messages={.yesexpr=tsc("^[+1yYହ]"),.noexpr=tsc("^[-0nNନ]"),.yesstr=tsc("ହଁ"),.nostr=tsc("ନା")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c ;%a ;%l"),.int_select=tsc("00"),.int_prefix=tsc("91")},.name={.name_fmt=tsc("%p%t%f%t%g"),.name_gen=tsc(""),.name_miss=tsc("Miss."),.name_mr=tsc("Mr."),.name_mrs=tsc("Mrs."),.name_ms=tsc("Ms.")},.address={.postal_fmt=tsc("%z%c%T%s%b%e%r"),.country_name=tsc("ଭାରତ"),.country_ab2=tsc("IN"),.country_ab3=tsc("IND"),.country_num=356,.country_car=tsc("IND"),.lang_name=tsc("ଓଡ଼ିଆ"),.lang_ab=tsc("or"),.lang_term=tsc("ori"),.lang_lib=tsc("ori")},.measurement={.measurement=1}};

inline constexpr basic_io_scatter_t<wchar_t> wtime_alt_digits_storage[]{tsc(L"୦"),tsc(L"୧"),tsc(L"୨"),tsc(L"୩"),tsc(L"୪"),tsc(L"୫"),tsc(L"୬"),tsc(L"୭"),tsc(L"୮"),tsc(L"୯"),tsc(L"୧୦"),tsc(L"୧୧"),tsc(L"୧୨"),tsc(L"୧୩"),tsc(L"୧୪"),tsc(L"୧୫"),tsc(L"୧୬"),tsc(L"୧୭"),tsc(L"୧୮"),tsc(L"୧୯"),tsc(L"୨୦"),tsc(L"୨୧"),tsc(L"୨୨"),tsc(L"୨୩"),tsc(L"୨୪"),tsc(L"୨୫"),tsc(L"୨୬"),tsc(L"୨୭"),tsc(L"୨୮"),tsc(L"୨୯"),tsc(L"୩୦"),tsc(L"୩୧"),tsc(L"୩୨"),tsc(L"୩୩"),tsc(L"୩୪"),tsc(L"୩୫"),tsc(L"୩୬"),tsc(L"୩୭"),tsc(L"୩୮"),tsc(L"୩୯"),tsc(L"୪୦"),tsc(L"୪୧"),tsc(L"୪୨"),tsc(L"୪୩"),tsc(L"୪୪"),tsc(L"୪୫"),tsc(L"୪୬"),tsc(L"୪୭"),tsc(L"୪୮"),tsc(L"୪୯"),tsc(L"୫୦"),tsc(L"୫୧"),tsc(L"୫୨"),tsc(L"୫୩"),tsc(L"୫୪"),tsc(L"୫୫"),tsc(L"୫୬"),tsc(L"୫୭"),tsc(L"୫୮"),tsc(L"୫୯"),tsc(L"୬୦"),tsc(L"୬୧"),tsc(L"୬୨"),tsc(L"୬୩"),tsc(L"୬୪"),tsc(L"୬୫"),tsc(L"୬୬"),tsc(L"୬୭"),tsc(L"୬୮"),tsc(L"୬୯"),tsc(L"୭୦"),tsc(L"୭୧"),tsc(L"୭୨"),tsc(L"୭୩"),tsc(L"୭୪"),tsc(L"୭୫"),tsc(L"୭୬"),tsc(L"୭୭"),tsc(L"୭୮"),tsc(L"୭୯"),tsc(L"୮୦"),tsc(L"୮୧"),tsc(L"୮୨"),tsc(L"୮୩"),tsc(L"୮୪"),tsc(L"୮୫"),tsc(L"୮୬"),tsc(L"୮୭"),tsc(L"୮୮"),tsc(L"୮୯"),tsc(L"୯୦"),tsc(L"୯୧"),tsc(L"୯୨"),tsc(L"୯୩"),tsc(L"୯୪"),tsc(L"୯୫"),tsc(L"୯୬"),tsc(L"୯୭"),tsc(L"୯୮"),tsc(L"୯୯")};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"or_IN"),.encoding=tsc(FAST_IO_LOCALE_LENCODING),.title=tsc(L"Odia language locale for India"),.source=tsc(L"IBM AP Linux Technology Center, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(L"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"fast_io"),.email=tsc(L"bug-glibc@gnu.org;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Odia"),.territory=tsc(L"India"),.revision=tsc(L"1.0"),.date=tsc(L"2006-05-25")},.monetary={.int_curr_symbol=tsc(L"INR "),.currency_symbol=tsc(L"₹"),.mon_decimal_point=tsc(L"."),.mon_thousands_sep=tsc(L","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(L"."),.thousands_sep=tsc(L","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(L"ରବି"),tsc(L"ସୋମ"),tsc(L"ମଙ୍ଗଳ"),tsc(L"ବୁଧ"),tsc(L"ଗୁରୁ"),tsc(L"ଶୁକ୍ର"),tsc(L"ଶନି")},.day={tsc(L"ରବିବାର"),tsc(L"ସୋମବାର"),tsc(L"ମଙ୍ଗଳବାର"),tsc(L"ବୁଧବାର"),tsc(L"ଗୁରୁବାର"),tsc(L"ଶୁକ୍ରବାର"),tsc(L"ଶନିବାର")},.abmon={tsc(L"ଜାନୁଆରୀ"),tsc(L"ଫେବୃଆରୀ"),tsc(L"ମାର୍ଚ୍ଚ"),tsc(L"ଅପ୍ରେଲ"),tsc(L"ମଇ"),tsc(L"ଜୁନ"),tsc(L"ଜୁଲାଇ"),tsc(L"ଅଗଷ୍ଟ"),tsc(L"ସେପ୍ଟେମ୍ବର"),tsc(L"ଅକ୍ଟୋବର"),tsc(L"ନଭେମ୍ବର"),tsc(L"ଡିସେମ୍ବର")},.mon={tsc(L"ଜାନୁଆରୀ"),tsc(L"ଫେବୃଆରୀ"),tsc(L"ମାର୍ଚ୍ଚ"),tsc(L"ଅପ୍ରେଲ"),tsc(L"ମଇ"),tsc(L"ଜୁନ"),tsc(L"ଜୁଲାଇ"),tsc(L"ଅଗଷ୍ଟ"),tsc(L"ସେପ୍ଟେମ୍ବର"),tsc(L"ଅକ୍ଟୋବର"),tsc(L"ନଭେମ୍ବର"),tsc(L"ଡିସେମ୍ବର")},.d_t_fmt=tsc(L"%Oe %B %Oy %OI:%OM:%OS %p"),.d_fmt=tsc(L"%Od-%Om-%Oy"),.t_fmt=tsc(L"%OI:%OM:%OS %p"),.t_fmt_ampm=tsc(L"%OI:%OM:%OS %p"),.date_fmt=tsc(L"%Oe %B %Oy %OI:%OM:%OS %p %Z"),.am_pm={tsc(L"AM"),tsc(L"PM")},.alt_digits={wtime_alt_digits_storage,100},.week={7,19971130,1}},.messages={.yesexpr=tsc(L"^[+1yYହ]"),.noexpr=tsc(L"^[-0nNନ]"),.yesstr=tsc(L"ହଁ"),.nostr=tsc(L"ନା")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c ;%a ;%l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"91")},.name={.name_fmt=tsc(L"%p%t%f%t%g"),.name_gen=tsc(L""),.name_miss=tsc(L"Miss."),.name_mr=tsc(L"Mr."),.name_mrs=tsc(L"Mrs."),.name_ms=tsc(L"Ms.")},.address={.postal_fmt=tsc(L"%z%c%T%s%b%e%r"),.country_name=tsc(L"ଭାରତ"),.country_ab2=tsc(L"IN"),.country_ab3=tsc(L"IND"),.country_num=356,.country_car=tsc(L"IND"),.lang_name=tsc(L"ଓଡ଼ିଆ"),.lang_ab=tsc(L"or"),.lang_term=tsc(L"ori"),.lang_lib=tsc(L"ori")},.measurement={.measurement=1}};

inline constexpr basic_io_scatter_t<char8_t> u8time_alt_digits_storage[]{tsc(u8"୦"),tsc(u8"୧"),tsc(u8"୨"),tsc(u8"୩"),tsc(u8"୪"),tsc(u8"୫"),tsc(u8"୬"),tsc(u8"୭"),tsc(u8"୮"),tsc(u8"୯"),tsc(u8"୧୦"),tsc(u8"୧୧"),tsc(u8"୧୨"),tsc(u8"୧୩"),tsc(u8"୧୪"),tsc(u8"୧୫"),tsc(u8"୧୬"),tsc(u8"୧୭"),tsc(u8"୧୮"),tsc(u8"୧୯"),tsc(u8"୨୦"),tsc(u8"୨୧"),tsc(u8"୨୨"),tsc(u8"୨୩"),tsc(u8"୨୪"),tsc(u8"୨୫"),tsc(u8"୨୬"),tsc(u8"୨୭"),tsc(u8"୨୮"),tsc(u8"୨୯"),tsc(u8"୩୦"),tsc(u8"୩୧"),tsc(u8"୩୨"),tsc(u8"୩୩"),tsc(u8"୩୪"),tsc(u8"୩୫"),tsc(u8"୩୬"),tsc(u8"୩୭"),tsc(u8"୩୮"),tsc(u8"୩୯"),tsc(u8"୪୦"),tsc(u8"୪୧"),tsc(u8"୪୨"),tsc(u8"୪୩"),tsc(u8"୪୪"),tsc(u8"୪୫"),tsc(u8"୪୬"),tsc(u8"୪୭"),tsc(u8"୪୮"),tsc(u8"୪୯"),tsc(u8"୫୦"),tsc(u8"୫୧"),tsc(u8"୫୨"),tsc(u8"୫୩"),tsc(u8"୫୪"),tsc(u8"୫୫"),tsc(u8"୫୬"),tsc(u8"୫୭"),tsc(u8"୫୮"),tsc(u8"୫୯"),tsc(u8"୬୦"),tsc(u8"୬୧"),tsc(u8"୬୨"),tsc(u8"୬୩"),tsc(u8"୬୪"),tsc(u8"୬୫"),tsc(u8"୬୬"),tsc(u8"୬୭"),tsc(u8"୬୮"),tsc(u8"୬୯"),tsc(u8"୭୦"),tsc(u8"୭୧"),tsc(u8"୭୨"),tsc(u8"୭୩"),tsc(u8"୭୪"),tsc(u8"୭୫"),tsc(u8"୭୬"),tsc(u8"୭୭"),tsc(u8"୭୮"),tsc(u8"୭୯"),tsc(u8"୮୦"),tsc(u8"୮୧"),tsc(u8"୮୨"),tsc(u8"୮୩"),tsc(u8"୮୪"),tsc(u8"୮୫"),tsc(u8"୮୬"),tsc(u8"୮୭"),tsc(u8"୮୮"),tsc(u8"୮୯"),tsc(u8"୯୦"),tsc(u8"୯୧"),tsc(u8"୯୨"),tsc(u8"୯୩"),tsc(u8"୯୪"),tsc(u8"୯୫"),tsc(u8"୯୬"),tsc(u8"୯୭"),tsc(u8"୯୮"),tsc(u8"୯୯")};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"or_IN"),.encoding=tsc(FAST_IO_LOCALE_u8ENCODING),.title=tsc(u8"Odia language locale for India"),.source=tsc(u8"IBM AP Linux Technology Center, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u8"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"fast_io"),.email=tsc(u8"bug-glibc@gnu.org;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Odia"),.territory=tsc(u8"India"),.revision=tsc(u8"1.0"),.date=tsc(u8"2006-05-25")},.monetary={.int_curr_symbol=tsc(u8"INR "),.currency_symbol=tsc(u8"₹"),.mon_decimal_point=tsc(u8"."),.mon_thousands_sep=tsc(u8","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u8"."),.thousands_sep=tsc(u8","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(u8"ରବି"),tsc(u8"ସୋମ"),tsc(u8"ମଙ୍ଗଳ"),tsc(u8"ବୁଧ"),tsc(u8"ଗୁରୁ"),tsc(u8"ଶୁକ୍ର"),tsc(u8"ଶନି")},.day={tsc(u8"ରବିବାର"),tsc(u8"ସୋମବାର"),tsc(u8"ମଙ୍ଗଳବାର"),tsc(u8"ବୁଧବାର"),tsc(u8"ଗୁରୁବାର"),tsc(u8"ଶୁକ୍ରବାର"),tsc(u8"ଶନିବାର")},.abmon={tsc(u8"ଜାନୁଆରୀ"),tsc(u8"ଫେବୃଆରୀ"),tsc(u8"ମାର୍ଚ୍ଚ"),tsc(u8"ଅପ୍ରେଲ"),tsc(u8"ମଇ"),tsc(u8"ଜୁନ"),tsc(u8"ଜୁଲାଇ"),tsc(u8"ଅଗଷ୍ଟ"),tsc(u8"ସେପ୍ଟେମ୍ବର"),tsc(u8"ଅକ୍ଟୋବର"),tsc(u8"ନଭେମ୍ବର"),tsc(u8"ଡିସେମ୍ବର")},.mon={tsc(u8"ଜାନୁଆରୀ"),tsc(u8"ଫେବୃଆରୀ"),tsc(u8"ମାର୍ଚ୍ଚ"),tsc(u8"ଅପ୍ରେଲ"),tsc(u8"ମଇ"),tsc(u8"ଜୁନ"),tsc(u8"ଜୁଲାଇ"),tsc(u8"ଅଗଷ୍ଟ"),tsc(u8"ସେପ୍ଟେମ୍ବର"),tsc(u8"ଅକ୍ଟୋବର"),tsc(u8"ନଭେମ୍ବର"),tsc(u8"ଡିସେମ୍ବର")},.d_t_fmt=tsc(u8"%Oe %B %Oy %OI:%OM:%OS %p"),.d_fmt=tsc(u8"%Od-%Om-%Oy"),.t_fmt=tsc(u8"%OI:%OM:%OS %p"),.t_fmt_ampm=tsc(u8"%OI:%OM:%OS %p"),.date_fmt=tsc(u8"%Oe %B %Oy %OI:%OM:%OS %p %Z"),.am_pm={tsc(u8"AM"),tsc(u8"PM")},.alt_digits={u8time_alt_digits_storage,100},.week={7,19971130,1}},.messages={.yesexpr=tsc(u8"^[+1yYହ]"),.noexpr=tsc(u8"^[-0nNନ]"),.yesstr=tsc(u8"ହଁ"),.nostr=tsc(u8"ନା")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c ;%a ;%l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"91")},.name={.name_fmt=tsc(u8"%p%t%f%t%g"),.name_gen=tsc(u8""),.name_miss=tsc(u8"Miss."),.name_mr=tsc(u8"Mr."),.name_mrs=tsc(u8"Mrs."),.name_ms=tsc(u8"Ms.")},.address={.postal_fmt=tsc(u8"%z%c%T%s%b%e%r"),.country_name=tsc(u8"ଭାରତ"),.country_ab2=tsc(u8"IN"),.country_ab3=tsc(u8"IND"),.country_num=356,.country_car=tsc(u8"IND"),.lang_name=tsc(u8"ଓଡ଼ିଆ"),.lang_ab=tsc(u8"or"),.lang_term=tsc(u8"ori"),.lang_lib=tsc(u8"ori")},.measurement={.measurement=1}};

inline constexpr basic_io_scatter_t<char16_t> u16time_alt_digits_storage[]{tsc(u"୦"),tsc(u"୧"),tsc(u"୨"),tsc(u"୩"),tsc(u"୪"),tsc(u"୫"),tsc(u"୬"),tsc(u"୭"),tsc(u"୮"),tsc(u"୯"),tsc(u"୧୦"),tsc(u"୧୧"),tsc(u"୧୨"),tsc(u"୧୩"),tsc(u"୧୪"),tsc(u"୧୫"),tsc(u"୧୬"),tsc(u"୧୭"),tsc(u"୧୮"),tsc(u"୧୯"),tsc(u"୨୦"),tsc(u"୨୧"),tsc(u"୨୨"),tsc(u"୨୩"),tsc(u"୨୪"),tsc(u"୨୫"),tsc(u"୨୬"),tsc(u"୨୭"),tsc(u"୨୮"),tsc(u"୨୯"),tsc(u"୩୦"),tsc(u"୩୧"),tsc(u"୩୨"),tsc(u"୩୩"),tsc(u"୩୪"),tsc(u"୩୫"),tsc(u"୩୬"),tsc(u"୩୭"),tsc(u"୩୮"),tsc(u"୩୯"),tsc(u"୪୦"),tsc(u"୪୧"),tsc(u"୪୨"),tsc(u"୪୩"),tsc(u"୪୪"),tsc(u"୪୫"),tsc(u"୪୬"),tsc(u"୪୭"),tsc(u"୪୮"),tsc(u"୪୯"),tsc(u"୫୦"),tsc(u"୫୧"),tsc(u"୫୨"),tsc(u"୫୩"),tsc(u"୫୪"),tsc(u"୫୫"),tsc(u"୫୬"),tsc(u"୫୭"),tsc(u"୫୮"),tsc(u"୫୯"),tsc(u"୬୦"),tsc(u"୬୧"),tsc(u"୬୨"),tsc(u"୬୩"),tsc(u"୬୪"),tsc(u"୬୫"),tsc(u"୬୬"),tsc(u"୬୭"),tsc(u"୬୮"),tsc(u"୬୯"),tsc(u"୭୦"),tsc(u"୭୧"),tsc(u"୭୨"),tsc(u"୭୩"),tsc(u"୭୪"),tsc(u"୭୫"),tsc(u"୭୬"),tsc(u"୭୭"),tsc(u"୭୮"),tsc(u"୭୯"),tsc(u"୮୦"),tsc(u"୮୧"),tsc(u"୮୨"),tsc(u"୮୩"),tsc(u"୮୪"),tsc(u"୮୫"),tsc(u"୮୬"),tsc(u"୮୭"),tsc(u"୮୮"),tsc(u"୮୯"),tsc(u"୯୦"),tsc(u"୯୧"),tsc(u"୯୨"),tsc(u"୯୩"),tsc(u"୯୪"),tsc(u"୯୫"),tsc(u"୯୬"),tsc(u"୯୭"),tsc(u"୯୮"),tsc(u"୯୯")};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"or_IN"),.encoding=tsc(FAST_IO_LOCALE_uENCODING),.title=tsc(u"Odia language locale for India"),.source=tsc(u"IBM AP Linux Technology Center, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"fast_io"),.email=tsc(u"bug-glibc@gnu.org;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Odia"),.territory=tsc(u"India"),.revision=tsc(u"1.0"),.date=tsc(u"2006-05-25")},.monetary={.int_curr_symbol=tsc(u"INR "),.currency_symbol=tsc(u"₹"),.mon_decimal_point=tsc(u"."),.mon_thousands_sep=tsc(u","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u"."),.thousands_sep=tsc(u","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(u"ରବି"),tsc(u"ସୋମ"),tsc(u"ମଙ୍ଗଳ"),tsc(u"ବୁଧ"),tsc(u"ଗୁରୁ"),tsc(u"ଶୁକ୍ର"),tsc(u"ଶନି")},.day={tsc(u"ରବିବାର"),tsc(u"ସୋମବାର"),tsc(u"ମଙ୍ଗଳବାର"),tsc(u"ବୁଧବାର"),tsc(u"ଗୁରୁବାର"),tsc(u"ଶୁକ୍ରବାର"),tsc(u"ଶନିବାର")},.abmon={tsc(u"ଜାନୁଆରୀ"),tsc(u"ଫେବୃଆରୀ"),tsc(u"ମାର୍ଚ୍ଚ"),tsc(u"ଅପ୍ରେଲ"),tsc(u"ମଇ"),tsc(u"ଜୁନ"),tsc(u"ଜୁଲାଇ"),tsc(u"ଅଗଷ୍ଟ"),tsc(u"ସେପ୍ଟେମ୍ବର"),tsc(u"ଅକ୍ଟୋବର"),tsc(u"ନଭେମ୍ବର"),tsc(u"ଡିସେମ୍ବର")},.mon={tsc(u"ଜାନୁଆରୀ"),tsc(u"ଫେବୃଆରୀ"),tsc(u"ମାର୍ଚ୍ଚ"),tsc(u"ଅପ୍ରେଲ"),tsc(u"ମଇ"),tsc(u"ଜୁନ"),tsc(u"ଜୁଲାଇ"),tsc(u"ଅଗଷ୍ଟ"),tsc(u"ସେପ୍ଟେମ୍ବର"),tsc(u"ଅକ୍ଟୋବର"),tsc(u"ନଭେମ୍ବର"),tsc(u"ଡିସେମ୍ବର")},.d_t_fmt=tsc(u"%Oe %B %Oy %OI:%OM:%OS %p"),.d_fmt=tsc(u"%Od-%Om-%Oy"),.t_fmt=tsc(u"%OI:%OM:%OS %p"),.t_fmt_ampm=tsc(u"%OI:%OM:%OS %p"),.date_fmt=tsc(u"%Oe %B %Oy %OI:%OM:%OS %p %Z"),.am_pm={tsc(u"AM"),tsc(u"PM")},.alt_digits={u16time_alt_digits_storage,100},.week={7,19971130,1}},.messages={.yesexpr=tsc(u"^[+1yYହ]"),.noexpr=tsc(u"^[-0nNନ]"),.yesstr=tsc(u"ହଁ"),.nostr=tsc(u"ନା")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c ;%a ;%l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"91")},.name={.name_fmt=tsc(u"%p%t%f%t%g"),.name_gen=tsc(u""),.name_miss=tsc(u"Miss."),.name_mr=tsc(u"Mr."),.name_mrs=tsc(u"Mrs."),.name_ms=tsc(u"Ms.")},.address={.postal_fmt=tsc(u"%z%c%T%s%b%e%r"),.country_name=tsc(u"ଭାରତ"),.country_ab2=tsc(u"IN"),.country_ab3=tsc(u"IND"),.country_num=356,.country_car=tsc(u"IND"),.lang_name=tsc(u"ଓଡ଼ିଆ"),.lang_ab=tsc(u"or"),.lang_term=tsc(u"ori"),.lang_lib=tsc(u"ori")},.measurement={.measurement=1}};

inline constexpr basic_io_scatter_t<char32_t> u32time_alt_digits_storage[]{tsc(U"୦"),tsc(U"୧"),tsc(U"୨"),tsc(U"୩"),tsc(U"୪"),tsc(U"୫"),tsc(U"୬"),tsc(U"୭"),tsc(U"୮"),tsc(U"୯"),tsc(U"୧୦"),tsc(U"୧୧"),tsc(U"୧୨"),tsc(U"୧୩"),tsc(U"୧୪"),tsc(U"୧୫"),tsc(U"୧୬"),tsc(U"୧୭"),tsc(U"୧୮"),tsc(U"୧୯"),tsc(U"୨୦"),tsc(U"୨୧"),tsc(U"୨୨"),tsc(U"୨୩"),tsc(U"୨୪"),tsc(U"୨୫"),tsc(U"୨୬"),tsc(U"୨୭"),tsc(U"୨୮"),tsc(U"୨୯"),tsc(U"୩୦"),tsc(U"୩୧"),tsc(U"୩୨"),tsc(U"୩୩"),tsc(U"୩୪"),tsc(U"୩୫"),tsc(U"୩୬"),tsc(U"୩୭"),tsc(U"୩୮"),tsc(U"୩୯"),tsc(U"୪୦"),tsc(U"୪୧"),tsc(U"୪୨"),tsc(U"୪୩"),tsc(U"୪୪"),tsc(U"୪୫"),tsc(U"୪୬"),tsc(U"୪୭"),tsc(U"୪୮"),tsc(U"୪୯"),tsc(U"୫୦"),tsc(U"୫୧"),tsc(U"୫୨"),tsc(U"୫୩"),tsc(U"୫୪"),tsc(U"୫୫"),tsc(U"୫୬"),tsc(U"୫୭"),tsc(U"୫୮"),tsc(U"୫୯"),tsc(U"୬୦"),tsc(U"୬୧"),tsc(U"୬୨"),tsc(U"୬୩"),tsc(U"୬୪"),tsc(U"୬୫"),tsc(U"୬୬"),tsc(U"୬୭"),tsc(U"୬୮"),tsc(U"୬୯"),tsc(U"୭୦"),tsc(U"୭୧"),tsc(U"୭୨"),tsc(U"୭୩"),tsc(U"୭୪"),tsc(U"୭୫"),tsc(U"୭୬"),tsc(U"୭୭"),tsc(U"୭୮"),tsc(U"୭୯"),tsc(U"୮୦"),tsc(U"୮୧"),tsc(U"୮୨"),tsc(U"୮୩"),tsc(U"୮୪"),tsc(U"୮୫"),tsc(U"୮୬"),tsc(U"୮୭"),tsc(U"୮୮"),tsc(U"୮୯"),tsc(U"୯୦"),tsc(U"୯୧"),tsc(U"୯୨"),tsc(U"୯୩"),tsc(U"୯୪"),tsc(U"୯୫"),tsc(U"୯୬"),tsc(U"୯୭"),tsc(U"୯୮"),tsc(U"୯୯")};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"or_IN"),.encoding=tsc(FAST_IO_LOCALE_UENCODING),.title=tsc(U"Odia language locale for India"),.source=tsc(U"IBM AP Linux Technology Center, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(U"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"fast_io"),.email=tsc(U"bug-glibc@gnu.org;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Odia"),.territory=tsc(U"India"),.revision=tsc(U"1.0"),.date=tsc(U"2006-05-25")},.monetary={.int_curr_symbol=tsc(U"INR "),.currency_symbol=tsc(U"₹"),.mon_decimal_point=tsc(U"."),.mon_thousands_sep=tsc(U","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(U"."),.thousands_sep=tsc(U","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(U"ରବି"),tsc(U"ସୋମ"),tsc(U"ମଙ୍ଗଳ"),tsc(U"ବୁଧ"),tsc(U"ଗୁରୁ"),tsc(U"ଶୁକ୍ର"),tsc(U"ଶନି")},.day={tsc(U"ରବିବାର"),tsc(U"ସୋମବାର"),tsc(U"ମଙ୍ଗଳବାର"),tsc(U"ବୁଧବାର"),tsc(U"ଗୁରୁବାର"),tsc(U"ଶୁକ୍ରବାର"),tsc(U"ଶନିବାର")},.abmon={tsc(U"ଜାନୁଆରୀ"),tsc(U"ଫେବୃଆରୀ"),tsc(U"ମାର୍ଚ୍ଚ"),tsc(U"ଅପ୍ରେଲ"),tsc(U"ମଇ"),tsc(U"ଜୁନ"),tsc(U"ଜୁଲାଇ"),tsc(U"ଅଗଷ୍ଟ"),tsc(U"ସେପ୍ଟେମ୍ବର"),tsc(U"ଅକ୍ଟୋବର"),tsc(U"ନଭେମ୍ବର"),tsc(U"ଡିସେମ୍ବର")},.mon={tsc(U"ଜାନୁଆରୀ"),tsc(U"ଫେବୃଆରୀ"),tsc(U"ମାର୍ଚ୍ଚ"),tsc(U"ଅପ୍ରେଲ"),tsc(U"ମଇ"),tsc(U"ଜୁନ"),tsc(U"ଜୁଲାଇ"),tsc(U"ଅଗଷ୍ଟ"),tsc(U"ସେପ୍ଟେମ୍ବର"),tsc(U"ଅକ୍ଟୋବର"),tsc(U"ନଭେମ୍ବର"),tsc(U"ଡିସେମ୍ବର")},.d_t_fmt=tsc(U"%Oe %B %Oy %OI:%OM:%OS %p"),.d_fmt=tsc(U"%Od-%Om-%Oy"),.t_fmt=tsc(U"%OI:%OM:%OS %p"),.t_fmt_ampm=tsc(U"%OI:%OM:%OS %p"),.date_fmt=tsc(U"%Oe %B %Oy %OI:%OM:%OS %p %Z"),.am_pm={tsc(U"AM"),tsc(U"PM")},.alt_digits={u32time_alt_digits_storage,100},.week={7,19971130,1}},.messages={.yesexpr=tsc(U"^[+1yYହ]"),.noexpr=tsc(U"^[-0nNନ]"),.yesstr=tsc(U"ହଁ"),.nostr=tsc(U"ନା")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c ;%a ;%l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"91")},.name={.name_fmt=tsc(U"%p%t%f%t%g"),.name_gen=tsc(U""),.name_miss=tsc(U"Miss."),.name_mr=tsc(U"Mr."),.name_mrs=tsc(U"Mrs."),.name_ms=tsc(U"Ms.")},.address={.postal_fmt=tsc(U"%z%c%T%s%b%e%r"),.country_name=tsc(U"ଭାରତ"),.country_ab2=tsc(U"IN"),.country_ab3=tsc(U"IND"),.country_num=356,.country_car=tsc(U"IND"),.lang_name=tsc(U"ଓଡ଼ିଆ"),.lang_ab=tsc(U"or"),.lang_term=tsc(U"ori"),.lang_lib=tsc(U"ori")},.measurement={.measurement=1}};


}
}

#include"../main.h"