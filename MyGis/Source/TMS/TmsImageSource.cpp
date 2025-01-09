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
    // 计算瓦片坐标
    const QRectF& wordExtent = settings.m_proj->getExtent();
    const QPointF& minLeftTop = wordExtent.topLeft();

    double _xmin = (mapExtent.left() - minLeftTop.x()) / settings.m_resolution / 256.0;
    double _ymin = (minLeftTop.y() - mapExtent.top()) / settings.m_resolution / 256.0;
    double _xmax = (mapExtent.right() - minLeftTop.x()) / settings.m_resolution / 256.0;
    double _ymax = (minLeftTop.y() - mapExtent.bottom()) / settings.m_resolution / 256.0;

    int xmin = std::max<int>(std::floor(_xmin), 0);
    int ymin = std::max<int>(std::floor(_ymin), 0);

    int cnt = std::pow(2, settings.m_zoom);
    int xmax = std::min<int>(std::ceil(_xmax), cnt);
    int ymax = std::min<int>(std::ceil(_ymax), cnt);

    std::unordered_set<TileId> syncRequesIds;
    syncRequesIds.reserve((ymax - ymin + 1) * (xmax - xmin + 1));

    for(int y = ymin; y < ymax; ++y)
    {
        for(int x = xmin; x < xmax; ++x)
        {
            // 计算瓦片的世界坐标
            QPointF worldPos(x * 256 * settings.m_resolution, y * 256 * settings.m_resolution);
            // 转换为像素坐标
            const QPointF& pixelPos = settings.m_proj->toPixel(worldPos) - QPointF(128, 128);
            syncRequesIds.insert(createId(settings.m_zoom, x, y, pixelPos));
        }
    }

    spdlog::info("开始请求: {}条", syncRequesIds.size());

    auto batch = std::make_shared<BatchContext>();
    batch->totalRequests = static_cast<int>(syncRequesIds.size());
    batch->cancelled = false;
    batch->onBatchComplete = OnBatchComplete;

    {
        std::lock_guard<std::mutex> lock(m_batchMutex);
        m_currentBatch = batch;
    }

    // 异步请求
    for(const TileId& id : syncRequesIds)
    {
        ThreadPool::globalInstance()->submit([this, id, OnTileLoaded]()
        {
            syncRequest(id, OnTileLoaded);
        });
    }
 
}

void TmsImageSource::cancelRequest()
{
    spdlog::info("取消请求");

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

TileId TmsImageSource::createId(int z, int x, int y, const QPointF& pixel)
{
    return TileId{ z, x, y, pixel };
}

void TmsImageSource::syncRequest(const TileId& id, const TileCallback& OnTileLoaded)
{
    std::shared_ptr<BatchContext> batch;
    {
        std::lock_guard<std::mutex> lock(m_batchMutex);
        batch = m_currentBatch;
    }

    if(batch && batch->cancelled)
    {
        batch->decreaseRequest();
        return;
    }

    if (findBlack(id))
    {
        if(batch)
        {
            batch->decreaseRequest();
        }
        return;
    }

    TileInfo info;
 
    if (!findCache(id, info))
    {
        QString fileName = m_url.arg(id.z).arg(id.x).arg(id.y);
        if (!QFileInfo::exists(fileName))
        {
            storeBlack(id);
            if (batch)
            {
                batch->decreaseRequest();
            }
            return;
        }

        info.image = QImage(fileName);

        if (info.image.isNull())
        {
            storeBlack(id);
            if (batch)
            {
                batch->decreaseRequest();
            }
            return;
        }

        spdlog::info("成功加载{}", fileName.toStdString());

        info.id = id;
        storeCache(info);
    }

    //spdlog::info("瓦片坐标{},{}, 像素坐标{},{}", id.x, id.y, id.pixel.x(), id.pixel.y());

    info.id = id;
    OnTileLoaded(info);

    if (batch)
    {
        batch->decreaseRequest();
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
