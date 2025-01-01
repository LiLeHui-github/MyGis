#include "Map.h"

#include <spdlog/spdlog.h>

#include "MyGis/Layer/ImageLayer.h"

Map::Map()
{
}

Map::~Map()
{
}

void Map::addLayer(ImageLayer* layer)
{
    auto it = m_layers.find(layer->layerId());
    if(it != m_layers.end())
    {
        spdlog::warn("图层已存在, 添加图层失败.");
        return;
    }

    m_layers[layer->layerId()].reset(layer);
}

void Map::removeLayer(const LayerId& id)
{
    auto it = m_layers.find(id);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
    }
}

ImageLayer* Map::getLayer(const LayerId& id) const
{
    ImageLayer* result = nullptr;

    auto it = m_layers.find(id);
    if (it != m_layers.end())
    {
        result = it->second.get();
    }

    return result;
}

void Map::getAllLayer(std::vector<ImageLayer*>& layers) const
{
    layers.reserve(m_layers.size());
    for(auto it = m_layers.begin(); it != m_layers.end(); ++it)
    {
        layers.push_back(it->second.get());
    }
}
