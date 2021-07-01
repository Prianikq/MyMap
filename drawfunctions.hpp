#pragma once
#include "rsxffile.h"
#include <QImage>
#include <QPainter>

namespace nDraw {
    QPoint coords_orig_g; // Условные координаты левого верхнего угла окна
    template<class T>
    QPointF FromMapPointToWindowPointF(const T& coords_input) {
        QPointF result(1.0*(coords_input.y() - coords_orig_g.y()), 1.0*(coords_orig_g.x() - coords_input.x()));
        return result;
    }
    QPoint FromMapPointToWindowPoint(const QPoint& coords_input) { // Добавлена для создания QRegion рамки
        QPoint result(coords_input.y() - coords_orig_g.y(), coords_orig_g.x() - coords_input.x());
        return result;
    }
    QVector<QPointF> GetMetricPoints(const nSXFFile::rRecord& record) {
        QVector<QPointF> result(record.m_title.m_count_metric_points);
        for (int32_t i = 0; i < record.m_title.m_count_metric_points; ++i) {
            if (!record.m_title.Has3DPresentation()) {
                if (!record.m_title.HasDoubleMetricType() && !record.m_title.HasBigMetricSize()) {
                    QPair<short, short>* get_point = static_cast<QPair<short, short>*>(record.m_points);
                    result[i] = FromMapPointToWindowPointF(QPoint(get_point[i].first, get_point[i].second));
                }
                else if (record.m_title.HasDoubleMetricType() && !record.m_title.HasBigMetricSize()) {
                    QPair<float, float>* get_point = static_cast<QPair<float, float>*>(record.m_points);
                    result[i] = FromMapPointToWindowPointF(QPointF(get_point[i].first, get_point[i].second));
                }
                else if (!record.m_title.HasDoubleMetricType() && record.m_title.HasBigMetricSize()) {
                    QPair<int32_t, int32_t>* get_point = static_cast<QPair<int32_t, int32_t>*>(record.m_points);
                    result[i] = FromMapPointToWindowPointF(QPoint(get_point[i].first, get_point[i].second));
                }
                else {
                    QPair<double, double>* get_point = static_cast<QPair<double, double>*>(record.m_points);
                    result[i] = FromMapPointToWindowPointF(QPointF(get_point[i].first, get_point[i].second));
                }
            }
            else {
                // Обработка трёхмерных координат (в рабочей карте нет)
            }
        }
        return result;
    }

    void DrawHydrography(QPainter& painter, const nSXFFile::rSXFFile &file) {
        static const int32_t CODES_COUNT = 9;
        static const int32_t HYDROGRAPHY_CODE[CODES_COUNT] = {31110000, 31120000, 31131000, 31132000, 31133000, 31134000,
                                               31135000, 31410000, 31431000}; // Классификационные коды объектов гидрографии из классификатора
        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            bool find = false;
            for (int32_t j = 0; j < CODES_COUNT; ++j) {
                if (file.m_records[i].m_title.m_classification_code_l == HYDROGRAPHY_CODE[j]) {
                    find = true;
                    break;
                }
            }
            if (!find) {
                continue;
            }
            QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
            painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
            painter.setBrush(QBrush(Qt::cyan, Qt::SolidPattern));
            short localiz_type = file.m_records[i].m_title.LocalizationType();
            if (localiz_type == nSXFFile::eLocalType::LINEAR) {
                painter.drawPolyline(points.data(), points.size());
            }
            else if (localiz_type == nSXFFile::eLocalType::AREAL) {
                painter.drawPolygon(points.data(), points.size());
            }
            else if (localiz_type == nSXFFile::eLocalType::POINT) {
                painter.drawPoint(points[0]);
            }
            else {
                 // Других типов характеров локализации среди объектов гидрографии нет
            }
        }
    }

    void Draw(const nSXFFile::rSXFFile& file) {
        QPoint left_bottom(file.m_passport.m_frame_coordinates.m_southwest.m_x, file.m_passport.m_frame_coordinates.m_southwest.m_y);
        QPoint right_bottom(file.m_passport.m_frame_coordinates.m_southeast.m_x, file.m_passport.m_frame_coordinates.m_southeast.m_y);
        QPoint left_top(file.m_passport.m_frame_coordinates.m_northwest.m_x, file.m_passport.m_frame_coordinates.m_northwest.m_y);
        QPoint right_top(file.m_passport.m_frame_coordinates.m_northeast.m_x, file.m_passport.m_frame_coordinates.m_northeast.m_y);
        static const int32_t OFFSET = 100; // Задает смещение от координат углов карты для задания области отрисовки

        coords_orig_g.setX(qMax(right_top.x(), left_top.x()) + OFFSET);
        coords_orig_g.setY(qMin(left_bottom.y(), left_top.y()) - OFFSET);
        int32_t image_size_x = qMax(right_bottom.y(), right_top.y()) - qMin(left_bottom.y(), left_top.y()) + 2 * OFFSET;
        int32_t image_size_y = qMax(right_top.x(), left_top.x()) - qMin(right_bottom.x(), left_bottom.x()) + 2 * OFFSET;
        std::cout << image_size_x << " " << image_size_y << std::endl;
        QImage picture(image_size_x, image_size_y, QImage::Format_RGB32);
        picture.fill(Qt::gray);
        QPainter painter;
        painter.begin(&picture);
        painter.setRenderHint(QPainter::Antialiasing, true);

        /* Отрисовка рамки карты */
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));

        QPolygonF frame;
        frame << FromMapPointToWindowPointF(left_bottom)
                << FromMapPointToWindowPointF(left_top)
                << FromMapPointToWindowPointF(right_top)
                << FromMapPointToWindowPointF(right_bottom);
        painter.drawPolygon(frame);


        left_bottom = FromMapPointToWindowPoint(left_bottom);
        left_bottom.setX(left_bottom.x()+1);
        left_bottom.setY(left_bottom.y()-1);
        left_top = FromMapPointToWindowPoint(left_top);
        left_top.setX(left_top.x()+1);
        left_top.setY(left_top.y()+1);
        right_bottom = FromMapPointToWindowPoint(right_bottom);
        right_bottom.setX(right_bottom.x()-1);
        right_bottom.setY(right_bottom.y()-1);
        right_top = FromMapPointToWindowPoint(right_top);
        right_top.setX(right_top.x()-1);
        right_top.setY(right_top.y()+1);
        painter.setClipRegion(QRegion(QPolygon() << left_bottom <<
                                      left_top << right_top <<
                                      right_bottom)); // Задание границ отрисовки (всё, что попадет за рамку отрисовано не будет)

        /* Отрисовка объектов гидрографии */
        DrawHydrography(painter, file);

        painter.end();
        picture.save("result.png", "PNG");
    }
}
