#pragma once

#include <functional>

#include <QtCore/QString>
#include <QtGui/QImage>

// 图层Id
using LayerId = QString;

// 定义瓦片id
struct TileId
{
    int z;
    int x;
    int y;

    QPointF pixel;

    bool operator == (const TileId& other) const noexcept
    {
        return z == other.z &&
            x == other.x &&
            y == other.y;
    }
};

// 定义瓦片信息结构
struct TileInfo
{
    TileId id;
    QImage image;
};

// 定义回调函数类型
using TileCallback = std::function<void(const TileInfo&)>;
using BatchCompleteCallback = std::function<void()>;
