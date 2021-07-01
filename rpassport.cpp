#include "functions.h"
#include "rpassport.h"
#include <iostream>

namespace nSXFFile {
    void rPassport::Print() {
        std::cout << "Паспортные данные файла: " << std::endl;
        std::cout << "\tИдентификатор начала файла: " << std::hex << this->m_identifier_l << std::endl;
        std::cout << "\tДлина записи паспорта: " << std::dec << this->m_length_record_l << std::endl;
        std::cout << "\tРедакция формата: " << std::hex << this->m_edition_l << std::endl;
        std::cout << "\tКонтрольная сумма: " << std::dec << this->m_checksum_l << std::endl;
        std::cout << "\tДата создания набора данных: " << this->m_date_creation_a_c << std::endl;
        std::cout << "\tНоменклатура листа: " << this->m_nomenclature_a_c << std::endl;
        std::cout << "\tМасштаб листа: " << std::dec << this->m_scale_l << std::endl;
        std::cout << "\tУсловное название листа: " << this->m_map_name_a_c << std::endl;
        std::cout << "\tИнформационные флажки: "; InformationFlagsPrint(this->m_information_flags_l);
        std::cout << "\tКод классификатора: " << std::hex << this->m_classifier_code_l << std::endl;

        std::cout << "\tПрямоугольные координаты углов листа: " << std::endl;
        std::cout << "\t\tЮго-западный угол: ";  this->m_rectang_coords.m_southwest.Print();
        std::cout << "\t\tСеверо-западный угол: "; this->m_rectang_coords.m_northwest.Print();
        std::cout << "\t\tСеверо-восточный угол: "; this->m_rectang_coords.m_northeast.Print();
        std::cout << "\t\tЮго-восточный угол: "; this->m_rectang_coords.m_southeast.Print();

        std::cout << "\tГеодезические координаты углов листа: " << std::endl;
        std::cout << "\t\tЮго-западный угол: ";  this->m_geo_coords.m_southwest.Print();
        std::cout << "\t\tСеверо-западный угол: "; this->m_geo_coords.m_northwest.Print();
        std::cout << "\t\tСеверо-восточный угол: "; this->m_geo_coords.m_northeast.Print();
        std::cout << "\t\tЮго-восточный угол: "; this->m_geo_coords.m_southeast.Print();

        std::cout << "\tМатематическая основа листа: ";
        for (int i = 0; i < 8; ++i) {
            std::cout << (int)this->m_mathematical_basis_a_c[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "\tСправочные материалы по исходному материалу: ?????? " << std::endl;
        std::cout << "\tРезерв: " << this->m_reserve_d << std::endl;
        std::cout << "\tРазрешающая способность прибора: " << this->m_instrument_resolution_l << std::endl;
        std::cout << "\tРасположение рамки на приборе:\n";
        std::cout << "\t\tЮго-западный угол: ";  this->m_frame_coordinates.m_southwest.Print();
        std::cout << "\t\tСеверо-западный угол: "; this->m_frame_coordinates.m_northwest.Print();
        std::cout << "\t\tСеверо-восточный угол: "; this->m_frame_coordinates.m_northeast.Print();
        std::cout << "\t\tЮго-восточный угол: "; this->m_frame_coordinates.m_southeast.Print();

        std::cout << "\tКлассификационный код рамки объекта: " << this->m_frame_code_l << std::endl;

        std::cout << "\tСправочные данные по проекции исходного материала: ?????? " << std::endl;

        std::cout << "Итого считано: " << sizeof(rPassport) << " байт." << std::endl;

    }
}
