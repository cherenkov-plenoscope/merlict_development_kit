//=================================
// include guard
#ifndef __SceneryFactory_H_INCLUDED__
#define __SceneryFactory_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "Xml/Xml.h"
#include "Tools/PathTools.h"
#include "Core/Frame.h"
#include "PhotonSensor/PhotonSensor.h"
#include "TelescopeArrayControl.h"
#include "Plenoscope/PlenoscopeInScenery.h"
#include "Scenery/Scenery.h"
using std::string;
using std::vector;

//------------------------------------------------------------------------------
namespace Xml {

    class SceneryFactory {
    public:
        vector<PhotonSensor::Sensor*>* raw_sensors;
        TelescopeArrayControl* telescopes;
        vector<Plenoscope::PlenoscopeInScenery> plenoscopes;

        string author;
        string comment;

        PathTools::Path xml_path;
        Document xml_doc;

        Scenery* scenery;

        SceneryFactory(const string path);
        PhotonSensor::Sensors sensors()const;
        void append_to_frame_in_scenery(Frame* frame, Scenery* scenery);
    private:

        void make_geometry(Frame* mother, const Node node);
        Frame* add_Frame(Frame* mother, const Node node);
        Frame* add_Disc(Frame* mother, const Node node);
        Frame* add_Sphere(Frame* mother, const Node node);
        Frame* add_Plane(Frame* mother, const Node node);
        Frame* add_HexPlane(Frame* mother, const Node node);
        Frame* add_Cylinder(Frame* mother, const Node node);
        Frame* add_Annulus(Frame* mother, const Node node);
        Frame* add_BiConvexLensHex(Frame* mother, const Node node);
        Frame* add_SphereCapWithHexagonalBound(Frame* mother, const Node node);
        Frame* add_SphereCapWithRectangularBound(Frame* mother, const Node node);
        Frame* add_Triangle(Frame* mother, const Node node);
        Frame* add_SegmentedReflector(Frame* mother, const Node node);
        Frame* add_STL(Frame* mother, const Node node);
        Frame* add_light_field_sensor(Frame* mother, const Node node);
        Frame* add_light_field_sensor_demonstration(Frame* mother, const Node node);

        const Color* surface_color(const Node node)const;
        const Function::Func1D* surface_refl(const Node node)const;
        const Function::Func1D* surface_refrac(const Node node)const;
        void add_to_sensors_if_sensitive(Frame* frame, const Node node);
        void add_to_array_if_telescope(Frame* frame, const Node node);

        void add_function(const Node node);
        void add_color(const Node node);
    };
}//Xml
#endif // __SceneryFactory_H_INCLUDED__ 