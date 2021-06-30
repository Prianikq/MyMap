#include "rsxffile.h"
#include <cstring>
#include <iostream>
#include <fstream>

namespace nSXFFile {
    const int TEST1 = 0; // УДАЛИТЬ
    const int TEST2 = 8000;
    void rSXFFile::Print() {
        this->m_passport.Print();
        this->m_descriptor.Print();
        //this->m_records[TEST - 1].Print();
        for (int i = TEST1; i < TEST2; ++i) { // ПОМЕНЯТЬ РАЗМЕР
            if (this->m_records[i].m_title.m_object_number_l == 16777794) { // ВНИМАНИЕ!
                std::cout << std::endl << "Информация о считанной записи №" << i + 1 << ": " << std::endl;
                this->m_records[i].Print();
            }

        }
    }
    void rSXFFile::Read(const char* p_str_filename) {
        std::ifstream in(p_str_filename, std::ios::binary);
        /*in.read((char*)&this->m_passport.m_identifier_l, sizeof(this->m_passport.m_identifier_l));
        std::cout << std::hex << this->m_passport.m_identifier_l << std::endl;*/
        in.read((char*)&this->m_passport, sizeof(this->m_passport));
        in.read((char*)&this->m_descriptor, sizeof(this->m_descriptor));
        this->m_records.resize(this->m_descriptor.m_number_records_l);
        for (int i = 0; i < this->m_descriptor.m_number_records_l; ++i) { // ПОМЕНЯТЬ РАЗМЕР
            in.read((char*)&this->m_records[i].m_title, sizeof(this->m_records[i].m_title));
            this->m_records[i].m_title.m_count_metric_points = this->m_records[i].m_title.m_metric_descriptor_a_s[1]; // Перенести значение в основное поле (см. структуру заголовка)
            long s_sz = 0;
            /*Считывание метрики записи */
            if (!this->m_records[i].m_title.Has3DPresentation()) {
                if (!this->m_records[i].m_title.HasDoubleMetricType() && !this->m_records[i].m_title.HasBigMetricSize()) {
                    this->m_records[i].m_points = new rRectang_coord2D<short>[this->m_records[i].m_title.m_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<short>);
                }
                else if (this->m_records[i].m_title.HasDoubleMetricType() && !this->m_records[i].m_title.HasBigMetricSize()) {
                    this->m_records[i].m_points = new rRectang_coord2D<float>[this->m_records[i].m_title.m_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<float>);
                }
                else if (!this->m_records[i].m_title.HasDoubleMetricType() && this->m_records[i].m_title.HasBigMetricSize()) {
                    this->m_records[i].m_points = new rRectang_coord2D<long>[this->m_records[i].m_title.m_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<long>);
                }
                else {
                    this->m_records[i].m_points = new rRectang_coord2D<double>[this->m_records[i].m_title.m_count_metric_points];
                    s_sz = sizeof(rRectang_coord2D<double>);
                }
            }
            else {
                // Для 3D представления?
            }
            in.read((char*)this->m_records[i].m_points, (std::streamsize)s_sz * this->m_records[i].m_title.m_count_metric_points);

            if (this->m_records[i].m_title.HasMetricText()) { // Если имеется метрика с текстом
                unsigned char us_text_length;
                in.read((char*)&us_text_length, sizeof(us_text_length));
                this->m_records[i].m_str_text = new char[us_text_length + 1]; // +1 т.к. последний нулевой символ не входит в длину текста
                in.read(this->m_records[i].m_str_text, (std::streamsize)us_text_length + 1);
            }

            /*Считывание подобъектов (если имеются) */
            if (this->m_records[i].m_title.m_metric_descriptor_a_s[0] != 0) {
                long l_length_ost = m_records[i].m_title.m_length_metric_l - s_sz * this->m_records[i].m_title.m_count_metric_points; // ВНИМАНИЕ! СЧИТЫВАЕТ ГРАФИЧЕСКОЕ ОПИСАНИЕ ОБЪЕКТА ТУДА ЖЕ
                this->m_records[i].m_subobjects = new char[l_length_ost];
                in.read(this->m_records[i].m_subobjects, l_length_ost);
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
                long l_semantics_length = this->m_records[i].m_title.m_length_record_l - this->m_records[i].m_title.m_length_metric_l - sizeof(rRecordTitle); // Вычисление длины семантики
                this->m_records[i].m_semantics = new char[l_semantics_length];
                in.read(this->m_records[i].m_semantics, l_semantics_length);
            }
        }
    }
}
