#pragma once

#include <memory>
#include <map>

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

    void setZoom(int zoom);

    void setMapViewpointForView(const QPointF& pixel);

    void setMapViewpointForProjection(const QPointF& projection);

    int getZoom() const;

    int getMinZoom() const;

    int getMaxZoom() const;

    double getResolution() const;

    double getMinResolution() const;

    double getMaxResolution() const;

    QPointF getMapViewpoint() const;

    void refresh();

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

    int findUpperZoomForResolution(double resolution) const;

    int findLowerZoomForResolution(double resolution) const;

    double resolutionConstraint(double resolution) const;

    void setResolution(double resolution);

    void setViewExtent(const QRectF& extent);

    // 放大
    void zoomIn();

    // 缩小
    void zoomOut();
        
    void updateProjectionMatrix();

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

    // 移动处理
    bool m_dragMap;
    QPointF m_dragPos;

};
