#ifndef DATAHANDING_SAVEFOCUSEDXYE_H_
#define DATAHANDING_SAVEFOCUSEDXYE_H_

//---------------------------------------------------
// Includes
//---------------------------------------------------
#include "MantidAPI/Algorithm.h"

namespace Mantid
{
namespace DataHandling
{
/**
     Saves a focused data set (usually output of a diffraction focusing routine but not exclusively)
     into a three column format containing X_i, Y_i, and E_i.
     For data where the focusing routine has generated several spectra (for example, multi-bank instruments),
     the option is provided for saving all spectra into a single file, separated by headers, or into
     several files that will be named "workspaceName_"+spectra_number.

     Required properties:
     <UL>
     <LI> InputWorkspace - The workspace name to save. </LI>
     <LI> Filename - The filename for output. </LI>
     </UL>

     Optional properties:
     <UL>
     <LI> SplitFiles    - Split into N files for workspace with N-spectra (default: true).</LI>
     <LI> Append        - Append to Filename, if it already exists (default: false).</LI>
     <LI> IncludeHeader - Include header (comment) lines in output (default: true).</LI>
     </UL>

     @author Laurent Chapon, ISIS Facility, Rutherford Appleton Laboratory
     @date 04/03/2009

     Copyright &copy; 2009-2010 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

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
class DLLExport SaveFocusedXYE : public API::Algorithm
{
public:
  /// (Empty) Constructor
  SaveFocusedXYE() : API::Algorithm(){}
  /// Virtual destructor
  virtual ~SaveFocusedXYE() {}
  /// Algorithm's name
  virtual const std::string name() const { return "SaveFocusedXYE"; }
  /// Algorithm's version
  virtual int version() const { return (1); }
  /// Algorithm's category for identification
  virtual const std::string category() const { return "Diffraction;DataHandling\\Text"; }

private:
  /// Sets documentation strings for this algorithm
  virtual void initDocs();
  /// Initialisation code
  void init();
  /// Execution code
  void exec();
  /// Write the header information
  void writeHeaders(std::ostream& os,API::MatrixWorkspace_const_sptr& workspace) const;
  /// sets non workspace properties for the algorithm
  void setOtherProperties(IAlgorithm* alg,const std::string & propertyName,const std::string &propertyValue,int perioidNum);
};

}
}
#endif //DATAHANDING_SAVEFOCUSEDXYE_H_
