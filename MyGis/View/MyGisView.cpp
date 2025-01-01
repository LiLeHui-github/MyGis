#include "MyGisView.h"

#include <cmath>

#include <QtCore/QTimer>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>

#include <spdlog/spdlog.h>

#include "MyGis/Core/Map.h"
#include "MyGis/Map/MapItemMap.h" 
#include "MyGis/Proj/MercatorProjection.h"

template <typename T>
T clamp(T val, T min, T max)
{
    return std::max<T>(min, std::min<T>(max, val));
}

MyGisView::MyGisView(QWidget* parent)
    : MyGisView(new Map(), parent)
{
}

MyGisView::MyGisView(Map* map, QWidget* parent)
    : QGraphicsView(parent)
{
    system("chcp 65001");

    QGraphicsScene* scene = new QGraphicsScene();
    setScene(scene);

    QSizeF size = viewport()->size();
    scene->setSceneRect(QRectF{ QPointF{0, 0}, size });

    m_map.reset(map);

    m_mapItem = std::make_unique<MapItemMap>(map, scene);
    m_mapItem->setItemSize(size);

    m_proj = std::make_unique<MercatorProjection>();

    m_refreshMapTimer = new QTimer(this);
    m_refreshMapTimer->setSingleShot(true);
    connect(m_refreshMapTimer, &QTimer::timeout, this, &MyGisView::refreshMap);

    initResolution(0, 21);
    setViewExtent(sceneRect());
    setCenterForProjection(QPointF{ 0, 0 });
}

MyGisView::~MyGisView()
{
}

Map* MyGisView::getMap() const
{
    return m_map.get();
}

void MyGisView::refresh()
{
    spdlog::info("refresh() called.");
    m_mapItem->stopRender();
    m_refreshMapTimer->start(30);
}

void MyGisView::setCenterForView(const QPointF& pixel)
{
    setCenterForProjection(m_proj->toProjection(m_settings, pixel));
}

void MyGisView::setCenterForProjection(const QPointF& projection)
{
    m_settings.m_mapViewPoint = projection;
    refresh();
}

void MyGisView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
}

void MyGisView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void MyGisView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void MyGisView::wheelEvent(QWheelEvent* event)
{
    double currentResolution = m_settings.m_resolution;
    int currentZoom = findZoomForResolution(currentResolution);
    double baseResolution = findResolutionForZoom(currentZoom);
    double newResolution = 0;

    if(event->angleDelta().y() > 0)
    {
        // 放大 
        double targetResolution = findResolutionForZoom(currentZoom + 1);
        double step = (baseResolution - targetResolution) * 0.5;
        newResolution = resolutionConstraint(currentResolution - step);
    }
    else
    {
        // 缩小
        double targetResolution = findResolutionForZoom(currentZoom - 1);
        double step = (targetResolution - baseResolution) * 0.5;
        newResolution = resolutionConstraint(currentResolution + step);
    }

    setResolution(newResolution);
}

void MyGisView::resizeEvent(QResizeEvent* event)
{
    QSizeF size = event->size();

    scene()->setSceneRect(QRectF{ QPointF{0, 0}, size });
    m_mapItem->setItemSize(size);
    setViewExtent( sceneRect());

    QGraphicsView::resizeEvent(event);
}

void MyGisView::initResolution(int min, int max)
{
    m_minZoom = min;
    m_maxZoom = max;

    double mapWidth = m_proj->getExtent().width();

    m_minResolution = mapWidth / (256.0 * std::pow(2, m_maxZoom));
    m_maxResolution = mapWidth / (256.0 * std::pow(2, m_minZoom));
}

double MyGisView::findResolutionForZoom(int zoom)  const
{
    return m_maxResolution / std::pow(2, zoom);
}

int MyGisView::findZoomForViewExtent(const QRectF& extent) const
{
    double size = std::min<double>(extent.width(), extent.height());
    return std::ceil( std::log2(size / 256.0) );
}

int MyGisView::findZoomForResolution(double resolution) const
{
    return std::ceil( std::log2(m_maxResolution / resolution) );
}

double MyGisView::resolutionConstraint(double resolution) const
{
    return clamp<double>(resolution, m_minResolution, m_maxResolution);
}

void MyGisView::setResolution(double resolution)
{
    m_settings.m_resolution = resolution;
    m_settings.m_zoom = findZoomForResolution(resolution);
    refresh();
}

void MyGisView::setZoom(int zoom)
{
    setResolution(findResolutionForZoom(zoom));
}

void MyGisView::setViewExtent(const QRectF& extent)
{
    m_settings.m_viewExtent = extent;
    setZoom(findZoomForViewExtent(extent));
}

void MyGisView::refreshMap()
{
    spdlog::info("refreshMap() called.");
    m_mapItem->startRender(m_settings);
}
