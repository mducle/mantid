// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2015 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "MantidAPI/Workspace.h"

#include <boost/python/object_fwd.hpp>

namespace Mantid {
namespace PythonInterface {

struct DLLExport ExtractWorkspace {
  ExtractWorkspace(const boost::python::object &pyvalue);
  bool check() const;
  const API::Workspace_sptr operator()() const;

private:
  API::Workspace_sptr m_value;
};
} // namespace PythonInterface
} // namespace Mantid
