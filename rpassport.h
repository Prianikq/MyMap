#pragma once
#include <iostream>
namespace nSXFFile {
    template<class T> struct rRectang_coord2D {
        T m_x;
        T m_y;
        void Print() {
             std::cout << "X = " << std::dec << this->m_x << "   Y = " << std::dec << this->m_y << std::endl;
        }
    };

    template<class T> struct rRectang_coord3D {
        T m_x;
        T m_y;
        T m_z;
        // void Print();
    };

    struct rGeo_coord {
        double m_b_d;
        double m_l_d;
        void Print() {
            std::cout << "B = " << this->m_b_d << "   L = " << this->m_l_d << std::endl;
        }
    };


    template<class T> struct rRectang_coord_of_corners {
        rRectang_coord2D<T> m_southwest;
        rRectang_coord2D<T> m_northwest;
        rRectang_coord2D<T> m_northeast;
        rRectang_coord2D<T> m_southeast;
    };
    struct rGeo_coord_of_corners {
        rGeo_coord m_southwest;
        rGeo_coord m_northwest;
        rGeo_coord m_northeast;
        rGeo_coord m_southeast;
    };

    struct rSource_material_ref {
        // ДОПИСАТЬ?????
    };

    struct rSource_material_proj {
        // ДОПИСАТЬ?????
    };

    struct rPassport {
        int32_t m_identifier_l;
        int32_t m_length_record_l;
        int32_t m_edition_l;
        int32_t m_checksum_l;
        char m_date_creation_a_c[12];
        char m_nomenclature_a_c[32];
        int32_t m_scale_l;
        char m_map_name_a_c[32];
        uint32_t m_information_flags_l;
        int32_t m_classifier_code_l;
        rRectang_coord_of_corners<double> m_rectang_coords;
        rGeo_coord_of_corners m_geo_coords;
        char m_mathematical_basis_a_c[8];
        char m_source_material_ref_a_c[64];
        double m_reserve_d;
        int32_t m_instrument_resolution_l;
        rRectang_coord_of_corners<int32_t> m_frame_coordinates;
        int32_t m_frame_code_l;
        char m_source_material_proj_a_c[48];
        void Print();
    };
}
