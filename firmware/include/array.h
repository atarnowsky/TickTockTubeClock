#pragma once

using size_t = unsigned int;

template<typename T, size_t s>
class array {
    public:
        constexpr array() = default;
        constexpr array(const array<T, s>&) = default;
        constexpr array(array<T, s>&&) = default;

        constexpr array<T,s>& operator=(const array<T, s>&) = default;
        constexpr array<T,s>& operator=(array<T, s>&&) = default;

        template<typename... args>
        constexpr array(args&&... init) : values{T(init)...} {}

        template<typename... args>
        constexpr array(const args&... init) : values{T(init)...} {}

        constexpr inline T& operator[](size_t i) {
            return values[i];
        }

        constexpr inline const T& operator[](size_t i) const {
            return values[i];
        }

        constexpr inline size_t size() const {
            return s;
        }

        // Todo: Implement proper iterators

    private:
        T values[s];
};