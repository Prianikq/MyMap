#pragma once
#include "rsxffile.h"
#include <QImage>
#include <QPainter>

/*! \file drawfunctions.hpp
    \brief Содержит набор функций и методов для оформления данных карты на выходном изображении.
 */

/*! \brief Содерджит необходимые процедуры и функции отрисовки.
 */
namespace nDraw {
    QPoint g_coords_orig; /*!< Условные координаты левого верхнего угла окна. */
    double g_d_meters_descrete_x; /*!< Определяет значение по X координате, сколько метров содержится в 1 дискрете. */
    double g_d_meters_descrete_y; /*!< Определяет значение по Y координате, сколько метров содержится в 1 дискрете. */
    double g_d_pixels_meters; /*!< Определяет значение, сколько метров в 1 пикселе. */

    int32_t g_l_image_size_x_pix; /*!< Размеры в пикселях выходного изображения (ширина). */
    int32_t g_l_image_size_y_pix; /*!< Размеры в пикселях выходного изображение (высота). */

    /*!
     * \brief GetRealSizes - вычисляет реальные размеры иконки (в пикселях), которую необходимо отрисовать.
     * Данное преобразование необходимо для того, чтобы иконки имели возможность масштабироваться при изменении размеров выходного изображения, которые задаются в программе.
     * \param p_s_width - хранит первоначальную ширину иконки в пикселях.
     * \param p_s_height - хранит первоначальную высоту иконки в пикселях.
     * \param p_d_real_size_x - хранит результат преобразования ширины иконки в пикселях.
     * \param p_d_real_size_y - хранит результат преобразования высоты иконки в пикселях.
     */
    void GetRealSizes(const short& p_s_width, const short& p_s_height, double& p_d_real_size_x, double& p_d_real_size_y) { // Функция, вычисляющая реальные размеры иконок
        /* Константы линейных функций для получения размера иконки по разрешению экрана (выводились экспериментально - по анализу изменения размера некоторой иконки) */
        static const double K_X = 12.0 / 3168.0; // <размер иконки по x> = K_X * <размер изображения по x> + B_X
        static const double B_X = 17.0 - 4608 * K_X;
        static const double K_Y = 9.0 / 3209.0;
        static const double B_Y = 15.0 - 4666 * K_Y;
        static const double REAL_SIZE_ICON_X = K_X * g_l_image_size_x_pix + B_X; // Размер иконки (по которой выводилась линейная функция) с учетом разрешения изображения
        static const double REAL_SIZE_ICON_Y = K_Y * g_l_image_size_y_pix + B_Y;
        static const double SIZE_ICON_X = 6; // Константы размера (в редакторе создания) той иконки, относительно который выводилась линейная функция
        static const double SIZE_ICON_Y = 5; // Данные константы нужны для относительного преобразования размеров иконок с учетом их изначальных размеров
        static const double BEGIN_SCALE_X = 17;
        static const double BEGIN_SCALE_Y = 15;

        if (qRound(REAL_SIZE_ICON_X) >= BEGIN_SCALE_X && qRound(REAL_SIZE_ICON_Y) >= BEGIN_SCALE_Y) {
            p_d_real_size_x = 1.0*REAL_SIZE_ICON_X * p_s_width / SIZE_ICON_X;
            p_d_real_size_y = 1.0*REAL_SIZE_ICON_Y * p_s_height / SIZE_ICON_Y;
        }
        else {
            p_d_real_size_x = p_s_width;
            p_d_real_size_y = p_s_height;
        }
    }

    /*!
     * \brief ChangeTextureSizes - производит машстабирование текстуры, которая будет накладываться на некоторый площадной объект.
     * \param p_icon - первоначальный объект неотмасшатбированной текстуры.
     * \param p_d_real_size_x - ширина иконки (в пикселях), которая заполняет данную текстуру.
     * \param p_d_real_size_y - высота иконки (в пикселях), которая заполняет данную текстуру.
     * \return объект QPixmap, представляющий отмасшатбированную текстуру.
     */
    QPixmap ChangeTextureSizes(const QPixmap& p_icon, const double& p_d_real_size_x, const double& p_d_real_size_y) {
        /* Постоянные линейных функций зависимости размера текстуры от размера значка (для отрисовки площадных объектов заполненных значками) */
        static const double K_X = 10;
        static const double B_X = 0;
        static const double K_Y = 8.2;
        static const double B_Y = 0;
        int32_t texture_size_x = qRound(K_X*p_d_real_size_x+B_X);
        int32_t texture_size_y = qRound(K_Y*p_d_real_size_y+B_Y);
        QPixmap result = p_icon;
        result = result.scaled(texture_size_x, texture_size_y, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        return result;
    }

    /*!
     * \brief FromMapPointToWindowPointF - выполняет преобразование координат точки из условной системы карты в пиксельную изображения.
     * \param p_coords_input - некоторый объект, представляющий точку в условной системе координат.
     * \return объект QPointF, представляющий точку с пиксельными координатами.
     */
    template<class T>
    QPointF FromMapPointToWindowPointF(const T& p_coords_input) {
        double x = (1.0 * g_coords_orig.x() - p_coords_input.x()) * g_d_meters_descrete_x / g_d_pixels_meters;
        double y = (p_coords_input.y() - 1.0 * g_coords_orig.y()) * g_d_meters_descrete_y / g_d_pixels_meters;
        QPointF result(y, x);
        return result;
    }
    QPoint FromMapPointToWindowPoint(const QPoint& p_coords_input) { // Добавлена для создания QRegion рамки
        double x = (1.0 * g_coords_orig.x() - p_coords_input.x()) * g_d_meters_descrete_x / g_d_pixels_meters;
        double y = (p_coords_input.y() - 1.0 * g_coords_orig.y()) * g_d_meters_descrete_y / g_d_pixels_meters;
        QPoint result(qRound(y), qRound(x));
        return result;
    }
    QVector<QPointF> GetMetricPoints(const nSXFFile::rRecord& p_record) {
        QVector<QPointF> result(p_record.m_title.m_l_count_metric_points);
        for (int32_t i = 0; i < p_record.m_title.m_l_count_metric_points; ++i) {
            if (!p_record.m_title.Has3DPresentation()) {
                if (!p_record.m_title.HasDoubleMetricType() && !p_record.m_title.HasBigMetricSize()) {
                    QPair<short, short>* get_point = static_cast<QPair<short, short>*>(p_record.m_p_points);
                    result[i] = FromMapPointToWindowPointF(QPoint(get_point[i].first, get_point[i].second));
                }
                else if (p_record.m_title.HasDoubleMetricType() && !p_record.m_title.HasBigMetricSize()) {
                    QPair<float, float>* get_point = static_cast<QPair<float, float>*>(p_record.m_p_points);
                    result[i] = FromMapPointToWindowPointF(QPointF(get_point[i].first, get_point[i].second));
                }
                else if (!p_record.m_title.HasDoubleMetricType() && p_record.m_title.HasBigMetricSize()) {
                    QPair<int32_t, int32_t>* get_point = static_cast<QPair<int32_t, int32_t>*>(p_record.m_p_points);
                    result[i] = FromMapPointToWindowPointF(QPoint(get_point[i].first, get_point[i].second));
                }
                else {
                    QPair<double, double>* get_point = static_cast<QPair<double, double>*>(p_record.m_p_points);
                    result[i] = FromMapPointToWindowPointF(QPointF(get_point[i].first, get_point[i].second));
                }
            }
            else {
                // Обработка трёхмерных координат (в рабочей карте нет)
            }
        }
        return result;
    }

    void DrawPointObject(const nSXFFile::rRecord& p_record, QPainter& p_painter) {
        double real_size_x, real_size_y;
        QPixmap image("images/point objects/" + QString::number(p_record.m_title.m_l_classification_code) + ".png");
        GetRealSizes(image.width(), image.height(), real_size_x, real_size_y);
        p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
        QVector<QPointF> points = GetMetricPoints(p_record);
        p_painter.drawPixmap(QRect(qRound(points[0].x() - real_size_x/2.0), qRound(points[0].y() - real_size_y/2),
                qRound(real_size_x), qRound(real_size_y)), image, QRect(0, 0, image.width(), image.height()));
    }
    void DrawSquareObjectWithIcons(const short& p_s_width, const short& p_s_height, const nSXFFile::rRecord& p_record, QPainter& p_painter) {
        double real_size_x, real_size_y;
        GetRealSizes(p_s_width, p_s_height, real_size_x, real_size_y);
        QPixmap icon("images/areal objects/" + QString::number(p_record.m_title.m_l_classification_code) + ".png");
        //std::cout << "BEFORE: " << icon.width() << " " << icon.height() << std::endl;
        icon = ChangeTextureSizes(icon, real_size_x, real_size_y);
        //std::cout << "AFTER: " << icon.width() << " " << icon.height() << std::endl;
        p_painter.setPen(QPen(Qt::black, 1, Qt::NoPen));
        p_painter.setBrush(QBrush(icon));
        QVector<QPointF> points = GetMetricPoints(p_record);
        p_painter.drawPolygon(points.data(), points.count());
    }
    bool SearchInGroupCodes(const int32_t* p_pl_array, const int32_t& p_l_array_size, const int32_t& p_l_code) {
        for (int32_t i = 0; i < p_l_array_size; ++i) {
            if (p_pl_array[i] == p_l_code) {
                return true;
            }
        }
        return false;
    }
    const nSXFFile::rSemantic GetSemanticValue(const nSXFFile::rRecord& p_record, const short& p_s_semantic_code) {
        for (size_t i = 0; i < p_record.m_semantics.size(); ++i) {
            if (p_record.m_semantics[i].m_s_semantic_code == p_s_semantic_code) {
                return p_record.m_semantics[i];
            }
        }
        std::cerr << "Error! No semantics found with required code!" << std::endl;
        return p_record.m_semantics.back();
    }

    void DrawVegitation(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды линейных объектов */
        static const int32_t LINEAR_CODES_COUNT = 2;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {71111110, 71131000};
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 4;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {71111110, 71111220, 71132000, 71211200};
        /* Коды площадных объектов */
        static const int32_t MEADOW_VEGITATION_CODE = 71314000;
        static const int32_t RICE_FIELDS_CODE = 71322100;
        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::eLocalType::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                p_painter.setPen(QPen(Qt::black, 1.5, Qt::DotLine));
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                p_painter.drawPolyline(points.data(), points.size());
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::eLocalType::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                DrawPointObject(p_file.m_records[i], p_painter);
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::eLocalType::AREAL) {
                if (p_file.m_records[i].m_title.m_l_classification_code == MEADOW_VEGITATION_CODE) {
                    static const short WIDTH = 3;
                    static const short HEIGHT = 4;
                    DrawSquareObjectWithIcons(WIDTH, HEIGHT, p_file.m_records[i], p_painter);
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == RICE_FIELDS_CODE) {

                }
            }
        }
    }

    void DrawLandRelief(QPainter& p_painter, const nSXFFile::rSXFFile &p_file) {
        /* Коды линейных объектов */
        static const int32_t LINEAR_CODES_COUNT = 6;
        static const int32_t HORIZONTALS_CODE = 21100000;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {HORIZONTALS_CODE, 21200000, 21300000, 22212000, 22221000, 22630000};
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {22250000, 22520000};

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                if (p_file.m_records[i].m_title.m_l_classification_code == HORIZONTALS_CODE) {
                    p_painter.setPen(QPen(QColor(180, 136, 60), 2, Qt::SolidLine));
                }
                else {
                    p_painter.setPen(QPen(QColor(180, 136, 60), 1, Qt::SolidLine));
                }
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                p_painter.drawPolyline(points.data(), points.size());
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                DrawPointObject(p_file.m_records[i], p_painter);
            }
        }
    }

    void DrawSoilsAndLavaSheets(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        static const int32_t SMOOTH_SANDS_CODE = 72255000;
        static const int32_t SWAMPS_CODE = 72310000;
        static const int32_t SALT_MARSHES_CODE = 72320000;

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() != nSXFFile::eLocalType::AREAL) {
                continue; // Все объекты, представленные в этом слое - площадные
            }
            if (p_file.m_records[i].m_title.m_l_classification_code == SMOOTH_SANDS_CODE) {
                p_painter.setPen(Qt::NoPen);
                p_painter.setBrush(QBrush(Qt::darkYellow, Qt::Dense6Pattern));
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                p_painter.drawPolygon(points.data(), points.size());
            }
            else if (p_file.m_records[i].m_title.m_l_classification_code == SWAMPS_CODE) {
                // Отрисовка не готова
            }
            else if (p_file.m_records[i].m_title.m_l_classification_code == SALT_MARSHES_CODE) {
                // Отрисовка не готова
            }
        }
    }

    void DrawHydrography(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды линейных объектов */
        static const int32_t RIVERS_CODE = 31410000;
        static const int32_t CHANNELS_CODE = 31431000;
        /* Коды площадных объектов */
        static const int32_t AREAL_CODES_COUNT = 5;
        static const int32_t LAKES_CODE = 31120000;
        static const int32_t AREAL_OBJECTS_CODES[AREAL_CODES_COUNT] = {LAKES_CODE, 31131000, 31132000, 31410000, 31431000};

        /* Коды семантик, влияющих на отрисовку объектов */
        static const short TYPE_OF_COASTLINE_CODE = 36;
        static const short SCALE_WIDTH_CODE = 15;
        static const short RELATIVE_TO_GROUND_LOC_CODE = 35;
        enum eTypeOfCoastline { PERMAMENT = 1, UNSTABLE = 2, UNDERGROUND = 4};
        enum eTypeOfWidth { LESS10M = 5, FROM10TO20M = 14, LESS20M = 15, EQ20MANDMORE = 16, FROM20TO120M = 19 };
        enum eTypeRelativeLocation { GROUND = 1, UNDERGROUNDE = 2, OVERWATER = 3, UNDERWATER = 4, DRYERS = 5, SEA = 6, MISSING = 7};

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (p_file.m_records[i].m_title.m_l_classification_code == RIVERS_CODE) {
                    if (p_file.m_records[i].m_title.HasSemantics()) {
                        QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                        char type_of_coastline_value = GetSemanticValue(p_file.m_records[i], TYPE_OF_COASTLINE_CODE).m_data.m_c_char;
                        char scale_width_value = GetSemanticValue(p_file.m_records[i], SCALE_WIDTH_CODE).m_data.m_c_char;
                        if (type_of_coastline_value == eTypeOfCoastline::UNSTABLE && scale_width_value == eTypeOfWidth::LESS20M) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::DashLine));
                        }
                        else if (type_of_coastline_value == eTypeOfCoastline::UNSTABLE && scale_width_value == eTypeOfWidth::FROM20TO120M) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 2, Qt::DashLine));
                        }
                        else if (type_of_coastline_value == eTypeOfCoastline::PERMAMENT && scale_width_value == eTypeOfWidth::LESS20M) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::SolidLine));
                        }
                        else if (type_of_coastline_value == eTypeOfCoastline::PERMAMENT && scale_width_value == eTypeOfWidth::FROM20TO120M) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 2, Qt::SolidLine));
                        }
                        p_painter.drawPolyline(points.data(), points.size());
                    }
                    else {
                        perror("Error! Object with the river code has no semantics!");
                    }
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == CHANNELS_CODE) {
                    if (p_file.m_records[i].m_title.HasSemantics()) {
                        QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                        char scale_width_value = GetSemanticValue(p_file.m_records[i], SCALE_WIDTH_CODE).m_data.m_c_char;
                        char relative_location_value = GetSemanticValue(p_file.m_records[i], RELATIVE_TO_GROUND_LOC_CODE).m_data.m_c_char;
                        if (relative_location_value == eTypeRelativeLocation::GROUND && scale_width_value == eTypeOfWidth::LESS10M) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::SolidLine));
                        }
                        else if (relative_location_value == eTypeRelativeLocation::GROUND && scale_width_value == eTypeOfWidth::FROM20TO120M) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 3, Qt::SolidLine));
                            p_painter.drawPolyline(points.data(), points.size());
                            p_painter.setPen(QPen(QColor(0, 191, 255), 1, Qt::SolidLine));
                        }
                        else if (relative_location_value == eTypeRelativeLocation::GROUND && scale_width_value == eTypeOfWidth::FROM10TO20M) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 2, Qt::SolidLine));
                        }
                        else if (relative_location_value == eTypeRelativeLocation::UNDERGROUNDE) {
                            p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::DotLine));
                        }
                        p_painter.drawPolyline(points.data(), points.size());
                    }
                    else {
                        perror("Error! Object with the channel code has no semantics!");
                    }
                }
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::AREAL) {
                if (!SearchInGroupCodes(AREAL_OBJECTS_CODES, AREAL_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                if (p_file.m_records[i].m_title.m_l_classification_code == LAKES_CODE) {
                    char type_of_coastline_value = GetSemanticValue(p_file.m_records[i], TYPE_OF_COASTLINE_CODE).m_data.m_c_char;
                    if (type_of_coastline_value == eTypeOfCoastline::PERMAMENT) {
                        p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::SolidLine));
                        p_painter.setBrush(QBrush(QColor(0, 191, 255), Qt::SolidPattern));
                    }
                    else if (type_of_coastline_value == eTypeOfCoastline::UNSTABLE) {
                        p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::DashLine));
                        p_painter.setBrush(QBrush(QColor(0, 191, 255), Qt::SolidPattern));
                    }
                }
                else {
                    p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::SolidLine));
                    p_painter.setBrush(QBrush(QColor(0, 191, 255), Qt::SolidPattern));
                }
                p_painter.drawPolygon(points.data(), points.size());
            }
        }
    }

    void DrawVegitationFilled(QPainter& p_painter, const nSXFFile::rSXFFile &p_file) {
        static const int32_t CODES_COUNT_GREEN = 2;
        static const int32_t VEGITATION_CODES_GREEN[CODES_COUNT_GREEN] = {71113000, 71211200};
        static const int32_t CODES_COUNT_DARK_GREEN = 2;
        static const int32_t VEGITATION_CODES_DARK_GREEN[CODES_COUNT_DARK_GREEN] = {71111110, 71123000};

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() != nSXFFile::eLocalType::AREAL) {
                continue; // Все объекты, представленные в этом слое - площадные
            }
            char find = 0;
            for (int32_t j = 0; j < CODES_COUNT_GREEN; ++j) {
                if (p_file.m_records[i].m_title.m_l_classification_code == VEGITATION_CODES_GREEN[j]) {
                    find = 1;
                    break;
                }
            }
            for (int32_t j = 0; j < CODES_COUNT_DARK_GREEN; ++j) {
                if (p_file.m_records[i].m_title.m_l_classification_code == VEGITATION_CODES_DARK_GREEN[j]) {
                    find = 2;
                    break;
                }
            }
            if (find == 0) {
                continue;
            }
            else if (find == 1) {
                p_painter.setPen(QPen(QColor(152, 251, 152), 1, Qt::SolidLine));
                p_painter.setBrush(QBrush(QColor(152, 251, 152), Qt::SolidPattern));
            }
            else {
                p_painter.setPen(QPen(QColor(154, 205, 50), 1, Qt::SolidLine));
                p_painter.setBrush(QBrush(QColor(154, 205, 50), Qt::SolidPattern));
            }
            QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
            p_painter.drawPolygon(points.data(), points.size());
        }
    }

    void DrawSettlements(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды площадных объектов */
        static const int32_t AREAL_CODES_COUNT = 4;
        static const int32_t AREAL_OBJECTS_CODES[AREAL_CODES_COUNT] = {41100000, 41200000, 42100000, 43100000};

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() != nSXFFile::eLocalType::AREAL) {
                if (!SearchInGroupCodes(AREAL_OBJECTS_CODES, AREAL_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                p_painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine));
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                p_painter.drawPolyline(points.data(), points.size());
            }
        }
    }

    void DrawReliefOfHydrography(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды линейных объектов */
        static const int32_t STEEP_BANKS_CODE = 31242000;
        static const int32_t LINEAR_CODES_COUNT = 3;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {STEEP_BANKS_CODE, 31310000, 31521000};
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {31510000, 31531000};
        /* Коды площадных объектов */
        static const int32_t SHALLOWS_CODE = 31211000;
        static const int32_t REED_VEGITATION_CODE = 71311000;

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                if (p_file.m_records[i].m_title.m_l_classification_code == STEEP_BANKS_CODE) {
                    p_painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                }
                else {
                    p_painter.setPen(QPen(QColor(30, 144, 255), 1, Qt::SolidLine));
                }
                p_painter.drawPolyline(points.data(), points.size());
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                DrawPointObject(p_file.m_records[i], p_painter);
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::AREAL) {
                if (p_file.m_records[i].m_title.m_l_classification_code == SHALLOWS_CODE) {
                    p_painter.setPen(Qt::NoPen);
                    p_painter.setBrush(QBrush(Qt::black, Qt::Dense6Pattern));
                    QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                    p_painter.drawPolygon(points.data(), points.size());
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == REED_VEGITATION_CODE) {
                    static const int32_t WIDTH = 5;
                    static const int32_t HEIGHT = 4;
                    p_painter.setPen(Qt::NoPen);
                    DrawSquareObjectWithIcons(WIDTH, HEIGHT, p_file.m_records[i], p_painter);
                }
            }
        }
    }

    void DrawHydraulicStructures(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 4;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {31612000, 31635000, 32250000, 32410000};
        /* Коды линейных объектов */
        static const int32_t GATEWAYS_CODE = 32120000;
        static const int32_t BANKS_WITH_FORTIF_SLOPES_CODE = 32150000;
        static const int32_t AUTOCARRIAGE_FERRIES_CODE = 33113000;
        static const int32_t LINEAR_CODES_COUNT = 3;
        static const int32_t LINEAR_OBJECTS_CODES[LINEAR_CODES_COUNT] = {GATEWAYS_CODE, BANKS_WITH_FORTIF_SLOPES_CODE, AUTOCARRIAGE_FERRIES_CODE};

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                DrawPointObject(p_file.m_records[i], p_painter);
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (!SearchInGroupCodes(LINEAR_OBJECTS_CODES, LINEAR_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                if (p_file.m_records[i].m_title.m_l_classification_code == GATEWAYS_CODE) {
                    p_painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
                    QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                    p_painter.drawPolyline(points);
                    p_painter.setPen(QPen(QColor(0, 191, 255), 1, Qt::SolidLine));
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == BANKS_WITH_FORTIF_SLOPES_CODE) {
                    p_painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == AUTOCARRIAGE_FERRIES_CODE)  {
                    p_painter.setPen(QPen(Qt::black, 1, Qt::DashLine));
                }
                p_painter.drawPolyline(points);
            }
        }
    }

    void DrawFillingAndCutting(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды линейных объектов */
        static const int32_t CUTTINGS_CODE = 62360000;

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::LINEAR) {
                if (p_file.m_records[i].m_title.m_l_classification_code == CUTTINGS_CODE) {
                    p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                    p_painter.drawPolyline(points);
                    p_painter.setPen(Qt::DotLine);
                    p_painter.drawPolyline(points);
                }
            }
        }
    }

    void DrawRoadNetwork(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды линейных объектов */
        static const int32_t OPERATING_RAILWAYS_CODE = 61111000;
        static const int32_t ACCESS_ROADS_CODE = 61122000;
        // В разработке
    }

    void DrawRoadStructures(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {62131000, 62133000};

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                DrawPointObject(p_file.m_records[i], p_painter);
            }
        }
    }

    void DrawSettlementsQuarters(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды точечных объектов */
        static const int32_t SEPARATE_BUILDINGS_CODE = 44200000;
        /* Коды площадных объектов */
        static const int32_t AREAL_CODES_COUNT = 3;
        static const int32_t QUARTERS_CODE = 45100000;
        static const int32_t AREAL_OBJECTS_CODES[AREAL_CODES_COUNT] = {42200000, 44200000, QUARTERS_CODE}; // Отрисовываются одинаково

        /* Коды семантик, влияющих на отрисовку объектов */
        static const short BUILDING_DENSITY_CODE = 45;
        static const short COLOUR_CODE = 94;
        enum eTypeBuildingDensity { TIGHTLY = 1, RARE = 2, UNDESIGNED = 3, DESTROYED = 4 };
        enum eTypeColour {GREY = 401, BLACK = 101, WHITE = 151, BROWN = 501, };

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (p_file.m_records[i].m_title.m_l_classification_code == SEPARATE_BUILDINGS_CODE) {
                    DrawPointObject(p_file.m_records[i], p_painter);
                }
            }
            else if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::AREAL) {
                if (!SearchInGroupCodes(AREAL_OBJECTS_CODES, AREAL_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                if (p_file.m_records[i].m_title.m_l_classification_code == QUARTERS_CODE) {
                    if (p_file.m_records[i].m_title.HasSemantics()) {
                        char building_density_value = GetSemanticValue(p_file.m_records[i], BUILDING_DENSITY_CODE).m_data.m_c_char;
                        short colour_value = GetSemanticValue(p_file.m_records[i], COLOUR_CODE).m_data.m_s_short;
                        p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                        if (building_density_value == eTypeBuildingDensity::TIGHTLY && colour_value == eTypeColour::BLACK) {
                            p_painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
                        }
                        else if (building_density_value == eTypeBuildingDensity::UNDESIGNED && colour_value == eTypeColour::WHITE) {
                            p_painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
                        }
                        else if (building_density_value == eTypeBuildingDensity::TIGHTLY && colour_value == eTypeColour::GREY) {
                            p_painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
                        }
                        else if (building_density_value == eTypeBuildingDensity::TIGHTLY && colour_value == eTypeColour::BROWN) {
                            p_painter.setBrush(QBrush(QColor(180, 136, 60), Qt::SolidPattern));
                        }
                        else if (building_density_value == eTypeBuildingDensity::RARE && colour_value == eTypeColour::WHITE) {
                            p_painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
                        }
                    }
                    else {
                        perror("Error! Object with the quarters code has no semantics!");
                    }
                }
                else {
                    p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    p_painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
                }
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                p_painter.drawPolygon(points);
            }
        }
    }

    void DrawIndustrialSocialFacilities(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 14;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {51130000, 51230000, 51410000, 51420000, 51460000, 51470000, 51500000, 52100000, 53180000, 53340000, 53420000, 53510000, 53530000, 53600000 };
        /* Коды площадных объектов */
        static const int32_t AREAL_CODES_COUNT = 6;
        static const int32_t QUARRIES_CODE = 51111000;
        static const int32_t INDUSTRIAL_FACILITIES_CODE = 51130000;
        static const int32_t TERRITORY_INDUSTR_FACIL_CODE = 51130001;
        static const int32_t SEDIM_TANKS_CODE = 51480000;
        static const int32_t AGRICULTURAL_ENTERP_CODE = 52100000;
        static const int32_t CEMETERY_CODE = 53510000;
        static const int32_t AREAL_OBJECTS_CODES[AREAL_CODES_COUNT] = {QUARRIES_CODE, INDUSTRIAL_FACILITIES_CODE, TERRITORY_INDUSTR_FACIL_CODE, SEDIM_TANKS_CODE, AGRICULTURAL_ENTERP_CODE, CEMETERY_CODE};
        /*Коды линейных объектов */
        static const int32_t GAS_PIPELINE_CODE = 51310000;

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                DrawPointObject(p_file.m_records[i], p_painter);
            }
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::AREAL) {
                if (!SearchInGroupCodes(AREAL_OBJECTS_CODES, AREAL_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                QVector<QPointF> points = GetMetricPoints(p_file.m_records[i]);
                if (p_file.m_records[i].m_title.m_l_classification_code == QUARRIES_CODE) {
                    p_painter.setPen(QPen(QColor(180, 136, 60), 0.5, Qt::SolidLine));
                    p_painter.setBrush(Qt::NoBrush);
                    p_painter.drawPolygon(points);
                    p_painter.setPen(QPen(QColor(180, 136, 60), 1, Qt::DotLine));
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == INDUSTRIAL_FACILITIES_CODE) {
                    p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    p_painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == TERRITORY_INDUSTR_FACIL_CODE) {
                    p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    p_painter.setBrush(QBrush(Qt::NoBrush));
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == SEDIM_TANKS_CODE) {
                    p_painter.setPen(QPen(QColor(0, 191, 255), 1, Qt::SolidLine));
                    p_painter.setBrush(QBrush(QColor(0, 191, 255), Qt::SolidPattern));
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == AGRICULTURAL_ENTERP_CODE) {
                    p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    p_painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
                }
                else if (p_file.m_records[i].m_title.m_l_classification_code == CEMETERY_CODE) {
                    static const short WIDTH = 5;
                    static const short HEIGHT = 5;
                    p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                    DrawSquareObjectWithIcons(WIDTH, HEIGHT, p_file.m_records[i], p_painter);
                    continue;
                }
                p_painter.drawPolygon(points);
            }
        }
    }

    void DrawPlannedHighRiseBasis(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        /* Коды точечных объектов */
        static const int32_t POINT_CODES_COUNT = 2;
        static const int32_t POINT_OBJECTS_CODES[POINT_CODES_COUNT] = {11200000, 12000000};

        for (int32_t i = 0; i < p_file.m_descriptor.m_l_number_records; ++i) {
            if (p_file.m_records[i].m_title.LocalizationType() == nSXFFile::POINT) {
                if (!SearchInGroupCodes(POINT_OBJECTS_CODES, POINT_CODES_COUNT, p_file.m_records[i].m_title.m_l_classification_code)) {
                    continue;
                }
                DrawPointObject(p_file.m_records[i], p_painter);
            }
        }
    }

    void DrawBordersAndFences(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        // В разработке
    }

    void DrawTitlesAndSignatures(QPainter& p_painter, const nSXFFile::rSXFFile& p_file) {
        // В разработке
    }

    void Draw(const nSXFFile::rSXFFile& p_file) {
        QPoint left_bottom(p_file.m_passport.m_frame_coordinates.m_southwest.m_x, p_file.m_passport.m_frame_coordinates.m_southwest.m_y);
        QPoint right_bottom(p_file.m_passport.m_frame_coordinates.m_southeast.m_x, p_file.m_passport.m_frame_coordinates.m_southeast.m_y);
        QPoint left_top(p_file.m_passport.m_frame_coordinates.m_northwest.m_x, p_file.m_passport.m_frame_coordinates.m_northwest.m_y);
        QPoint right_top(p_file.m_passport.m_frame_coordinates.m_northeast.m_x, p_file.m_passport.m_frame_coordinates.m_northeast.m_y);
        static const int32_t OFFSET = 100; // Задает смещение от координат углов карты для задания области отрисовки

        g_coords_orig.setX(qMax(right_top.x(), left_top.x()) + OFFSET);
        g_coords_orig.setY(qMin(left_bottom.y(), left_top.y()) - OFFSET);

        /* Вычисление размеров выходного изображения (считая, что коордианты рамки <-> координаты углов листа) */
        double meters_length = p_file.m_passport.m_rectang_coords.m_northwest.m_x - p_file.m_passport.m_rectang_coords.m_southwest.m_x;
        double meters_width = p_file.m_passport.m_rectang_coords.m_northeast.m_y - p_file.m_passport.m_rectang_coords.m_northwest.m_y;
        double discrete_length = left_top.x() - left_bottom.x();
        double discrete_width = right_top.y() - left_top.y();

        g_d_meters_descrete_x = meters_length/discrete_length;
        g_d_meters_descrete_y = meters_width/discrete_width;

        double image_size_x_disc = qMax(right_bottom.y(), right_top.y()) - qMin(left_bottom.y(), left_top.y()) + 2 * OFFSET;
        image_size_x_disc *= g_d_meters_descrete_y; // Перевод из дискрет в метры
        double image_size_y_disc = qMax(right_top.x(), left_top.x()) - qMin(right_bottom.x(), left_bottom.x()) + 2 * OFFSET;
        image_size_y_disc *= g_d_meters_descrete_x;

        /* Вычисление размера пикселя в метрах ( подробнее об этом тут https://help13.gisserver.ru/russian/panorama/index.html?vekbmp.html ) */
        int32_t precision_met = /*file.m_passport.m_instrument_resolution_l*/ 6000; // Внимание! От этого значения зависит размер изображения, его можно изменять (уменьшать - меньше разрешение изображения)
        g_d_pixels_meters = 1.0f * (p_file.m_passport.m_l_scale / precision_met);
        g_l_image_size_x_pix = qRound(image_size_x_disc / g_d_pixels_meters);
        g_l_image_size_y_pix = qRound(image_size_y_disc / g_d_pixels_meters);
        std::cout << "РАЗМЕРЫ: " << g_l_image_size_x_pix << " " << g_l_image_size_y_pix << std::endl;

        QImage picture(g_l_image_size_x_pix, g_l_image_size_y_pix, QImage::Format_RGB32);
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
        DrawSettlements(painter, p_file);

        /* Отрисовка объектов растительности с заливкой */
        DrawVegitationFilled(painter, p_file);

        /* Отрисовка грунтов и лавовых покровов */
        DrawSoilsAndLavaSheets(painter, p_file);

        /* Отрисовка рельефа суши */
        DrawLandRelief(painter, p_file);

        /* Отрисовка растительности */
        DrawVegitation(painter, p_file);

        /* Отрисовка гидрографии */
        DrawHydrography(painter, p_file);

        /* Отрисовка рельефа гидрографии */
        DrawReliefOfHydrography(painter, p_file);

        /* Отрисовка гидротехнических сооружений */
        DrawHydraulicStructures(painter, p_file);

        /* Отрисовка насыпей и выемок */
        DrawFillingAndCutting(painter, p_file);

        /* Отрисовка дорожной сети */
        DrawRoadNetwork(painter, p_file);

        /* Отрисовка дорожных сооружений */
        DrawRoadStructures(painter, p_file);

        /* Отрисовка населённых пунктов (кварталов) */
        DrawSettlementsQuarters(painter, p_file);

        /* Отрисовка промышленных, социальных объектов */
        DrawIndustrialSocialFacilities(painter, p_file);

        /* Отрисовка границ и ограждений */
        DrawBordersAndFences(painter, p_file);

        /* Отрисовка планово-высотной основы */
        DrawPlannedHighRiseBasis(painter, p_file);

        /* Отрисовка названий и подписей */
        DrawTitlesAndSignatures(painter, p_file);

        painter.end();
        picture.save("result.png", "PNG");
    }
}
