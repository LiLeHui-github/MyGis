#pragma once

#include "MyGis/Layer/ImageLayer.h"

class TmsLayer : public ImageLayer
{
public:
    explicit TmsLayer(const QString& url);
    ~TmsLayer() override;

protected:
    LayerId layerId() const override;
    void startRender(const MapSettings& settings) override;
    void cancelRender() const override;
    QImage getImage() override;
};

