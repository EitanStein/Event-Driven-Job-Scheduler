#pragma once

#include <cstddef>

struct Resource {
    size_t memory;
    size_t cpu;

    [[nodiscard]] constexpr bool operator>(const Resource& other) const{
        return (memory > other.memory && cpu > other.cpu);
    }
    [[nodiscard]] constexpr bool operator>=(const Resource& other) const{
        return (memory >= other.memory && cpu >= other.cpu);
    }
    [[nodiscard]] constexpr bool operator<(const Resource& other) const{
        return (memory < other.memory && cpu < other.cpu);
    }
    [[nodiscard]] constexpr bool operator<=(const Resource& other) const{
        return (memory <= other.memory && cpu <= other.cpu);
    }
    [[nodiscard]] constexpr bool operator==(const Resource& other) const{
        return (memory == other.memory && cpu == other.cpu);
    }

    constexpr void operator-=(const Resource& other){
        memory -= other.memory; 
        cpu -= other.cpu;
    }

    constexpr void operator+=(const Resource& other){
        memory += other.memory; 
        cpu += other.cpu;
    }

};