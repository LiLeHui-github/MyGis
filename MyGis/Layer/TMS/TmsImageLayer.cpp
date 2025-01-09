#include "TmsImageLayer.h"

#include <sstream>

#include <QtGui/QPainter>

#include <spdlog/spdlog.h>

#include "MyGis/Source/TMS/TmsImageSource.h"
#include "Proj/Projection.h"

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
    // 新地图范围
    const QRectF& mapExtent = settings.getMapExtent();

    if(m_lastMapExtent.isEmpty())
    {
        m_lastMapExtent = mapExtent;
    }

    m_image = QImage(settings.m_viewExtent.width(), settings.m_viewExtent.height(), QImage::Format_ARGB32_Premultiplied);

    // 请求瓦片
    getImageSource()->requestTilesByExtent(settings,
        mapExtent,
        m_lastMapExtent,
        [this](const TileInfo& ti)
        {
            tileResponse(ti);
        },
        [this]()
        {
            tileBatchComplete();
        });

    m_lastMapExtent = mapExtent;
    m_lastResolution = settings.m_resolution;

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
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        QPainter painter(&m_image);
        //painter.drawImage(ti.id.pixel, ti.image);
        QRectF rect = QRectF(ti.id.pixel, QSizeF(256, 256));
        painter.drawRect(rect);
        painter.drawText(rect, QString("%1/%2/%3").arg(ti.id.z).arg(ti.id.x).arg(ti.id.y), QTextOption(Qt::AlignCenter));
    }
   
    notifyImageUpdate();
}

void TmsImageLayer::tileBatchComplete()
{
    spdlog::info("批量加载完成\n");
}
