//=================================
// include guard
#ifndef __StereoLitographyIo_H_INCLUDE__
#define __StereoLitographyIo_H_INCLUDE__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <iostream> 
#include <string>
#include "TracerException.h"
#include "Tools/Tools.h"
#include "Tools/FileTools.h"
#include "Tools/StringTools.h"
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "Core/Frame.h"

//=================================
/* 
https://en.wikipedia.org/wiki/STL_%28file_format%29#Binary_STL

__Binary STL (STereoLithography)__

A binary STL file has an 80-character header (which is generally ignored, 
but should never begin with "solid" because that will lead most software to 
assume that this is an ASCII STL file[citation needed]). Following the header 
is a 4-byte unsigned integer indicating the number of triangular facets in the 
file. Following that is data describing each triangle in turn. The file simply 
ends after the last triangle.

Each triangle is described by twelve 32-bit floating-point numbers: three for 
the normal and then three for the X/Y/Z coordinate of each vertex – just as 
with the ASCII version of STL. After these follows a 2-byte ("short") unsigned 
integer that is the "attribute byte count" – in the standard format, this 
should be zero because most software does not understand anything else.[6]

Floating-point numbers are represented as IEEE floating-point numbers and are 
assumed to be little-endian, although this is not stated in documentation.
*/
namespace StereoLitographyIo {

	Frame* read(const std::string filename, const double scale = 1.0);
	// The STL file format has arbitrary units. Some files are stored in meters
	// others are in millimeters or inches. To correct for this, a scaling
	// factor can be used during construction.

//--------------------------------
// AUXILUARY
//--------------------------------

	struct Facet {
		Vector3D n;
		Vector3D a;
		Vector3D b;
		Vector3D c;	
	};

	class Io {
	public:

		class CanNotReadFile : public TracerException {
			using TracerException::TracerException;
		};

		class BadNormal : public TracerException {
			using TracerException::TracerException;
		};

		class CanNotReadAscii : public TracerException {
			using TracerException::TracerException;
		};
	};

//--------------------------------
// Facet2mctracerTriangle
//--------------------------------

	Frame* facets_2_mctracer_triangles(
		const std::vector<Facet> facets,
		const double scale
	);

//--------------------------------
// BINARY IO
//--------------------------------
	class BinaryIo :public Io {
	protected:
			
		const uint header_size_in_chars = 80;
		const uint triangle_size_in_32bit_floats = 12;
	public:

	};
//--------------------------------
// BINARY WRITER
//--------------------------------

	class BinaryWriter :public BinaryIo{

		std::ofstream file;
		std::string filename;
		std::vector<Facet> facets;
	public:

		void add_facets(const std::vector<Facet> additional_facets);

		void add_facet_normal_and_three_vertices(
			const Vector3D n,
			const Vector3D a,
			const Vector3D b,
			const Vector3D c
		);

		void write_to_file(const std::string filename);
	private:

		void assert_file_is_open()const;
		void assert_normal_is_actually_normalized(const Vector3D normal);
		void write_stl_header();
		void write_number_of_facets(const uint32_t i);
		void write_attribute_count(const uint16_t i);
		void write_vector(const Vector3D &vec);
		void write_facets();
		void write_facet(const Facet &facet);
	};
//--------------------------------
// BINARY READER
//--------------------------------
	
	class BinaryReader :public BinaryIo{

		const std::string filename;
		std::ifstream stl_file;
		std::string stl_header;

		std::vector<Facet> facets;

		uint current_triangle_number = 0;
		uint32_t total_number_of_facets;

	    // keep track of facets with bad "attribute byte count"
	    std::vector<uint> facets_with_bad_attribute_count;
	public:

	    BinaryReader(const std::string filename);
	    std::vector<Facet> get_facets()const;
	    std::string get_print()const;
	    std::string get_header()const;
	    uint get_number_of_facets()const;
	    std::string get_report()const;
	private:

		void read_header();
		void read_total_number_of_facets();
		void start();
		Facet read_and_create_next_facet();
		bool stl_header_implies_ascii_format()const;
		void assert_is_no_ascii_format()const;
		void assert_normal_is_actually_normalized(const Vector3D nomral);
		void assert_file_is_open()const;
		void read_facets();
		std::vector<float> read_floats(const uint n);
		std::string read_chars(const uint n);
		uint32_t read_single_uint32();
		uint16_t read_single_uint16();
		void check_attribute_byte_count_is_zero(
			const uint16_t attribute_byte_count
		); 
	};
} // StereoLitographyIo
#endif // __StereoLitographyIo_H_INCLUDE__ 
