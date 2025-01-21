#pragma once

#include <QtCore/QObject>
#include <QtCore/QSizeF>
#include <QtWidgets/QGraphicsItem>

#include "Core/MyGisDefs.h"

class QGraphicsScene;

namespace lh
{

class Map;
class MapSettings;
class ImageLayer;

class MapItemMap : public QObject,  public QGraphicsItem
{
    Q_OBJECT
public:
    explicit MapItemMap(Map* map, QGraphicsScene* scene, QGraphicsItem* parent = nullptr);
    ~MapItemMap() override;

    void setItemSize(const QSizeF& size);
    QRectF boundingRect() const override;

    void setRenderSettings(const MapSettings& settings);

    void startRenderBySettings(const MapSettings& settings);

    void startRender();

    void stopRender();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private Q_SLOTS:
    void OnLayerAdded(ImageLayer* layer);
    void OnLayerRemoved(const LayerId& layerId);
    void OnLayerChanged(ImageLayer* layer);
    void OnImageUpdate(ImageLayer* layer);

private:
    void refreshMap();

private:
    QSizeF m_itemSize;
    Map* m_map;
    std::shared_ptr<MapSettings> m_renderSettings;
};

}
