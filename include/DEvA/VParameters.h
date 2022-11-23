#pragma once

#include <array>
#include <variant>

namespace DEvA {
    template <size_t lookingFor, size_t curIndex, typename Type, typename ... Types>
    constexpr auto getType() {
        if constexpr (curIndex == lookingFor) {
            return Type();
        } else {
            return getType<lookingFor, curIndex + 1, Types ...>();
        }
    }

    template <size_t lookingFor, typename ... Types>
    constexpr auto getType() {
        return getType<lookingFor, 0, Types ...>();
    }

    template <typename EnumClass, typename ... Types>
    class VParameters {
        private:
            using PVariant = std::variant<Types...>;
            using PEnum = EnumClass;
            std::array<PVariant, sizeof...(Types)> pvec;

        public:
            template <size_t ind>
            requires (ind <= sizeof...(Types))
                constexpr auto & get() {
                return std::get<decltype(getType<ind, 0, Types...>())>(pvec[ind]);
            };
            template <size_t ind>
            requires (ind <= sizeof...(Types))
                constexpr void set(decltype(getType<ind, 0, Types...>()) t) {
                pvec[ind] = t;
            };
            template <PEnum ind>
            requires (static_cast<size_t>(ind) <= sizeof...(Types))
                constexpr auto & get() {
                constexpr size_t ind_ = static_cast<size_t>(ind);
                return std::get<decltype(getType<ind_, 0, Types...>())>(pvec[ind_]);
            };
            template <PEnum ind>
            requires (static_cast<size_t>(ind) <= sizeof...(Types))
                constexpr void set(decltype(getType<static_cast<size_t>(ind), 0, Types...>()) t) {
                constexpr size_t ind_ = static_cast<size_t>(ind);
                pvec[ind_] = t;
            };
            template <typename T>
            constexpr auto & get(size_t ind) {
                return std::get<T>(pvec[ind]);
            };
            template <typename T>
            constexpr void set(size_t ind, T t) {
                pvec[ind] = t;
            };
            constexpr size_t size() {
                return sizeof...(Types);
            };
    };

    enum class NullEnum : size_t {};
    using NullVParameters = VParameters<NullEnum, int>;
}
