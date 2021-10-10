#pragma once
#undef min
#undef max
#define ETL_NO_STL
#include <etl/array.h>
#include <etl/type_traits.h>
#include <etl/utility.h>
using namespace etl;

// ETL does not include i.e. make_index_sequence, this library does:
#include <ArxTypeTraits.h>

namespace internal {
    template<typename TSearch, typename THead, typename... TTail>
    constexpr size_t resolve_index_of()
    {
        if constexpr (std::is_same<TSearch, THead>::value) {
            return 0;
        }
        else {
            return 1 + resolve_index_of<TSearch, TTail...>();
        }
    }

    template<typename TSearch, typename THead, typename... TTail>
    constexpr size_t resolve_auto_index_of()
    {
        if constexpr (std::is_same<TSearch, THead>::value) {
            return 0;
        }
        else {
            return (TSearch::critical == THead::critical ? 1 : 0) + resolve_index_of<TSearch, TTail...>();
        }
    }

    // TODO: Can these two functions be done less verbose?

    template<typename THead, typename... TTail>
    constexpr size_t resolve_sizeof_critical()
    {        
        if constexpr (THead::critical) {
            if constexpr (sizeof...(TTail) == 0)
                return 1;
            else
                return 1 + resolve_sizeof_critical<TTail...>();
        }
        else {
            if constexpr (sizeof...(TTail) == 0)
                return 0;
            else
                return 0 + resolve_sizeof_critical<TTail...>();
        }
    }

    template<typename THead, typename... TTail>
    constexpr size_t resolve_sizeof_relaxed()
    {        
        if constexpr (THead::critical) {
            if constexpr (sizeof...(TTail) == 0)
                return 0;
            else
                return 0 + resolve_sizeof_relaxed<TTail...>();
        }
        else {
            if constexpr (sizeof...(TTail) == 0)
                return 1;
            else
                return 1 + resolve_sizeof_relaxed<TTail...>();
        }
    }
}

template<typename Search, typename... Types>
constexpr size_t index_of() {
    return internal::resolve_index_of<Search, Types...>();
}

template<typename Search, typename... Types>
constexpr size_t auto_index_of() {
    return internal::resolve_auto_index_of<Search, Types...>();
}



template<typename... Types>
constexpr size_t sizeof_critical() {
    return internal::resolve_sizeof_critical<Types...>() + 1;
}



template<typename... Types>
constexpr size_t sizeof_relaxed() {
    return internal::resolve_sizeof_relaxed<Types...>() + 1;
}
