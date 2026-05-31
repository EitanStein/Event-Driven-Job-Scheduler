#pragma once

#include <cstddef>

struct Resource {
    size_t memory;
    size_t cpu;

    Resource() : memory(0), cpu(0) {}
    Resource(size_t mem, size_t cpu) : memory(mem), cpu(cpu) {}
    Resource(const Resource&) = default;
    Resource(Resource&&) = default;
};