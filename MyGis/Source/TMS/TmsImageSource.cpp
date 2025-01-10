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

    Projection* proj = settings.m_proj;

    const RectangleExtent& worldExtent = proj->getExtent();

    int zoom = settings.m_zoom;
    double unitTile = settings.m_resolution * 256.0;
    double invertUnitTile = 1.0 / unitTile;

    int last_xmin = std::floor((lastMapExtent.xMin - worldExtent.xMin) * invertUnitTile);
    int last_ymin = std::floor((worldExtent.yMax - lastMapExtent.yMax) * invertUnitTile);
    int last_xmax = std::ceil((lastMapExtent.xMax - worldExtent.xMin) * invertUnitTile);
    int last_ymax = std::ceil((worldExtent.yMax - lastMapExtent.yMin) * invertUnitTile);

    int xmin = std::floor((mapExtent.xMin - worldExtent.xMin) * invertUnitTile);
    int ymin = std::floor((worldExtent.yMax - mapExtent.yMax) * invertUnitTile);
    int xmax = std::ceil((mapExtent.xMax - worldExtent.xMin) * invertUnitTile);
    int ymax = std::ceil((worldExtent.yMax - mapExtent.yMin) * invertUnitTile);

    std::vector<TileId> requestTileIds;

    for(int y = ymin; y < ymax; ++y)
    {
        for (int x = xmin; x < xmax; ++x)
        {

        }
    }
 

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
