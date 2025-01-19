#pragma once

#include <QtGui/QTransform>

#include "MyGis/Proj/Projection.h"

class MercatorProjection : public Projection
{
public:
    MercatorProjection();
    ~MercatorProjection() override;

protected:
    RectangleExtent getExtent() const override;
    void updateMatrix(const MapSettings& settings) override;
    QPointF toProjection(double px, double py) const override;
    QPointF toPixel(double x, double y) const override;

private:
    RectangleExtent m_extent;

    QTransform m_pixel_to_projection_matrix;
    QTransform m_projection_to_pixel_matrix;

};
