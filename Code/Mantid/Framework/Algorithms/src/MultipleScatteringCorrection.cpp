//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "MantidAlgorithms/MultipleScatteringCorrection.h"
#include "MantidAlgorithms/MultipleScattering/MayersMSCorrection.h"
#include "MantidAPI/Progress.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/WorkspaceValidators.h"
#include "MantidKernel/CompositeValidator.h"

namespace Mantid {
namespace Algorithms {

using API::InstrumentValidator;
using API::MatrixWorkspace_sptr;
using API::Progress;
using API::SampleValidator;
using API::WorkspaceFactory;
using API::WorkspaceProperty;
namespace Exception = Kernel::Exception;
using Geometry::IDetector_const_sptr;
using Kernel::CompositeValidator;
using Kernel::Direction;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(MultipleScatteringCorrection)

//------------------------------------------------------------------------------
// Public members
//------------------------------------------------------------------------------

/**
 * Constructor
 */
MultipleScatteringCorrection::MultipleScatteringCorrection()
    : API::Algorithm() {}

/// Algorithms name for identification. @see Algorithm::name
const std::string MultipleScatteringCorrection::name() const {
  return "MultipleScatteringCorrection";
}

/// Algorithm's version for identification. @see Algorithm::version
int MultipleScatteringCorrection::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string MultipleScatteringCorrection::category() const {
  return "Corrections";
}

/// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
const std::string MultipleScatteringCorrection::summary() const {
  return "Corrects the input data for the effects of multiple scattering";
}

/** Initialize the algorithm's properties.
 */
void MultipleScatteringCorrection::init() {
  declareProperty(new WorkspaceProperty<>("InputWorkspace", "",
                                          Direction::Input,
                                          createInputWSValidator()),
                  "An input workspace.");
  declareProperty(
      new WorkspaceProperty<>("OutputWorkspace", "", Direction::Output),
      "An output workspace.");
}

/**
 */
void MultipleScatteringCorrection::exec() {
  MatrixWorkspace_sptr inputWS = getProperty("InputWorkspace");
  MatrixWorkspace_sptr outputWS = WorkspaceFactory::Instance().create(inputWS);

  // Invariant algorithm parameters
  MayersMSCorrection::Parameters baseParams;
  auto instrument = inputWS->getInstrument();
  const auto sourcePos = instrument->getSource()->getPos();
  const auto samplePos = instrument->getSample()->getPos();
  baseParams.l1 = samplePos.distance(sourcePos);
  g_log.warning("Ignoring out of place scattering");
  baseParams.phi = 0.0;

  const auto &sampleShape = inputWS->sample().getShape();
  // Current Object code computes quite an inaccurate bounding box so we do
  // something better for the time being
  const double big(100.); // seems to be a sweet spot...
  double minX(-big), maxX(big), minY(-big), maxY(big), minZ(-big), maxZ(big);
  sampleShape.getBoundingBox(maxX, maxY, maxZ, minX, minY, minZ);
  baseParams.cylRadius = 0.5*(maxX - minX);
  baseParams.cylHeight = (maxY - minY);

// reintroduce this when the general code is improved
//  const auto &bboxWidth = sampleShape.getBoundingBox().width();
//  baseParams.cylRadius = 0.5*bboxWidth[0];
//  baseParams.cylHeight = bboxWidth[1];

  g_log.warning("Assuming Y is up and X is horizontal");
  const auto &sampleMaterial = sampleShape.material();
  baseParams.rho = sampleMaterial.numberDensity();
  baseParams.sigmaAbs = sampleMaterial.absorbXSection();
  baseParams.sigmaSc = sampleMaterial.totalScatterXSection();

  const size_t nhist(inputWS->getNumberHistograms());
  Progress prog(this, 0., 1., nhist);
  prog.setNotifyStep(0.01);
  for (size_t i = 0; i < nhist; ++i) {
    // Copy the X values over
    const auto &inX = inputWS->readX(i);
    outputWS->dataX(i) = inX;
    IDetector_const_sptr det;
    try {
      det = inputWS->getDetector(i);
    } catch (Exception::NotFoundError &) {
      continue;
    }
    if(det->isMonitor() || det->isMasked()) continue;

    auto spectrumParams = baseParams;
    spectrumParams.l2 = det->getPos().distance(samplePos);
    spectrumParams.twoTheta = inputWS->detectorTwoTheta(det);
    MayersMSCorrection correction(spectrumParams, inX, inputWS->readY(i),
                                  inputWS->readE(i));
    correction.apply(outputWS->dataY(i), outputWS->dataE(i));
    prog.report();
  }

  setProperty("OutputWorkspace", outputWS);
}

//------------------------------------------------------------------------------
// Private members
//------------------------------------------------------------------------------
/**
 * @return The validator required for the input workspace
 */
Kernel::IValidator_sptr
MultipleScatteringCorrection::createInputWSValidator() const {
  auto validator = boost::make_shared<CompositeValidator>();

  unsigned int requires = (InstrumentValidator::SamplePosition |
                           InstrumentValidator::SourcePosition);
  validator->add<InstrumentValidator, unsigned int>(requires);

  requires = (SampleValidator::Shape | SampleValidator::Material);
  validator->add<SampleValidator, unsigned int>(requires);

  return validator;
}

} // namespace Algorithms
} // namespace Mantid
