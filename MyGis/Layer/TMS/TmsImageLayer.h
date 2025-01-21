#pragma once

#include "MyGis/Core/RWLock.h"

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
    void tileLoadeComplate(const TileId& id, const QImage& image);
    void tileBatchLoadComplete();

private:
    RWLock m_rwLock;
    QImage m_image;

    QPointF m_lastViewCenter;
    Rectangle m_lastMapExtent;
    double m_lastResolution;

};

}

