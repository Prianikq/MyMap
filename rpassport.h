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
        double m_d_b;
        double m_d_l;
        void Print() {
            std::cout << "B = " << this->m_d_b << "   L = " << this->m_d_l << std::endl;
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
        int32_t m_l_identifier;
        int32_t m_l_length_record;
        int32_t m_l_edition;
        int32_t m_l_checksum;
        char m_ac_date_creation[12];
        char m_ac_nomenclature[32];
        int32_t m_l_scale;
        char m_ac_map_name[32];
        uint32_t m_ul_information_flags;
        int32_t m_l_classifier_code;
        rRectang_coord_of_corners<double> m_rectang_coords;
        rGeo_coord_of_corners m_geo_coords;
        char m_ac_mathematical_basis[8];
        char m_ac_source_material_ref[64];
        double m_d_reserve;
        int32_t m_l_instrument_resolution;
        rRectang_coord_of_corners<int32_t> m_frame_coordinates;
        int32_t m_l_frame_code;
        char m_ac_source_material_proj[48];
        void Print();
    };
}
