export namespace fast_io
{
    using ::fast_io::crc32_context;
    using ::fast_io::crc32c_context;
    using ::fast_io::aes_ctx;
    using ::fast_io::aes128;
    using ::fast_io::aes192;
    using ::fast_io::aes256;
    using ::fast_io::ed25519;
    namespace diffie_hellman
    {
        using ::fast_io::diffie_hellman::x25519;
    }
    namespace curve25519
    {
        using ::fast_io::curve25519::field_number;
        using ::fast_io::curve25519::edp_blinding_context;
        using ::fast_io::curve25519::ed25519_verify_context;
        using ::fast_io::curve25519::default_blinding;
    }
}