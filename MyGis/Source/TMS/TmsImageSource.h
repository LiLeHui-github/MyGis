#pragma once

#include "MyGis/Source/ImageSource.h"

class TmsImageSource : public ImageSource
{
public:
    explicit TmsImageSource(const QString& url);
    ~TmsImageSource() override;

    void requestTilesByExtent(const MapSettings& settings,
        TileCallback onTileLoaded,
        BatchCompleteCallback onComplete) override;

    void cancelRequest() override;

private:
    QString m_url;
};
