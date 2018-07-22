#include<type_traits>
#include<utility>
#include<functional>

namespace expression_templates
{
	namespace impl
	{
		template<typename t>
		class value
		{
			template<typename t>
			friend value<t> take(t&& a);
			template<typename funct_ty, typename...args_ty>
			friend auto call(funct_ty&& funct, args_ty&&...args);
		public:

			value(value&& a)
			{
				held = std::move(a.held); //in order to lambda capture variadic lists by move without tuples
			}

			t operator()() const
			{
				return std::move(held);
			}
			
			~value() = default;

		private:

			value(t&& a) :
				held(std::move(a))
			{}

			value(value const& a)
			{
				held = std::move(a.held); //in order to lambda capture variadic lists by move without tuples
			}

			void operator=(value&&) = delete;
			void operator=(value const&) = delete;

			mutable t held;
		};

		template<typename t>
		class value<t&>
		{
			template<typename t>
			friend value<t&> ref(t& a);
			template<typename funct_ty, typename...args_ty>
			friend auto call(funct_ty&& funct, args_ty&&...args);
		public:

			value(value&& a)
			{
				held = a.held;
			}

			t& operator()() const
			{
				return *held;
			}

			~value() = default;
		
		private:

			value(t& a) :
				held(&a)
			{}

			value(value const& a)
			{
				held = a.held;
			}

			void operator=(value&&) = delete;
			void operator=(value const&) = delete;

			mutable t* held;
		};

		template<typename t>
		class value<t&&>
		{
			template<typename t>
			friend value<t&&> use(t&& a);
			template<typename funct_ty, typename...args_ty>
			friend auto call(funct_ty&& funct, args_ty&&...args);
		public:

			value(value&& a)
			{
				held = a.held;
			}

			t&& operator()() const
			{
				return std::move(*held);
			}
			
			~value() = default;

		private:

			value(t&& a) :
				held(&a)
			{}

			value(value const& a)
			{
				held = a.held;
			}

			void operator=(value&&) = delete;
			void operator=(value const&) = delete;

			mutable t* held;
		};

		template<typename lambda_ty>
		class result
		{
			template<typename funct_ty, typename...args_ty>
			friend auto call(funct_ty&& funct, args_ty&&...args);
		public:

			result(result&& a)
			{
				held = std::move(a.held); //in order to lambda capture variadic lists by move without tuples
			}

			decltype(std::declval<lambda_ty>()())
				operator()() const
			{
				return std::move(held)();
			}
			
			~result() = default;

		private:

			result(lambda_ty&& a) :
				held(std::move(a))
			{}

			result(result const& a)
			{
				held = std::move(a.held); //in order to lambda capture variadic lists by move without tuples
			}

			void operator=(result&&) = delete;
			void operator=(result const&) = delete;

			mutable lambda_ty held;
		};


		template<typename t>
		struct types
		{
			static constexpr bool is_template()
			{
				return false;
			}
		};

		template<typename t>
		struct types<value<t>>
		{
			static constexpr bool is_template()
			{
				return true;
			}
		};

		template<typename t>
		struct types<result<t>>
		{
			static constexpr bool is_template()
			{
				return true;
			}
		};

		template<typename t>
		value<t> take(t&& a)
		{
			return value<t>(std::move(a));
		}

		template<typename t>
		value<t&> ref(t& a)
		{
			return value<t&>(a);
		}

		template<typename t>
		value<t&&> use(t&& a)
		{
			return value<t&&>(std::move(a));
		}


		template<bool a = true, bool...r>
		constexpr bool and_all()
		{
			return (sizeof...(r)==0)  ?  (a)  :  (a && and_all<r...>());
		}

		template<typename funct_ty, typename...args_ty>
		auto call(funct_ty&& funct, args_ty&&...args)
		{
			static_assert(and_all<types<args_ty>::is_template()...>(),"arguments of \"call\" must be created through \"take\", \"ref\", \"use\", or another \"call\"");
			
			auto&& lambda =
			[=,funct = std::move(funct)]() mutable
			{
				return funct(args()...);
			};
			return result<std::remove_reference_t<decltype(lambda)>>(std::move(lambda));
		}
	}

	using impl::take;
	using impl::ref;
	using impl::use;
	using impl::call;
}