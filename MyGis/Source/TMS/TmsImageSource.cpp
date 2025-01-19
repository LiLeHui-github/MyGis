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

void TmsImageSource::requestTilesByExtent(const MapSettings& settings, const RectangleExtent& mapExtent, const RectangleExtent& lastMapExtent, const TileCallback& OnTileLoaded, const BatchCompleteCallback& OnBatchComplete)
{
    //spdlog::info("开始请求");

    // 计算瓦片坐标
    if(m_worldExtent.isNull())
    {
        Projection* proj = settings.m_proj;
        m_worldExtent = proj->getExtent();
    }

    int zoom = settings.m_zoom;

    // 考虑缩放
    double baseResolution = m_worldExtent.width() / (std::pow(2, zoom) * 256.0);
    double scale = baseResolution / settings.m_resolution;

    double unitTile = settings.m_resolution * 256.0 * scale;
    double invertUnitTile = 1.0 / unitTile;

    TileRange curRange;
    calcTileRange(invertUnitTile, mapExtent, curRange);

    std::vector<TileId> requestTileIds;
    for(int y = curRange.yMin; y < curRange.yMax; ++y)
    {
        for (int x = curRange.xMin; x < curRange.xMax; ++x)
        {
            // 创建新的瓦片ID并添加到请求列表
            QPointF pixel = settings.m_proj->toPixel(QPointF{m_worldExtent.xMin + x * unitTile, m_worldExtent.yMax - y * unitTile }) - (QPointF{128, 128} * scale);
            requestTileIds.emplace_back(zoom, x, y, scale, pixel);
        }
    }

    spdlog::info("{}, {}, {}", zoom, scale, requestTileIds.size());

    std::shared_ptr<BatchContext> batch = std::make_shared<BatchContext>();
    batch->totalRequests = static_cast<int>(requestTileIds.size());
    batch->cancelled = false;
    batch->onBatchComplete = OnBatchComplete;

    {
        std::lock_guard<std::mutex> lock(m_batchMutex);
        m_currentBatch = batch;
    }

    for(const TileId& id : requestTileIds)
    {
        if(batch->cancelled)
        {
            batch->cancelAllRequest();
            return;
        }

        if(findBlack(id))
        {
            batch->completeRequest();
            continue;
        }

        QImage img;
        if(findCache(id, img))
        {
            OnTileLoaded(id, img);
            batch->completeRequest();
            continue;
        }

        ThreadPool::globalInstance()->submit([this, id, OnTileLoaded]()
        {
            syncRequest(id, OnTileLoaded);
        });
    }

    //spdlog::info("请求数量{}", requestTileIds.size());
}

void TmsImageSource::cancelRequest()
{
    //spdlog::info("取消请求");

    std::shared_ptr<BatchContext> batch;
    {
        std::lock_guard<std::mutex> lock(m_batchMutex);
        batch = m_currentBatch;
    }

    if(batch)
    {
        batch->cancelled = true;
    }
}

void TmsImageSource::calcTileRange(double invertUnitTile,
    const RectangleExtent& mapExtent,
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
    range.xMax = static_cast<int>(std::ceil(xMax));
    range.yMax = static_cast<int>(std::ceil(yMax));
}

void TmsImageSource::syncRequest(const TileId& id, const TileCallback& OnTileLoaded)
{
    std::shared_ptr<BatchContext> batch;
    {
        std::lock_guard<std::mutex> lock(m_batchMutex);
        batch = m_currentBatch;
    }

    if (batch && batch->cancelled)
    {
        batch->completeRequest();
        return;
    }

    QString url = m_url.arg(id.z).arg(id.x).arg(id.y);
    QImage image(url);

    if(image.isNull())
    {
        storeBlack(id);
    }
    else
    {
        storeCache(id, image);
        OnTileLoaded(id, image);
    }

    if (batch)
    {
        batch->completeRequest();
    }
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

bool TmsImageSource::findCache(const TileId& id, QImage& image)
{
    ReadGuard lock(m_rwlock);
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
    WriteGuard lock(m_rwlock);

    // 如果缓存已满，删除最早的条目
    if (m_caches.size() >= 1000)
    {
        m_caches.erase(m_caches.begin());
    }

    m_caches[id] = image;
}
