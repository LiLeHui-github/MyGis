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

    struct TileRange
    {
        int xMin;
        int xMax;
        int yMin;
        int yMax;
    };

    explicit TmsImageSource(const QString& url);
    ~TmsImageSource() override;

    void requestTilesByExtent(const MapSettings& settings, const RectangleExtent& mapExtent, const RectangleExtent& lastMapExtent, const TileCallback& OnTileLoaded, const BatchCompleteCallback& OnBatchComplete) override;

    void cancelRequest() override;

private:
    void calcTileRange(double invertUnitTile, const RectangleExtent& mapExtent, TileRange& range) const;

    void syncRequest(const TileId& id, const TileCallback& OnTileLoaded);

    bool findBlack(const TileId& id);

    void storeBlack(const TileId& id);

    bool findCache(const TileId& id, QImage& image);

    void storeCache(const TileId& id, const QImage& image);

    struct BatchContext
    {
        // 请求数量
        std::atomic<int> totalRequests { 0 };
        // 请求状态
        std::atomic<bool> cancelled { false };
        // 请求完成回调
        BatchCompleteCallback onBatchComplete;

        void completeRequest()
        {
            if (--totalRequests == 0)
            {
                if (onBatchComplete)
                {
                    onBatchComplete();
                }
            }
        }

        void cancelAllRequest()
        {
            totalRequests = 0;
            if (onBatchComplete)
            {
                onBatchComplete();
            }
        }

    };

private:
    QString m_url;

    RWLock m_rwlock;

    RectangleExtent m_worldExtent;

    // 黑名单
    std::unordered_set<TileId> m_blacks;

    // 缓存
    std::unordered_map<TileId, QImage> m_caches;

    // 当前批次的上下文
    std::mutex m_batchMutex;
    std::shared_ptr<BatchContext> m_currentBatch;

};
