#pragma once
#include "rsxffile.h"
#include <QImage>
#include <QPainter>

namespace nDraw {
    QPoint coords_orig_g; // Условные координаты левого верхнего угла окна
    double METERS_DESCRETE_X_D; // Определяет значение, сколько метров содержится в 1 дискрете (по X и Y координате отдельно)
    double METERS_DESCRETE_Y_D;
    double PIXEL_METERS_D; // Определяет значение, сколько метров в 1 пикселе

    int32_t IMAGE_SIZE_X_PIX; // Размеры в пикселях выходного изображения
    int32_t IMAGE_SIZE_Y_PIX;

    void GetRealSizes(const short& width, const short& height, double& real_size_x, double& real_size_y) { // Функция, вычисляющая реальные размеры иконок
        /* Константы линейных функций для получения размера иконки по разрешению экрана (выводились экспериментально - по анализу изменения размера некоторой иконки) */
        static const double K_X = 12.0 / 3168.0; // <размер иконки по x> = K_X * <размер изображения по x> + B_X
        static const double B_X = 17.0 - 4608 * K_X;
        static const double K_Y = 9.0 / 3209.0;
        static const double B_Y = 15.0 - 4666 * K_Y;
        static const double REAL_SIZE_ICON_X = K_X * IMAGE_SIZE_X_PIX + B_X; // Размер иконки (по которой выводилась линейная функция) с учетом разрешения изображения
        static const double REAL_SIZE_ICON_Y = K_Y * IMAGE_SIZE_Y_PIX + B_Y;
        static const double SIZE_ICON_X = 6; // Константы размера (в редакторе создания) той иконки, относительно который выводилась линейная функция
        static const double SIZE_ICON_Y = 5; // Данные константы нужны для относительного преобразования размеров иконок с учетом их изначальных размеров
        static const double BEGIN_SCALE_X = 17;
        static const double BEGIN_SCALE_Y = 15;

        if (qRound(REAL_SIZE_ICON_X) >= BEGIN_SCALE_X && qRound(REAL_SIZE_ICON_Y) >= BEGIN_SCALE_Y) {
            real_size_x = 1.0*REAL_SIZE_ICON_X * width / SIZE_ICON_X;
            real_size_y = 1.0*REAL_SIZE_ICON_Y * height / SIZE_ICON_Y;
        }
        else {
            real_size_x = width;
            real_size_y = height;
        }
    }
    QPixmap ChangeTextureSizes(const QPixmap& icon, const double& real_size_x, const double& real_size_y) {
        /* Постоянные линейных функций зависимости размера текстуры от размера значка (для отрисовки площадных объектов заполненных значками) */
        static const double K_X = 10;
        static const double B_X = 0;
        static const double K_Y = 8.2;
        static const double B_Y = 0;
        int32_t texture_size_x = qRound(K_X*real_size_x+B_X);
        int32_t texture_size_y = qRound(K_Y*real_size_y+B_Y);
        QPixmap result = icon;
        result = result.scaled(texture_size_x, texture_size_y, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        return result;
    }

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

    void DrawPointObject(const nSXFFile::rRecord& record, QPainter& painter) {
        double real_size_x, real_size_y;
        QPixmap image("images/" + QString::number(record.m_title.m_classification_code_l) + ".png");
        GetRealSizes(image.width(), image.height(), real_size_x, real_size_y);
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
        QVector<QPointF> points = GetMetricPoints(record);
        painter.drawPixmap(QRect(qRound(points[0].x() - real_size_x/2.0), qRound(points[0].y() - real_size_y/2),
                qRound(real_size_x), qRound(real_size_y)), image, QRect(0, 0, image.width(), image.height()));
    }
    void DrawSquareObjectWithIcons(const short& width, const short& height, const nSXFFile::rRecord& record, QPainter& painter) {
        double real_size_x, real_size_y;
        GetRealSizes(width, height, real_size_x, real_size_y);
        QPixmap icon("images/" + QString::number(record.m_title.m_classification_code_l) + ".png");
        //std::cout << "BEFORE: " << icon.width() << " " << icon.height() << std::endl;
        icon = ChangeTextureSizes(icon, real_size_x, real_size_y);
        //std::cout << "AFTER: " << icon.width() << " " << icon.height() << std::endl;
        painter.setPen(QPen(Qt::black, 1, Qt::NoPen));
        painter.setBrush(QBrush(icon));
        QVector<QPointF> points = GetMetricPoints(record);
        painter.drawPolygon(points.data(), points.count());
    }
    bool SearchInGroupCodes(const int32_t* array, const int32_t& array_size, const int32_t& code) {
        for (int32_t i = 0; i < array_size; ++i) {
            if (array[i] == code) {
                return true;
            }
        }
        return false;
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
        }
    }

    void DrawLandRelief(QPainter& painter, const nSXFFile::rSXFFile &file) {
        /* Коды линейных объектов */
        static const int32_t LINEAR_CODES_COUNT = 6;
        static const int32_t HORIZONTALS_CODE = 21100000;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {HORIZONTALS_CODE, 21200000, 21300000, 22212000, 22221000, 22630000};
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {22250000, 22520000};

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                if (file.m_records[i].m_title.m_classification_code_l == HORIZONTALS_CODE) {
                    painter.setPen(QPen(QColor(180, 136, 60), 2, Qt::SolidLine));
                }
                else {
                    painter.setPen(QPen(QColor(180, 136, 60), 1, Qt::SolidLine));
                }
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                painter.drawPolyline(points.data(), points.size());
            }
            else if (file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                DrawPointObject(file.m_records[i], painter);
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
        /* Коды линейных объектов */
        static const int32_t LINEAR_CODES_COUNT = 2;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {71111110, 71131000};
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 4;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {71111110, 71111220, 71132000, 71211200};
        /* Коды площадных объектов */
        static const int32_t MEADOW_VEGITATION_CODE = 71314000;
        static const int32_t RICE_FIELDS_CODE = 71322100;
        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::eLocalType::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                painter.setPen(QPen(Qt::black, 1.5, Qt::DotLine));
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                painter.drawPolyline(points.data(), points.size());
            }
            else if (file.m_records[i].m_title.LocalizationType() == nSXFFile::eLocalType::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                DrawPointObject(file.m_records[i], painter);
            }
            else if (file.m_records[i].m_title.LocalizationType() == nSXFFile::eLocalType::AREAL) {
                if (file.m_records[i].m_title.m_classification_code_l == MEADOW_VEGITATION_CODE) {
                    static const short WIDTH = 3;
                    static const short HEIGHT = 4;
                    DrawSquareObjectWithIcons(WIDTH, HEIGHT, file.m_records[i], painter);
                }
                else if (file.m_records[i].m_title.m_classification_code_l == RICE_FIELDS_CODE) {

                }
            }
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
        /* Коды линейных объектов */
        static const int32_t STEEP_BANKS_CODE = 31242000;
        static const int32_t LINEAR_CODES_COUNT = 3;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {STEEP_BANKS_CODE, 31310000, 31521000};
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {31510000, 31531000};
        /* Коды площадных объектов */
        static const int32_t SHALLOWS_CODE = 31211000;

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
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
            else if (file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                DrawPointObject(file.m_records[i], painter);
            }
            else if (file.m_records[i].m_title.LocalizationType() == nSXFFile::AREAL) {
                if (file.m_records[i].m_title.m_classification_code_l == SHALLOWS_CODE) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QBrush(Qt::black, Qt::Dense6Pattern));
                    QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                    painter.drawPolygon(points.data(), points.size());
                }
            }
        }
    }

    void DrawHydraulicStructures(QPainter& painter, const nSXFFile::rSXFFile& file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 4;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {31612000, 31635000, 32250000, 32410000};
        /* Коды линейных объектов */
        static const int32_t GATEWAYS_CODE = 32120000;
        static const int32_t BANKS_WITH_FORTIF_SLOPES_CODE = 32150000;
        static const int32_t AUTOCARRIAGE_FERRIES_CODE = 33113000;
        static const int32_t LINEAR_CODES_COUNT = 3;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {GATEWAYS_CODE, BANKS_WITH_FORTIF_SLOPES_CODE, AUTOCARRIAGE_FERRIES_CODE};

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                DrawPointObject(file.m_records[i], painter);
            }
            else if (file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                if (file.m_records[i].m_title.m_classification_code_l == GATEWAYS_CODE) {
                    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
                    QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                    painter.drawPolyline(points);
                    painter.setPen(QPen(QColor(0, 191, 255), 1, Qt::SolidLine));
                }
                else if (file.m_records[i].m_title.m_classification_code_l == BANKS_WITH_FORTIF_SLOPES_CODE) {
                    painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                }
                else if (file.m_records[i].m_title.m_classification_code_l == AUTOCARRIAGE_FERRIES_CODE)  {
                    painter.setPen(QPen(Qt::black, 1, Qt::DashLine));
                }
                painter.drawPolyline(points);
            }
        }
    }

    void DrawFillingAndCutting(QPainter& painter, const nSXFFile::rSXFFile& file) {
        /* Коды линейных объектов */
        static const int32_t CUTTINGS_CODE = 62360000;

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (file.m_records[i].m_title.m_classification_code_l == CUTTINGS_CODE) {
                    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                    painter.drawPolyline(points);
                    painter.setPen(Qt::DotLine);
                    painter.drawPolyline(points);
                }
            }
        }
    }

    void DrawRoadNetwork(QPainter& painter, const nSXFFile::rSXFFile& file) {
        /* Коды линейных объектов */
        static const int32_t OPERATING_RAILWAYS_CODE = 61111000;
        static const int32_t ACCESS_ROADS_CODE = 61122000;
        // В разработке
    }

    void DrawRoadStructures(QPainter& painter, const nSXFFile::rSXFFile& file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {62131000, 62133000};

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                DrawPointObject(file.m_records[i], painter);
            }
        }
    }

    void DrawSettlementsQuarters(QPainter& painter, const nSXFFile::rSXFFile& file) {
        /* Коды точечных объектов */
        static const int32_t SEPARATE_BUILDINGS_CODE = 44200000;
        /* Коды площадных объектов */
        static const int32_t AREAL_CODES_COUNT = 2;
        static const int32_t AREAL_OBJECTS_CODES[AREAL_CODES_COUNT] = {42200000, 44200000}; // Отрисовываются одинаково

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (file.m_records[i].m_title.m_classification_code_l == SEPARATE_BUILDINGS_CODE) {
                    DrawPointObject(file.m_records[i], painter);
                }
            }
            else if (file.m_records[i].m_title.LocalizationType() == nSXFFile::AREAL) {
                if (!SearchInGroupCodes(AREAL_OBJECTS_CODES, AREAL_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
                QVector<QPointF> points = GetMetricPoints(file.m_records[i]);
                painter.drawPolygon(points);
            }
        }
    }

    void DrawIndustrialSocialFacilities(QPainter& painter, const nSXFFile::rSXFFile& file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 14;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {51130000, 51230000, 51410000, 51420000, 51460000, 51470000, 51500000, 52100000, 53180000, 53340000, 53420000, 53510000, 53530000, 53600000 };

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                DrawPointObject(file.m_records[i], painter);
            }
        }
    }

    void DrawPlannedHighRiseBasis(QPainter& painter, const nSXFFile::rSXFFile& file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {11200000, 12000000};

        for (int32_t i = 0; i < file.m_descriptor.m_number_records_l; ++i) {
            if (file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, file.m_records[i].m_title.m_classification_code_l)) {
                    continue;
                }
                DrawPointObject(file.m_records[i], painter);
            }
        }
    }

    void DrawBordersAndFences(QPainter& painter, const nSXFFile::rSXFFile& file) {
        // В разработке
    }

    void DrawTitlesAndSignatures(QPainter& painter, const nSXFFile::rSXFFile& file) {
        // В разработке
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
        IMAGE_SIZE_X_PIX = qRound(image_size_x_disc / PIXEL_METERS_D);
        IMAGE_SIZE_Y_PIX = qRound(image_size_y_disc / PIXEL_METERS_D);
        std::cout << "РАЗМЕРЫ: " << IMAGE_SIZE_X_PIX << " " << IMAGE_SIZE_Y_PIX << std::endl;

        QImage picture(IMAGE_SIZE_X_PIX, IMAGE_SIZE_Y_PIX, QImage::Format_RGB32);
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

        /* Отрисовка гидротехнических сооружений */
        DrawHydraulicStructures(painter, file);

        /* Отрисовка насыпей и выемок */
        DrawFillingAndCutting(painter, file);

        /* Отрисовка дорожной сети */
        DrawRoadNetwork(painter, file);

        /* Отрисовка дорожных сооружений */
        DrawRoadStructures(painter, file);

        /* Отрисовка населённых пунктов (кварталов) */
        DrawSettlementsQuarters(painter, file);

        /* Отрисовка промышленных, социальных объектов */
        DrawIndustrialSocialFacilities(painter, file);

        /* Отрисовка границ и ограждений */
        DrawBordersAndFences(painter, file);

        /* Отрисовка планово-высотной основы */
        DrawPlannedHighRiseBasis(painter, file);

        /* Отрисовка названий и подписей */
        DrawTitlesAndSignatures(painter, file);

        painter.end();
        picture.save("result.png", "PNG");
    }
}
