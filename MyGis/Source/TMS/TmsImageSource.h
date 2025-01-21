#pragma once

#include <mutex>
#include <atomic>

#include <unordered_set>
#include <unordered_map>

#include "MyGis/Core/ReadWriteLocker.h"
#include "MyGis/Core/TileDefsHashFunctions.h"
#include "MyGis/Source/ImageSource.h"

namespace lh
{

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

    void requestTiles(const MapSettings& settings,
        const Rectangle& lastMapExtent, 
        const TileLoadCallback& tileLoadCallback,
        const TileBatchLoadCompleteCallback& tileBatchLoadComplete) override;

    void cancelRequest() override;

private:
    void calcTileRange(double invertUnitTile, const Rectangle& mapExtent, TileRange& range) const;

    void asyncRequest(const TileId& id, const TileLoadCallback& tileLoadCallback);

    bool findBlack(const TileId& id);

    void storeBlack(const TileId& id);

    bool findCache(const TileId& id, QImage& image);

    void storeCache(const TileId& id, const QImage& image);

private:
    QString m_url;

    ReadWriteLocker m_rwlock;

    Rectangle m_worldExtent;

    // 黑名单
    std::unordered_set<TileId> m_blacks;

    // 缓存
    std::unordered_map<TileId, QImage> m_caches;

};

}