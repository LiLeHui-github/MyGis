#pragma once

#include <type_traits>

#include <QtCore/QString>
#include <QtCore/QHashFunctions>

#include "MyGis/Core/MyGisDefs.h"

template<>
struct std::hash<QString>
{
    std::size_t operator() (const QString& str) const noexcept
    {
        return qHash(str);
    }
};

template<>
struct std::hash<lh::TileId>
{
    std::size_t operator() (const lh::TileId& id) const noexcept
    {
        auto hash = std::hash<int>();
        return hash(id.z) ^ hash(id.x) ^ hash(id.y);
    }
};
