#pragma once

#include <QtGui/QImage>

#include "MyGis/Core/MapSettings.h"
#include "MyGis/Core/MyGisDefs.h"

class ImageSource
{
public:
    virtual ~ImageSource() = default;

    // 请求
    virtual void requestTilesByExtent(
        const MapSettings& settings,
        TileCallback onTileLoaded,                    // 单片瓦片加载完成回调
        BatchCompleteCallback onComplete = nullptr    // 所有瓦片加载完成回调
    ) = 0;

    // 取消请求
    virtual void cancelRequest() = 0;

};
