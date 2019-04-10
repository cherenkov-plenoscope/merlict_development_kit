// Copyright 2014 Sebastian A. Mueller
#include "merlict_visual/PortablePixMap.h"
#include <stdint.h>
#include <string>
#include <sstream>
#include "merlict/txt.h"
#include "merlict/binio.h"

namespace merlict {
namespace visual {

void append_image_to_file(const Image &img, std::ostream &fout) {
    std::stringstream head;
    head << "P6\n";
    head << "# CREATOR: MERLICT\n";
    head << int(img.num_cols) << " ";
    head << int(img.num_rows) << "\n";
    head << "255\n";
    fout << head.str();
    for (unsigned int row = 0; row < img.num_rows; row++) {
        for (unsigned int col = 0; col < img.num_cols; col++) {
            const Color c = img.at_col_row(col, row);
            append_uint8(c.r, fout);
            append_uint8(c.g, fout);
            append_uint8(c.b, fout);
        }
    }
    return;
}

Image read_image_from_file(std::istream &fin) {
    std::string magic;
    std::getline(fin, magic);
    std::string comment;
    std::getline(fin, comment);
    std::string resolution;
    std::getline(fin, resolution);

    if (!txt::is_equal(magic, "P6")) {
        std::stringstream info;
        info << "PortablePixMap::" << __func__ << "()\n";
        info << "Expected magic file-descriptor to be 'P6', ";
        info << "but actually it is '" << magic << "'\n";
        throw std::runtime_error(info.str());
    }

    const int num_columns = txt::to_int(
        txt::cut_leading_token_infront_of_delimiter(&resolution, ' '));
    if (num_columns < 0) {
        std::stringstream info;
        info << "PortablePixMap::" << __func__ << "()\n";
        info << "Expected num_columns >= 0, but actually it is '";
        info << num_columns << "'\n";
        throw std::runtime_error(info.str());
    }

    const int num_rows = txt::to_int(
        txt::cut_leading_token_infront_of_delimiter(&resolution, ' '));
    if (num_rows < 0) {
        std::stringstream info;
        info << "PortablePixMap::" << __func__ << "()\n";
        info << "Expected num_rows >= 0, but actually it is '";
        info << num_rows << "'\n";
        throw std::runtime_error(info.str());
    }

    std::string color_depth;
    std::getline(fin, color_depth);
    const int max_depth = txt::to_int(color_depth);
    if (max_depth != 255) {
        std::stringstream info;
        info << "PortablePixMap::" << __func__ << "()\n";
        info << "Expected color-depth == 255, but actually it is '";
        info << max_depth << "'\n";
        throw std::runtime_error(info.str());
    }

    Image img(num_columns, num_rows);
    for (unsigned int row = 0; row < img.num_rows; row++) {
        for (unsigned int col = 0; col < img.num_cols; col++) {
            const float r = read_uint8(fin);
            const float g = read_uint8(fin);
            const float b = read_uint8(fin);
            img.set_col_row(col, row, Color(r, g, b));
        }
    }
    return img;
}

void write_image_to_path(const Image &image, const std::string path) {
    std::ofstream fout;
    fout.open(path.c_str(), std::ios::out | std::ios::binary);
    if (fout.is_open()) {
        append_image_to_file(image, fout);
        fout.close();
    } else {
        std::stringstream info;
        info << __FILE__ << " " << __LINE__ <<"\n";
        info << __func__ << "()\n";
        info << "Can not open " << path << "\n";
        throw std::runtime_error(info.str());
    }
}

Image read_image_from_path(const std::string path) {
    std::ifstream fin;
    fin.open(path.c_str(), std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        std::stringstream info;
        info << __FILE__ << " " << __LINE__ <<"\n";
        info << __func__ << "()\n";
        info << "Can not open " << path << "\n";
        throw std::runtime_error(info.str());
    } else {
        Image out = read_image_from_file(fin);
        fin.close();
        return out;
    }
}

}  // namespace visual
}  // namespace merlict
