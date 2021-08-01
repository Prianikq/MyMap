#pragma once
#include <vector>
#include <fstream>

namespace nSXFFile {
    enum eLocalType { LINEAR = 0, AREAL, POINT, SIGNATURE, VECTOR, SIGNATURE_TEMPLATE};
    struct rRecordTitle {
        int32_t m_l_identifier;
        int32_t m_l_length_record;
        int32_t m_l_length_metric;
        int32_t m_l_classification_code;
        int32_t m_l_object_number;
        uint32_t m_ul_reference_data;
        int32_t m_l_count_metric_points;
        unsigned short m_as_metric_descriptor[2];
        void Print() const;
        bool HasBigMetricSize() const;
        bool HasDoubleMetricType() const;
        bool Has3DPresentation() const;
        bool HasGraphicDescription() const;
        bool HasAnchorVector() const;
        bool HasSemantics() const;
        bool HasMetricText() const;
        short LocalizationType() const;
        int32_t GetSemanticLength() const;
    };
    enum eSemanticDataType { ASCIIZ = 0, CHAR = 1, SHORT = 2, LONG = 4, DOUBLE = 8, ANSI = 126, UNICODE = 127 };
    union rData {
        static const short STRING_SIZE = 256;
        char m_c_char;
        short m_s_short;
        int32_t m_l_long;
        double m_d_double;
        char m_ac_ansi[STRING_SIZE];
    };
    struct rSemantic {
        eSemanticDataType m_type;
        rData m_data;
        char m_c_scale_factor;
        short m_s_semantic_code;
    };

    struct rRecord {
        rRecordTitle m_title;
        void* m_p_points;
        char* m_p_str_text; // Текст подписи (если имеется)
        std::vector<rSemantic> m_semantics;
        char* m_p_subobjects; // Считывается целиком, в данной версии программы не обрабатывается
        void Print();
        rRecord();
        ~rRecord();
    };
}
