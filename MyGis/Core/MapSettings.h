#pragma once

#include <QtCore/QPointF>
#include <QtCore/QRectF>

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
};

