#include "TmsImageLayer.h"

#include <sstream>

#include <spdlog/spdlog.h>

#include "MyGis/Source/TMS/TmsImageSource.h"

TmsImageLayer::TmsImageLayer(const QString& url)
    : ImageLayer(new TmsImageSource(url))
{
}

TmsImageLayer::~TmsImageLayer()
{
}

LayerId TmsImageLayer::layerId() const
{
    return "tms";
}

void TmsImageLayer::startRender(const MapSettings& settings)
{
    //std::stringstream ss;
    //ss << std::this_thread::get_id();
    //spdlog::info("[{}] 级别:{} 分辨率:{} 视点:({},{})", ss.str(), settings.m_zoom, settings.m_resolution, settings.m_mapViewPoint.x(), settings.m_mapViewPoint.y());

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_image = QImage(settings.m_viewExtent.width(), settings.m_viewExtent.height(), QImage::Format_ARGB32_Premultiplied);
    }

    // 请求瓦片
    getImageSource()->requestTilesByExtent(settings, 
    [this](const TileInfo& ti)
    {
        tileResponse(ti);
    },
    [this]()
    {
        tileBatchComplete();
    });

}

void TmsImageLayer::cancelRender()
{
    getImageSource()->cancelRequest();
}

QImage TmsImageLayer::getImage()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_image;
}

void TmsImageLayer::tileResponse(const TileInfo& ti)
{
}

void TmsImageLayer::tileBatchComplete()
{
}
