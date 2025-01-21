#pragma once

#include <functional>

#include <QtCore/QString>
#include <QtGui/QImage>

namespace lh
{

// 图层Id
using LayerId = QString;

// 定义瓦片id
struct TileId
{
    int z;
    int x;
    int y;

    double scale;
    QPointF pixel;

    TileId() : z(0), x(0), y(0), scale(1)
    {
    }

    TileId(int _z, int _x, int _y, double _scale) : z(_z), x(_x), y(_y), scale(_scale)
    {
    }

    TileId(int _z, int _x, int _y, double _scale, const QPointF& _pixel) : z(_z), x(_x), y(_y), scale(_scale), pixel(_pixel)
    {
    }

    bool operator == (const TileId& other) const noexcept
    {
        return z == other.z &&
            x == other.x &&
            y == other.y;
    }
};

// 定义回调函数类型
using TileCallback = std::function<void(const TileId& id, const QImage& image)>;
using BatchCompleteCallback = std::function<void()>;

}