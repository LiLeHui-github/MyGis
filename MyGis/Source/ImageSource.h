#pragma once

#include <QtGui/QImage>

#include "MyGis/Core/MapSettings.h"
#include "MyGis/Core/MyGisDefs.h"

class ImageSource
{
public:
    virtual ~ImageSource() = default;

    // 请求
    virtual void requestTilesByExtent(const MapSettings& settings, const RectangleExtent& mapExtent, const RectangleExtent& lastMapExtent, const TileCallback& OnTileLoaded, const BatchCompleteCallback& OnBatchComplete = nullptr) = 0;

    // 取消请求
    virtual void cancelRequest() = 0;

};
