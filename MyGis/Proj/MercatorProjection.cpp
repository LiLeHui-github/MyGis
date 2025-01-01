#include "MercatorProjection.h"

MercatorProjection::MercatorProjection()
{
    m_extent = QRectF(-20037508.342789244, -20037508.342789244, 20037508.342789244, 20037508.342789244);
}

MercatorProjection::~MercatorProjection()
{
}

QRectF MercatorProjection::getExtent() const
{
    return m_extent;
}

QPointF MercatorProjection::toProjection(const MapSettings& settings, const QPointF& pixel) const
{  
    // 将像素坐标转换到墨卡托投影坐标
    const QPointF& offset = (pixel - settings.m_viewExtent.center()) * settings.m_resolution;
    return settings.m_mapViewPoint + QPointF(offset.x(), -offset.y());
}

QPointF MercatorProjection::toPixel(const MapSettings& settings, const QPointF& projection) const
{
    // 将墨卡托投影坐标转换为像素坐标
    const QPointF& offset = (projection - settings.m_mapViewPoint) / settings.m_resolution;
    return settings.m_viewExtent.center() + QPointF(offset.x(), -offset.y());
}
