#pragma once

#include <type_traits>

#include "MyGis/Core/MyGisDefs.h"

template<>
struct std::hash<TileId>
{
    std::size_t operator() (const TileId& id) const noexcept
    {
        auto hash = std::hash<int>();
        return hash(id.z) ^ hash(id.x) ^ hash(id.y);
    }
};
