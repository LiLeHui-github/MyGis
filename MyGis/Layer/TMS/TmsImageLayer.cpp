#include "TmsImageLayer.h"

#include <sstream>

#include <QtGui/QPainter>

#include <spdlog/spdlog.h>

#include "MyGis/Source/TMS/TmsImageSource.h"
#include "MyGis/Proj/Projection.h"

namespace lh
{

TmsImageLayer::TmsImageLayer(const QString& url)
    : ImageLayer(new TmsImageSource(url))
    , m_lastResolution(0)
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

}

void TmsImageLayer::cancelRender()
{
    getImageSource()->cancelRequest();
}

QImage TmsImageLayer::getImage()
{
    ReadGuard lock(m_rwLock);
    return m_image;
}

void TmsImageLayer::tileResponse(const TileId& id, const QImage& image)
{
  
}

void TmsImageLayer::tileBatchComplete()
{
}

}