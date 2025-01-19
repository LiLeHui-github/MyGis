#pragma once

#include <QtCore/QPointF>

#include "MyGis/Core/MapSettings.h"

// 投影接口类, 用于将像素坐标转换到投影坐标
class Projection
{
public:
    virtual ~Projection() = default;

    // 获取投影范围
    virtual RectangleExtent getExtent() const = 0;

    // 更新变换矩阵
    virtual void updateMatrix(const MapSettings& settings) {}

    // 将像素坐标转换到投影坐标
    virtual QPointF toProjection(double px, double py) const = 0;

    // 将投影坐标转换到像素坐标
    virtual QPointF toPixel(double x, double y) const = 0;

    // 将像素坐标转换到投影坐标
    QPointF toProjection(const QPointF &pixel) const
    {
        return toProjection(pixel.x(), pixel.y());
    }

    // 将投影坐标转换到像素坐标
    QPointF toPixel(const QPointF &projection) const
    {
        return toPixel(projection.x(), projection.y());
    }
};