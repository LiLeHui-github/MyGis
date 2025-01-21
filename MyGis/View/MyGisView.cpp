#include "MyGisView.h"

#include <cmath>

#include <QtCore/QTimer>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>

#include <spdlog/spdlog.h>

#include "MyGis/Core/Map.h"
#include "MyGis/Map/MapItemMap.h" 
#include "MyGis/Proj/MercatorProjection.h"

namespace lh
{

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
    , m_dragMap(false)
{
    system("chcp 65001");

    spdlog::set_pattern("[thread %t] %+");

    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene* scene = new QGraphicsScene();
    setScene(scene);

    QSizeF size = viewport()->size();
    scene->setSceneRect(QRectF{ QPointF{0, 0}, size });

    m_map.reset(map);

    m_mapItem = std::make_unique<MapItemMap>(map, scene);
    m_mapItem->setItemSize(size);

    m_proj = std::make_unique<MercatorProjection>();

    m_settings.m_proj = m_proj.get();

    m_refreshMapTimer = new QTimer(this);
    m_refreshMapTimer->setSingleShot(true);
    connect(m_refreshMapTimer, &QTimer::timeout, this, &MyGisView::refreshMap);

    initResolution(0, 21);
    setViewExtent(sceneRect());
    //setMapViewpointForProjection(QPointF{ 13526041.708072895 , 3664472.687460804 });
    setMapViewpointForProjection(QPointF{ 0 , 0 });
}

MyGisView::~MyGisView()
{
}

Map* MyGisView::getMap() const
{
    return m_map.get();
}

void MyGisView::setZoom(int zoom)
{
    setResolution(findResolutionForZoom(zoom));
}

int MyGisView::getMinZoom() const
{
    return m_minZoom;
}

int MyGisView::getMaxZoom() const
{
    return m_maxZoom;
}

void MyGisView::setMapViewpointForView(const QPointF& pixel)
{
    setMapViewpointForProjection(m_proj->toProjection(pixel));
}

void MyGisView::setMapViewpointForProjection(const QPointF& projection)
{
    m_settings.m_mapViewPoint = projection;
    updateProjectionMatrix();
    refresh();
}

int MyGisView::getZoom() const
{
    return m_settings.m_zoom;
}

double MyGisView::getResolution() const
{
    return m_settings.m_resolution;
}

double MyGisView::getMinResolution() const
{
    return m_minResolution;
}

double MyGisView::getMaxResolution() const
{
    return m_maxResolution;
}

QPointF MyGisView::getMapViewpoint() const
{
    return m_settings.m_mapViewPoint;
}

void MyGisView::refresh()
{
    //spdlog::info("refresh() called.");
    m_refreshMapTimer->start(1);
}

void MyGisView::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        m_dragMap = true;
        m_dragPos = event->pos();
    }
}

void MyGisView::mouseMoveEvent(QMouseEvent* event)
{
    if(m_dragMap)
    {
        const QPoint& current = event->pos();
        const QPointF& offset = (m_dragPos - current) * getResolution();
        m_dragPos = current;

        // 移动视点
        setMapViewpointForProjection(getMapViewpoint() + QPointF(offset.x(), -offset.y()));
    }
}

void MyGisView::mouseReleaseEvent(QMouseEvent* event)
{
    m_dragMap = false;
}

void MyGisView::wheelEvent(QWheelEvent* event)
{

    if(event->angleDelta().y() > 0)
    {
        // 放大
        zoomIn();
    }
    else
    {
        // 缩小
        zoomOut();
    }

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
    return getMaxResolution() / std::pow(2, zoom - getMinZoom());
}

int MyGisView::findZoomForViewExtent(const QRectF& extent) const
{
    double size = std::max<double>(extent.width(), extent.height());
    double zoom = std::log2(size / 256.0);
    return std::ceil(zoom);
}

int MyGisView::findUpperZoomForResolution(double resolution) const
{
    double zoom = std::log2(getMaxResolution() / resolution);
    return getMinZoom() + std::ceil(zoom);
}

int MyGisView::findLowerZoomForResolution(double resolution) const
{
    double zoom = std::log2(getMaxResolution() / resolution);
    return getMinZoom() + std::floor(zoom);
}

double MyGisView::resolutionConstraint(double resolution) const
{
    return clamp<double>(resolution, getMinResolution(), getMaxResolution());
}

void MyGisView::setResolution(double resolution)
{
    m_settings.m_resolution = resolution;
    m_settings.m_zoom = findLowerZoomForResolution(resolution);
    updateProjectionMatrix();
    refresh();
}

void MyGisView::setViewExtent(const QRectF& extent)
{
    m_settings.m_viewExtent = extent;
    int zoom = findZoomForViewExtent(extent);
    if(zoom > getZoom())
    {
        setZoom(zoom);
    }
    else
    {
        updateProjectionMatrix();
        refresh();
    }
}

void MyGisView::zoomIn()
{
    double currentResolution = getResolution();

    // 检查是否已经达到最小分辨率（最大缩放级别）
    if (currentResolution <= getMinResolution())
    {
        return;  // 已经是最大缩放级别，不能再放大
    }

    int currentZoom = findLowerZoomForResolution(currentResolution);
    double currentbaseResolution = findResolutionForZoom(currentZoom);
    double nextBaseResolution = findResolutionForZoom(currentZoom + 1);

    double newResolution = (currentbaseResolution + nextBaseResolution) / 2.0;

    // 如果当前分辨率等于两个基准分辨率的中值, 直接为下一级的分辨率
    if ((currentResolution - newResolution) <= std::numeric_limits<double>::epsilon())
    {
        newResolution = nextBaseResolution;
    }

    setResolution(resolutionConstraint(newResolution));
}

void MyGisView::zoomOut()
{
    double currentResolution = getResolution();

    // 检查是否已经达到最大分辨率（最小缩放级别）
    if (currentResolution >= getMaxResolution())
    {
        return;  // 已经是最小缩放级别，不能再缩小
    }

    int currentZoom = findUpperZoomForResolution(currentResolution);
    double currentbaseResolution = findResolutionForZoom(currentZoom);
    double nextBaseResolution = findResolutionForZoom(currentZoom - 1);

    double newResolution = (currentbaseResolution + nextBaseResolution) / 2.0;

    // 如果当前分辨率等于两个基准分辨率的中值, 直接为上一级的分辨率
    if ((newResolution - currentResolution) <= std::numeric_limits<double>::epsilon())
    {
        newResolution = nextBaseResolution;
    }

    setResolution(resolutionConstraint(newResolution));
}

void MyGisView::updateProjectionMatrix()
{
    m_proj->updateMatrix(m_settings);
}

void MyGisView::refreshMap()
{
    m_mapItem->stopRender();
    m_mapItem->startRender(m_settings);
}

}