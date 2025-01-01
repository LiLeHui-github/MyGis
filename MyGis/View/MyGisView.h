#pragma once

#include <memory>

#include <QtWidgets/QGraphicsView>

#include "MyGis/Core/MapSettings.h"

class QTimer;
class Map;
class MapItemMap;
class Projection;

class MyGisView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MyGisView(QWidget *parent = nullptr);
    explicit MyGisView(Map* map, QWidget *parent = nullptr);
    ~MyGisView() override;

    Map* getMap() const;

    void refresh();

    void setCenterForView(const QPointF& pixel);

    void setCenterForProjection(const QPointF& projection);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void initResolution(int min, int max);
    double findResolutionForZoom(int zoom) const;
    int findZoomForViewExtent(const QRectF& extent) const;
    int findZoomForResolution(double resolution) const;
    double resolutionConstraint(double resolution) const;

    void setResolution(double resolution);

    void setZoom(int zoom);

    void setViewExtent(const QRectF& extent);

    void refreshMap();

private:
    int m_minZoom;
    int m_maxZoom;
    double m_minResolution;
    double m_maxResolution;

    std::unique_ptr<Map> m_map;
    std::unique_ptr<MapItemMap> m_mapItem;
    std::unique_ptr<Projection> m_proj;
    MapSettings m_settings;

    QTimer* m_refreshMapTimer;
};
