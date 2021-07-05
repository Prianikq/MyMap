#pragma once
#include <vector>
#include <fstream>

namespace nSXFFile {
    enum eLocalType { LINEAR = 0, AREAL, POINT, SIGNATURE, VECTOR, SIGNATURE_TEMPLATE};
    struct rRecordTitle {
        int32_t m_identifier_l;
        int32_t m_length_record_l;
        int32_t m_length_metric_l;
        int32_t m_classification_code_l;
        int32_t m_object_number_l;
        uint32_t m_reference_data_l;
        int32_t m_count_metric_points;
        unsigned short m_metric_descriptor_a_s[2];
        void Print() const;
        bool HasBigMetricSize() const;
        bool HasDoubleMetricType() const;
        bool Has3DPresentation() const;
        bool HasGraphicDescription() const;
        bool HasAnchorVector() const;
        bool HasSemantics() const;
        bool HasMetricText() const;
        short LocalizationType() const;
    };
    struct rRecord {
        rRecordTitle m_title;
        void* m_points;
        char* m_str_text; // Текст подписи (если имеется)
        char* m_semantics; // Считывается целиком, в данной версии программы не обрабатывается
        char* m_subobjects; // Считывается целиком, в данной версии программы не обрабатывается
        void Print();
        rRecord();
        ~rRecord();
    };
}
