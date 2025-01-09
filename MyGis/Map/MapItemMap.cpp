#include "MapItemMap.h"

#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include <spdlog/spdlog.h>

#include "MyGis/Core/Map.h"

#include "Layer/ImageLayer.h"
#include "MyGis/Core/MapSettings.h"
#include "MyGis/Core/ThreadPool.h"

MapItemMap::MapItemMap(Map* map, QGraphicsScene* scene, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_map(map)
{
    scene->addItem(this);

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

void MapItemMap::startRender(const MapSettings& settings)
{
    std::vector<ImageLayer*> layers;
    m_map->getAllLayer(layers);

    auto renderSettings = std::make_shared<MapSettings>(settings);

    for(ImageLayer* layer : layers)
    {
        ThreadPool::globalInstance()->submit([layer, renderSettings]()
        {
            layer->startRender(*renderSettings);
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
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    //painter->setClipRect(option->exposedRect);

    std::vector<ImageLayer*> layers;
    m_map->getAllLayer(layers);

    for (ImageLayer* layer : layers)
    {
        painter->drawImage(QPointF(0, 0), layer->getImage());
    }

}

void MapItemMap::OnLayerChanged(ImageLayer* layer)
{
    update();
}

void MapItemMap::OnImageUpdate(ImageLayer* layer)
{
    update();
}
