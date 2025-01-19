#pragma once

#include <QtCore/QPointF>
#include <QtCore/QSizeF>

class RectangleExtent
{
public:
    RectangleExtent()
        : xMin(0), xMax(0), yMin(0), yMax(0)
    {
    }

    RectangleExtent(double _xmin, double _xmax, double _ymin, double _ymax)
        : xMin(_xmin), xMax(_xmax), yMin(_ymin), yMax(_ymax)
    {
    }

    double width () const
    {
        return xMax - xMin;
    }

    double height () const
    {
        return yMax - yMin;
    }

    QPointF center() const
    {
        return QPointF{ xMin + width() * 0.5, yMin + height() * 0.5 };
    }

    QSizeF size() const
    {
        return QSizeF(width(), height());
    }

    bool isNull() const
    {
        return width() <= 0 || height() <= 0;
    }

    double xMin;
    double xMax;
    double yMin;
    double yMax;
};
