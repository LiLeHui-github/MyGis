#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <QtCore/QHashFunctions>

#include "MyGisDefs.h"

class ImageLayer;

class Map
{
public:
    Map();
    ~Map();

    // 添加图层
    void addLayer(ImageLayer* layer);

    // 删除图层
    void removeLayer(const LayerId& id);

    // 获取图层
    ImageLayer* getLayer(const LayerId& id) const;

    // 获取所有图层
    void getAllLayer(std::vector<ImageLayer*>& layers) const;

private:
    std::unordered_map<QString, std::unique_ptr<ImageLayer>> m_layers;

};
