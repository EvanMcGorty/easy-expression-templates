#include<type_traits>
#include<utility>

namespace expression_templates
{
	namespace impl
	{

		template<typename funct_t>
		class delayed_result
		{
			template<typename t>
			friend struct taker;
		public:
			delayed_result(funct_t&& a) :
				res(std::move(a))
			{}

			delayed_result(delayed_result&&) = default;
			delayed_result(delayed_result const&) = default;

		private:
			funct_t res;
		};

		template<typename t>
		struct taken
		{
			using type = t & ;
		};

		template<typename t>
		struct taken<delayed_result<t>>
		{
			using type = decltype(std::declval<t>()());
		};

		template<typename t>
		struct taken<delayed_result<t>&>
		{
			using type = decltype(std::declval<t>()());
		};

		template<typename t>
		struct taker
		{
			static t& on(t& a)
			{
				return a;
			}

			static t& unprepare(t& a)
			{
				return a;
			}

			static t prepare(t&& a)
			{
				return a;
			}
		};

		template<typename t>
		struct taker<t&>
		{
			static t& on(t& a)
			{
				return a;
			}

			static t& unprepare(t* a)
			{
				return *a;
			}

			static t* prepare(t& a)
			{
				return &a;
			}
		};

		template<typename t>
		struct taker<delayed_result<t>>
		{
			static auto on(delayed_result<t>& a)
			{
				return std::move(a.res)();
			}

			static delayed_result<t>& unprepare(delayed_result<t>& a)
			{
				return a;
			}

			static delayed_result<t> prepare(delayed_result<t>&& a)
			{
				return a;
			}
		};

		template<typename t>
		struct taker<delayed_result<t>&>
		{
			static auto on(delayed_result<t>& a)
			{
				return std::move(a.res)();
			}

			static delayed_result<t>& unprepare(delayed_result<t>* a)
			{
				return *a;
			}

			static delayed_result<t>* prepare(delayed_result<t>& a)
			{
				return &a;
			}
		};

		template<typename t>
		typename taken<t>::type take(t& a)
		{
			return taker<t>::on(a);
		}


		template<typename funct_t, typename...args_t>
		auto call(funct_t&& to_call, args_t&&...args)
		{
			auto&& lambda =
				[to_call = std::move(to_call), args = taker<args_t>::prepare(std::forward<args_t>(args))...]() mutable
			{
				return std::move(to_call)(std::forward<typename taken<args_t>::type>(take<args_t>(taker<args_t>::unprepare(args)))...);
			};

			return delayed_result<std::remove_reference_t<decltype(lambda)>>(std::move(lambda));
		}
	}

	using impl::call;
	using impl::take;
}