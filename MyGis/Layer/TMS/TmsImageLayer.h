#pragma once

#include "MyGis/Core/ReadWriteLocker.h"

#include "MyGis/Core/MyGisDefs.h"
#include "MyGis/Core/Rectangle.h"
#include "MyGis/Layer/ImageLayer.h"

namespace lh
{

class TmsImageLayer : public ImageLayer
{
public:
    explicit TmsImageLayer(const QString& url);
    ~TmsImageLayer() override;

protected:
    LayerId layerId() const override;
    void startRender(const MapSettings& settings) override;
    void cancelRender() override;
    QImage getImage() override;

private:
    void tileLoadeComplete(const TileId& id, const QImage& image);
    void tileBatchLoadComplete();

private:
    ReadWriteLocker m_canvasLock;
    QImage m_canvas;

    ReadWriteLocker m_tempCanvasLock;
    QImage m_tempCanvas;

    QPointF m_lastViewCenter;
    Rectangle m_lastMapExtent;
    double m_lastResolution;

};

}

