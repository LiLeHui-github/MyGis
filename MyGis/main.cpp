#include <QtWidgets/QApplication>

#include "MyGis/Core/Map.h"
#include "MyGis/View/MyGisView.h"
#include "MyGis/Layer/TMS/TmsLayer.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MyGisView w;
    w.show();
    w.getMap()->addLayer(new TmsLayer("E:/BaiduNetdiskDownload/GoogleMapTiles/%1/%2/%3.png"));

    return a.exec();
}