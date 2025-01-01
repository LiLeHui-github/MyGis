#pragma once

#include <QtCore/QString>
#include <QtGui/QImage>

#include "MyGis/Core/MyGisDefs.h"
#include "MyGis/Core/MapSettings.h"

class ImageLayer
{
public:
    virtual ~ImageLayer() = default;
    virtual LayerId layerId() const = 0;
    virtual void startRender(const MapSettings& settings) = 0;
    virtual void cancelRender() const {}
    virtual QImage getImage() = 0;
};
