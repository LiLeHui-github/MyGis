#pragma once

#include <mutex>

#include "MyGis/Core/MyGisDefs.h"
#include "MyGis/Layer/ImageLayer.h"

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
    void tileResponse(const TileInfo& ti);
    void tileBatchComplete();

private:
    std::mutex m_mutex;
    QImage m_image;

};

