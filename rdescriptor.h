#pragma once
#include <iostream>
namespace nSXFFile {
    struct rDescriptor {
        int32_t m_l_identifier;
        int32_t m_l_length_descriptor;
        char m_ac_nomenclature[32];
        int32_t m_l_number_records;
        uint32_t m_ul_information_flags;
        short m_s_object_classifier;
        short m_s_semantics_classifier;
        void Print();
    };
}
