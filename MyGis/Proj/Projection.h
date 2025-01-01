#pragma once

#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include "MyGis/Core/MapSettings.h"

// 投影接口类, 用于将像素坐标转换到投影坐标
class Projection
{
public:
    virtual ~Projection() = default;

    // 获取投影范围
    virtual QRectF getExtent() const = 0;

    // 更新变换矩阵
    virtual void updateMatrix(const MapSettings& settings) {}

    // 将像素坐标转换到投影坐标
    virtual QPointF toProjection(const QPointF &pixel) const = 0;

    // 将投影坐标转换到像素坐标
    virtual QPointF toPixel(const QPointF &projection) const = 0;
};