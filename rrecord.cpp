#include "rrecord.h"
#include "functions.h"
#include "rpassport.h"
#include <iostream>

namespace nSXFFile {
    rRecord::rRecord() {
        this->m_p_points = nullptr;
        this->m_p_str_text = nullptr;
        this->m_p_subobjects = nullptr;
    }
    rRecord::~rRecord() {
        if (this->m_p_points != nullptr) {
            if (!this->m_title.HasDoubleMetricType() && !this->m_title.HasBigMetricSize()) {
                rRectang_coord2D<short>* get_points = static_cast<rRectang_coord2D<short>*>(this->m_p_points);
                delete[] get_points;
            }
            else if (this->m_title.HasDoubleMetricType() && !this->m_title.HasBigMetricSize()) {
                rRectang_coord2D<float>* get_points = static_cast<rRectang_coord2D<float>*>(this->m_p_points);
                delete[] get_points;
            }
            else if (!this->m_title.HasDoubleMetricType() && this->m_title.HasBigMetricSize()) {
                rRectang_coord2D<int32_t>* get_points = static_cast<rRectang_coord2D<int32_t>*>(this->m_p_points);
                delete[] get_points;
            }
            else {
                rRectang_coord2D<double>* get_points = static_cast<rRectang_coord2D<double>*>(this->m_p_points);
                delete[] get_points;
            }
        }
        if (this->m_p_str_text != nullptr) {
            delete[] this->m_p_str_text;
        }
        if (this->m_p_subobjects != nullptr) {
            delete[] this->m_p_subobjects;
        }
    }

    void rRecordTitle::Print() const {
        std::cout << "\tДанные заголовка записи: " << std::endl;
        std::cout << "\t\tИдентификатор начала записи: " << std::hex << this->m_l_identifier << std::endl;
        std::cout << "\t\tОбщая длина записи: " << std::dec << this->m_l_length_record << std::endl;
        std::cout << "\t\tДлина метрики: " << std::dec << this->m_l_length_metric << std::endl;
        std::cout << "\t\tКлассификационный код: " << std::dec << this->m_l_classification_code << std::endl;
        std::cout << "\t\tСобственный номер объекта: " << std::dec << this->m_l_object_number << std::endl;
        std::cout << "\t\tСправочные данные: "; InformationFlagsPrint(this->m_ul_reference_data);
        std::cout << "\t\tЧисло точек метрики: " << std::dec << this->m_l_count_metric_points << std::endl;
        std::cout << "\t\tОписатель метрики: " << std::endl;
        std::cout << "\t\t\tЧисло подобъектов: " << std::dec << this->m_as_metric_descriptor[0] << std::endl;
        std::cout << "\t\t\tЧисло точек метрики: " << std::dec << this->m_as_metric_descriptor[1] << std::endl;
        std::cout << "\tИтого считано: " << sizeof(rRecordTitle) << " байт." << std::endl;
    }
    bool rRecordTitle::HasBigMetricSize() const {
        static const int32_t METRIC_SIZE_POS = (1 << 10);
        return ((this->m_ul_reference_data & METRIC_SIZE_POS) == METRIC_SIZE_POS ? true : false);
    }
    bool rRecordTitle::HasDoubleMetricType() const  {
        static const int32_t METRIC_TYPE_POS = (1 << 18);
        return ((this->m_ul_reference_data & METRIC_TYPE_POS) == METRIC_TYPE_POS ? true : false);
    }
    bool rRecordTitle::Has3DPresentation() const {
        static const int32_t POS_3D_PRESENTATION = (1 << 17);
        return ((this->m_ul_reference_data & POS_3D_PRESENTATION) == POS_3D_PRESENTATION ? true : false);
    }
    bool rRecordTitle::HasGraphicDescription() const {
        static const int32_t GRAPHIC_DESCRIPTION_POS = (1 << 20);
        return ((this->m_ul_reference_data & GRAPHIC_DESCRIPTION_POS) == GRAPHIC_DESCRIPTION_POS ? true : false);
    }
    bool rRecordTitle::HasAnchorVector() const {
        static const int32_t ANCHOR_VECTOR_POS = (1 << 11);
        return ((this->m_ul_reference_data & ANCHOR_VECTOR_POS) == ANCHOR_VECTOR_POS ? true : false);
    }
    bool rRecordTitle::HasSemantics() const {
        static const int32_t SEMANTICS_POS = (1 << 9);
        return ((this->m_ul_reference_data & SEMANTICS_POS) == SEMANTICS_POS ? true : false);
    }
    bool rRecordTitle::HasMetricText() const {
        static const int32_t METRIC_TEXT_POS = (1 << 19);
        return ((this->m_ul_reference_data & METRIC_TEXT_POS) == METRIC_TEXT_POS ? true : false);
    }
    short rRecordTitle::LocalizationType() const {
        static const short LOCALIZATION_TYPE_POS = (1 << 4) - 1;
        return (this->m_ul_reference_data & LOCALIZATION_TYPE_POS);
    }
    int32_t rRecordTitle::GetSemanticLength() const {
        int32_t l_semantics_length = this->m_l_length_record - this->m_l_length_metric - sizeof(rRecordTitle);
        return l_semantics_length;
    }

    void rRecord::Print() {
        this->m_title.Print();
        std::cout << std::endl << "\tСписок координат точек метрики: " << std::endl;
        for (int i = 0; i < this->m_title.m_l_count_metric_points; ++i) {
            std::cout << "\t\t" << i+1 << ": ";
            if (!this->m_title.Has3DPresentation()) {
                if (!this->m_title.HasDoubleMetricType() && !this->m_title.HasBigMetricSize()) {
                    rRectang_coord2D<short>* get_point = static_cast<rRectang_coord2D<short>*>(this->m_p_points);
                    get_point[i].Print();
                }
                else if (this->m_title.HasDoubleMetricType() && !this->m_title.HasBigMetricSize()) {
                    rRectang_coord2D<float>* get_point = static_cast<rRectang_coord2D<float>*>(this->m_p_points);
                    get_point[i].Print();
                }
                else if (!this->m_title.HasDoubleMetricType() && this->m_title.HasBigMetricSize()) {
                    rRectang_coord2D<int32_t>* get_point = static_cast<rRectang_coord2D<int32_t>*>(this->m_p_points);
                    get_point[i].Print();
                }
                else {
                    rRectang_coord2D<double>* get_point = static_cast<rRectang_coord2D<double>*>(this->m_p_points);
                    get_point[i].Print();
                }
            }
            else {
                // Обработка точек метрики в трёхмерных координатах (в рабочей карте таких нет)
            }
        }
        if (this->m_title.HasMetricText()) { // Если в метрике имеется подпись
            std::cout << std::endl << "\tИмеется текст подписи: " << this->m_p_str_text << std::endl;
        }

        if (this->m_title.m_as_metric_descriptor[0]) { // Если имеет подобъекты
            std::cout << std::endl << "\tОписание подобъектов: " << std::endl;
            for (int i = 0; i < this->m_title.m_as_metric_descriptor[0]; ++i) {} // То по идее их нужно вывести?
        }

        if (this->m_title.HasGraphicDescription()) {
            std::cout << std::endl << "\tГрафическое описание объекта имеется: " << std::endl;
        }
        else {
            std::cout << std::endl << "\tГрафическое описание объекта не имеется. " << std::endl;
        }

        if (this->m_title.HasAnchorVector()) {
            std::cout << std::endl << "\tОписание вектора привязки имеется: " << std::endl;
        }
        else {
            std::cout << std::endl << "\tОписание вектора привязки не имеется. " << std::endl;
        }

        if (this->m_title.HasSemantics()) {
            std::cout << std::endl << "\tОписание семантики имеется: " << std::endl;
        }
        else {
            std::cout << std::endl << "\tОписание семантики не имеется. " << std::endl;
        }
    }
}
