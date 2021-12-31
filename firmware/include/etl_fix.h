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
            if constexpr (TSearch::critical == THead::critical) {
                if constexpr (TSearch::critical) {
                    if constexpr (TSearch::cycle_count != 0)
                        return 1 + resolve_index_of<TSearch, TTail...>();
                    else 
                        return resolve_index_of<TSearch, TTail...>();
                } else {
                    if constexpr (TSearch::minimal_delay != 0)
                        return 1 + resolve_index_of<TSearch, TTail...>();
                    else
                        return resolve_index_of<TSearch, TTail...>();
                }
            } else {
                return resolve_index_of<TSearch, TTail...>();
            }            
        }
    }

    // TODO: Can these two functions be done less verbose?

    template<typename THead, typename... TTail>
    constexpr size_t resolve_sizeof_critical()
    {        
        if constexpr (THead::critical) {
            if constexpr (sizeof...(TTail) == 0)
                return (THead::cycle_count != 0 ? 1 : 0);
            else
                return (THead::cycle_count != 0 ? 1 : 0) + resolve_sizeof_critical<TTail...>();
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
        if constexpr (!THead::critical) {
            if constexpr (sizeof...(TTail) == 0)
                return (THead::minimal_delay != 0 ? 1 : 0);
            else
                return (THead::minimal_delay != 0 ? 1 : 0) + resolve_sizeof_relaxed<TTail...>();
        }
        else {
            if constexpr (sizeof...(TTail) == 0)
                return 0;
            else
                return 0 + resolve_sizeof_relaxed<TTail...>();
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
    return internal::resolve_sizeof_critical<Types...>() + 2;
}



template<typename... Types>
constexpr size_t sizeof_relaxed() {
    return internal::resolve_sizeof_relaxed<Types...>() + 2; // FIXME: Array is to small, but why?
}


template <typename... Types, class Funct>
constexpr void call_nth(uint8_t index, Funct&& fun)
{
    uint8_t counter = 0;
    auto selector = [&](auto&& T) -> bool{
        if(index == counter++) {
            fun(T);
            return false;
        }
        return true;
    }; 
    // TODO: This probably can be done in a more elegant way
    // without needing to instanciate the class. (The impact shouldn't
    // be that critical, though, since they are all pure static).
    (selector(Types()) && ...);
}

// Intels fastrand
// Found here: https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c

constexpr uint16_t FASTRAND_MAX = 0x7FFF;

inline uint16_t fastrand() { 
  static uint32_t seed = 0;
  seed = (214013*seed+2531011); 
  return ((seed>>16)&0x7FFF); 
} 

inline uint16_t fastrand_10() { 
  return fastrand() >> 5; 
} 

inline uint16_t fastrand(uint16_t max) { 
    return max * (fastrand()/float(FASTRAND_MAX));
} 

inline uint16_t fastrand_sq(uint16_t max) { 
    const float tmp = fastrand()/float(FASTRAND_MAX);
    return max * tmp * tmp;
} 