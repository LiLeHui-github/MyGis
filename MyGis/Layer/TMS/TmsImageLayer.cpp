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
    double width = settings.m_viewExtent.width() * settings.m_resolution;
    double height = settings.m_viewExtent.height() * settings.m_resolution;

    const QPointF& viewport = settings.m_mapViewPoint;
    double helfWidth = width * 0.5;
    double helfHeight = height * 0.5;

    QRectF mapExtent = QRectF{
        QPointF{viewport.x() - helfWidth, viewport.y() + helfHeight},
        QPointF{viewport.x() + helfWidth, viewport.y() - helfHeight }
    };

    spdlog::info("{}", mapExtent.top());

    if(m_lastMapExtent.isEmpty())
    {
        m_lastMapExtent = mapExtent;
    }

    bool needNewImage = false;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_image.size() != settings.m_viewExtent.size())
        {
            m_image = QImage(settings.m_viewExtent.width(), settings.m_viewExtent.height(), QImage::Format_ARGB32_Premultiplied);
            needNewImage = true;
        }
    }

    if(m_lastResolution != settings.m_resolution)
    {
        needNewImage = true;
    }

    //// 移动视图
    //if(!needNewImage)
    //{
    //    const QPointF& offset = m_lastMapExtent.center() - settings.m_mapViewPoint;

    //    double invertResolution = 1.0 / settings.m_resolution;
    //    double x = offset.x() * invertResolution;
    //    double y = offset.y() * invertResolution;

    //    {
    //        std::lock_guard<std::mutex> lock(m_mutex);
    //        QPainter painter(&m_image);
    //        painter.drawImage(x, y, m_image);
    //    }
    //}

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
        //painter.drawRect(ti.id.px, ti.id.py, 256, 256);
        painter.drawImage(ti.id.pixel, ti.image);
    }
   
    notifyImageUpdate();
}

void TmsImageLayer::tileBatchComplete()
{
    spdlog::info("批量加载完成\n");
    //notifyImageUpdate();
}
