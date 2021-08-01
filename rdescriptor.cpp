#include "rdescriptor.h"
#include "functions.h"
#include <iostream>

namespace nSXFFile {
    void rDescriptor::Print() {
        std::cout << std::endl << "Дескриптор данных файла: " << std::endl;
        std::cout << "\tИдентификатор данных: " << std::hex << this->m_l_identifier << std::endl;
        std::cout << "\tДлина дексриптора: " << std::dec << this->m_l_length_descriptor << std::endl;
        std::cout << "\tНоменклатура листа: " << this->m_ac_nomenclature << std::endl;
        std::cout << "\tЧисло записей данных: " << std::dec << this->m_l_number_records << std::endl;
        std::cout << "\tИнформационные флажки: "; InformationFlagsPrint(this->m_ul_information_flags);
        std::cout << "\tНомер классификатора: " << std::dec << this->m_s_object_classifier << " " << this->m_s_semantics_classifier << std::endl;
        std::cout << "Итого считано: " << sizeof(rDescriptor) << " байт." << std::endl;
    }
}
