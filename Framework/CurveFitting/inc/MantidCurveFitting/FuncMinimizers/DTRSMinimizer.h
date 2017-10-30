#ifndef MANTID_CURVEFITTING_DTRSMINIMIZER_H_
#define MANTID_CURVEFITTING_DTRSMINIMIZER_H_

#include "MantidCurveFitting/FuncMinimizers/TrustRegionMinimizer.h"

namespace Mantid {
namespace CurveFitting {
namespace FuncMinimisers {

/** A GALAHAD trust region minimizer.

    Copyright &copy; 2009 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
    National Laboratory & European Spallation Source

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

    File change history is stored at: <https://github.com/mantidproject/mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport DTRSMinimizer : public TrustRegionMinimizer {
public:
  /// constructor and destructor
  DTRSMinimizer();
  /// Name of the minimizer.
  std::string name() const override;

private:
  void calculateStep(const DoubleFortranMatrix &J, const DoubleFortranVector &f,
                     const DoubleFortranMatrix &hf,
                     const DoubleFortranVector &g, double Delta,
                     DoubleFortranVector &d, double &normd,
                     const NLLS::nlls_options &options) override;

  void solveDtrs(const DoubleFortranMatrix &J, const DoubleFortranVector &f,
                 const DoubleFortranMatrix &hf, double Delta,
                 DoubleFortranVector &d, double &normd,
                 const NLLS::nlls_options &options);

  // Used for calculating step
  DoubleFortranMatrix m_A, m_ev;
  DoubleFortranVector m_ew, m_v, m_v_trans, m_d_trans;
  NLLS::all_eig_symm_work m_all_eig_symm_ws;
  NLLS::apply_scaling_work m_apply_scaling_ws;
};

} // namespace FuncMinimisers
} // namespace CurveFitting
} // namespace Mantid

#endif /*MANTID_CURVEFITTING_DTRSMINIMIZER_H_*/
