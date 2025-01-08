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
    void updateMatrix(const MapSettings& settings) override;
    QPointF toProjection(const QPointF &pixel) const override;
    QPointF toPixel(const QPointF &projection) const override;

private:
    QRectF m_extent;

    QTransform m_pixel_to_projection_matrix;
    QTransform m_projection_to_pixel_matrix;

};
