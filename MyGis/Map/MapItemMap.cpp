#include "MapItemMap.h"

#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include <spdlog/spdlog.h>

#include "MyGis/Core/Map.h"

#include "Layer/ImageLayer.h"
#include "MyGis/Core/MapSettings.h"
#include "MyGis/Core/ThreadPool.h"

namespace lh
{

MapItemMap::MapItemMap(Map* map, QGraphicsScene* scene, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_map(map)
{
    scene->addItem(this);

    connect(map, &Map::layerAdded, this, &MapItemMap::OnLayerAdded);
    connect(map, &Map::layerRemoved, this, &MapItemMap::OnLayerRemoved);
    connect(map, &Map::layerChanged, this, &MapItemMap::OnLayerChanged);
    connect(map, &Map::imageUpdate, this, &MapItemMap::OnImageUpdate);
}

MapItemMap::~MapItemMap()
{
    scene()->removeItem(this);
}

void MapItemMap::setItemSize(const QSizeF& size)
{
    m_itemSize = size;
    prepareGeometryChange();
    update();
}

QRectF MapItemMap::boundingRect() const
{
    return QRectF{ QPointF{0, 0}, m_itemSize };
}

void MapItemMap::startRenderBySettings(const MapSettings& settings)
{
    m_renderSettings = std::make_shared<MapSettings>(settings);
    startRender();
}

void MapItemMap::startRender()
{
    std::vector<ImageLayer*> layers;
    m_map->getAllLayer(layers);

    for (ImageLayer* layer : layers)
    {
        ThreadPool::globalInstance()->submit([layer, this]()
        {
            layer->startRender(*m_renderSettings);
        });
    }
}

void MapItemMap::stopRender()
{
    std::vector<ImageLayer*> layers;
    m_map->getAllLayer(layers);

    for (ImageLayer* layer : layers)
    {
        layer->cancelRender();
    }
}

void MapItemMap::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHints(QPainter::Antialiasing);

    std::vector<ImageLayer*> layers;
    m_map->getAllLayer(layers);

    for (ImageLayer* layer : layers)
    {
        painter->drawImage(QPointF(0, 0), layer->getImage());
    }

}

void MapItemMap::OnLayerAdded(ImageLayer* layer)
{
    refreshMap();
}

void MapItemMap::OnLayerRemoved(const LayerId& layerId)
{
    refreshMap();
}

void MapItemMap::OnLayerChanged(ImageLayer* layer)
{
    update();
}

void MapItemMap::OnImageUpdate(ImageLayer* layer)
{
    update();
}

void MapItemMap::refreshMap()
{
    stopRender();
    startRender();
}

}
