#include "TmsImageSource.h"

#include <QtCore/QFileInfo>

#include "spdlog/spdlog.h"

#include "MyGis/Core/ThreadPool.h"
#include "MyGis/Proj/Projection.h"

TmsImageSource::TmsImageSource(const QString& url)
    : m_url(url)
{
}

TmsImageSource::~TmsImageSource()
{
}

void TmsImageSource::requestTilesByExtent(const MapSettings& settings, const QRectF& mapExtent, const QRectF& lastMapExtent, const TileCallback& OnTileLoaded, const BatchCompleteCallback& OnBatchComplete)
{
    spdlog::info("开始请求");
    // 计算瓦片坐标
 
}

void TmsImageSource::cancelRequest()
{
    spdlog::info("取消请求");
}

TileId TmsImageSource::createId(int z, int x, int y, const QPointF& pixel)
{
    return TileId{ z, x, y, pixel };
}

void TmsImageSource::syncRequest(const TileId& id, const TileCallback& OnTileLoaded)
{
 
}

bool TmsImageSource::findBlack(const TileId& id)
{
    ReadGuard lock(m_rwlock);
    auto it = m_blacks.find(id);
    return it != m_blacks.end();
}

void TmsImageSource::storeBlack(const TileId& id)
{
    WriteGuard lock(m_rwlock);
    m_blacks.insert(id);
}

bool TmsImageSource::findCache(const TileId& id, TileInfo& info)
{
    ReadGuard lock(m_rwlock);
    auto it = m_caches.find(id);
    if(it != m_caches.end())
    {
        info = it->second;
        return true;
    }
    return false;
}

void TmsImageSource::storeCache(const TileInfo& info)
{
    WriteGuard lock(m_rwlock);

    // 如果缓存已满，删除最早的条目
    if (m_caches.size() >= 1000)
    {
        m_caches.erase(m_caches.begin());
    }

    m_caches[info.id] = info;
}
