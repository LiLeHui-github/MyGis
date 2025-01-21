#include "TmsImageSource.h"

#include <QtCore/QFileInfo>

#include "spdlog/spdlog.h"

#include "MyGis/Core/ThreadPool.h"
#include "MyGis/Proj/Projection.h"

namespace lh
{

TmsImageSource::TmsImageSource(const QString& url)
    : m_url(url)
{
}

TmsImageSource::~TmsImageSource()
{
}

void TmsImageSource::requestTiles(const MapSettings& settings,
    const Rectangle& lastMapExtent,
    const TileLoadCallback& tileLoadCallback,
    const TileBatchLoadCompleteCallback& tileBatchLoadComplete)
{
    m_worldExtent = settings.m_proj->getExtent();
    const Rectangle& mapExtent = settings.getMapExtent();

    int zoom = settings.m_zoom;
    double scale = settings.m_resolution / (m_worldExtent.width() / (256.0 * std::pow(2, zoom)));
    double unitTileWidth = 256.0 * settings.m_resolution * scale;

    TileRange range;
    calcTileRange(1.0 / unitTileWidth, mapExtent, range);


}

void TmsImageSource::cancelRequest()
{
}

void TmsImageSource::calcTileRange(double invertUnitTile,
    const Rectangle& mapExtent,
    TileRange& range) const
{
    // 计算相对于世界范围左上角的偏移距离
    double offsetX = mapExtent.xMin - m_worldExtent.xMin;
    double offsetY = m_worldExtent.yMax - mapExtent.yMax;
    double offsetMaxX = mapExtent.xMax -  m_worldExtent.xMin;
    double offsetMaxY = m_worldExtent.yMax - mapExtent.yMin;

    double xMin = offsetX * invertUnitTile;
    double yMin = offsetY * invertUnitTile;
    double xMax = offsetMaxX * invertUnitTile;
    double yMax = offsetMaxY * invertUnitTile;

    // 转换为瓦片索引
    range.xMin = static_cast<int>(std::floor(xMin));
    range.yMin = static_cast<int>(std::floor(yMin));
    range.xMax = static_cast<int>(std::floor(xMax));
    range.yMax = static_cast<int>(std::floor(yMax));
}

void TmsImageSource::asyncRequest(const TileId& id, const TileLoadCallback& tileLoadCallback)
{
}

bool TmsImageSource::findBlack(const TileId& id)
{
    auto it = m_blacks.find(id);
    return it != m_blacks.end();
}

void TmsImageSource::storeBlack(const TileId& id)
{
    m_blacks.insert(id);
}

bool TmsImageSource::findCache(const TileId& id, QImage& image)
{
    auto it = m_caches.find(id);
    if(it != m_caches.end())
    {
        image = it->second;
        return true;
    }
    return false;
}

void TmsImageSource::storeCache(const TileId& id, const QImage& image)
{
    // 如果缓存已满，删除最早的条目
    if (m_caches.size() >= 1000)
    {
        m_caches.erase(m_caches.begin());
    }

    m_caches[id] = image;
}

}