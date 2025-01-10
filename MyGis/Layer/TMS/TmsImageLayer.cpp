#include "TmsImageLayer.h"

#include <sstream>

#include <QtGui/QPainter>

#include <spdlog/spdlog.h>

#include "MyGis/Source/TMS/TmsImageSource.h"
#include "MyGis/Proj/Projection.h"

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
    const RectangleExtent& mapExtent = settings.getMapExtent();

    bool newImage = false;
    {
        WriteGuard lock(m_rwLock);
        if(m_image.size() != settings.m_viewExtent.size() || m_lastResolution != settings.m_resolution)
        {
            m_image = QImage(settings.m_viewExtent.width(), settings.m_viewExtent.height(), QImage::Format_ARGB32_Premultiplied);
            newImage = true;
        }
    }

    if(!newImage)
    {
        const QPointF& offset = m_lastViewCenter - settings.m_viewExtent.center();

        {
            WriteGuard lock(m_rwLock);
            QPainter painter(&m_image);
            painter.drawImage(offset, m_image);
        }
    }
  
    // 请求瓦片
    getImageSource()->requestTilesByExtent(settings,
        mapExtent,
        m_lastMapExtent,
        [this](const TileId& id, const QImage& image)
        {
            tileResponse(id, image);
        },
        [this]()
        {
            tileBatchComplete();
        });

    m_lastViewCenter = settings.m_viewExtent.center();
    m_lastMapExtent = mapExtent;
    m_lastResolution = settings.m_resolution;

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
    {
        WriteGuard lock(m_rwLock);
        QPainter painter(&m_image);
        painter.drawImage(id.pixel, image);
        //QRectF rect = QRectF(id.pixel, QSizeF(256, 256));
        //painter.drawText(rect, QString("%1/%2/%3").arg(id.z).arg(id.x).arg(id.y), QTextOption(Qt::AlignCenter));
        //painter.drawRect(rect);
    }
   
    notifyImageUpdate();
}

void TmsImageLayer::tileBatchComplete()
{
    //spdlog::info("批量加载完成\n");
}
