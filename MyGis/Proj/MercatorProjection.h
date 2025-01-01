#pragma once

#include <QtGui/QTransform>

#include "MyGis/Proj/Projection.h"

class MercatorProjection : public Projection
{
public:
    MercatorProjection();
    ~MercatorProjection() override;

protected:
    QRectF getExtent() const override;
    QPointF toProjection(const MapSettings& settings, const QPointF &pixel) const override;
    QPointF toPixel(const MapSettings& settings, const QPointF &projection) const override;

private:
    QRectF m_extent;
};
