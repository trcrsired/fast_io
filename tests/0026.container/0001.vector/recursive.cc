#include <variant>
#include <fast_io_dsal/vector.h>

struct Base;

struct Node1
{};
struct Node3
{};

struct Node2
{
	fast_io::vector<Base> subast;

	constexpr Node2();
	constexpr Node2(fast_io::vector<Base> sub);
	constexpr ~Node2();

	template <typename>
	constexpr int a_method(int) const noexcept;
};

struct Base
{
	std::variant<Node1, Node2, Node3> data;


	Base(Node1 n) : data(std::move(n))
	{}
	Base(Node2 n) : data(std::move(n))
	{}
	Base(Node3 n) : data(std::move(n))
	{}
};

constexpr Node2::Node2() = default;
constexpr Node2::Node2(fast_io::vector<Base> sub) : subast(std::move(sub))
{}
constexpr Node2::~Node2() = default;

template <typename>
constexpr int Node2::a_method(int) const noexcept
{
	return 42;
}

using Ast = fast_io::vector<Base>;

int main()
{
	Ast ast;
	ast.emplace_back(Node1{});
	ast.emplace_back(Node3{});


	Node2 nested;
	nested.subast.emplace_back(Node1{});
	ast.emplace_back(std::move(nested));


	for (auto const &node : ast)
	{
		std::visit([](auto const &n) {
			using T = std::decay_t<decltype(n)>;
			if constexpr (std::is_same_v<T, Node1>)
			{
			}
			else if constexpr (std::is_same_v<T, Node2>)
			{
				int const v{n.template a_method<int>(0)};
				if (v != 42)
				{
					::fast_io::fast_terminate();
				}
			}
			else if constexpr (std::is_same_v<T, Node3>)
			{
			}
		},
				   node.data);
	}
	return 0;
}
