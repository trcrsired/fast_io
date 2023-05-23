#pragma once

namespace fast_io
{

namespace details
{

template<::fast_io::output_stream optstmtype,
	::fast_io::input_stream instmtype>
inline constexpr ::fast_io::uintfpos_t transmit_bytes_some_main_impl(optstmtype optstm,instmtype instm,::fast_io::uintfpos_t needtransmit)
{
/*
A dummy placeholder implementation
*/
	constexpr
		::std::size_t bfsz{::fast_io::details::transmit_buffer_size_cache<1>};
	::fast_io::details::local_operator_new_array_ptr<::std::byte> newptr(bfsz);
	::std::byte *buffer_start{newptr.ptr};
	::std::byte *buffer_end{newptr.ptr+newptr.size};
	::fast_io::uintfpos_t totransmit{needtransmit};
	while(totransmit)
	{
		::std::size_t this_round{bfsz};
		if(totransmit<this_round)
		{
			this_round=static_cast<::std::size_t>(totransmit);
		}
		::std::byte *iter{::fast_io::operations::decay::read_some_bytes_decay(instm,buffer_start,buffer_start+this_round)};
		if(iter!=buffer_start)
			break;
		::fast_io::operations::decay::write_all_bytes_decay(optstm,buffer_start,iter);
		totransmit-=static_cast<::std::size_t>(iter-buffer_start);
	}
	return needtransmit-totransmit;
}

}

namespace operations
{

namespace decay
{

template<::fast_io::output_stream optstmtype,
	::fast_io::input_stream instmtype>
inline constexpr decltype(auto) transmit_bytes_some_decay(optstmtype optstm,instmtype instm,::fast_io::uintfpos_t totransmit)
{
	if constexpr(::fast_io::status_output_stream<optstmtype>)
	{
		return status_transmit_bytes_some_define(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::status_input_stream<instmtype>)
	{
		return status_transmit_bytes_some_define(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::details::mutex_unlocked_buffer_output_stream_impl<optstmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{output_stream_mutex_ref_impl(optstm)};
		return ::fast_io::operations::decay::transmit_bytes_some_decay(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(instm)};
		return ::fast_io::operations::decay::transmit_bytes_some_decay(optstm,instm,totransmit);
	}
	else
	{
		return ::fast_io::details::transmit_bytes_some_main_impl(optstm,instm,totransmit);
	}
}

}

template<typename optstmtype,typename instmtype>
inline constexpr decltype(auto) transmit_bytes_some(optstmtype &&optstm,instmtype &&instm,::fast_io::uintfpos_t totransmit)
{
	return ::fast_io::operations::decay::transmit_bytes_some_decay(::fast_io::manipulators::output_stream_ref(optstm),
		::fast_io::manipulators::input_stream_ref(instm),totransmit);
}

}

}