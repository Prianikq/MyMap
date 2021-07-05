#pragma once
#include "rsxffile.h"
#include <QImage>
#include <QPainter>

namespace nDraw {
    QPoint coords_orig_g; // Условные координаты левого верхнего угла окна
    double METERS_DESCRETE_X_D; // Определяет значение, сколько метров содержится в 1 дискрете (по X и Y координате отдельно)
    double METERS_DESCRETE_Y_D;
    double PIXEL_METERS_D; // Определяет значение, сколько метров в 1 пикселе
    template<class T>
    QPointF FromMapPointToWindowPointF(const T& coords_input) {
        double x = (1.0*coords_orig_g.x() - coords_input.x()) * METERS_DESCRETE_X_D / PIXEL_METERS_D;
        double y = (coords_input.y() - 1.0*coords_orig_g.y()) * METERS_DESCRETE_Y_D / PIXEL_METERS_D;
        QPointF result(y, x);
        return result;
    }
    QPoint FromMapPointToWindowPoint(const QPoint& coords_input) { // Добавлена для создания QRegion рамки
        double x = (1.0*coords_orig_g.x() - coords_input.x()) * METERS_DESCRETE_X_D / PIXEL_METERS_D;
        double y = (coords_input.y() - 1.0*coords_orig_g.y()) * METERS_DESCRETE_Y_D / PIXEL_METERS_D;
        QPoint result(qRound(y), qRound(x));
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
            painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::SolidLine));
            painter.setBrush(QBrush(QColor(0, 191, 255), Qt::SolidPattern));
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

    void DrawLandRelief(QPainter& painter, const nSXFFile::rSXFFile &file) {
        static const int32_t CODES_COUNT = 6;
        static const int32_t LAND_RELIEF_CODE_LINEAR[CODES_COUNT] = {21100000, 21200000, 21300000, 22212000, 22221000, 22630000};
        static const int32_t PIT_CODE = 22250000;
        static const int32_t BARROW_CODE = 22520000;
        static const int32_t HORIZONTALS_CODE = 21100000;
        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.m_classification_code_l == PIT_CODE) {
                // Отрисовка значка
            }
            else if (file.m_records[i].m_title.m_classification_code_l == BARROW_CODE) {
                // Отрисовка значка
            }
            else {
                bool find = false;
                for (int32_t j = 0; j < CODES_COUNT; ++j) {
                    if (file.m_records[i].m_title.m_classification_code_l == LAND_RELIEF_CODE_LINEAR[j]) {
                        find = true;
                        break;
                    }
                }
                if (!find) {
                    continue;
                }
                if (file.m_records[i].m_title.m_classification_code_l == HORIZONTALS_CODE) {
                    painter.setPen(QPen(Qt::darkYellow, 2, Qt::SolidLine));
                }
                else {
                    painter.setPen(QPen(Qt::darkYellow, 1, Qt::SolidLine));
                }
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                painter.drawPolyline(points.data(), points.size());
            }
        }
    }

    void DrawVegitationFilled(QPainter& painter, const nSXFFile::rSXFFile &file) {
        static const int32_t CODES_COUNT_GREEN = 2;
        static const int32_t VEGITATION_CODES_GREEN[CODES_COUNT_GREEN] = {71113000, 71211200};
        static const int32_t CODES_COUNT_DARK_GREEN = 2;
        static const int32_t VEGITATION_CODES_DARK_GREEN[CODES_COUNT_DARK_GREEN] = {71111110, 71123000};

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() != nSXFFile::eLocalType::AREAL) {
                continue; // Все объекты, представленные в этом слое - площадные
            }
            char find = 0;
            for (int32_t j = 0; j < CODES_COUNT_GREEN; ++j) {
                if (file.m_records[i].m_title.m_classification_code_l == VEGITATION_CODES_GREEN[j]) {
                    find = 1;
                    break;
                }
            }
            for (int32_t j = 0; j < CODES_COUNT_DARK_GREEN; ++j) {
                if (file.m_records[i].m_title.m_classification_code_l == VEGITATION_CODES_DARK_GREEN[j]) {
                    find = 2;
                    break;
                }
            }
            if (find == 0) {
                continue;
            }
            else if (find == 1) {
                painter.setPen(QPen(QColor(152, 251, 152), 1, Qt::SolidLine));
                painter.setBrush(QBrush(QColor(152, 251, 152), Qt::SolidPattern));
            }
            else {
                painter.setPen(QPen(QColor(154, 205, 50), 1, Qt::SolidLine));
                painter.setBrush(QBrush(QColor(154, 205, 50), Qt::SolidPattern));
            }
            QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
            painter.drawPolygon(points.data(), points.size());
        }
    }

    void DrawSettlements(QPainter& painter, const nSXFFile::rSXFFile& file) {
        static const int32_t CODES_COUNT = 4;
        static const int32_t SETTLEMENTS_CODES[CODES_COUNT] = {41100000, 41200000, 42100000, 43100000};

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() != nSXFFile::eLocalType::AREAL) {
                continue; // Все объекты, представленные в этом слое - площадные
            }
            bool find = false;
            for (int32_t j = 0; j < CODES_COUNT; ++j) {
                if (file.m_records[i].m_title.m_classification_code_l == SETTLEMENTS_CODES[j]) {
                    find = true;
                    break;
                }
            }
            if (!find) {
                continue;
            }
            painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine));
            QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
            painter.drawPolyline(points.data(), points.size());
        }
    }

    void DrawVegitation(QPainter& painter, const nSXFFile::rSXFFile& file) {
        static const int32_t CODES_LINE_COUNT = 2;
        static const int32_t VEGITATION_LINE_CODES[CODES_LINE_COUNT] = {71111110, 71131000}; // Массив с кодами линейных объектов (отрисовываются одинаково)
        /* Здесь также нужно отрисовать несколько точечных объектов-значков и 2 площадных с заполнением в виде значков - это пока не реализовано */
        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() != nSXFFile::eLocalType::LINEAR) {
                continue; // Все объекты, представленные в этом слое - линейные
            }
            bool find = false;
            for (int32_t j = 0; j < CODES_LINE_COUNT; ++j) {
                if (file.m_records[i].m_title.m_classification_code_l == VEGITATION_LINE_CODES[j]) {
                    find = true;
                    break;
                }
            }
            if (!find) {
                continue;
            }
            painter.setPen(QPen(Qt::black, 2, Qt::DotLine));
            QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
            painter.drawPolyline(points.data(), points.size());
        }
    }

    void DrawSoilsAndLavaSheets(QPainter& painter, const nSXFFile::rSXFFile& file) {
        static const int32_t SMOOTH_SANDS_CODE = 72255000;
        static const int32_t SWAMPS_CODE = 72310000;
        static const int32_t SALT_MARSHES_CODE = 72320000;

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() != nSXFFile::eLocalType::AREAL) {
                continue; // Все объекты, представленные в этом слое - площадные
            }
            if (file.m_records[i].m_title.m_classification_code_l == SMOOTH_SANDS_CODE) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(Qt::darkYellow, Qt::Dense6Pattern));
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                painter.drawPolygon(points.data(), points.size());
            }
            else if (file.m_records[i].m_title.m_classification_code_l == SWAMPS_CODE) {
                // Отрисовка не готова
            }
            else if (file.m_records[i].m_title.m_classification_code_l == SALT_MARSHES_CODE) {
                // Отрисовка не готова
            }
        }
    }

    void DrawReliefOfHydrography(QPainter& painter, const nSXFFile::rSXFFile& file) {
        static const int32_t SHALLOWS_CODE = 31211000;
        static const int32_t STEEP_BANKS_CODE = 31242000;
        static const int32_t LINEAR_CODES_COUNT = 3;
        static const int32_t RELIEF_HYDROGRAPHY_LINEAR_CODES[LINEAR_CODES_COUNT] = {STEEP_BANKS_CODE, 31310000, 31521000};

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.m_classification_code_l == SHALLOWS_CODE) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(Qt::black, Qt::Dense6Pattern));
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                painter.drawPolygon(points.data(), points.size());
            }
            else {
                bool find = false;
                for (int j = 0; j < LINEAR_CODES_COUNT; ++j) {
                    if (file.m_records[i].m_title.m_classification_code_l == RELIEF_HYDROGRAPHY_LINEAR_CODES[j]) {
                        find = true;
                        break;
                    }
                }
                if (!find) {
                    continue;
                }
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                if (file.m_records[i].m_title.m_classification_code_l == STEEP_BANKS_CODE) {
                    painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                }
                else {
                    painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::SolidLine));
                }
                painter.drawPolyline(points.data(), points.size());
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

        /* Вычисление размеров выходного изображения (считая, что коордианты рамки <-> координаты углов листа) */
        double meters_length = file.m_passport.m_rectang_coords.m_northwest.m_x - file.m_passport.m_rectang_coords.m_southwest.m_x;
        double meters_width = file.m_passport.m_rectang_coords.m_northeast.m_y - file.m_passport.m_rectang_coords.m_northwest.m_y;
        double discrete_length = left_top.x() - left_bottom.x();
        double discrete_width = right_top.y() - left_top.y();

        METERS_DESCRETE_X_D = meters_length/discrete_length;
        METERS_DESCRETE_Y_D = meters_width/discrete_width;

        double image_size_x_disc = qMax(right_bottom.y(), right_top.y()) - qMin(left_bottom.y(), left_top.y()) + 2 * OFFSET;
        image_size_x_disc *= METERS_DESCRETE_Y_D; // Перевод из дискрет в метры
        double image_size_y_disc = qMax(right_top.x(), left_top.x()) - qMin(right_bottom.x(), left_bottom.x()) + 2 * OFFSET;
        image_size_y_disc *= METERS_DESCRETE_X_D;

        /* Вычисление размера пикселя в метрах ( подробнее об этом тут https://help13.gisserver.ru/russian/panorama/index.html?vekbmp.html ) */
        int32_t precision_met = /*file.m_passport.m_instrument_resolution_l*/ 6000; // Внимание! От этого значения зависит размер изображения, его можно изменять (уменьшать - меньше разрешение изображения)
        PIXEL_METERS_D = 1.0f*(file.m_passport.m_scale_l / precision_met);
        int32_t image_size_x_pix = qRound(image_size_x_disc / PIXEL_METERS_D);
        int32_t image_size_y_pix = qRound(image_size_y_disc / PIXEL_METERS_D);
        std::cout << "РАЗМЕРЫ: " << image_size_x_pix << " " << image_size_y_pix << std::endl;

        QImage picture(image_size_x_pix, image_size_y_pix, QImage::Format_RGB32);
        picture.fill(Qt::lightGray);
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

        /* Отрисовка населенных пунктов */
        DrawSettlements(painter, file);

        /* Отрисовка объектов растительности с заливкой */
        DrawVegitationFilled(painter, file);

        /* Отрисовка грунтов и лавовых покровов */
        DrawSoilsAndLavaSheets(painter, file);

        /* Отрисовка рельефа суши */
        DrawLandRelief(painter, file);

        /* Отрисовка растительности */
        DrawVegitation(painter, file);

        /* Отрисовка гидрографии */
        DrawHydrography(painter, file);

        /* Отрисовка рельефа гидрографии */
        DrawReliefOfHydrography(painter, file);

        painter.end();
        picture.save("result.png", "PNG");
    }
}
