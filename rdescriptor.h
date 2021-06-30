#pragma once
#include <iostream>
namespace nSXFFile {
    struct rDescriptor {
        int32_t m_identifier_l;
        int32_t m_length_descriptor_l;
        char m_nomenclature_a_c[32];
        int32_t m_number_records_l;
        uint32_t m_information_flags_l;
        short m_object_classifier_s;
        short m_semantics_classifier_s;
        void Print();
    };
}
