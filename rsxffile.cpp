#include "rsxffile.h"
#include <cstring>
#include <iostream>
#include <fstream>

namespace nSXFFile {
    void rSXFFile::Print() {
        this->m_passport.Print();
        this->m_descriptor.Print();
        for (int i = this->m_descriptor.m_l_number_records; i < this->m_descriptor.m_l_number_records+1; ++i) { // ПОМЕНЯТЬ РАЗМЕР
            if (1) { // ВНИМАНИЕ!
                std::cout << std::endl << "Информация о считанной записи №" << i + 1 << ": " << std::endl;
                this->m_records[this->m_descriptor.m_l_number_records-1].Print();
            }

        }
    }
    void rSXFFile::Read(const char* p_pc_filename) {
        std::ifstream in(p_pc_filename, std::ios::binary);
        in.read((char*)&this->m_passport, sizeof(this->m_passport));
        in.read((char*)&this->m_descriptor, sizeof(this->m_descriptor));
        this->m_records.resize(this->m_descriptor.m_l_number_records);
        for (int i = 0; i < this->m_descriptor.m_l_number_records; ++i) {
            in.read((char*)&this->m_records[i].m_title, sizeof(this->m_records[i].m_title));
            this->m_records[i].m_title.m_l_count_metric_points = this->m_records[i].m_title.m_as_metric_descriptor[1]; // Перенести значение в основное поле (см. структуру заголовка)
            int32_t s_sz = 0;
            /*Считывание метрики записи */
            if (!this->m_records[i].m_title.Has3DPresentation()) {
                if (!this->m_records[i].m_title.HasDoubleMetricType() && !this->m_records[i].m_title.HasBigMetricSize()) {
                    this->m_records[i].m_p_points = new rRectang_coord2D<short>[this->m_records[i].m_title.m_l_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<short>);
                }
                else if (this->m_records[i].m_title.HasDoubleMetricType() && !this->m_records[i].m_title.HasBigMetricSize()) {
                    this->m_records[i].m_p_points = new rRectang_coord2D<float>[this->m_records[i].m_title.m_l_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<float>);
                }
                else if (!this->m_records[i].m_title.HasDoubleMetricType() && this->m_records[i].m_title.HasBigMetricSize()) {
                    this->m_records[i].m_p_points = new rRectang_coord2D<long>[this->m_records[i].m_title.m_l_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<long>);
                }
                else {
                    this->m_records[i].m_p_points = new rRectang_coord2D<double>[this->m_records[i].m_title.m_l_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<double>);
                }
            }
            else {
                // Для 3D представления?
            }
            in.read((char*)this->m_records[i].m_p_points, (std::streamsize)s_sz * this->m_records[i].m_title.m_l_count_metric_points);

            if (this->m_records[i].m_title.HasMetricText()) { // Если имеется метрика с текстом
                unsigned char us_text_length;
                in.read((char*)&us_text_length, sizeof(us_text_length));
                this->m_records[i].m_p_str_text = new char[us_text_length + 1]; // +1 т.к. последний нулевой символ не входит в длину текста
                in.read(this->m_records[i].m_p_str_text, (std::streamsize)us_text_length + 1);
            }

            /*Считывание подобъектов (если имеются) */
            if (this->m_records[i].m_title.m_as_metric_descriptor[0] != 0) {
                int32_t l_length_ost = m_records[i].m_title.m_l_length_metric - s_sz * this->m_records[i].m_title.m_l_count_metric_points; // ВНИМАНИЕ! СЧИТЫВАЕТ ГРАФИЧЕСКОЕ ОПИСАНИЕ ОБЪЕКТА ТУДА ЖЕ
                this->m_records[i].m_p_subobjects = new char[l_length_ost];
                in.read(this->m_records[i].m_p_subobjects, l_length_ost);
            }

            /*Считывание графического описания объекта (если имеется) */
            if (this->m_records[i].m_title.HasGraphicDescription()) {
                std::cout << "ВНИМАНИЕ! ИМЕЕТ ГРАФ. ОПИСАНИЕ, КОТОРОЕ ЕЩЕ НЕ НАПИСАНО!" << std::endl;
            }

            /*Считывание описания вектора привязки 3D-модели объекта (если имеется) */
            if (this->m_records[i].m_title.HasAnchorVector()) {
                std::cout << "ВНИМАНИЕ! ИМЕЕТ ОПИСАНИЕ ВЕКТОРА ПРИВЯЗКИ, КОТОРОЕ ЕЩЕ НЕ НАПИСАНО!" << std::endl;
            }

            /*Считывание семантики объекта (если имеется) */
            if (this->m_records[i].m_title.HasSemantics()) {
                int32_t l_pointer = 0;
                int32_t l_semantic_length = m_records[i].m_title.GetSemanticLength();
                while (l_pointer != l_semantic_length) {
                    this->m_records[i].m_semantics.push_back(rSemantic());
                    in.read((char*)&this->m_records[i].m_semantics.back().m_s_semantic_code, sizeof(rSemantic::m_s_semantic_code));
                    in.read((char*)&this->m_records[i].m_semantics.back().m_type, sizeof(char)); // ??
                    in.read((char*)&this->m_records[i].m_semantics.back().m_c_scale_factor, sizeof(rSemantic::m_c_scale_factor));
                    l_pointer += sizeof(rSemantic::m_s_semantic_code) + sizeof(char) + sizeof(rSemantic::m_c_scale_factor);
                    if (this->m_records[i].m_semantics.back().m_type == eSemanticDataType::CHAR) {
                        in.read((char*)&this->m_records[i].m_semantics.back().m_data.m_c_char, sizeof(rData::m_c_char));
                        l_pointer += sizeof(rData::m_c_char);
                    }
                    else if (this->m_records[i].m_semantics.back().m_type == eSemanticDataType::SHORT) {
                        in.read((char*)&this->m_records[i].m_semantics.back().m_data.m_s_short, sizeof(rData::m_s_short));
                        l_pointer += sizeof(rData::m_s_short);
                    }
                    else if (this->m_records[i].m_semantics.back().m_type == eSemanticDataType::LONG) {
                        in.read((char*)&this->m_records[i].m_semantics.back().m_data.m_l_long, sizeof(rData::m_l_long));
                        l_pointer += sizeof(rData::m_l_long);
                    }
                    else if (this->m_records[i].m_semantics.back().m_type == eSemanticDataType::DOUBLE) {
                        in.read((char*)&this->m_records[i].m_semantics.back().m_data.m_d_double, sizeof(rData::m_d_double));
                        l_pointer += sizeof(rData::m_d_double);
                    }
                    else if (this->m_records[i].m_semantics.back().m_type == eSemanticDataType::ANSI) {
                        in.read(this->m_records[i].m_semantics.back().m_data.m_ac_ansi, this->m_records[i].m_semantics.back().m_c_scale_factor+1);
                        l_pointer += this->m_records[i].m_semantics.back().m_c_scale_factor+1;
                    }
                    else if(this->m_records[i].m_semantics.back().m_type == eSemanticDataType::ASCIIZ) {
                        in.read(this->m_records[i].m_semantics.back().m_data.m_ac_ansi, this->m_records[i].m_semantics.back().m_c_scale_factor+1);
                        l_pointer += this->m_records[i].m_semantics.back().m_c_scale_factor+1;
                    }
                }
            }
        }
    }
}
