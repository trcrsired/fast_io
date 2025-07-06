#pragma once

#include "fast_io_dsal/deque.h"

namespace fast_io::containers {
    template<::std::movable T, typename allocator>
    class deque;

    namespace details {
        template<typename A>
        concept mini_alloc = requires(A &a)
        {
            typename A::value_type;
            a.allocate(0uz);
        };

        // 用于从参数包中获得前两个对象（只有两个）的引用的辅助函数
#if not defined(__cpp_pack_indexing)
        template<typename Tuple>
        inline constexpr auto get_first_two(Tuple args) noexcept {
            auto &first = ::std::get<0uz>(args);
            auto &second = ::std::get<1uz>(args);
            struct iter_ref_pair {
                decltype(first) &begin;
                decltype(second) &end;
            };
            return iter_ref_pair{first, second};
        }
#else
#if defined(__cpp_pack_indexing) // make clang happy
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++26-extensions"
#endif
        template <typename... Args>
        inline constexpr auto get(Args &&...args) noexcept
        {
            auto &first = args...[0uz];
            auto &second = args...[1uz];
            struct iter_ref_pair
            {
                decltype(first) &begin;
                decltype(second) &end;
            };
            return iter_ref_pair{first, second};
        }
#if defined(__cpp_pack_indexing)
#pragma clang diagnostic pop
#endif
#endif


        template<typename T>
        inline constexpr ::std::size_t block_elements_v = 16uz > 4096uz / sizeof(T) ? 16uz : 4096uz / sizeof(T);

        template<typename T>
        inline constexpr auto to_address(T const t) noexcept {
            return ::std::to_address(t);
        }

        inline constexpr auto to_address(::std::nullptr_t) noexcept {
            return nullptr;
        }

        // 构造函数和赋值用，计算如何分配和构造
        template<typename T>
        inline constexpr auto calc_cap(::std::size_t const size) noexcept {
            auto const block_elems = block_elements_v<T>;
            struct cap_t {
                ::std::size_t block_size; // 需要分配多少block
                ::std::size_t full_blocks; // 分配了几个完整block
                ::std::size_t rem_elems; // 剩下的不完整block有多少元素
            };
            return cap_t{(size + block_elems - 1uz) / block_elems, size / block_elems, size % block_elems};
        }

        // 该函数计算位置，参数front_size是起始位置和块首的距离，pos是目标位置
        // 对于负数pos返回负数位置
        template<typename T>
        inline constexpr auto calc_pos(::std::ptrdiff_t const front_size, ::std::ptrdiff_t const pos) noexcept {
            ::std::ptrdiff_t const block_elems = block_elements_v<T>;
            struct pos_t {
                ::std::ptrdiff_t block_step; // 移动块的步数
                ::std::ptrdiff_t elem_step; // 移动元素的步数（相对于块首）
            };
            if (pos >= 0z) {
                auto const new_pos = pos + front_size;
                return pos_t{new_pos / block_elems, new_pos % block_elems};
            } else {
                auto const new_pos = pos + front_size - block_elems + 1z;
                return pos_t{new_pos / block_elems, new_pos % block_elems - 1z + block_elems};
            }
        }

        template<typename T>
        inline constexpr auto calc_pos(::std::size_t const front_size, ::std::size_t const pos) noexcept {
            ::std::size_t const block_elems = block_elements_v<T>;
            struct pos_t {
                ::std::size_t block_step; // 移动块的步数
                ::std::size_t elem_step; // 移动元素的步数（相对于块首）
            };
            auto const new_pos = pos + front_size;
            return pos_t{new_pos / block_elems, new_pos % block_elems};
        }


        template<typename T, typename Block>
        class bucket_type;

        template<typename T, typename Block>
        class bucket_iterator {
            using RConstT = ::std::remove_const_t<T>;

            friend bucket_type<RConstT, Block>;
            friend bucket_type<T, Block>;
            friend bucket_iterator<T const, Block>;

            Block *block_elem_begin_{};
            Block *block_elem_end_{};
            Block *block_elem_curr_{};
            RConstT *elem_begin_begin_{};
            RConstT *elem_begin_end_{};
            RConstT *elem_end_begin_{};
            RConstT *elem_end_end_{};
            RConstT *elem_curr_begin_{};
            RConstT *elem_curr_end_{};

            constexpr bucket_iterator(Block *const block_elem_begin, Block *const block_elem_end,
                                      Block *const block_elem_curr,
                                      RConstT *const elem_begin_begin, RConstT *const elem_begin_end,
                                      RConstT *const elem_end_begin, RConstT *const elem_end_end,
                                      RConstT *const elem_curr_begin, RConstT *const elem_curr_end) noexcept
                : block_elem_begin_(block_elem_begin), block_elem_end_(block_elem_end),
                  block_elem_curr_(block_elem_curr),
                  elem_begin_begin_(elem_begin_begin), elem_begin_end_(elem_begin_end), elem_end_begin_(elem_end_begin),
                  elem_end_end_(elem_end_end), elem_curr_begin_(elem_curr_begin), elem_curr_end_(elem_curr_end) {
            }

            [[nodiscard]] constexpr bucket_iterator<RConstT, Block> remove_const_() const
                requires(::std::is_const_v<T>) {
                return {
                    block_elem_begin_, block_elem_end_, block_elem_curr_, elem_begin_begin_, elem_begin_end_,
                    elem_end_begin_, elem_end_end_, elem_curr_begin_, elem_curr_end_
                };
            }

            constexpr bucket_iterator &plus_and_assign_(::std::ptrdiff_t const pos) noexcept {
                block_elem_curr_ += pos;
                if (block_elem_curr_ + 1uz == block_elem_end_) {
                    elem_curr_begin_ = elem_end_begin_;
                    elem_curr_end_ = elem_end_end_;
                } else if (block_elem_curr_ == block_elem_begin_) {
                    elem_curr_begin_ = elem_begin_begin_;
                    elem_curr_end_ = elem_begin_end_;
                } else {
                    elem_curr_begin_ = *block_elem_begin_;
                    elem_curr_end_ = elem_begin_begin_ + block_elements_v<T>;
                }
                if (!(block_elem_curr_ < block_elem_end_ && block_elem_curr_ >= block_elem_begin_)) [[unlikely]] {
                    fast_terminate();
                }
                return *this;
            }

        public:
            using difference_type = ::std::ptrdiff_t;
            using value_type = ::std::span<T>;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = ::std::random_access_iterator_tag;

            constexpr bucket_iterator() noexcept = default;

            constexpr bucket_iterator(bucket_iterator const &other) noexcept = default;

            constexpr bucket_iterator &operator=(bucket_iterator const &other) noexcept = default;

            constexpr ~bucket_iterator() = default;

            constexpr bucket_iterator &operator++() noexcept {
                ++block_elem_curr_;
                if (block_elem_curr_ + 1uz == block_elem_end_) {
                    elem_curr_begin_ = elem_end_begin_;
                    elem_curr_end_ = elem_end_end_;
                } else {
                    elem_curr_begin_ = *block_elem_begin_;
                    elem_curr_end_ = elem_begin_begin_ + block_elements_v<T>;
                }
                if (!(block_elem_curr_ < block_elem_end_)) [[unlikely]] {
                    fast_terminate();
                }
                return *this;
            }

            constexpr bucket_iterator operator++(int) noexcept {
#if defined(__cpp_auto_cast)
                return ++auto{*this};
#else
                auto temp = *this;
                ++temp;
                return temp;
#endif
            }

            constexpr bucket_iterator &operator--() noexcept {
                --block_elem_curr_;
                if (block_elem_curr_ == block_elem_begin_) {
                    elem_curr_begin_ = elem_begin_begin_;
                    elem_curr_end_ = elem_begin_end_;
                } else {
                    elem_begin_begin_ = *(block_elem_begin_ - 1uz);
                    elem_begin_end_ = elem_begin_begin_ + block_elements_v<T>;
                }
                if (!(block_elem_curr_ >= block_elem_begin_)) [[unlikely]] {
                    fast_terminate();
                }
                return *this;
            }

            constexpr bucket_iterator operator--(int) noexcept {
#if defined(__cpp_auto_cast)
                return --auto{*this};
#else
                auto temp = *this;
                --temp;
                return temp;
#endif
            }

            constexpr bool operator==(bucket_iterator const &other) const noexcept {
                return block_elem_curr_ == other.block_elem_curr_;
            }

            constexpr ::std::strong_ordering operator<=>(bucket_iterator const &other) const noexcept {
                return block_elem_curr_ <=> other.block_elem_curr_;
            }

            constexpr ::std::ptrdiff_t operator-(bucket_iterator const &other) const noexcept {
                return block_elem_curr_ - other.block_elem_curr_;
            }

            constexpr ::std::span<T> operator[](::std::ptrdiff_t const pos) {
#if defined(__cpp_auto_cast)
                return *(auto{*this} += pos);
#else
                auto temp = *this;
                temp += pos;
                return *temp;
#endif
            }

            constexpr ::std::span<T> operator[](::std::ptrdiff_t const pos) const noexcept {
#if defined(__cpp_auto_cast)
                return *(auto{*this} += pos);
#else
                auto temp = *this;
                temp += pos;
                return *temp;
#endif
            }

            constexpr value_type operator*() noexcept {
                return {elem_curr_begin_, elem_curr_end_};
            }

            constexpr value_type operator*() const noexcept {
                return {elem_curr_begin_, elem_curr_end_};
            }

            constexpr bucket_iterator &operator+=(::std::ptrdiff_t const pos) noexcept {
                return plus_and_assign_(pos);
            }

            constexpr bucket_iterator &operator-=(::std::ptrdiff_t const pos) noexcept {
                return plus_and_assign_(-pos);
            }

            friend constexpr bucket_iterator operator+(bucket_iterator const &it, ::std::ptrdiff_t const pos) noexcept {
#if defined(__cpp_auto_cast)
                return auto{it}.plus_and_assign_(pos);
#else
                auto temp = it;
                temp.plus_and_assign_(pos);
                return temp;
#endif
            }

            friend constexpr bucket_iterator operator+(::std::ptrdiff_t const pos, bucket_iterator const &it) noexcept {
                return it + pos;
            }

            friend constexpr bucket_iterator operator-(::std::ptrdiff_t const pos, bucket_iterator const &it) noexcept {
                return it + (-pos);
            }

            friend constexpr bucket_iterator operator-(bucket_iterator const &it, ::std::ptrdiff_t pos) noexcept {
                return it + (-pos);
            }

            constexpr explicit operator bucket_iterator<T const, Block>() const
                requires(not::std::is_const_v<T>) {
                return {
                    block_elem_begin_, block_elem_end_, block_elem_curr_, elem_begin_begin_, elem_begin_end_,
                    elem_end_begin_, elem_end_end_, elem_curr_begin_, elem_curr_end_
                };
            }
        };

#if !defined(NDEBUG)
        static_assert(::std::random_access_iterator<bucket_iterator<int, int *> >);
        static_assert(::std::random_access_iterator<bucket_iterator<const int, int *> >);
#endif

        template<typename T, typename Block>
        class bucket_type : public ::std::ranges::view_interface<bucket_type<T, Block> > {
            using RConstT = ::std::remove_const_t<T>;

            template<::std::movable U, typename Allocator>
            friend class ::fast_io::containers::deque;
            friend bucket_type<::std::remove_const_t<T>, Block>;

            Block *block_elem_begin_{};
            Block *block_elem_end_{};
            RConstT *elem_begin_begin_{};
            RConstT *elem_begin_end_{};
            RConstT *elem_end_begin_{};
            RConstT *elem_end_end_{};

            template<typename U, typename V>
            constexpr bucket_type(U const block_elem_begin, U const block_elem_end, V const elem_begin_begin,
                                  V const elem_begin_end, V const elem_end_begin, V const elem_end_end) noexcept
                : block_elem_begin_(::std::to_address(block_elem_begin)),
                  block_elem_end_(::std::to_address(block_elem_end)),
                  elem_begin_begin_(::std::to_address(elem_begin_begin)),
                  elem_begin_end_(::std::to_address(elem_begin_end)),
                  elem_end_begin_(::std::to_address(elem_end_begin)), elem_end_end_(::std::to_address(elem_end_end)) {
            }

            constexpr ::std::span<T> at_impl(::std::size_t const pos) const noexcept {
                if (!(block_elem_begin_ + pos <= block_elem_end_)) [[unlikely]] {
                    fast_terminate();
                }
                if (pos == 0uz) {
                    return {elem_begin_begin_, elem_begin_end_};
                } else if (block_elem_begin_ + pos + 1uz == block_elem_end_) {
                    return {elem_end_begin_, elem_end_end_};
                } else {
                    auto const begin = *(block_elem_begin_ + pos);
                    return {begin, begin + block_elements_v<T>};
                }
            }

        public:
            using value_type = ::std::span<T>;
            using pointer = value_type *;
            using reference = value_type &;
            using const_pointer = value_type const *;
            using const_reference = value_type const &;
            using size_type = ::std::size_t;
            using difference_type = ::std::ptrdiff_t;
            using iterator = bucket_iterator<T, Block>;
            using const_iterator = bucket_iterator<T const, Block>;
            using reverse_iterator = ::std::reverse_iterator<iterator>;
            using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

            constexpr bucket_type() = default;

            constexpr ~bucket_type() = default;

            constexpr bucket_type(bucket_type const &) = default;

            constexpr bucket_type &operator=(bucket_type const &) = default;

            constexpr ::std::size_t size() const noexcept {
                return block_elem_end_ - block_elem_begin_;
            }

            // empty and operator bool provided by view_interface

            // Since bucket_iterator is not a continuous iterator,
            // view_interface does not provide the member function data
            // constexpr void data() const noexcept = delete;

            constexpr ::std::span<T> front() const noexcept {
                return {elem_begin_begin_, elem_begin_end_};
            }

            constexpr ::std::span<T const> front() noexcept
                requires(not::std::is_const_v<T>) {
                return {elem_begin_begin_, elem_begin_end_};
            }

            constexpr ::std::span<T> back() const noexcept {
                return {elem_end_begin_, elem_end_end_};
            }

            constexpr ::std::span<T const> back() noexcept
                requires(not::std::is_const_v<T>) {
                return {elem_end_begin_, elem_end_end_};
            }

            constexpr ::std::span<T> at(::std::size_t const pos) noexcept {
                return at_impl(pos);
            }

            constexpr ::std::span<const T> at(::std::size_t const pos) const noexcept
                requires(not::std::is_const_v<T>) {
                auto const s = at_impl(pos);
                return {s.data(), s.size()};
            }

            constexpr const_iterator begin() const noexcept {
                return {
                    block_elem_begin_, block_elem_end_, block_elem_begin_, elem_begin_begin_, elem_begin_end_,
                    elem_end_begin_, elem_end_end_, elem_begin_begin_, elem_begin_end_
                };
            }

            constexpr const_iterator end() const noexcept {
                if (block_elem_begin_ == block_elem_end_) {
                    return {
                        block_elem_begin_, block_elem_end_, block_elem_end_, elem_begin_begin_, elem_begin_end_,
                        elem_end_begin_, elem_end_end_, elem_end_begin_, elem_end_end_
                    };
                } else {
                    return {
                        block_elem_begin_, block_elem_end_, block_elem_end_ - 1uz, elem_begin_begin_, elem_begin_end_,
                        elem_end_begin_, elem_end_end_, elem_end_begin_, elem_end_end_
                    };
                }
            }

            constexpr iterator begin() noexcept {
                return static_cast<bucket_type const &>(*this).begin().remove_const_();
            }

            constexpr iterator end() noexcept {
                return static_cast<bucket_type const &>(*this).end().remove_const_();
            }

            constexpr const_iterator cbegin() const noexcept {
                return begin();
            }

            constexpr const_iterator cend() const noexcept {
                return end();
            }

            constexpr auto rbegin() noexcept {
                return reverse_iterator{end()};
            }

            constexpr auto rend() noexcept {
                return reverse_iterator{begin()};
            }

            constexpr auto rbegin() const noexcept {
                return const_reverse_iterator{end()};
            }

            constexpr auto rend() const noexcept {
                return const_reverse_iterator{begin()};
            }

            constexpr auto rcbegin() const noexcept {
                return const_reverse_iterator{end()};
            }

            constexpr auto rcend() const noexcept {
                return const_reverse_iterator{begin()};
            }

            constexpr operator bucket_type<T const, Block>() const
                requires(not::std::is_const_v<T>) {
                return {
                    block_elem_begin_, block_elem_end_, elem_begin_begin_, elem_begin_end_, elem_end_begin_,
                    elem_end_end_
                };
            }
        };


        template<typename T, typename Block>
        class deque_iterator {
            using RConstT = ::std::remove_const_t<T>;

            template<::std::movable U, typename Allocator>
            friend class ::fast_io::containers::deque;
            friend deque_iterator<RConstT, Block>;
            friend deque_iterator<T const, Block>;

            Block *block_elem_curr_{};
            Block *block_elem_end_{};
            RConstT *elem_begin_{};
            RConstT *elem_curr_{};

            template<typename U, typename V>
            constexpr deque_iterator(Block *block_curr, Block *block_end, U const begin, V const pos) noexcept
                : block_elem_curr_(block_curr), block_elem_end_(block_end), elem_begin_(details::to_address(begin)),
                  elem_curr_(details::to_address(pos)) {
            }

            constexpr deque_iterator<RConstT, Block> remove_const_() const noexcept
                requires(::std::is_const_v<T>) {
                return {block_elem_curr_, block_elem_end_, elem_begin_, elem_curr_};
            }

            constexpr T &at_impl_(::std::ptrdiff_t const pos) const noexcept {
                auto const [block_step, elem_step] = details::calc_pos<T>(elem_curr_ - elem_begin_, pos);
                auto const target_block = block_elem_curr_ + block_step;
                if (!(target_block < block_elem_end_)) [[unlikely]] {
                    fast_terminate();
                }
                return *((*target_block) + elem_step);
            }

            constexpr deque_iterator &plus_and_assign_(::std::ptrdiff_t const pos) noexcept {
                if (pos != 0z) {
                    auto const [block_step, elem_step] = details::calc_pos<T>(elem_curr_ - elem_begin_, pos);
                    auto const target_block = block_elem_curr_ + block_step;
                    if (target_block < block_elem_end_) {
                        block_elem_curr_ = target_block;
                        elem_begin_ = ::std::to_address(*target_block);
                        elem_curr_ = elem_begin_ + elem_step;
                    } else {
                        if (!(target_block == block_elem_end_)) [[unlikely]] {
                            fast_terminate();
                        }
                        if (!(elem_step == 0uz)) [[unlikely]] {
                            fast_terminate();
                        }
                        block_elem_curr_ = target_block - 1uz;
                        elem_begin_ = ::std::to_address(*(target_block - 1uz));
                        elem_curr_ = elem_begin_ + details::block_elements_v<T>;
                    }
                }
                return *this;
            }

        public:
            using difference_type = ::std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = ::std::random_access_iterator_tag;

            constexpr deque_iterator() noexcept = default;

            constexpr deque_iterator(deque_iterator const &other) noexcept = default;

            constexpr deque_iterator &operator=(deque_iterator const &other) noexcept = default;

            constexpr ~deque_iterator() = default;

            constexpr bool operator==(deque_iterator const &other) const noexcept {
                return elem_curr_ == other.elem_curr_;
            }

            constexpr ::std::strong_ordering operator<=>(deque_iterator const &other) const noexcept {
                if (!(block_elem_end_ == other.block_elem_end_)) [[unlikely]] {
                    fast_terminate();
                }
                if (block_elem_curr_ < other.block_elem_curr_)
                    return ::std::strong_ordering::less;
                if (block_elem_curr_ > other.block_elem_curr_)
                    return ::std::strong_ordering::greater;
                if (elem_curr_ < other.elem_curr_)
                    return ::std::strong_ordering::less;
                if (elem_curr_ > other.elem_curr_)
                    return ::std::strong_ordering::greater;
                return ::std::strong_ordering::equal;
            }

            constexpr T &operator*() noexcept {
                if (!(elem_curr_ != elem_begin_ + details::block_elements_v<T>)) [[unlikely]] {
                    fast_terminate();
                }
                return *elem_curr_;
            }

            constexpr T &operator*() const noexcept {
                if (!(elem_curr_ != elem_begin_ + details::block_elements_v<T>)) [[unlikely]] {
                    fast_terminate();
                }
                return *elem_curr_;
            }

            constexpr deque_iterator &operator++() noexcept {
                // 空deque的迭代器不能自增，不需要考虑
                if (!(elem_curr_ != elem_begin_ + details::block_elements_v<T>)) [[unlikely]] {
                    fast_terminate();
                }
                ++elem_curr_;
                if (elem_curr_ == elem_begin_ + details::block_elements_v<T>) {
                    if (block_elem_curr_ + 1uz != block_elem_end_) {
                        ++block_elem_curr_;
                        elem_begin_ = ::std::to_address(*block_elem_curr_);
                        elem_curr_ = elem_begin_;
                    }
                }
                return *this;
            }

            constexpr deque_iterator operator++(int) noexcept {
#if defined(__cpp_auto_cast)
        return ++auto{*this};
#else
                auto temp(*this);
                ++temp;
                return temp;
#endif
            }

            constexpr deque_iterator &operator--() noexcept {
                if (elem_curr_ == elem_begin_) {
                    --block_elem_curr_;
                    elem_begin_ = ::std::to_address(*block_elem_curr_);
                    elem_curr_ = elem_begin_ + details::block_elements_v<T>;
                }
                --elem_curr_;
                return *this;
            }

            constexpr deque_iterator operator--(int) noexcept {
#if defined(__cpp_auto_cast)
        return --auto{*this};
#else
                auto temp(*this);
                --temp;
                return temp;
#endif
            }

            constexpr T &operator[](::std::ptrdiff_t const pos) noexcept {
                return at_impl_(pos);
            }

            constexpr T &operator[](::std::ptrdiff_t const pos) const noexcept {
                return at_impl_(pos);
            }

            friend constexpr ::std::ptrdiff_t operator-(deque_iterator const &lhs, deque_iterator const &rhs) noexcept {
                if (!(lhs.block_elem_end_ == rhs.block_elem_end_)) [[unlikely]] {
                    fast_terminate();
                }
                auto const block_size = lhs.block_elem_curr_ - rhs.block_elem_curr_;
                return block_size * static_cast<::std::ptrdiff_t>(block_elements_v<T>) + lhs.elem_curr_ - lhs.
                       elem_begin_ -
                       (rhs.elem_curr_ - rhs.elem_begin_);
            }

            constexpr deque_iterator &operator+=(::std::ptrdiff_t const pos) noexcept {
                return plus_and_assign_(pos);
            }

            friend constexpr deque_iterator operator+(deque_iterator const &it, ::std::ptrdiff_t const pos) noexcept {
#if defined(__cpp_auto_cast)
        return auto{it}.plus_and_assign_(pos);
#else
                auto temp = it;
                temp.plus_and_assign_(pos);
                return temp;
#endif
            }

            friend constexpr deque_iterator operator+(::std::ptrdiff_t const pos, deque_iterator const &it) noexcept {
                return it + pos;
            }

            constexpr deque_iterator &operator-=(::std::ptrdiff_t const pos) noexcept {
                return plus_and_assign_(-pos);
            }

            friend constexpr deque_iterator operator-(deque_iterator const &it, ::std::ptrdiff_t const pos) noexcept {
                return it + (-pos);
            }

            friend constexpr deque_iterator operator-(::std::ptrdiff_t const pos, deque_iterator const &it) noexcept {
                return it + (-pos);
            }

            constexpr operator deque_iterator<T const, Block>() const
                requires(not::std::is_const_v<T>) {
                return {block_elem_curr_, block_elem_end_, elem_begin_, elem_curr_};
            }
        };
    }

    template<::std::movable T, typename allocator>
    class deque {
        using typed_allocator_type = typed_generic_allocator_adapter<allocator, T>;
        using typed_allocator_pointer_type = typed_generic_allocator_adapter<allocator, T *>;

        static constexpr bool is_default_operation_ = requires(allocator &a)
        {
            a.construct(static_cast<T *>(nullptr));
        };

        // 给natvis使用，注意不要在其它函数中使用它，以支持使用不完整类型实例化。
        static inline constexpr ::std::size_t block_elements = details::block_elements_v<T>;

        using Block = T *;
        using BlockFP = Block *;

        // 块数组的起始地址
        BlockFP block_ctrl_begin_fancy_{};
        // 块数组的结束地址
        Block *block_ctrl_end_{};
        // 已分配块的起始地址
        Block *block_alloc_begin_{};
        // 已分配块结束地址
        Block *block_alloc_end_{};
        // 已用块的首地址
        Block *block_elem_begin_{};
        // 已用块的结束地址
        Block *block_elem_end_{};
        // 首个有效块的起始分配地址
        T *elem_begin_first_{};
        // 首个有效块的首元素地址
        T *elem_begin_begin_{};
        // 首个有效块的结束分配以及尾后元素地址
        T *elem_begin_end_{};
        // 有效末尾块的起始分配以及起始元素地址
        T *elem_end_begin_{};
        // 有效末尾块的尾后元素地址
        T *elem_end_end_{};
        // 有效末尾块的结束分配地址
        T *elem_end_last_{};

        [[nodiscard]] constexpr Block *block_ctrl_begin_() const noexcept {
            return details::to_address(block_ctrl_begin_fancy_);
        }

        constexpr void dealloc_block_(Block b) noexcept {
            if constexpr (typed_allocator_type::has_deallocate) {
                typed_allocator_type::deallocate(b);
            } else {
                typed_allocator_type::deallocate_n(b, details::block_elements_v<T>);
            }
        }

        constexpr Block alloc_block_() {
            return typed_allocator_type::allocate(details::block_elements_v<T>);
        }

        constexpr BlockFP alloc_ctrl_(::std::size_t const size) {
            return typed_allocator_pointer_type::allocate(size);
        }

        constexpr void dealloc_ctrl_() noexcept {
            if (block_ctrl_end_ != block_ctrl_begin_()) {
                if constexpr (typed_allocator_type::has_deallocate) {
                    typed_allocator_pointer_type::deallocate(block_ctrl_begin_fancy_);
                } else {
                    typed_allocator_pointer_type::deallocate_n(block_ctrl_begin_fancy_,
                                                               static_cast<::std::size_t>(
                                                                   block_ctrl_end_ - block_ctrl_begin_()));
                }
            }
        }

        constexpr void destroy_elems_() const noexcept
            requires ::std::is_trivially_destructible_v<T> && is_default_operation_ {
            /* */
        }

        // 空deque安全，但执行后必须手动维护状态合法
        constexpr void destroy_elems_() noexcept {
            // 4种情况，0，1，2，3+个块有元素
            auto const block_size = block_elem_size_();
            if (block_size) {
                for (auto const &i: ::std::ranges::subrange{elem_begin_begin_, elem_begin_end_}) {
                    ::std::destroy_at(::std::addressof(i));
                }
            }
            // 清理中间的块
            if (block_size > 2uz) {
                for (auto const block_begin: ::std::ranges::subrange{block_elem_begin_ + 1uz, block_elem_end_ - 1uz}) {
                    for (auto const &i:
                         ::std::ranges::subrange{block_begin, block_begin + details::block_elements_v<T>}) {
                        std::destroy_at(::std::addressof((i)));
                    }
                }
            }
            if (block_size > 1uz) {
                for (auto const &i: ::std::ranges::subrange{elem_end_begin_, elem_end_end_}) {
                    std::destroy_at(::std::addressof((i)));
                }
            }
        }

        // 完全等于析构函数
        constexpr void destroy_() noexcept {
            destroy_elems_();
            // 清理块数组
            for (auto const i: ::std::ranges::subrange{block_alloc_begin_, block_alloc_end_}) {
                dealloc_block_(i);
            }
            dealloc_ctrl_();
        }

        template<typename U, typename V, typename W>
        constexpr void elem_begin_(U const begin, V const end, W const first) noexcept {
            elem_begin_begin_ = details::to_address(begin);
            elem_begin_end_ = details::to_address(end);
            elem_begin_first_ = details::to_address(first);
        }

        template<typename U, typename V, typename W>
        constexpr void elem_end_(U const begin, V const end, W const last) noexcept {
            elem_end_begin_ = details::to_address(begin);
            elem_end_end_ = details::to_address(end);
            elem_end_last_ = details::to_address(last);
        }

        [[nodiscard]] constexpr ::std::size_t block_elem_size_() const noexcept {
            return block_elem_end_ - block_elem_begin_;
        }

        [[nodiscard]] constexpr ::std::size_t block_ctrl_size_() const noexcept {
            return block_ctrl_end_ - block_ctrl_begin_();
        }

        [[nodiscard]] constexpr ::std::size_t block_alloc_size_() const noexcept {
            return block_alloc_end_ - block_alloc_begin_;
        }

        constexpr void swap(deque &other) noexcept {
            using ::std::ranges::swap;
            swap(block_ctrl_begin_fancy_, other.block_ctrl_begin_fancy_);
            swap(block_ctrl_end_, other.block_ctrl_end_);
            swap(block_alloc_begin_, other.block_alloc_begin_);
            swap(block_alloc_end_, other.block_alloc_end_);
            swap(block_elem_begin_, other.block_elem_begin_);
            swap(block_elem_end_, other.block_elem_end_);
            swap(elem_begin_first_, other.elem_begin_first_);
            swap(elem_begin_begin_, other.elem_begin_begin_);
            swap(elem_begin_end_, other.elem_begin_end_);
            swap(elem_end_begin_, other.elem_end_begin_);
            swap(elem_end_end_, other.elem_end_end_);
            swap(elem_end_last_, other.elem_end_last_);
        }

    public:
        using value_type = T;
        using pointer = value_type *;
        using reference = value_type &;
        using const_pointer = pointer const;
        using const_reference = value_type const &;
        using size_type = ::std::size_t;
        using difference_type = ::std::ptrdiff_t;
        using iterator = details::deque_iterator<T, Block>;
        using reverse_iterator = ::std::reverse_iterator<details::deque_iterator<T, Block> >;
        using const_iterator = details::deque_iterator<T const, Block>;
        using const_reverse_iterator = ::std::reverse_iterator<details::deque_iterator<T const, Block> >;
        using bucket_type = details::bucket_type<T, Block>;
        using const_bucket_type = details::bucket_type<T const, Block>;
        using allocator_type = allocator;

        constexpr bucket_type buckets() noexcept {
            return {
                block_elem_begin_, block_elem_end_, elem_begin_begin_, elem_begin_end_, elem_end_begin_, elem_end_end_
            };
        }

        constexpr const_bucket_type buckets() const noexcept {
            return {
                block_elem_begin_, block_elem_end_, elem_begin_begin_, elem_begin_end_, elem_end_begin_, elem_end_end_
            };
        }

        constexpr ~deque() {
            destroy_();
        }

        constexpr bool empty() const noexcept {
            return elem_begin_begin_ == nullptr;
        }

        constexpr void clear() noexcept {
            destroy_elems_();
            block_elem_begin_ = block_alloc_begin_;
            block_elem_end_ = block_alloc_begin_;
            elem_begin_(nullptr, nullptr, nullptr);
            elem_end_(nullptr, nullptr, nullptr);
        }

        // 空deque安全
        [[nodiscard]] constexpr ::std::size_t size() const noexcept {
            auto const block_size = block_elem_size_();
            auto result = 0uz;
            if (block_size) {
                result += elem_begin_end_ - elem_begin_begin_;
            }
            if (block_size > 2uz) {
                result += (block_size - 2uz) * details::block_elements_v<T>;
            }
            if (block_size > 1uz) {
                result += elem_end_end_ - elem_end_begin_;
            }
            return result;
        }

        [[nodiscard]] static constexpr ::std::size_t max_size() noexcept {
            return static_cast<::std::size_t>(-1) / 2uz;
        }

#if !defined(NDEBUG)
        static_assert(::std::random_access_iterator<iterator>);
        static_assert(::std::sentinel_for<iterator, iterator>);
#endif

        [[nodiscard]] constexpr const_iterator begin() const noexcept {
            if (block_elem_size_() == 0uz) {
                return const_iterator{nullptr, nullptr, nullptr, nullptr};
            }
            return const_iterator{block_elem_begin_, block_elem_end_, *block_elem_begin_, elem_begin_begin_};
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept {
            if (block_elem_size_() == 0uz) {
                return const_iterator{nullptr, nullptr, nullptr, nullptr};
            }
            return const_iterator{block_elem_end_ - 1uz, block_elem_end_, *(block_elem_end_ - 1uz), elem_end_end_};
        }

        constexpr iterator begin() noexcept {
            return static_cast<const deque &>(*this).begin().remove_const_();
        }

        constexpr iterator end() noexcept {
            return static_cast<const deque &>(*this).end().remove_const_();
        }

        constexpr const_iterator cbegin() const noexcept {
            return begin();
        }

        [[nodiscard]] constexpr const_iterator cend() const noexcept {
            return end();
        }

        constexpr auto rbegin() noexcept {
            return reverse_iterator{end()};
        }

        constexpr auto rend() noexcept {
            return reverse_iterator{begin()};
        }

        constexpr auto rbegin() const noexcept {
            return const_reverse_iterator{end()};
        }

        constexpr auto rend() const noexcept {
            return const_reverse_iterator{begin()};
        }

        constexpr auto rcbegin() const noexcept {
            return const_reverse_iterator{end()};
        }

        constexpr auto rcend() const noexcept {
            return const_reverse_iterator{begin()};
        }

    private:
        // 负责分配块数组
        // 构造和扩容时都可以使用
        struct ctrl_alloc_ {
            deque &d;
            BlockFP block_ctrl_begin_fancy{}; // A
            Block *block_ctrl_end{}; // D

            // 替换块数组到deque
            // 构造函数专用
            // 对空deque安全
            constexpr void replace_ctrl() const noexcept {
                d.block_ctrl_begin_fancy_ = block_ctrl_begin_fancy;
                d.block_ctrl_end_ = block_ctrl_end;
                d.block_alloc_begin_ = details::to_address(block_ctrl_begin_fancy);
                d.block_alloc_end_ = d.block_alloc_begin_;
                d.block_elem_begin_ = d.block_alloc_begin_;
                d.block_elem_end_ = d.block_alloc_begin_;
            }

            // 扩容时，back为插入元素的方向
            // 对空deque安全
            constexpr void replace_ctrl_back() const noexcept {
                d.align_elem_alloc_as_ctrl_back_(details::to_address(block_ctrl_begin_fancy));
                d.dealloc_ctrl_();
                // 注意顺序
                // 从alloc替换回deque
                d.block_ctrl_begin_fancy_ = block_ctrl_begin_fancy;
                d.block_ctrl_end_ = block_ctrl_end;
            }

            constexpr void replace_ctrl_front() const noexcept {
                d.align_elem_alloc_as_ctrl_front_(block_ctrl_end);
                d.dealloc_ctrl_();
                // 注意顺序
                // 从alloc替换回deque
                d.block_ctrl_begin_fancy_ = block_ctrl_begin_fancy;
                d.block_ctrl_end_ = block_ctrl_end;
            }

            // 参数是新大小
            constexpr ctrl_alloc_(deque &dq, ::std::size_t const ctrl_size) : d(dq) {
                auto const size = (ctrl_size + (4uz - 1uz)) / 4uz * 4uz;
                block_ctrl_begin_fancy = d.alloc_ctrl_(size);
                block_ctrl_end = details::to_address(block_ctrl_begin_fancy) + size;
            }
        };

        // 对齐控制块
        // 对齐alloc和ctrl的begin
        constexpr void align_alloc_as_ctrl_back_() noexcept {
            ::std::ranges::copy(block_alloc_begin_, block_alloc_end_, block_ctrl_begin_());
            auto const block_size = block_alloc_size_();
            block_alloc_begin_ = block_ctrl_begin_();
            block_alloc_end_ = block_ctrl_begin_() + block_size;
        }

        // 对齐控制块
        // 对齐alloc和ctrl的end
        constexpr void align_alloc_as_ctrl_front_() noexcept {
            ::std::ranges::copy_backward(block_alloc_begin_, block_alloc_end_, block_ctrl_end_);
            auto const block_size = block_alloc_size_();
            block_alloc_end_ = block_ctrl_end_;
            block_alloc_begin_ = block_ctrl_end_ - block_size;
        }

        // 对齐控制块
        // 对齐elem和alloc的begin
        constexpr void align_elem_as_alloc_back_() noexcept {
            ::std::ranges::rotate(block_alloc_begin_, block_elem_begin_, block_elem_end_);
            auto const block_size = block_elem_size_();
            block_elem_begin_ = block_alloc_begin_;
            block_elem_end_ = block_alloc_begin_ + block_size;
        }

        // 对齐控制块
        // 对齐elem和alloc的end
        constexpr void align_elem_as_alloc_front_() noexcept {
            ::std::ranges::rotate(block_elem_begin_, block_elem_end_, block_alloc_end_);
            auto const block_size = block_elem_size_();
            block_elem_end_ = block_alloc_end_;
            block_elem_begin_ = block_alloc_end_ - block_size;
        }

        // ctrl_begin可以是自己或者新ctrl的
        // 对齐控制块所有指针
        constexpr void align_elem_alloc_as_ctrl_back_(Block *const ctrl_begin) noexcept {
            align_elem_as_alloc_back_();
            auto const alloc_block_size = block_alloc_size_();
            auto const elem_block_size = block_elem_size_();
            ::std::ranges::copy(block_alloc_begin_, block_alloc_end_, ctrl_begin);
            block_alloc_begin_ = ctrl_begin;
            block_alloc_end_ = ctrl_begin + alloc_block_size;
            block_elem_begin_ = ctrl_begin;
            block_elem_end_ = ctrl_begin + elem_block_size;
        }

        // ctrl_end可以是自己或者新ctrl的
        // 对齐控制块所有指针
        constexpr void align_elem_alloc_as_ctrl_front_(Block *const ctrl_end) noexcept {
            align_elem_as_alloc_front_();
            auto const alloc_block_size = block_alloc_size_();
            auto const elem_block_size = block_elem_size_();
            ::std::ranges::copy_backward(block_alloc_begin_, block_alloc_end_, ctrl_end);
            block_alloc_end_ = ctrl_end;
            block_alloc_begin_ = ctrl_end - alloc_block_size;
            block_elem_end_ = ctrl_end;
            block_elem_begin_ = ctrl_end - elem_block_size;
        }

        // 向前分配新block，需要block_size小于等于(block_alloc_begin -
        // block_ctrl_begin) 且不block_alloc_X不是空指针
        constexpr void extent_block_front_uncond_(::std::size_t const block_size) {
            if (!(block_alloc_begin_ != block_ctrl_begin_())) [[unlikely]] {
                fast_terminate();
            }
            if (!(block_alloc_begin_)) [[unlikely]] {
                fast_terminate();
            }
            for (auto i = 0uz; i != block_size; ++i) {
                --block_alloc_begin_;
                *block_alloc_begin_ = alloc_block_();
            }
        }

        // 向后分配新block，需要block_size小于等于(block_ctrl_end - block_alloc_end)
        // 且不block_alloc_X不是空指针
        constexpr void extent_block_back_uncond_(::std::size_t const block_size) {
            if (!(block_alloc_end_ != block_ctrl_end_)) [[unlikely]] {
                fast_terminate();
            }
            if (!(block_alloc_end_)) [[unlikely]] {
                fast_terminate();
            }
            for (auto i = 0uz; i != block_size; ++i) {
                *block_alloc_end_ = alloc_block_();
                ++block_alloc_end_;
            }
        }

        // 向back扩展
        // 对空deque安全
        constexpr void reserve_back_(::std::size_t const add_elem_size) {
            // 计算现有头尾是否够用
            // 头部块的cap
            auto const head_block_cap = (block_elem_begin_ - block_alloc_begin_) * details::block_elements_v<T>;
            // 尾部块的cap
            auto const tail_block_cap = (block_alloc_end_ - block_elem_end_) * details::block_elements_v<T>;
            // 尾块的已使用大小
            auto const tail_cap = elem_end_last_ - elem_end_end_ + 0uz;
            // non_move_cap为尾部-尾部已用，不移动块时cap
            auto const non_move_cap = tail_block_cap + tail_cap;
            // 首先如果cap足够，则不需要分配新block
            if (non_move_cap >= add_elem_size) {
                return;
            }
            // move_cap为头部+尾部-尾部已用，移动已分配块的cap
            auto const move_cap = head_block_cap + non_move_cap;
            // 如果move_cap够则移动
            if (move_cap >= add_elem_size) {
                align_elem_as_alloc_back_();
                return;
            }
            // 计算需要分配多少块数组，无论接下来是什么逻辑都直接使用它
            auto const add_block_size =
                    (add_elem_size - move_cap + details::block_elements_v<T> - 1uz) / details::block_elements_v<T>;
            // 获得目前控制块容许容量
            auto const ctrl_cap = ((block_alloc_begin_ - block_ctrl_begin_()) + (block_ctrl_end_ - block_alloc_end_)) *
                                  details::block_elements_v<T> +
                                  move_cap;
            // 如果容许容量足够，那么移动alloc
            if (ctrl_cap >= add_elem_size) {
                align_elem_alloc_as_ctrl_back_(block_ctrl_begin_());
            } else {
                // 否则扩展控制块
                ctrl_alloc_ const ctrl{*this, block_alloc_size_() + add_block_size}; // may throw
                ctrl.replace_ctrl_back();
            }
            extent_block_back_uncond_(add_block_size);
        }

        // 向back扩展
        // 对空deque安全
        constexpr void reserve_one_back_() {
            if (block_alloc_end_ != block_elem_end_) {
                return;
            }
            if (block_elem_begin_ != block_alloc_begin_) {
                align_elem_as_alloc_back_();
                return;
            }
            if ((block_alloc_begin_ - block_ctrl_begin_()) + (block_ctrl_end_ - block_alloc_end_) != 0uz) {
                align_elem_alloc_as_ctrl_back_(block_ctrl_begin_());
            } else {
                // 否则扩展控制块
                ctrl_alloc_ const ctrl{*this, block_alloc_size_() + 1uz}; // may throw
                ctrl.replace_ctrl_back();
            }
            extent_block_back_uncond_(1uz);
        }

        // 从front扩展block，空deque安全
        constexpr void reserve_front_(::std::size_t const add_elem_size) {
            // 计算现有头尾是否够用
            // 头部块的cap
            auto const head_block_alloc_cap = (block_elem_begin_ - block_alloc_begin_) * details::block_elements_v<T>;
            // 尾部块的cap
            auto const tail_block_alloc_cap = (block_alloc_end_ - block_elem_end_) * details::block_elements_v<T>;
            // 头块的已使用大小
            auto const head_cap = elem_begin_begin_ - elem_begin_first_ + 0uz;
            // non_move_cap为头部-头部已用，不移动块时cap
            auto const non_move_cap = head_block_alloc_cap + head_cap;
            // 首先如果cap足够，则不需要分配新block
            if (non_move_cap >= add_elem_size) {
                return;
            }
            // move_cap为头部-头部已用+尾部，移动已分配块的cap
            auto const move_cap = non_move_cap + tail_block_alloc_cap;
            // 如果move_cap够则移动
            if (move_cap >= add_elem_size) {
                align_elem_as_alloc_front_();
                return;
            }
            // 计算需要分配多少块数组，无论接下来是什么逻辑都直接使用它
            auto const add_block_size =
                    (add_elem_size - move_cap + details::block_elements_v<T> - 1uz) / details::block_elements_v<T>;
            // 获得目前控制块容许容量
            auto const ctrl_cap = ((block_alloc_begin_ - block_ctrl_begin_()) + (block_ctrl_end_ - block_alloc_end_)) *
                                  details::block_elements_v<T> +
                                  move_cap;
            if (ctrl_cap >= add_elem_size) {
                align_elem_alloc_as_ctrl_front_(block_ctrl_end_);
            } else {
                // 否则扩展控制块
                ctrl_alloc_ const ctrl{*this, block_alloc_size_() + add_block_size}; // may throw
                ctrl.replace_ctrl_front();
            }
            // 必须最后执行
            extent_block_front_uncond_(add_block_size);
        }

        // 向back扩展
        // 对空deque安全
        constexpr void reserve_one_front_() {
            if (block_elem_begin_ != block_alloc_begin_) {
                return;
            }
            if (block_alloc_end_ != block_elem_end_) {
                align_elem_as_alloc_front_();
                return;
            }
            if ((block_alloc_begin_ - block_ctrl_begin_()) + (block_ctrl_end_ - block_alloc_end_) != 0uz) {
                align_elem_alloc_as_ctrl_front_(block_ctrl_end_);
            } else {
                // 否则扩展控制块
                ctrl_alloc_ const ctrl{*this, block_alloc_size_() + 1uz}; // may throw
                ctrl.replace_ctrl_front();
            }
            extent_block_front_uncond_(1uz);
        }

        struct construct_guard_ {
        private:
            deque *d;

        public:
            constexpr explicit construct_guard_(deque *dp) noexcept : d(dp) {
            }

            constexpr void release() noexcept {
                d = nullptr;
            }

            constexpr ~construct_guard_() {
                if (d) {
                    d->destroy_();
                }
            }
        };

        // 构造函数和赋值的辅助函数
        // 调用后可直接填充元素
        constexpr void extent_block_(::std::size_t const new_block_size) {
            if (new_block_size != 0uz) {
                auto const ctrl_block_size = block_ctrl_size_();
                auto const alloc_block_size = block_alloc_size_();
                if (ctrl_block_size == 0uz) {
                    ctrl_alloc_ const ctrl(*this, new_block_size); // may throw
                    ctrl.replace_ctrl();
                    extent_block_back_uncond_(new_block_size); // may throw
                    return;
                }
                if (alloc_block_size >= new_block_size) {
                    return;
                }
                if (ctrl_block_size < new_block_size) {
                    ctrl_alloc_ const ctrl(*this, new_block_size); // may throw
                    ctrl.replace_ctrl_back();
                } else {
                    align_alloc_as_ctrl_back_();
                }
                extent_block_back_uncond_(new_block_size - alloc_block_size); // may throw
            }
        }

        // 构造函数和复制赋值的辅助函数，调用前必须分配内存，以及用于构造时使用guard
        template<bool move = false>
        constexpr void copy_(const_bucket_type const other, ::std::size_t const block_size) {
            if (block_size) {
                // 此时最为特殊，因为只有一个有效快时，可以从头部生长也可以从尾部生长
                // 这里选择按头部生长简化代码
                auto const elem_size = other.elem_begin_end_ - other.elem_begin_begin_;
                auto const first = *block_elem_end_;
                auto const last = first + details::block_elements_v<T>;
                auto const begin = last - elem_size;
                if constexpr (move) {
                    ::std::ranges::uninitialized_move(other.elem_begin_begin_, other.elem_begin_end_, begin,
                                                      ::std::unreachable_sentinel);
                } else {
                    ::std::ranges::uninitialized_copy(other.elem_begin_begin_, other.elem_begin_end_, begin,
                                                      ::std::unreachable_sentinel);
                }
                elem_begin_(begin, last, first);
                elem_end_(begin, last, last);
                ++block_elem_end_;
            }
            if (block_size > 2uz) {
                for (auto const block_begin:
                     ::std::ranges::subrange{other.block_elem_begin_ + 1uz, other.block_elem_end_ - 1uz}) {
                    auto const begin = *block_elem_end_;
                    auto const src_begin = block_begin;
                    if constexpr (move) {
                        ::std::ranges::uninitialized_move(src_begin, src_begin + details::block_elements_v<T>, begin,
                                                          ::std::unreachable_sentinel);
                    } else {
                        ::std::ranges::uninitialized_copy(src_begin, src_begin + details::block_elements_v<T>, begin,
                                                          ::std::unreachable_sentinel);
                    }
                    elem_end_(begin, begin + details::block_elements_v<T>, elem_end_last_);
                    ++block_elem_end_;
                }
                elem_end_last_ = elem_end_end_;
            }
            if (block_size > 1uz) {
                auto const begin = *block_elem_end_;
                if constexpr (move) {
                    ::std::ranges::uninitialized_move(other.elem_end_begin_, other.elem_end_end_, begin,
                                                      ::std::unreachable_sentinel);
                } else {
                    ::std::ranges::uninitialized_copy(other.elem_end_begin_, other.elem_end_end_, begin,
                                                      ::std::unreachable_sentinel);
                }
                elem_end_(begin, begin + (other.elem_end_end_ - other.elem_end_begin_),
                          begin + details::block_elements_v<T>);
                ++block_elem_end_;
            }
        }

        static consteval void is_iterator_(iterator const &) noexcept {
            /* */
        }

    public:
        constexpr deque() noexcept(::std::is_nothrow_default_constructible_v<allocator>)
            requires ::std::default_initializable<allocator>
        = default;

    private:
        // 万能构造
        // 使用count、count和T、或者随机访问迭代器进行构造
        // 注意异常安全，需要调用者使用guard，并且分配好足够多内存
        template<typename... Ts>
        constexpr void construct_(::std::size_t const full_blocks, ::std::size_t const rem_elems, Ts &&... ts) {
            // 由于析构优先考虑elem_begin，因此必须独立构造elem_begin
            if (full_blocks) {
                auto const begin = *block_elem_end_;
                auto const end = begin + details::block_elements_v<T>;
                if constexpr (sizeof...(Ts) == 0uz) {
                    if constexpr (is_default_operation_) {
                        ::std::ranges::uninitialized_value_construct(begin, end);
                        elem_begin_(begin, end, begin);
                    } else {
                        elem_begin_(begin, begin, begin);
                        for (auto &i: ::std::ranges::subrange(begin, end)) {
                            ::std::construct_at(&i);
                            ++elem_begin_end_;
                        }
                    }
                } else if constexpr (sizeof...(Ts) == 1uz) {
                    if constexpr (is_default_operation_) {
                        ::std::ranges::uninitialized_fill(begin, end, ts...);
                        elem_begin_(begin, end, begin);
                    } else {
                        elem_begin_(begin, begin, begin);
                        for (auto &i: ::std::ranges::subrange(begin, end)) {
                            ::std::construct_at(&i, ts...);
                            ++elem_begin_end_;
                        }
                    }
                } else if constexpr (sizeof...(Ts) == 2uz) {
#if defined(__cpp_pack_indexing)
                auto [src_begin, src_end] = details::get(ts...);
#else
                    auto [src_begin, src_end] = details::get_first_two(::std::forward_as_tuple(ts...));
#endif
                    ::std::ranges::uninitialized_copy(src_begin, ::std::unreachable_sentinel, begin,
                                                      begin + details::block_elements_v<T>);
                    src_begin += details::block_elements_v<T>;
                    elem_begin_(begin, end, begin);
                } else {
                    static_assert(false);
                }
                elem_end_(begin, end, end);
                ++block_elem_end_;
            }
            if (full_blocks > 1uz) {
                for (auto i = 0uz; i != full_blocks - 1uz; ++i) {
                    auto const begin = *block_elem_end_;
                    auto const end = begin + details::block_elements_v<T>;
                    if constexpr (sizeof...(Ts) == 0uz) {
                        if constexpr (is_default_operation_) {
                            ::std::ranges::uninitialized_value_construct(begin, end);
                            elem_end_(begin, end, elem_end_last_);
                        } else {
                            elem_end_(begin, begin, elem_end_last_);
                            for (auto &j: ::std::ranges::subrange(begin, end)) {
                                ::std::construct_at(&j);
                                ++elem_end_end_;
                            }
                        }
                    } else if constexpr (sizeof...(Ts) == 1uz) {
                        if constexpr (is_default_operation_) {
                            ::std::ranges::uninitialized_fill(begin, end, ts...);
                            elem_end_(begin, end, elem_end_last_);
                        } else {
                            elem_end_(begin, begin, elem_end_last_);
                            for (auto &j: ::std::ranges::subrange(begin, end)) {
                                ::std::construct_at(&j, ts...);
                                ++elem_end_end_;
                            }
                        }
                    } else if constexpr (sizeof...(Ts) == 2uz) {
#if defined(__cpp_pack_indexing)
                    auto [src_begin, src_end] = details::get(ts...);
#else
                        auto [src_begin, src_end] = details::get_first_two(::std::forward_as_tuple(ts...));
#endif
                        ::std::ranges::uninitialized_copy(src_begin, ::std::unreachable_sentinel, begin,
                                                          begin + details::block_elements_v<T>);
                        src_begin += details::block_elements_v<T>;
                        elem_end_(begin, end, elem_end_last_);
                    } else {
                        static_assert(false);
                    }
                    ++block_elem_end_;
                }
                elem_end_last_ = elem_end_end_;
            }
            if (rem_elems) {
                auto const begin = *block_elem_end_;
                auto const end = begin + rem_elems;
                if constexpr (sizeof...(Ts) == 0uz) {
                    if constexpr (is_default_operation_) {
                        ::std::ranges::uninitialized_value_construct(begin, end);
                        elem_end_(begin, end, begin + details::block_elements_v<T>);
                    } else {
                        elem_end_(begin, begin, begin + details::block_elements_v<T>);
                        for (auto &i: ::std::ranges::subrange(begin, end)) {
                            ::std::construct_at(&i);
                            ++elem_end_end_;
                        }
                    }
                } else if constexpr (sizeof...(Ts) == 1uz) {
                    if constexpr (is_default_operation_) {
                        ::std::ranges::uninitialized_fill(begin, end, ts...);
                        elem_end_(begin, end, begin + details::block_elements_v<T>);
                    } else {
                        elem_end_(begin, begin, begin + details::block_elements_v<T>);
                        for (auto &i: ::std::ranges::subrange(begin, end)) {
                            ::std::construct_at(&i, ts...);
                            ++elem_end_end_;
                        }
                    }
                } else if constexpr (sizeof...(Ts) == 2uz) {
#if defined(__cpp_pack_indexing)
                auto [src_begin, src_end] = details::get(ts...);
#else
                    auto [src_begin, src_end] = details::get_first_two(::std::forward_as_tuple(ts...));
#endif
                    ::std::ranges::uninitialized_copy(src_begin, src_end, begin, ::std::unreachable_sentinel);
                    elem_end_(begin, end, begin + details::block_elements_v<T>);
                } else {
                    static_assert(false);
                }
                if (full_blocks == 0uz) // 注意
                {
                    elem_begin_(begin, end, begin);
                }
                ++block_elem_end_;
            }
        }

        // 参考emplace_front
        template<typename... V>
        constexpr T &emplace_back_pre_(::std::size_t const block_size, V &&... v) {
            auto const end = elem_end_end_;
            std::construct_at(end, ::std::forward<V>(v)...);
            elem_end_end_ = end + 1uz;
            // 修正elem_begin
            if (block_size == 1uz) {
                elem_begin_end_ = end + 1uz;
            }
            return *end;
        }

        // 参考emplace_front
        template<typename... V>
        constexpr T &emplace_back_post_(::std::size_t const block_size, V &&... v) {
            auto const begin = ::std::to_address(*block_elem_end_);
            ::std::construct_at(begin, ::std::forward<V>(v)...);
            elem_end_(begin, begin + 1uz, begin + details::block_elements_v<T>);
            ++block_elem_end_;
            // 修正elem_begin，如果先前为0，说明现在是1，修正elem_begin等于elem_end
            if (block_size == 0uz) {
                elem_begin_(begin, begin + 1uz, begin);
            }
            return *begin;
        }

    public:
        template<typename... V>
        constexpr T &emplace_back(V &&... v) {
            auto const block_size = block_elem_size_();
            if (elem_end_end_ != elem_end_last_) {
                return emplace_back_pre_(block_size, ::std::forward<V>(v)...);
            } else {
                reserve_one_back_();
                return emplace_back_post_(block_size, ::std::forward<V>(v)...);
            }
        }

        explicit constexpr deque(::std::size_t const count) {
            auto const [block_size, full_blocks, rem_elems] = details::calc_cap<T>(count);
            construct_guard_ guard(this);
            extent_block_(block_size);
            construct_(full_blocks, rem_elems);
            guard.release();
        }

        constexpr deque(::std::size_t const count, T const &t) {
            auto const [block_size, full_blocks, rem_elems] = details::calc_cap<T>(count);
            construct_guard_ guard(this);
            extent_block_(block_size);
            construct_(full_blocks, rem_elems, t);
            guard.release();
        }

    private:
        template<::std::input_iterator U, typename V>
        constexpr void from_range_noguard_(U &&first, V &&last) {
            for (; first != last; ++first) {
                emplace_back(*first);
            }
        }

        template<::std::random_access_iterator U>
        constexpr void from_range_noguard_(U &&first, U &&last) {
            if (first != last) {
                auto const [block_size, full_blocks, rem_elems] = details::calc_cap<T>(last - first);
                extent_block_(block_size);
                construct_(full_blocks, rem_elems, ::std::forward<U>(first), ::std::forward<U>(last));
            }
        }

        constexpr void from_range_noguard_(iterator &first, iterator &last) {
            if (first != last) {
                if (first.block_elem_curr_ == last.block_elem_curr_) {
                    bucket_type bucket{
                        first.block_elem_curr_, last.block_elem_curr_ + 1uz,
                        first.elem_curr_, last.elem_curr_,
                        last.elem_begin_, last.elem_begin_
                    };
                    auto const block_size = bucket.size();
                    extent_block_(block_size);
                    copy_(bucket, block_size);
                } else {
                    bucket_type bucket{
                        first.block_elem_curr_, last.block_elem_curr_ + 1uz,
                        first.elem_curr_, first.elem_begin_ + details::block_elements_v<T>,
                        last.elem_begin_, last.elem_curr_
                    };
                    auto const block_size = bucket.size();
                    extent_block_(block_size);
                    copy_(bucket, block_size);
                }
            }
        }

        constexpr void from_range_noguard_(iterator &&first, iterator &&last) {
            return from_range_noguard_(first, last);
        }

        template<typename R>
        constexpr void from_range_noguard_(R &&rg) {
            if constexpr (requires { is_iterator_(::std::ranges::begin(rg)); }) {
                from_range_noguard_(::std::ranges::begin(rg), ::std::ranges::end(rg));
            } else if constexpr (::std::ranges::sized_range<R>) {
                if (auto const size = ::std::ranges::size(rg)) {
                    auto const [block_size, full_blocks, rem_elems] = details::calc_cap<T>(size);
                    extent_block_(block_size);
                    construct_(full_blocks, rem_elems, ::std::ranges::begin(rg), ::std::ranges::end(rg));
                }
            } else if constexpr (::std::random_access_iterator<decltype(::std::ranges::begin(rg))>) {
                from_range_noguard_(::std::ranges::begin(rg), ::std::ranges::end(rg));
            }
#if defined(__cpp_lib_ranges_reserve_hint) && __cpp_lib_ranges_reserve_hint >= 202502L
        else if constexpr (::std::ranges::approximately_sized_range<R>)
        {
            if (auto const size = ::std::ranges::reserve_hint(rg))
            {
                reserve_back(size);
                auto begin = ::std::ranges::begin(rg);
                auto end = ::std::ranges::end(rg);
                for (; begin != end; ++begin)
                {
                    emplace_back_noalloc_(*begin);
                }
            }
        }
#endif
            else {
                from_range_noguard_(::std::ranges::begin(rg), ::std::ranges::end(rg));
            }
        }

    public:
        template<::std::input_iterator U, typename V>
        constexpr deque(U first, V last) {
            construct_guard_ guard(this);
            from_range_noguard_(::std::move(first), ::std::move(last));
            guard.release();
        }
#if defined(__cpp_lib_containers_ranges)
        template<::std::ranges::input_range R>
            requires ::std::convertible_to<::std::ranges::range_value_t<R>, T>
        constexpr deque(::std::from_range_t, R &&rg) {
            construct_guard_ guard(this);
            from_range_noguard_(rg);
            guard.release();
        }
#endif

        // 复制构造采取按结构复制的方法
        constexpr deque(deque const &other) {
            if (!other.empty()) {
                construct_guard_ guard(this);
                auto const block_size = other.block_elem_size_();
                extent_block_(block_size);
                copy_(other.buckets(), block_size);
                guard.release();
            }
        }

        constexpr deque(deque &&other) noexcept {
            other.swap(*this);
        }

        explicit constexpr deque(::std::initializer_list<T> const ilist) {
            if (ilist.size()) {
                construct_guard_ guard(this);
                from_range_noguard_(ilist.begin(), ilist.end());
                guard.release();
            }
        }

        constexpr deque &operator=(const deque &other) {
            if (this != ::std::addressof(other)) {
                clear();
                if (!other.empty()) {
                    auto const block_size = other.block_elem_size_();
                    extent_block_(block_size);
                    copy_(other.buckets(), block_size);
                }
            }
            return *this;
        }

        constexpr deque &operator=(::std::initializer_list<T> ilist) {
            clear();
            if (ilist.size()) {
                auto const [block_size, full_blocks, rem_elems] = details::calc_cap<T>(ilist.size());
                extent_block_(block_size);
                construct_(full_blocks, rem_elems, ::std::ranges::begin(ilist), ::std::ranges::end(ilist));
            }
            return *this;
        }

        constexpr deque &operator=(deque &&other) noexcept {
            if (this == &other) {
                return *this;
            }
            other.swap(*this);
            return *this;
        }

        constexpr void assign_range(deque &&d) {
            *this = ::std::move(d);
        }

        constexpr void assign_range(deque const &d) {
            *this = d;
        }

        template<::std::ranges::input_range R>
            requires ::std::convertible_to<::std::ranges::range_value_t<R>, T>
        constexpr void assign_range(R &&rg) {
            clear();
            from_range_noguard_(::std::forward<R>(rg));
        }

        constexpr void assign(::std::size_t const count, T const &value) {
            clear();
            if (count) {
                auto const [block_size, full_blocks, rem_elems] = details::calc_cap<T>(count);
                extent_block_(block_size);
                construct_(full_blocks, rem_elems, value);
            }
            /*
            assign_range(::std::ranges::views::repeat(value, count));
            */
        }

        template<::std::input_iterator U, typename V>
        constexpr void assign(U first, V last) {
            clear();
            from_range_noguard_(::std::move(first), ::std::move(last));
        }

        constexpr void assign(::std::initializer_list<T> const ilist) {
            clear();
            from_range_noguard_(ilist.begin(), ilist.end());
        }

    private:
        // 几乎等于iterator的at，但具有检查和断言
        template<bool throw_exception = false>
        constexpr T &at_impl_(::std::size_t const pos) const noexcept(!throw_exception) {
            auto const front_size = static_cast<::std::size_t>(elem_begin_begin_ - elem_begin_first_);
            auto const [block_step, elem_step] = details::calc_pos<T>(front_size, pos);
            auto const target_block = block_elem_begin_ + block_step;
            auto const check_block = target_block < block_elem_end_;
            auto const check_elem = (target_block + 1uz == block_elem_end_)
                                        ? (::std::to_address(*target_block) + elem_step < elem_end_end_)
                                        : true;
            if constexpr (throw_exception) {
                if (not(check_block && check_elem))
                    throw ::std::out_of_range{"deque::at"};
            } else {
                if (!(check_block && check_elem)) [[unlikely]] {
                    fast_terminate();
                }
            }
            return *((*target_block) + elem_step);
        }

        // 首块有空余时使用
        template<typename... V>
        constexpr T &emplace_front_pre_(::std::size_t const block_size, V &&... v) {
            auto const begin = ::std::to_address(elem_begin_begin_ - 1uz);
            ::std::construct_at(begin, ::std::forward<V>(v)...); //may throw
            elem_begin_begin_ = begin;
            if (block_size == 1uz) {
#if __has_cpp_attribute(assume)
            [[assume(begin + 1uz == elem_begin_begin_)]];
#endif
                elem_end_begin_ = begin;
            }
            return *begin;
        }

        // 首块没有空余，切换到下一个块
        template<typename... V>
        constexpr T &emplace_front_post_(::std::size_t const block_size, V &&... v) {
            auto const block = block_elem_begin_ - 1uz;
            auto const first = ::std::to_address(*block);
            auto const end = first + details::block_elements_v<T>;
            ::std::construct_at(end - 1uz, ::std::forward<V>(v)...);
            elem_begin_(end - 1uz, end, first);
#if __has_cpp_attribute(assume)
        [[assume(block + 1uz == block_elem_begin_)]];
#endif
            --block_elem_begin_;
            // 修正elem_end
            if (block_size == 0uz) {
                elem_end_(end - 1uz, end, end);
            }
            return *(end - 1uz);
        }

    public:
        template<typename... V>
        constexpr T &emplace_front(V &&... v) {
            auto const block_size = block_elem_size_();
            if (elem_begin_begin_ != elem_begin_first_) {
                return emplace_front_pre_(block_size, ::std::forward<V>(v)...);
            } else {
                reserve_one_front_();
                return emplace_front_post_(block_size, ::std::forward<V>(v)...);
            }
        }

        constexpr T &at(::std::size_t const pos) noexcept {
            return at_impl_<true>(pos);
        }

        constexpr T const &at(::std::size_t const pos) const noexcept {
            return at_impl_<true>(pos);
        }

        constexpr T &operator[](::std::size_t const pos) noexcept {
            return at_impl_(pos);
        }

        constexpr T const &operator[](::std::size_t const pos) const noexcept {
            return at_impl_(pos);
        }

        // 不会失败且不移动元素
        constexpr void shrink_to_fit() noexcept {
            if (block_alloc_size_() != 0uz) // 保证fill_block_alloc_end
            {
                for (auto const i: ::std::ranges::subrange{block_alloc_begin_, block_elem_begin_}) {
                    dealloc_block_(i);
                }
                block_alloc_begin_ = block_elem_begin_;
                for (auto const i: ::std::ranges::subrange{block_elem_end_, block_alloc_end_}) {
                    dealloc_block_(i);
                }
                block_alloc_end_ = block_elem_end_;
            }
        }

        constexpr void push_back(T const &t) {
            emplace_back(t);
        }

        constexpr void push_back(T &&t) {
            emplace_back(::std::move(t));
        }

        constexpr void push_front(T const &value) {
            emplace_front(value);
        }

        constexpr void push_front(T &&value) {
            emplace_front(::std::move(value));
        }

        // 该函数调用后如果容器大小为0，则使得elem_begin/end为nullptr
        // 这是emplace_back的先决条件
        constexpr void pop_back() noexcept {
            if (empty()) [[unlikely]] {
                fast_terminate();
            }
            --elem_end_end_;
            ::std::destroy_at(elem_end_begin_);
            if (elem_end_end_ == elem_end_begin_) {
                --block_elem_end_;
                auto const block_size = block_elem_size_();
                if (block_size == 1uz) {
                    elem_end_(elem_begin_begin_, elem_begin_end_, elem_begin_end_);
                } else if (block_size) {
                    auto const begin = *(block_elem_end_ - 1uz);
                    auto const last = begin + details::block_elements_v<T>;
                    elem_end_(begin, last, last);
                } else {
                    elem_begin_(nullptr, nullptr, nullptr);
                    elem_end_(nullptr, nullptr, nullptr);
                }
            } else if (block_elem_size_() == 1uz) {
                --elem_begin_end_;
            }
        }

        // 参考pop_back
        constexpr void pop_front() noexcept {
            if (empty()) [[unlikely]] {
                fast_terminate();
            }
            ::std::destroy_at(elem_begin_begin_);
            ++elem_begin_begin_;
            if (elem_begin_end_ == elem_begin_begin_) {
                ++block_elem_begin_;
                auto const block_size = block_elem_size_();
                // 注意，如果就剩最后一个block，那么应该采用end的位置而不是计算得到
                if (block_size == 1uz) {
                    elem_begin_(elem_end_begin_, elem_end_end_, elem_end_begin_);
                } else if (block_size) {
                    auto const begin = *block_elem_begin_;
                    auto const last = begin + details::block_elements_v<T>;
                    elem_begin_(begin, last, begin);
                } else {
                    elem_begin_(nullptr, nullptr, nullptr);
                    elem_end_(nullptr, nullptr, nullptr);
                }
            } else if (block_elem_size_() == 1uz) {
                ++elem_end_begin_;
            }
        }

        constexpr T &front() noexcept {
            if (empty()) [[unlikely]] {
                fast_terminate();
            }
            return *(elem_begin_begin_);
        }

        constexpr T &back() noexcept {
            if (empty()) [[unlikely]] {
                fast_terminate();
            }
            return *(elem_end_end_ - 1uz);
        }

        constexpr T const &front() const noexcept {
            if (empty()) [[unlikely]] {
                fast_terminate();
            }
            return *(elem_begin_begin_);
        }

        constexpr T const &back() const noexcept {
            if (empty()) [[unlikely]] {
                fast_terminate();
            }
            return *(elem_end_end_ - 1uz);
        }

    private:
        constexpr void pop_back_n_(::std::size_t const count) noexcept {
            for (auto i = 0uz; i != count; ++i) {
                if (empty()) [[unlikely]] {
                    fast_terminate();
                }
                pop_back();
            }
        }

        constexpr void pop_front_n_(::std::size_t const count) noexcept {
            for (auto i = 0uz; i != count; ++i) {
                if (empty()) [[unlikely]] {
                    fast_terminate();
                }
                pop_front();
            }
        }

        template<bool back>
        struct partial_guard_ {
            deque *d;
            ::std::size_t const size;

        public:
            constexpr partial_guard_(deque *dp, ::std::size_t const old_size) noexcept : d(dp), size(old_size) {
            }

            constexpr void release() noexcept {
                d = nullptr;
            }

            constexpr ~partial_guard_() {
                if (d != nullptr) {
                    if constexpr (back) {
                        d->resize_shrink_(d->size(), size);
                    } else {
                        d->pop_front_n_(d->size() - size);
                    }
                }
            }
        };

        // 用于范围构造，该函数不分配内存
        // 需要在调用前reserve足够大
        template<typename... V>
        constexpr T &emplace_front_noalloc_(V &&... v) {
            auto const block_size = block_elem_size_();
            if (elem_begin_begin_ != elem_begin_first_) {
                return emplace_front_pre_(block_size, ::std::forward<V>(v)...);
            } else {
                return emplace_front_post_(block_size, ::std::forward<V>(v)...);
            }
        }

        // 见emplace_front_noalloc
        template<typename... V>
        constexpr T &emplace_back_noalloc_(V &&... v) {
            auto const block_size = block_elem_size_();
            if (elem_end_end_ != elem_end_last_) {
                return emplace_back_pre_(block_size, ::std::forward<V>(v)...);
            } else {
                return emplace_back_post_(block_size, ::std::forward<V>(v)...);
            }
        }

        template<::std::input_iterator U, typename V>
        constexpr void append_range_noguard_(U &&first, V &&last) {
            for (; first != last; ++first) {
                emplace_back(*first);
            }
        }

        template<::std::random_access_iterator U>
        constexpr void append_range_noguard_(U &&first, U &&last) {
            reserve_back_(last - first);
            for (; first != last; ++first) {
                emplace_back_noalloc_(*first);
            }
        }

        template<typename R>
        constexpr void append_range_noguard_(R &&rg) {
            if (::std::ranges::empty(rg)) {
                return;
            }
#if defined(__cpp_lib_ranges_reserve_hint) && __cpp_lib_ranges_reserve_hint >= 202502L
        if constexpr (::std::ranges::approximately_sized_range<R>)
        {
            reserve_back_(::std::ranges::reserve_hint(rg));
#else
            if constexpr (::std::ranges::sized_range<R>) {
                reserve_back_(::std::ranges::size(rg));
#endif
                for (auto &&i: rg) {
                    emplace_back_noalloc_(::std::forward<decltype(i)>(i));
                }
            } else {
                append_range_noguard_(::std::ranges::begin(rg), ::std::ranges::end(rg));
            }
        }

        template<::std::input_iterator U, typename V>
        constexpr void prepend_range_noguard_(U &&first, V &&last) {
            auto const old_size = size();
            for (; first != last; ++first) {
                emplace_front(*first);
            }
            ::std::ranges::reverse(begin(), begin() + (size() - old_size));
        }

        template<::std::bidirectional_iterator U>
        constexpr void prepend_range_noguard_(U &&first, U &&last) {
            for (; first != last;) {
                --last;
                emplace_front(*last);
            }
        }

        template<::std::random_access_iterator U>
        constexpr void prepend_range_noguard_(U &&first, U &&last) {
            reserve_front_(last - first);
            for (; first != last;) {
                --last;
                emplace_front_noalloc_(*last);
            }
        }

        template<typename R>
        constexpr void prepend_range_noguard_(R &&rg) {
            if (::std::ranges::empty(rg)) {
                return;
            }
#if defined(__cpp_lib_ranges_reserve_hint) && __cpp_lib_ranges_reserve_hint >= 202502L
        if constexpr (::std::ranges::approximately_sized_range<R> && ::std::ranges::bidirectional_range<R>)
        {
            reserve_front_(::std::ranges::reserve_hint(rg));
#else
            if constexpr (::std::ranges::sized_range<R> && ::std::ranges::bidirectional_range<R>) {
                reserve_front_(::std::ranges::size(rg));
#endif
                auto first = ::std::ranges::begin(rg);
                auto last = ::std::ranges::end(rg);
                for (; first != last;) {
                    --last;
                    emplace_front_noalloc_(*last);
                }
            } else if constexpr (::std::ranges::bidirectional_range<R>) {
                prepend_range_noguard_(::std::ranges::begin(rg), ::std::ranges::end(rg));
            }
#if defined(__cpp_lib_ranges_reserve_hint) && __cpp_lib_ranges_reserve_hint >= 202502L
        else if constexpr (::std::ranges::approximately_sized_range<R>)
        {
            reserve_front(::std::ranges::reserve_hint(rg));
            auto const old_size = size();
            for (auto &&i : rg)
            {
                emplace_front_noalloc_(::std::forward<decltype(i)>(i));
            }
            ::std::ranges::reverse(begin(), begin() + (size() - old_size));
        }
#endif
            else if constexpr (::std::ranges::sized_range<R>) {
                auto const count = ::std::ranges::size(rg);
                reserve_front_(count);
                for (auto &&i: rg) {
                    emplace_front_noalloc_(::std::forward<decltype(i)>(i));
                }
                ::std::ranges::reverse(begin(), begin() + count);
            } else {
                prepend_range_noguard_(::std::ranges::begin(rg), ::std::ranges::end(rg));
            }
        }

    public:
        template<::std::ranges::input_range R>
            requires ::std::convertible_to<::std::ranges::range_value_t<R>, T>
        constexpr void append_range(R &&rg) {
            partial_guard_<true> guard(this, size());
            append_range_noguard_(::std::forward<R>(rg));
            guard.release();
        }

        template<::std::ranges::input_range R>
            requires ::std::convertible_to<::std::ranges::range_value_t<R>, T>
        constexpr void prepend_range(R &&rg) {
            auto const old_size = size();
            partial_guard_<false> guard(this, old_size);
            prepend_range_noguard_(::std::forward<R>(rg));
            guard.release();
        }

    private:
        // 收缩专用
        constexpr void resize_shrink_(::std::size_t const old_size, ::std::size_t const new_size) noexcept {
            if (!(old_size >= new_size)) [[unlikely]] {
                fast_terminate();
            }
            if constexpr (::std::is_trivially_destructible_v<T> && is_default_operation_) {
                auto const [block_step, elem_step] =
                        details::calc_pos<T>(static_cast<::std::size_t>(elem_begin_begin_ - elem_begin_first_),
                                             new_size);
                if (block_step == 0uz) {
                    auto const begin = elem_begin_first_;
                    elem_end_(elem_begin_begin_, begin + elem_step, begin + details::block_elements_v<T>);
                    block_elem_end_ = block_elem_begin_ + 1uz;
                    elem_begin_(elem_begin_begin_, begin + elem_step, begin);
                } else {
                    auto const target_block = block_elem_begin_ + block_step;
                    auto const begin = *target_block;
                    elem_end_(begin, begin + elem_step, begin + details::block_elements_v<T>);
                    block_elem_end_ = target_block + 1uz;
                }
            } else {
                auto const count = old_size - new_size;
                for (auto i = 0uz; i != count; ++i) {
                    if (empty()) [[unlikely]] {
                        fast_terminate();
                    }
                    pop_back();
                }
            }
        }

        template<typename... Ts>
        constexpr void resize_unified_(::std::size_t const new_size, Ts &&... ts) {
            if (auto const old_size = size(); new_size < old_size) {
                resize_shrink_(old_size, new_size);
            } else {
                auto const diff = new_size - old_size;
                reserve_back_(diff);
                partial_guard_<true> guard(this, old_size);
                for (auto i = 0uz; i != diff; ++i) {
                    emplace_back_noalloc_(ts...);
                }
                guard.release();
            }
        }

    public:
        // 注意必须调用clear，使得空容器的elem_begin/elem_end都为空指针
        constexpr void resize(::std::size_t const new_size) {
            new_size == 0uz ? clear() : resize_unified_(new_size);
        }

        constexpr void resize(::std::size_t const new_size, T const &t) {
            new_size == 0uz ? clear() : resize_unified_(new_size, t);
        }

    private:
        // 用于emplace的辅助函数，调用前需要判断方向
        // 该函数将后半部分向后移动1个位置
        // 从最后一个块开始
        constexpr void back_emplace_(Block *const block_curr, T *const elem_curr) {
            auto const block_end = block_elem_end_;
            auto const block_size = block_end - block_curr - 1uz;
            // 每次移动时留下的空位
            auto last_elem = elem_end_begin_;
            // 先记录尾块块尾位置
            auto end = elem_end_end_;
            // 再emplace_back
            emplace_back_noalloc_(::std::move(back()));
            // 如果大于一个块，那么移动整个尾块
            if (block_size > 0uz) {
                auto const begin = last_elem;
                ::std::ranges::move_backward(begin, end - 1uz, end);
            }
            // 移动中间的块
            if (block_size > 1uz) {
                auto target_block_end = block_end - 1uz;
                for (; target_block_end != block_curr + 1uz;) {
                    --target_block_end;
                    auto const target_begin = ::std::to_address(*target_block_end);
                    auto const target_end = target_begin + details::block_elements_v<T>;
                    *last_elem = ::std::move(*(target_end - 1uz));
                    last_elem = target_begin;
                    ::std::ranges::move_backward(target_begin, target_end - 1uz, target_end);
                }
            }
            // 移动插入位置所在的块
            {
                // 如果插入位置就是尾块，那么采纳之前储存的end作为移动使用的end
                if (block_end - 1uz != block_curr) {
                    // 否则使用计算出来的end
                    end = ::std::to_address(*block_curr + details::block_elements_v<T>);
                    // 将当前块的最后一个移动到上一个块的第一个
                    *last_elem = ::std::move(*(end - 1uz));
                }
                // 把插入位置所在块整体右移1
                ::std::ranges::move_backward(elem_curr, end - 1uz, end);
            }
        }

        // 将前半部分向前移动1
        constexpr void front_emplace_(Block *const block_curr, T *const elem_curr) {
            auto const block_begin = block_elem_begin_;
            auto const block_size = block_curr - block_begin + 0uz;
            // 向前移动后尾部空出来的的后面一个位置
            auto const last_elem_begin = elem_begin_begin_;
            auto last_elem_end = elem_begin_end_;
            emplace_front_noalloc_(::std::move(front()));
            // 如果block_curr是首个块，那么elem_curr就是终点
            if (block_begin == block_curr) {
                last_elem_end = elem_curr;
            }
            // 否则之前储存的last_elem_end是终点
            ::std::ranges::move(last_elem_begin + 1uz, last_elem_end, last_elem_begin);
            if (block_size > 1uz) {
                auto target_block_begin = block_begin + 1uz;
                for (; target_block_begin != block_curr; ++target_block_begin) {
                    auto const begin = ::std::to_address(*target_block_begin);
                    auto const end = begin + details::block_elements_v<T>;
                    *(last_elem_end - 1uz) = ::std::move(*begin);
                    last_elem_end = end;
                    ::std::ranges::move(begin + 1uz, end, begin);
                }
            }
            if (block_size > 0uz) {
                auto const begin = ::std::to_address(*block_curr);
                if (elem_curr != begin) {
                    *(last_elem_end - 1uz) = ::std::move(*begin);
                    ::std::ranges::move(begin + 1uz, elem_curr, begin);
                }
            }
        }

    public:
        template<typename... Args>
        constexpr iterator emplace(const_iterator const pos, Args &&... args) {
            auto const begin_pre = begin();
            auto const end_pre = end();
            if (pos == end_pre) {
                emplace_back(::std::forward<Args>(args)...);
                return end() - 1z;
            }
            if (pos == begin_pre) {
                emplace_front(::std::forward<Args>(args)...);
                return begin();
            }
            // 此时容器一定不为空
            auto const back_diff = end_pre - pos + 0uz;
            auto const front_diff = pos - begin_pre + 0uz;
            // NB:
            // 如果args是当前容器的元素的引用，那么必须使得该元素先被emplace_back/front后再被移动到正确位置，否则该引用会失效，同时reserve不会导致引用失效
            // 此处逻辑和无分配器版本稍微不一样
            if (back_diff <= front_diff || (block_elem_size_() == 1uz && elem_end_end_ != elem_end_last_)) {
                reserve_back_(2uz);
                emplace_back_noalloc_(::std::forward<Args>(args)...); // 满足标准要求经过A::construct
                // back_emplace向后移动1个元素并插入，因此先reserve以获得一个不失效的pos
                auto new_pos = begin() + front_diff;
                back_emplace_(new_pos.block_elem_curr_, new_pos.elem_curr_);
                *new_pos = ::std::move(back());
                pop_back();
                return new_pos;
            } else {
                reserve_front_(2uz);
                emplace_front_noalloc_(::std::forward<Args>(args)...);
                auto new_pos = end() - back_diff;
                front_emplace_(new_pos.block_elem_curr_, new_pos.elem_curr_);
                *(--new_pos) = ::std::move(front());
                pop_front();
                return new_pos;
            }
        }

        constexpr iterator insert(const_iterator const pos, T const &value) {
            return emplace(pos, value);
        }

        constexpr iterator insert(const_iterator const pos, T &&value) {
            return emplace(pos, ::std::move(value));
        }

    private:
        static inline constexpr auto synth_three_way_ = []<class U, class V>(const U &u, const V &v) {
            if constexpr (::std::three_way_comparable_with<U, V>) {
                return u <=> v;
            } else {
                if (u < v)
                    return ::std::weak_ordering::less;
                if (v < u)
                    return ::std::weak_ordering::greater;
                return ::std::weak_ordering::equivalent;
            }
        };

    public:
        template<::std::ranges::input_range R>
            requires ::std::convertible_to<::std::ranges::range_value_t<R>, T>
        constexpr iterator insert_range(const_iterator const pos, R &&rg) {
            if (::std::ranges::empty(rg)) {
                return pos.remove_const_();
            }
            auto const begin_pre = begin();
            auto const end_pre = end();
            if (pos == end_pre) {
                auto const old_size = size();
                append_range_noguard_(::std::forward<R>(rg));
                return begin() + old_size;
            }
            if (pos == begin_pre) {
                prepend_range_noguard_(::std::forward<R>(rg));
                return begin();
            }
            auto const back_diff = end_pre - pos + 0uz;
            auto const front_diff = pos - begin_pre + 0uz;

            if (back_diff <= front_diff) {
                auto const old_size = size();
                append_range_noguard_(::std::forward<R>(rg));
                ::std::ranges::rotate(begin() + front_diff, begin() + old_size, end());
                return begin() + front_diff;
            } else {
                auto const old_size = size();
                prepend_range_noguard_(::std::forward<R>(rg));
                auto const count = size() - old_size;
                ::std::ranges::rotate(begin(), begin() + count, begin() + (count + front_diff));
                return begin() + front_diff;
            }
        }

        // 几乎等于insert_range,但是使用迭代器版本以支持input iterator
        template<::std::input_iterator U, typename V>
        constexpr iterator insert(const_iterator const pos, U first, V last) {
            if (first == last) {
                return pos.remove_const_();
            }
            auto const begin_pre = begin();
            auto const end_pre = end();
            if (pos == end_pre) {
                auto const old_size = size();
                append_range_noguard_(::std::forward<U>(first), ::std::forward<V>(last));
                return begin() + old_size;
            }
            if (pos == begin_pre) {
                prepend_range_noguard_(::std::forward<U>(first), ::std::forward<V>(last));
                return begin();
            }
            auto const back_diff = end_pre - pos + 0uz;
            auto const front_diff = pos - begin_pre + 0uz;
            if (back_diff <= front_diff) {
                auto const old_size = size();
                append_range_noguard_(::std::forward<U>(first), ::std::forward<V>(last));
                ::std::ranges::rotate(begin() + front_diff, begin() + old_size, end());
                return begin() + front_diff;
            } else {
                auto const old_size = size();
                prepend_range_noguard_(::std::forward<U>(first), ::std::forward<V>(last));
                auto const count = size() - old_size;
                ::std::ranges::rotate(begin(), begin() + count, begin() + (count + front_diff));
                return begin() + front_diff;
            }
        }

        constexpr iterator insert(const_iterator const pos, ::std::initializer_list<T> const ilist) {
            return insert(pos, ilist.begin(), ilist.end());
        }

        constexpr iterator insert(const_iterator const pos, ::std::size_t const count, T const &value) {
#if __cplusplus >= 202302L
            return insert_range(pos, ::std::ranges::views::repeat(value, count));
#else
            auto repeat_view = std::ranges::views::iota(0u, count)
                               | std::ranges::views::transform([&](auto) { return value; });
            return insert_range(pos, repeat_view);
#endif
        }

        constexpr iterator erase(const_iterator const pos) {
            auto const begin_pre = begin();
            auto const end_pre = end();
            if (pos == begin_pre) {
                pop_front();
                return begin();
            }
            if (pos + 1uz == end_pre) {
                pop_back();
                return end();
            }
            auto const back_diff = end_pre - pos + 0uz;
            auto const front_diff = pos - begin_pre + 0uz;
            if (back_diff <= front_diff) {
                ::std::ranges::move((pos + 1uz).remove_const_(), end(), pos.remove_const_());
                pop_back();
                return begin() + front_diff;
            } else {
                ::std::ranges::move_backward(begin(), pos.remove_const_(), (pos + 1uz).remove_const_());
                pop_front();
                return begin() + front_diff;
            }
        }

        constexpr iterator erase(const_iterator const first, const_iterator const last) {
            auto const begin_pre = begin();
            auto const end_pre = end();
            if (first == begin_pre) {
                pop_front_n_(last - first);
                return begin();
            }
            if (last == end_pre) {
                pop_back_n_(last - first);
                return end();
            }
            auto const back_diff = end_pre - last + 0uz;
            auto const front_diff = first - begin_pre + 0uz;
            if (back_diff <= front_diff) {
                ::std::ranges::move(last, end(), first.remove_const_());
                pop_back_n_(last - first);
                return begin() + front_diff;
            } else {
                ::std::ranges::move_backward(begin(), first.remove_const_(), last.remove_const_());
                pop_front_n_(last - first);
                return begin() + front_diff;
            }
        }
    };

    template<::std::movable T, typename allocator1, typename allocator2>
        requires ::std::equality_comparable<T>
    inline constexpr bool operator==(deque<T, allocator1> const &lhs, deque<T, allocator2> const &rhs) noexcept {
        if (auto const s = lhs.size(); s != rhs.size())
            return false;
        else if (s != 0uz) {
            auto first = lhs.begin();
            auto last = lhs.end();
            auto first1 = rhs.begin();
            for (; first != last; ++first, ++first1) {
                if (*first != *first1)
                    return false;
            }
        }
        return true;
    }

#if defined(__cpp_lib_three_way_comparison)
    template<::std::movable T, typename allocator1, typename allocator2>
        requires ::std::three_way_comparable<T>
    inline constexpr auto operator<=>(deque<T, allocator1> const &lhs, deque<T, allocator2> const &rhs) noexcept
        requires requires(T const &t, T const &t1)
        {
            { t < t1 } -> ::std::convertible_to<bool>;
        } {
        return ::std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                                                        []<class U, class V>(const U &u, const V &v) {
                                                            if constexpr (::std::three_way_comparable_with<U, V>)
                                                                return u <=> v;
                                                            else {
                                                                if (u < v)
                                                                    return ::std::weak_ordering::less;
                                                                if (v < u)
                                                                    return ::std::weak_ordering::greater;
                                                                return ::std::weak_ordering::equivalent;
                                                            }
                                                        });
    }
#endif

    template<::std::movable T, typename allocator>
    constexpr void swap(deque<T, allocator> &lhs, deque<T, allocator> &rhs) noexcept {
        lhs.swap(rhs);
    }
}
