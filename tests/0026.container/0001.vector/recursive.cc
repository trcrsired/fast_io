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

	Node2();
	Node2(fast_io::vector<Base> sub);
	~Node2();
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

Node2::Node2() = default;
Node2::Node2(fast_io::vector<Base> sub) : subast(std::move(sub))
{}
Node2::~Node2() = default;

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
			}
			else if constexpr (std::is_same_v<T, Node3>)
			{
			}
		},
				   node.data);
	}
	return 0;
}
