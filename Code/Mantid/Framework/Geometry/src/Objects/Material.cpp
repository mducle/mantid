//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "MantidGeometry/Objects/Material.h"

namespace Mantid
{

  namespace Geometry
  {

    using PhysicalConstants::NeutronAtom;

    /**
     * Construct an "empty" material. Everything returns zero
     */
    Material::Material() : 
      m_name(), m_element(0,0,0.0,0.0,0.0,0.0,0.0,0.0),
      m_numberDensity(0.0), m_temperature(0.0), m_pressure(0.0)
    {
    }

    /**
    * Construct a material object
    * @param name :: The name of the material
    * @param element :: The element it is composed from
    * @param numberDensity :: Density in A^-3
    * @param temperature :: The temperature in Kelvin (Default = 300K)
    * @param pressure :: Pressure in kPa (Default: 101.325 kPa)
    */
    Material::Material(const std::string & name, const NeutronAtom element, 
		       const double numberDensity, const double temperature, 
		       const double pressure) :
      m_name(name), m_element(element), m_numberDensity(numberDensity), 
      m_temperature(temperature), m_pressure(pressure)
    {
    }   
    
    /** 
     * Returns the name 
     * @returns A string containing the name of the material
     */
    const std::string & Material::name() const 
    { 
      return m_name; 
    }

    /** 
     * Get the number density
     * @returns The number density of the material in A^-3
     */
    double Material::numberDensity() const
    { 
      return m_numberDensity; 
    }
    
    /** 
     * Get the temperature
     * @returns The temperature of the material in Kelvin
     */
    double Material::temperature() const 
    { 
      return m_temperature; 
    }
    
    /** 
     * Get the pressure
     * @returns The pressure of the material
     */
    double Material::pressure() const
    { 
      return m_pressure; 
    }

    /**
     * Get the coherent scattering cross section for a given wavelength.
     * CURRENTLY this simply returns the value for the underlying element
     * @param lambda :: The wavelength to evaluate the cross section
     * @returns The value of the coherent scattering cross section at 
     * the given wavelength
     */ 
    double Material::cohScatterXSection(const double lambda) const
    {
      (void)lambda;
      return m_element.coh_scatt_xs;
      
    }
    
    /**
     * Get the incoherent scattering cross section for a given wavelength
     * CURRENTLY this simply returns the value for the underlying element
     * @param lambda :: The wavelength to evaluate the cross section
     * @returns The value of the coherent scattering cross section at 
     * the given wavelength
     */
    double Material::incohScatterXSection(const double lambda) const
    {
      (void)lambda;
      return m_element.inc_scatt_xs;
    }

    /**
     * Get the total scattering cross section for a given wavelength
     * CURRENTLY this simply returns the value for sum of the incoherent
     * and coherent scattering cross sections.
     * @param lambda :: The wavelength to evaluate the cross section
     * @returns The value of the total scattering cross section at 
     * the given wavelength
     */
    double Material::totalScatterXSection(const double lambda) const
    {
      return (cohScatterXSection(lambda) + incohScatterXSection(lambda));
    }

    /**
     * Get the absorption cross section for a given wavelength.
     * CURRENTLY This assumes a linear dependence on the wavelength with the reference
     * wavelegnth = 1.7982 angstroms.
     * @param lambda :: The wavelength to evaluate the cross section
     * @returns The value of the absoprtioncross section at 
     * the given wavelength
     */
    double Material::absorbXSection(const double lambda) const
    {
      return (m_element.abs_scatt_xs) * (lambda / NeutronAtom::ReferenceLambda);
    }


    /** Save the object to an open NeXus file.
     * @param file :: open NeXus file
     * @param group :: name of the group to create
     */
    void Material::saveNexus(::NeXus::File * file, const std::string & group) const
    {
      file->makeGroup(group, "NXsample", 1);
      file->putAttr("version", 1);
      file->putAttr("name", m_name);
      file->writeData("element_Z", m_element.z_number);
      file->writeData("element_A", m_element.a_number);
      file->writeData("number_density", m_numberDensity);
      file->writeData("temperature", m_temperature);
      file->writeData("pressure", m_pressure);
      file->closeGroup();
    }

    /** Load the object from an open NeXus file.
     * @param file :: open NeXus file
     * @param group :: name of the group to open
     */
    void Material::loadNexus(::NeXus::File * file, const std::string & group)
    {
      file->openGroup(group, "NXsample");
      file->getAttr("name", m_name);

      // Find the element
      uint16_t element_Z, element_A;
      file->readData("element_Z", element_Z);
      file->readData("element_A", element_A);
      m_element = Mantid::PhysicalConstants::getNeutronAtom(element_Z, element_A);

      file->readData("number_density", m_numberDensity);
      file->readData("temperature", m_temperature);
      file->readData("pressure", m_pressure);
      file->closeGroup();
    }

  } 

}  
