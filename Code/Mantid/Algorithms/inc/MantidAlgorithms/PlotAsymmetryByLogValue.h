#ifndef MANTID_ALGORITHM_PLOTASYMMETRYBULOGVALUE_H_
#define MANTID_ALGORITHM_PLOTASYMMETRYBULOGVALUE_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/Algorithm.h"

namespace Mantid
{
//----------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------
  namespace DataObjects
  {
      class Workspace2D;
  }

  namespace Algorithms
  {
    /**Takes a muon workspace as input and sums all the spectra into two spectra which represent
	  the two detector groupings. The resultant spectra are used to calculate (F-aB) / (F+aB) the results of which
	  are stored in the output workspace.

    Required Properties:
    <UL>
    <LI> InputWorkspace - The name of the Workspace2D to take as input </LI>
    <LI> OutputWorkspace - The name of the workspace in which to store the result </LI>
    <LI> ForwardSpectra - The detector number of the first group </LI>
    <LI> BackwardSpectra - The detector number of the second group </LI>
    <LI> Alpha - ?? </LI>
    </UL>


    @author
    @date 11/07/2008

    Copyright &copy; 2008 STFC Rutherford Appleton Laboratories

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
    Code Documentation is available at: <http://doxygen.mantidproject.org>
    */
    class DLLExport PlotAsymmetryByLogValue : public API::Algorithm
    {
    public:
      /// Default constructor
      PlotAsymmetryByLogValue() : API::Algorithm() {};
      /// Destructor
      virtual ~PlotAsymmetryByLogValue() {};
      /// Algorithm's name for identification overriding a virtual method
      virtual const std::string name() const { return "PlotAsymmetryByLogValue";}
      /// Algorithm's version for identification overriding a virtual method
      virtual const int version() const { return 1;}
      /// Algorithm's category for identification overriding a virtual method
      virtual const std::string category() const { return "Muon";}

    private:
      // Overridden Algorithm methods
      void init();
      void exec();

      /// Calculate the integral asymmetry for a workspace (single period)
      void calcIntAsymmetry(boost::shared_ptr<DataObjects::Workspace2D> ws, double& Y, double& E);

      /// Calculate the integral asymmetry for a workspace (red & green)
      void calcIntAsymmetry(boost::shared_ptr<DataObjects::Workspace2D> ws_red, 
                            boost::shared_ptr<DataObjects::Workspace2D> ws_geen,double& Y, double& E);
      /// Group detectors
      void groupDetectors(boost::shared_ptr<DataObjects::Workspace2D> ws,const std::vector<int>& spectraList);

      /// Stores property "Int"
      bool m_int;
      /// Store forward spectra
      std::vector<int> m_forward_list;
      /// Store backward spectra
	    std::vector<int> m_backward_list;
      /// If true call LoadMuonNexus with Autogroup on
      bool m_autogroup;

    };

  } // namespace Algorithm
} // namespace Mantid

#endif /*MANTID_ALGORITHM_PLOTASYMMETRYBULOGVALUE_H_*/
