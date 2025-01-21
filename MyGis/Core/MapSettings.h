#pragma once

#include <QtCore/QRectF>
#include "MyGis/Core/Rectangle.h"

namespace lh
{

class Projection;

class MapSettings
{
public:
    // 缩放级别
    int m_zoom = 0;

    // 分辨率
    double m_resolution = 0;

    // 地图中心视点坐标
    QPointF m_mapViewPoint;

    // 视口范围
    QRectF m_viewExtent;

    // 投影
    Projection* m_proj;

    Rectangle getMapExtent() const
    {
        double width = m_viewExtent.width() * m_resolution;
        double height = m_viewExtent.height() * m_resolution;

        double helfWidth = width * 0.5;
        double helfHeight = height * 0.5;

        return Rectangle{
            m_mapViewPoint.x() - helfWidth, m_mapViewPoint.x() + helfWidth,
            m_mapViewPoint.y() - helfHeight, m_mapViewPoint.y() + helfHeight
        };
    }
};

}