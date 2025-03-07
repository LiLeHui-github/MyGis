#include <QtWidgets/QApplication>

#include "MyGis/Core/Map.h"
#include "MyGis/View/MyGisView.h"
#include "MyGis/Layer/TMS/TmsImageLayer.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    lh::MyGisView w;
    w.show();
    w.getMap()->addLayer(new lh::TmsImageLayer("E:/BaiduNetdiskDownload/BingMapTiles/%1/%2/%3.png"));


    return a.exec();
}