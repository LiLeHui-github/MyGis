#pragma once

#include <functional>

#include <QtCore/QString>
#include <QtGui/QImage>

// 图层Id
using LayerId = QString;

// 定义瓦片信息结构
struct TileInfo {
    int x;
    int y;
    int zoom;
    QImage image;
};

// 定义回调函数类型
using TileCallback = std::function<void(const TileInfo&)>;
using BatchCompleteCallback = std::function<void()>;
