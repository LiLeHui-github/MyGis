#pragma once

#include <mutex>
#include <atomic>

#include <unordered_set>
#include <unordered_map>

#include "MyGis/Core/RWLock.h"
#include "MyGis/Core/TileDefsHashFunctions.h"
#include "MyGis/Source/ImageSource.h"

class TmsImageSource : public ImageSource
{
public:
    explicit TmsImageSource(const QString& url);
    ~TmsImageSource() override;

    void requestTilesByExtent(const MapSettings& settings, const QRectF& mapExtent, const QRectF& lastMapExtent, const TileCallback& OnTileLoaded, const BatchCompleteCallback& OnBatchComplete) override;

    void cancelRequest() override;

private:
    static TileId createId(int z, int x, int y, const QPointF& pixel);

    void syncRequest(const TileId& id, const TileCallback& OnTileLoaded);

    bool findBlack(const TileId& id);

    void storeBlack(const TileId& id);

    bool findCache(const TileId& id, TileInfo& info);

    void storeCache(const TileInfo& info);

    struct BatchContext
    {
        // 请求数量
        std::atomic<int> totalRequests { 0 };
        // 请求状态
        std::atomic<bool> cancelled { false };
        // 请求完成回调
        BatchCompleteCallback onBatchComplete;

        void decreaseRequest()
        {
            if (--totalRequests == 0)
            {
                callBatchComplete();
            }
        }

        void callBatchComplete()
        {
            if(onBatchComplete)
            {
                onBatchComplete();
            }
        }
    };

private:
    QString m_url;

    RWLock m_rwlock;

    // 黑名单
    std::unordered_set<TileId> m_blacks;

    // 缓存
    std::unordered_map<TileId, TileInfo> m_caches;

    // 当前批次的上下文
    std::mutex m_batchMutex;
    std::shared_ptr<BatchContext> m_currentBatch;

};
