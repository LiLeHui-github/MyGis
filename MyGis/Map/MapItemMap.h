#pragma once

#include <QtCore/QObject>
#include <QtCore/QSizeF>
#include <QtWidgets/QGraphicsItem>

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

    void startRender(const MapSettings& settings);
    void stopRender();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private Q_SLOTS:
    void OnLayerChanged(ImageLayer* layer);
    void OnImageUpdate(ImageLayer* layer);

private:
    QSizeF m_itemSize;
    Map* m_map;
};

}
