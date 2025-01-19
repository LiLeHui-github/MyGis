#include "MercatorProjection.h"

MercatorProjection::MercatorProjection()
{
    //x -20037508.3427892,20037508.3427892
    //y -20037508.3427892,20037508.3427892
    m_extent = RectangleExtent(-20037508.342789244, 20037508.342789244, -20037508.342789244, 20037508.342789244);
}

MercatorProjection::~MercatorProjection()
{
}

RectangleExtent MercatorProjection::getExtent() const
{
    return m_extent;
}

void MercatorProjection::updateMatrix(const MapSettings& settings)
{
    // 获取视口中心点（像素坐标）
    const QPointF& viewCenter = settings.m_viewExtent.center();
    // 获取视图中心点（投影坐标）
    const QPointF& center = settings.m_mapViewPoint;
    // 分辨率（比例尺）
    double resolution = settings.m_resolution;

    // 构建变换矩阵
    // 第一步计算从中心点到像素坐标的像素偏移
    // 第二步计算偏移的地图单位
    // 第三步从视点偏移

    m_pixel_to_projection_matrix = QTransform()
        .translate(center.x(), center.y())
        .scale(resolution, -resolution)
        .translate(-viewCenter.x(), -viewCenter.y());

    // 第一步计算从视点到投影坐标的地图单位偏移
    // 第二步计算偏移的像素
    // 第三步从视口中心偏移

    double invertResolution = 1.0 / resolution;
    m_projection_to_pixel_matrix = QTransform()
        .translate(viewCenter.x(), viewCenter.y())
        .scale(invertResolution, -invertResolution)
        .translate(-center.x(), -center.y());
}

QPointF MercatorProjection::toProjection(double px, double py) const
{
    double rx;
    double ry;

    // 将像素坐标转换到墨卡托投影坐标
    m_pixel_to_projection_matrix.map(px, py, &rx, &ry);

    return QPointF{ rx, ry };
}

QPointF MercatorProjection::toPixel(double x, double y) const
{
    double rx;
    double ry;

    // 将像素坐标转换到墨卡托投影坐标
    m_projection_to_pixel_matrix.map(x, y, &rx, &ry);

    return QPointF{ rx, ry };
}
