#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::std::uint_least64_t rapidhash_secret_fio[]{
	0x2d358dccaa6c78a5, 0x8bb84b93962eacc9, 0x4b33a62ed433d4a3,
	0x4d5a2da51de1aa47, 0xa0761d6478bd642f, 0xe7037ed1a0b428db,
	0x90ed1765281c388c, 0xaaaaaaaaaaaaaaaa};

inline constexpr ::std::uint_least64_t
rapidhash_mix_fio(::std::uint_least64_t a, ::std::uint_least64_t b) noexcept
{
	::std::uint_least64_t high;
	auto low{::fast_io::intrinsics::umul(a, b, high)};
	return low ^ high;
}

inline ::std::uint_least64_t
rapidhash_read64_fio(void const *p) noexcept
{
	::std::uint_least64_t v;
#if defined(__GNUC__) || defined(__clang__)
	__builtin_memcpy
#else
	::std::memcpy
#endif
		(__builtin_addressof(v), p, sizeof(v));
	return v;
}

inline ::std::uint_least32_t
rapidhash_read32_fio(void const *p) noexcept
{
	::std::uint_least32_t v;
#if defined(__GNUC__) || defined(__clang__)
	__builtin_memcpy
#else
	::std::memcpy
#endif
		(__builtin_addressof(v), p, sizeof(v));
	return v;
}

inline ::std::uint_least64_t
rapidhash_64bits_fio(void const *data, ::std::size_t len) noexcept
{
	auto p{reinterpret_cast<::std::byte const *>(data)};
	::std::uint_least64_t seed{};

	// Seed initialization
	seed ^= rapidhash_mix_fio(seed ^ rapidhash_secret_fio[2],
							  rapidhash_secret_fio[1]);

	::std::uint_least64_t a{}, b{};

	if (len <= 16)
	{
		if (len >= 4)
		{
			seed ^= len;
			if (len >= 8)
			{
				a = rapidhash_read64_fio(p);
				b = rapidhash_read64_fio(p + len - 8);
			}
			else
			{
				a = rapidhash_read32_fio(p);
				b = rapidhash_read32_fio(p + len - 4);
			}
		}
		else if (len > 0)
		{
			a = (static_cast<::std::uint_least64_t>(p[0]) << 45) | p[len - 1];
			b = p[len >> 1];
		}
	}
	else
	{
		auto i{len};

		// Long input: 7 parallel accumulators, 112-byte stride
		if (i > 112)
		{
			auto s1{seed}, s2{seed}, s3{seed}, s4{seed}, s5{seed}, s6{seed};
			do
			{
				seed = rapidhash_mix_fio(rapidhash_read64_fio(p) ^ rapidhash_secret_fio[0],
										 rapidhash_read64_fio(p + 8) ^ seed);
				s1 = rapidhash_mix_fio(rapidhash_read64_fio(p + 16) ^ rapidhash_secret_fio[1],
									   rapidhash_read64_fio(p + 24) ^ s1);
				s2 = rapidhash_mix_fio(rapidhash_read64_fio(p + 32) ^ rapidhash_secret_fio[2],
									   rapidhash_read64_fio(p + 40) ^ s2);
				s3 = rapidhash_mix_fio(rapidhash_read64_fio(p + 48) ^ rapidhash_secret_fio[3],
									   rapidhash_read64_fio(p + 56) ^ s3);
				s4 = rapidhash_mix_fio(rapidhash_read64_fio(p + 64) ^ rapidhash_secret_fio[4],
									   rapidhash_read64_fio(p + 72) ^ s4);
				s5 = rapidhash_mix_fio(rapidhash_read64_fio(p + 80) ^ rapidhash_secret_fio[5],
									   rapidhash_read64_fio(p + 88) ^ s5);
				s6 = rapidhash_mix_fio(rapidhash_read64_fio(p + 96) ^ rapidhash_secret_fio[6],
									   rapidhash_read64_fio(p + 104) ^ s6);
				p += 112;
				i -= 112;
			} while (i > 112);
			seed ^= s1;
			s2 ^= s3;
			s4 ^= s5;
			seed ^= s6;
			s2 ^= s4;
			seed ^= s2;
		}

		// Medium input: cascading mix chain
		if (i > 16)
		{
			seed = rapidhash_mix_fio(rapidhash_read64_fio(p) ^ rapidhash_secret_fio[2],
									 rapidhash_read64_fio(p + 8) ^ seed);
			if (i > 32)
			{
				seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 16) ^ rapidhash_secret_fio[2],
										 rapidhash_read64_fio(p + 24) ^ seed);
				if (i > 48)
				{
					seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 32) ^ rapidhash_secret_fio[1],
											 rapidhash_read64_fio(p + 40) ^ seed);
					if (i > 64)
					{
						seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 48) ^ rapidhash_secret_fio[1],
												 rapidhash_read64_fio(p + 56) ^ seed);
						if (i > 80)
						{
							seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 64) ^ rapidhash_secret_fio[2],
													 rapidhash_read64_fio(p + 72) ^ seed);
							if (i > 96)
							{
								seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 80) ^ rapidhash_secret_fio[1],
														 rapidhash_read64_fio(p + 88) ^ seed);
							}
						}
					}
				}
			}
		}

		// Tail
		a = rapidhash_read64_fio(p + i - 16) ^ i;
		b = rapidhash_read64_fio(p + i - 8);
	}

	// Final avalanche
	a ^= rapidhash_secret_fio[1];
	b ^= seed;
	::std::uint_least64_t high;
	a = ::fast_io::intrinsics::umul(a, b, high);
	b = high;
	return rapidhash_mix_fio(a ^ rapidhash_secret_fio[7],
							 b ^ rapidhash_secret_fio[1] ^ len);
}

} // namespace details

} // namespace fast_io
