/*
AsciiPointBase is an abstract class holding the functionality for the
SaveILLCosmosAscii and SaveANSTOAscii export-only Acii-based save formats. It is
based on a python script by Maximilian Skoda, written for the ISIS Reflectometry
GUI
*/
#include "MantidDataHandling/AsciiPointBase.h"
#include "MantidAPI/FileProperty.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidKernel/ListValidator.h"

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include <boost/make_shared.hpp>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>

namespace Mantid {
namespace DataHandling {
using namespace Kernel;
using namespace API;

/// Initialisation method.
void AsciiPointBase::init() {
  declareProperty(
      make_unique<WorkspaceProperty<>>("InputWorkspace", "", Direction::Input),
      "The name of the workspace containing the data you want to save.");
  declareProperty(Kernel::make_unique<FileProperty>("Filename", "",
                                                    FileProperty::Save, ext()),
                  "The filename of the output file.");
  extraProps();
}

/**
*   Executes the algorithm. In this case it provides the process for any child
* classes as this class is abstract
*/
void AsciiPointBase::exec() {
  std::string filename = getProperty("Filename");
  g_log.information("FILENAME: " + filename);
  if (this->existsProperty("Separator")) {
    const std::string sepOption = getProperty("Separator");
    if (sepOption == "comma") {
      m_sep = ',';
    } else if (sepOption == "space") {
      m_sep = ' ';
    }
  }
  std::ofstream file(filename.c_str());
  if (!file) {
    g_log.error("Unable to create file: " + filename);
    throw Exception::FileError("Unable to create file: ", filename);
  }
  MatrixWorkspace_const_sptr ws = getProperty("InputWorkspace");
  if (!ws)
    throw std::runtime_error("Cannot treat InputWorkspace");
  m_length = ws->y(0).size();
  extraHeaders(file);
  data(file);
}

/** virtual method to add information to the file before the data
 *  @param file :: pointer to output file stream
 *  @param exportDeltaQ :: bool on whether deltaQ column to be printed
 */
void AsciiPointBase::data(std::ofstream &file, bool exportDeltaQ) {
  MatrixWorkspace_const_sptr ws = getProperty("InputWorkspace");
  try {
    file << std::scientific;
    // file << std::setprecision(std::numeric_limits<long double>::digits10 +
    // 1);
    const auto points = ws->points(0);
    const auto &yData = ws->y(0);
    const auto &eData = ws->e(0);
    for (size_t i = 0; i < m_length; ++i) {
      outputval(points[i], file, leadingSep());
      outputval(yData[i], file);
      outputval(eData[i], file);
      if (exportDeltaQ) {
        if (ws->hasDx(0))
          outputval(ws->dx(0)[i], file);
        else
          outputval(0., file);
      }
      file << '\n';
    }
  } catch (std::range_error) {
    g_log.error("InputWorkspace does not contain data");
  }
}

/** writes a properly formatted line of data
 *  @param val :: the double value to be written
 *  @param file :: pointer to output file stream
 *  @param leadingSep :: boolean to determine if there should be a separator
 * before this value, default true
 */
void AsciiPointBase::outputval(double val, std::ofstream &file,
                               bool leadingSep) {
  bool nancheck = std::isnan(val);
  bool infcheck = std::isinf(val);
  if (leadingSep)
    file << m_sep;
  if (!nancheck && !infcheck)
    file << val;
  else if (infcheck)
    file << "inf";
  else
    file << "nan";
}

/// appends the separator property to the algorithm
void AsciiPointBase::appendSeparatorProperty() {
  std::vector<std::string> propOptions;
  propOptions.push_back("comma");
  propOptions.push_back("space");
  propOptions.push_back("tab");
  declareProperty("Separator", "tab",
                  boost::make_shared<StringListValidator>(propOptions),
                  "The separator used for splitting data columns.");
}
} // namespace DataHandling
} // namespace Mantid
