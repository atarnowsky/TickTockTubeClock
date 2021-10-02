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
}

template<typename Search, typename... Types>
constexpr size_t index_of() {
    return internal::resolve_index_of<Search, Types...>();
}