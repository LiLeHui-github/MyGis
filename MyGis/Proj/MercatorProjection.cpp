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

void MercatorProjection::updateMatrix(const MapSettings& settings)
{
    const QPointF& viewExtentCenter = settings.m_viewExtent.center();
    const QPointF& mapViewpoint = settings.m_mapViewPoint;
    double resolution = settings.m_resolution;

    m_matrix = QTransform()
        .translate(mapViewpoint.x(), mapViewpoint.y())
        .scale(resolution, -resolution)
        .translate(-viewExtentCenter.x(), -viewExtentCenter.y());
}

QPointF MercatorProjection::toProjection(const MapSettings& settings, const QPointF& pixel) const
{
    // 将像素坐标转换到墨卡托投影坐标
    const QPointF& offset = (pixel - settings.m_viewExtent.center()) * settings.m_resolution;
    auto p1 = settings.m_mapViewPoint + QPointF(offset.x(), -offset.y());

    return m_matrix.map(pixel);
}

QPointF MercatorProjection::toPixel(const MapSettings& settings, const QPointF& projection) const
{
    // 将墨卡托投影坐标转换为像素坐标
    double invertResolution = 1.0 / settings.m_resolution;
    const QPointF& offset = (projection - settings.m_mapViewPoint) * invertResolution;
    auto p1 = settings.m_viewExtent.center() + QPointF(offset.x(), -offset.y());

    return m_matrix.inverted().map(projection);
}
