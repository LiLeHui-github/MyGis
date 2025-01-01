#include "TmsLayer.h"

#include <sstream>

#include <spdlog/spdlog.h>

TmsLayer::TmsLayer(const QString& url)
{
}

TmsLayer::~TmsLayer()
{
}

LayerId TmsLayer::layerId() const
{
    return "tms";
}

void TmsLayer::startRender(const MapSettings& settings)
{
    std::stringstream ss;
    ss << std::this_thread::get_id();

    spdlog::info("[{}] 级别:{} 分辨率:{} 视点:({},{})", ss.str(), settings.m_zoom, settings.m_resolution, settings.m_mapViewPoint.x(), settings.m_mapViewPoint.y());

}

void TmsLayer::cancelRender() const
{
}

QImage TmsLayer::getImage()
{
    return {};
}
