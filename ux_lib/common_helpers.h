//
// Created by Bernd Lappas on 22.11.15.
//

#ifndef OPERATOR_FORWARDING_COMMON_HELPERS_H
#define OPERATOR_FORWARDING_COMMON_HELPERS_H


// http://florianjw.de/en/variadic_templates.html

template<typename Fun, typename...Ts>
void sequential_foreach(Fun f, const Ts &... args)
{
    (void) std::initializer_list<int>{(f(args), 0)...};
}

template<typename...Ts>
void print_all(std::ostream &stream, const Ts &... args)
{
    sequential_foreach([&](const auto &arg) { stream << arg; }, args...);
}

#endif //OPERATOR_FORWARDING_COMMON_HELPERS_H
