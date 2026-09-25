export namespace fast_io
{
    using ::fast_io::crc32_context;
    using ::fast_io::crc32c_context;
    using ::fast_io::aes_ctx;
    using ::fast_io::aes128;
    using ::fast_io::aes192;
    using ::fast_io::aes256;
    namespace details
    {
        using ::fast_io::details::support_hardware_crc32;
        using ::fast_io::details::support_hardware_crc32c;
    }
}