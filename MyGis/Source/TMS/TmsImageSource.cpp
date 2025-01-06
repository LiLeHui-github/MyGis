#include "TmsImageSource.h"

TmsImageSource::TmsImageSource(const QString& url)
    : m_url(url)
{
}

TmsImageSource::~TmsImageSource()
{
}

void TmsImageSource::requestTilesByExtent(const MapSettings& settings,
    TileCallback onTileLoaded,
    BatchCompleteCallback onComplete)
{
}

void TmsImageSource::cancelRequest()
{
}
