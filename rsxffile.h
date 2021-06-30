#pragma once
#include "rdescriptor.h"
#include "rpassport.h"
#include "rrecord.h"
#include <vector>
namespace nSXFFile {
    struct rSXFFile {
        rPassport m_passport;
        rDescriptor m_descriptor;
        std::vector<rRecord> m_records;
        void Print();
        void Read(const char* p_str_filename);
    };
}
