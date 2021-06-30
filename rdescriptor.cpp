#include "rdescriptor.h"
#include "functions.h"
#include <iostream>

namespace nSXFFile {
    void rDescriptor::Print() {
        std::cout << std::endl << "Дескриптор данных файла: " << std::endl;
        std::cout << "\tИдентификатор данных: " << std::hex << this->m_identifier_l << std::endl;
        std::cout << "\tДлина дексриптора: " << std::dec << this->m_length_descriptor_l << std::endl;
        std::cout << "\tНоменклатура листа: " << this->m_nomenclature_a_c << std::endl;
        std::cout << "\tЧисло записей данных: " << std::dec << this->m_number_records_l << std::endl;
        std::cout << "\tИнформационные флажки: "; InformationFlagsPrint(this->m_information_flags_l);
        std::cout << "\tНомер классификатора: " << std::dec << this->m_object_classifier_s << " " << this->m_semantics_classifier_s << std::endl;
        std::cout << "Итого считано: " << sizeof(rDescriptor) << " байт." << std::endl;
    }
}
