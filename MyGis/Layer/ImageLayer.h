#pragma once

#include <QtCore/QObject>
#include <QtGui/QImage>

#include "MyGis/Core/MapSettings.h"
#include "MyGis/Core/MyGisDefs.h"

namespace lh
{

class ImageSource;

class ImageLayer : public QObject
{
    Q_OBJECT
public:
    explicit ImageLayer(ImageSource* source) : m_source(source) {}
    ~ImageLayer() override = default;

    virtual LayerId layerId() const = 0;

    virtual void startRender(const MapSettings& settings) = 0;
    virtual void cancelRender() {}

    virtual QImage getImage() = 0;

    void setImageSource(ImageSource* source) { m_source = source; }
    ImageSource* getImageSource() { return m_source; }
    const ImageSource* getImageSource() const { return m_source; }

Q_SIGNALS:
    void layerChanged();
    void imageUpdate();

protected:
    inline void notifyLayerChanged() { Q_EMIT layerChanged(); }
    inline void notifyImageUpdate() { Q_EMIT imageUpdate(); }

private:
    ImageSource* m_source = nullptr;

};

}