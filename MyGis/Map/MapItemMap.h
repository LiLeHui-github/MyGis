#pragma once

#include <QtCore/QSizeF>
#include <QtWidgets/QGraphicsItem>

class QGraphicsScene;
class Map;
class MapSettings;

class MapItemMap : public QGraphicsItem
{
public:
    explicit MapItemMap(Map* map, QGraphicsScene* scene, QGraphicsItem* parent = nullptr);
    ~MapItemMap() override;

    void setItemSize(const QSizeF& size);
    QRectF boundingRect() const override;

    void startRender(const MapSettings& settings);
    void stopRender();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QSizeF m_itemSize;
    Map* m_map;
};

