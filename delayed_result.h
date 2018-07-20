#include<utility>
#include<string>

namespace expression_templates
{
    namespace impl
    {

        template<typename funct_t>
        class delayed_result
        {
            template<typename t>
            friend class taker;
        public:
            delayed_result(funct_t&& a) :
                res(std::move(a))
            {}
        private:
            funct_t res;
        };


        template<typename t>
        struct taker
        {
            static t on(t&& a)
            {
                return std::forward<t>(a);
            }
        };

        template<typename t>
        struct taker<delayed_result<t>&>
        {
            static auto on(delayed_result<t>& a)
            {
                return std::move(a.res)();
            }
        };

        template<typename t>
        auto take(t&& a)
        {
            return taker<t>::on(std::forward<t>(a));
        }

        
        template<typename funct_t, typename...args_t>
        auto call(funct_t to_call, args_t&&...args)
        {
            auto lambda = 
                [&]()
                {
                    return to_call(take(std::forward<args_t>(args))...);
                };

            return delayed_result<decltype(lambda)>(std::move(lambda));
        }
    }
}