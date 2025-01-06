#pragma once

#include <QtGui/QImage>

#include "MyGis/Core/MapSettings.h"
#include "MyGis/Core/MyGisDefs.h"

class ImageSource;

class ImageLayer
{
public:
    explicit ImageLayer(ImageSource* source) : m_source(source) {}
    virtual ~ImageLayer() = default;
    virtual LayerId layerId() const = 0;
    virtual void startRender(const MapSettings& settings) = 0;
    virtual void cancelRender() {}
    virtual QImage getImage() = 0;

    void setImageSource(ImageSource* source) { m_source = source; }
    ImageSource* getImageSource() { return m_source; }
    const ImageSource* getImageSource() const { return m_source; }

private:
    ImageSource* m_source = nullptr;

};
