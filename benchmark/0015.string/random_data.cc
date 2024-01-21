﻿#include <fast_io.h>
#include <fast_io_device.h>
#include <random>
#include <fast_io_driver/timer.h>
using namespace fast_io::io;

int main(int argc, char **argv) {
    std::size_t n{100000};
    if (1 < argc) {
        n = fast_io::to<std::size_t>(fast_io::mnp::os_c_str(argv[1]));
    }
    fast_io::timer tm(u8"ibuf_white_hole_engine");
    fast_io::u8obuf_file obf(u"ibuf_white_hole_engine.txt");
    fast_io::ibuf_white_hole_engine eng;
    std::uniform_int_distribution<std::size_t> ud(0, 61);
    std::uniform_int_distribution<std::size_t> rlen(10000, 20000);
    std::uniform_int_distribution<unsigned> spacegen(0, 5);
    for (std::size_t i{}; i != n; ++i) {
        for (std::size_t j{}, s(rlen(eng)); j != s; ++j) {
            char8_t ch(static_cast<char8_t>(ud(eng)));
            if (ch < 10u)
                ch += u8'0';
            else if (ch < 36u)
                ch = ch - 10u + u8'a';
            else
                ch = ch - 36u + u8'A';
            print(obf, fast_io::mnp::chvw(ch));
        }
        unsigned picked{spacegen(eng)};
        char8_t ch;
        if (picked == 5) {
            ch = u8'\n';
        } else {
            ch = static_cast<char8_t>(u8'\t' + picked);
        }
        print(obf, fast_io::mnp::chvw(ch));
    }
}
