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
    // 保存旧图像
    QSize oldSize;
    QImage oldImage;
    {
        ReadGuard lock(m_canvasLock);
        oldSize = m_canvas.size();
        oldImage = m_canvas;
    }

    // 尺寸如果发生改变就重新创建图片
    {
        QSize currentSize = settings.m_viewExtent.size().toSize();
        if(oldSize != currentSize)
        {
            WriteGuard lock(m_canvasLock);
            m_canvas = QImage(currentSize, QImage::Format::Format_ARGB32);
            m_canvas.fill(Qt::transparent);

            WriteGuard lock2(m_tempCanvasLock);
            m_tempCanvas = m_canvas;
        }
    }

    // 分辨率相同就偏移就图像
    if(!oldImage.isNull() && std::abs(settings.m_resolution - m_lastResolution) < 1e-10)
    {
        auto proj = settings.m_proj;
        QPointF offset = proj->toPixel(settings.m_mapViewPoint) - proj->toPixel(m_lastViewCenter);

        {
            WriteGuard lock(m_canvasLock);
            QPainter painter(&m_canvas);
            painter.drawImage(offset, oldImage);
        }
    }

    auto tileLoadCompleteCallback = [this](const TileId& id, const QImage& image)
    {
        tileLoadeComplete(id, image);
    };

    auto tileBatchLoadCompleteCallback = [this]()
    {
        tileBatchLoadComplete();
    };

    // 请求瓦片
    getImageSource()->requestTiles(settings, m_lastMapExtent, tileLoadCompleteCallback, tileBatchLoadCompleteCallback);

    m_lastViewCenter = settings.m_mapViewPoint;
    m_lastMapExtent = settings.getMapExtent();
    m_lastResolution = settings.m_resolution;
}

void TmsImageLayer::cancelRender()
{
    getImageSource()->cancelRequest();
}

QImage TmsImageLayer::getImage()
{
    ReadGuard lock(m_canvasLock);
    return m_canvas;
}

void TmsImageLayer::tileLoadeComplete(const TileId& id, const QImage& image)
{

}

void TmsImageLayer::tileBatchLoadComplete()
{
}

}