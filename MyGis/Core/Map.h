#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <QtCore/QObject>
#include <QtCore/QHashFunctions>

#include "MyGisDefs.h"


namespace lh
{

class ImageLayer;
    
class Map : public QObject
{
    Q_OBJECT
public:
    Map();
    ~Map() override;

    // 添加图层
    void addLayer(ImageLayer* layer);

    // 删除图层
    void removeLayer(const LayerId& id);

    // 获取图层
    ImageLayer* getLayer(const LayerId& id) const;

    // 获取所有图层
    void getAllLayer(std::vector<ImageLayer*>& layers) const;

Q_SIGNALS:
    void layerAdded(ImageLayer* layer);
    void layerRemoved(const LayerId& layerId);
    void layerChanged(ImageLayer* layer);
    void imageUpdate(ImageLayer* layer);

private Q_SLOTS:
    void OnLayerChanged();
    void OnImageUpdate();

private:
    std::unordered_map<QString, std::unique_ptr<ImageLayer>> m_layers;

};

}
