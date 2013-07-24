/*WIKI* 

== Used Subalgorithms ==

The algorithm uses [[Unit_Factory|Unit Factory ]] and existing unit conversion procedures from input Workspace Units to the Units, necessary for transformation into correspondent MD Event workspace. It also uses [[PreprocessDetectorsToMD]] algorithm to help with transformation to reciprocal space.

== Notes ==
<ol>
<li> For elastic analysis (<math> dEAnalysisMode=Elastic</math>) the target [[units|unit]] is momentum <math>k</math>.
<li> For no analysis (CopyToMD) mode, the units remain the one, previously defined along the workspace's axes. 
<li> When units of input Matrix 2D workspace (Histogram workspace) are not Momentums for Elastic or EnergyTransfer for inelastic mode, the algorithm uses internal unit conversion of input X-values based on central average of a bin ranges. Namely, value <math>X_c = 0.5*(X_i+X_{i+1})</math> is calculated and converted to Momentum or EnergyTransfer correspondingly. This can give slightly different result from the case, when input workspace has been converted into correspondent units before converting to MDEvents. 
<li> Confusing message "Error in execution of algorithm ConvertToMD: emode must be equal to 1 or 2 for energy transfer calculation" is generated when one tries to process the results of inelastic scattering experiment in elastic mode. This message is generated by units conversion routine, which finds out that one of the workspace axis is in [[units|unit]] of DeltaE. These units can not be directly converted into momentum or energy, necessary for elastic mode. Select Direct or Indirect mode and integrate over whole energy transfer range to obtain MD workspace, which would correspond to an Elastic mode. 
<li> A good guess on the limits can be obtained from the [[ConvertToMDHelper]] algorithm.
</ol>


== How to write custom ConvertToMD plugin ==

This information intended for developers who have at least basic knowledge of C++ and needs to write its own [[Writing custom ConvertTo MD transformation |convertToMD plugin]].

== Usage examples ==
The examples below demonstrate the usages of the algorithm in most common situations. They work with the data files which already used by Mantid for different testing tasks.

=== Convert re-binned MARI 2D workspace to 3D MD workspace for further analysis/merging with data at different temperatures ===

<div style="border:1pt dashed blue; background:#f9f9f9;padding: 1em 0;">
<source lang="python">

Load(Filename='MAR11001.nxspe',OutputWorkspace='MAR11001')
SofQW3(InputWorkspace='MAR11001',OutputWorkspace='MAR11001Qe2',QAxisBinning='0,0.1,7',EMode='Direct')
AddSampleLog(Workspace='MAR11001Qe2',LogName='T',LogText='100',LogType='Number Series')

ConvertToMD(InputWorkspace='MAR11001Qe2',OutputWorkspace='MD3',QDimensions='CopyToMD',OtherDimensions='T',\
MinValues='-10,0,0',MaxValues='10,6,500',SplitInto='50,50,5')

</source>
</div>
Output '''MD3''' workspace can be viewed in slice-viewer as 3D workspace with T-axis having single value.

=== Convert Set of Event Workspaces (Horace scan) to 4D MD workspace, direct mode: ===

This example is based on CNCS_7860_event.nxs file, available in Mantid test folder. The same script without any changes would produce similar MD workspace given histogram data obtained from inelastic instruments and stored in nxspe files.

<div style="border:1pt dashed blue; background:#f9f9f9;padding: 1em 0;">
<source lang="python">
# let's load test event workspace, which has been already preprocessed and available in Mantid Test folder
WS_Name='CNCS_7860_event'
Load(Filename=WS_Name,OutputWorkspace=WS_Name)
# this workspace has been  obtained from an inelastic experiment with input energy Ei = 3. 
# Usually this energy is stored in workspace
# but if it is not, we have to provide it for inelastic conversion to work.
AddSampleLog(Workspace=WS_Name,LogName='Ei',LogText='3',LogType='Number')
#
# set up target ws name and remove target workspace with the same name which can occasionally exist.
RezWS = 'WS_4D'
try:
   DeleteWorkspace(RezWS)
except ValueError:
   print "Target ws ",RezWS," not found in analysis data service\n"
#
#---> Start loop over contributing files
for i in range(0,20,5):
     # the following operations simulate different workspaces, obtained from experiment using rotating crystal;
     # For real experiment we  usually just load these workspaces from nxspe files with proper Psi values defined there
     # and have to set up ub matrix
     SourceWS = 'SourcePart'+str(i)
     # it should be :
     #     Load(Filename=SourceWS_fileName,OutputWorkspace=WS_SourceWS)
     # here, but the test does not have these data so we emulate the data by the following rows: 
     # ws emulation begin ----> 
     CloneWorkspace(InputWorkspace=WS_Name,OutputWorkspace=SourceWS)
     # using scattering on a crystal with cubic lattice and 1,0,0 direction along the beam.
     SetUB(Workspace=SourceWS,a='1.4165',b='1.4165',c='1.4165',u='1,0,0',v='0,1,0')	
     # rotated by proper number of degrees around axis Y
     AddSampleLog(Workspace=SourceWS,LogName='Psi',LogText=str(i),LogType='Number Series')
     SetGoniometer(Workspace=SourceWS,Axis0='Psi,0,1,0,1')
     # ws emulation, end ---------------------------------------------------------------------------------------
     
     ConvertToMD(InputWorkspace=SourceWS,OutputWorkspace=RezWS,QDimensions='Q3D',QConversionScales='HKL',\
     OverwriteExisting=0,\ 
     dEAnalysisMode='Direct',MinValues='-3,-3,-3,-1',MaxValues='3,3,3,3',\
     SplitInto="20,20,1,1")
     # delete source workspace from memory;
     DeleteWorkspace(SourceWS)
#---> End loop
# plot results using sliceviewer
plotSlice(RezWS, xydim=["[H,0,0]","[0,K,0]"], slicepoint=[0,0] )
</source>
</div>

=== Convert set of inelastic results obtained in Powder mode (direct) as function of temperature to a 3D workspace: ===

The test example is based on MAR1011.nxspe data file, obtained by reducing test data from the MARI experiment. The data for the experiment can be located in [http://github.com/mantidproject/systemtests Mantid system test] folder. The text will produce 3-dimensional dataset, with temperature axis. The image does not change with temperature, as we have just cloned initial workspace without any changes to the experimental data.

<div style="border:1pt dashed blue; background:#f9f9f9;padding: 1em 0;">
<source lang="python">
# let's load test event workspace, which has been already preprocessed and availible in Mantid Test folder 
WS_Name='MAR11001.nxspe'
Load(Filename=WS_Name,OutputWorkspace=WS_Name)
# this workspace has been  obtained from an inelastic experiment with input energy 
# nxspe file has input energy stored in it so no need to add energy artificially
#AddSampleLog(Workspace=WS_Name,LogName='Ei',LogText='3',LogType='Number')

# set up target ws name and remove target workspace with the same name which can occasionally exist.
RezWS = 'WS_3D'
try:
    DeleteWorkspace(RezWS)
except ValueError:
    print "Target ws ",RezWS," not found in analysis data service\n"
i=0
# let's assume this is the temperature range obtained in experiments and 
# each data file is obtained for particular temperature. 
T = [1,1.5,2,2.5,3,3.5,4.,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10]
for i in range(0,len(T),1):
    # EMULATE LOAD OF DIFFERENT results obtained for different temperatures. ------>
    SourceWS = 'SourcePart'+str(i)
    # Load(Filename=WS_Name,OutputWorkspace=WS_Name)	
    CloneWorkspace(InputWorkspace=WS_Name,OutputWorkspace=SourceWS)
    # Each workspace has the temperature from the list above associated with it through the correspondent log file
    AddSampleLog(Workspace=SourceWS,LogName='T',LogText=str(T[i]),LogType='Number Series')
    # END EMULATION ---------------------------------------------------------------------

    ConvertToMD(InputWorkspace=SourceWS,OutputWorkspace=RezWS,QDimensions='|Q|',OverwriteExisting=0,\
        dEAnalysisMode='Direct',OtherDimensions='T',PreprocDetectorsWS='DetWS',
        MinValues='0,-10,0',MaxValues='12,10,10',SplitInto="100,100,20")
    # delete source workspace from memory;
    DeleteWorkspace(SourceWS)

plotSlice(RezWS, xydim=["|Q|","DeltaE"], slicepoint=[0,0] )

</source>
</div>


*WIKI*/

#include "MantidMDAlgorithms/ConvertToMD.h"

#include "MantidKernel/PhysicalConstants.h"
#include "MantidKernel/ProgressText.h"
#include "MantidKernel/IPropertyManager.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/IPropertySettings.h"
#include "MantidKernel/ArrayLengthValidator.h"
#include "MantidKernel/VisibleWhenProperty.h"
//
#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidAPI/Progress.h"
#include "MantidAPI/WorkspaceValidators.h"
#include "MantidMDEvents/MDWSTransform.h"
//
#include "MantidDataObjects/Workspace2D.h"

#include <algorithm>
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/ListValidator.h"
#include "MantidMDEvents/ConvToMDSelector.h"
#include "MantidDataObjects/TableWorkspace.h" 


using namespace Mantid;
using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::DataObjects;
using namespace Mantid::Geometry;
using namespace Mantid::MDEvents;
using namespace Mantid::MDEvents::CnvrtToMD;

namespace Mantid
{
namespace MDAlgorithms
{

// logger for the algorithm workspaces  
Kernel::Logger& ConvertToMD::g_Log =Kernel::Logger::get("MD-Algorithms");
//
Mantid::Kernel::Logger & 
ConvertToMD::getLogger(){return g_Log;}
//
// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(ConvertToMD)


// Sets documentation strings for this algorithm
void ConvertToMD::initDocs()
{
    this->setWikiSummary("<p>Transforms a workspace into MDEvent workspace with dimensions defined by user.</p><p>"  

"Gateway for set of subalgorithms, combined together to convert an input 2D matrix workspace or an event workspace with any units along X-axis into  multidimensional event workspace. </p><p>"

"Depending on the user input and the data found in the input workspace, the algorithms transform the input workspace into 1 to 4 dimensional MDEvent workspace and adds to this workspace additional dimensions, which are described by the workspace properties, and requested by user. </p><p>"

"The table contains the description of the main algorithm dialogue. More detailed description of the properties, relevant for each MD conversion type can be found on [[MD Transformation factory]] page.</p><p>"

"The '''Box Splitting Settings''' specifies the controller parameters, which define the target workspace binning: 	(see [[CreateMDWorkspace]] description)</p>");
    this->setOptionalMessage("Create a MDEventWorkspace with selected dimensions, e.g. the reciprocal space of momentums (Qx, Qy, Qz) or momentums modules |Q|, energy transfer dE if availible and any other user specified log values which can be treated as dimensions.");
}
//----------------------------------------------------------------------------------------------
/** Destructor
 */
ConvertToMD::~ConvertToMD()
{
}
/// Template to check if a variable equal to NaN
template <class T>
inline bool isNaN(T val)
{
  volatile T buf=val;
  return (val!=buf);
}
//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void 
ConvertToMD::init()
{
      auto ws_valid = boost::make_shared<CompositeValidator>();
      //
      ws_valid->add<InstrumentValidator>();
      // the validator which checks if the workspace has axis and any units
      ws_valid->add<WorkspaceUnitValidator>("");


    declareProperty(new WorkspaceProperty<MatrixWorkspace>("InputWorkspace","",Direction::Input,ws_valid),
        "An input Matrix Workspace (2DMatrix or Event workspace) ");
   
     declareProperty(new WorkspaceProperty<IMDEventWorkspace>("OutputWorkspace","",Direction::Output),
                  "Name of the output [[MDEventWorkspace]].");

     declareProperty(new PropertyWithValue<bool>("OverwriteExisting", true, Direction::Input),
              "By default  (''\"1\"''), existing Output Workspace will be replaced. Select false (''\"0\"'') if you want to add new events to the workspace, which already exist. "
              "\nChoosing ''\"0\"''' can be very inefficient for file-based workspaces");

     std::vector<std::string> Q_modes = MDEvents::MDTransfFactory::Instance().getKeys();
     // something to do with different moments of thime when algorithm or test loads library. To avoid empty factory always do this. 
     if(Q_modes.empty()) Q_modes.assign(1,"ERROR IN LOADING Q-converters");
  
     /// this variable describes default possible ID-s for Q-dimensions   
     declareProperty("QDimensions",Q_modes[0],boost::make_shared<StringListValidator>(Q_modes),
"String, describing available analysis modes, registered with [[MD Transformation factory]].\n "
"There are 3 modes currently available and described in details on [[MD Transformation factory]] page. \n"
"The modes names are '''CopyToMD''', '''mod<nowiki>|Q|</nowiki>''' and '''Q3D'''",
                     Direction::InOut);
     /// temporary, untill dEMode is not properly defined on Workspace
     std::vector<std::string> dE_modes = Kernel::DeltaEMode().availableTypes();
     declareProperty("dEAnalysisMode",dE_modes[Kernel::DeltaEMode::Direct],boost::make_shared<StringListValidator>(dE_modes),
"You can analyse neutron energy transfer in '''Direct''', '''Indirect''' or '''Elastic''' mode. \n"
"The analysis mode has to correspond to experimental set up. Selecting inelastic mode increases \n"
"the number of the target workspace dimensions by one. See [[MD Transformation factory]] for further details.",
                     Direction::InOut);

    MDEvents::MDWSTransform QSclAndFrames;
    std::vector<std::string> TargFrames = QSclAndFrames.getTargetFrames();
    declareProperty("Q3DFrames", TargFrames[CnvrtToMD::AutoSelect],boost::make_shared<StringListValidator>(TargFrames),
      "What will be the Q-dimensions of the output workspace in Q3D case?\n"
      "   AutoSelect: Choose the target coordinate frame as the function of goniometer and UB matrix values set on the input workspace\n"
      "  Q (lab frame): Wave-vector change of the lattice in the lab frame.\n"
      "  Q (sample frame): Wave-vector change of the lattice in the frame of the sample (taking out goniometer rotation).\n"
      "  HKL: Use the sample's UB matrix to convert to crystal's HKL indices.\n"
      "See [[ MD_Transformation_factory#Q3D |MD Transformation factory]] for more details about this. "
       );


     std::vector<std::string> QScales = QSclAndFrames.getQScalings();
     declareProperty("QConversionScales",QScales[CnvrtToMD::NoScaling], boost::make_shared<StringListValidator>(QScales),
"This property to normalize three momentums obtained in '''Q3D''' mode. See [[MD Transformation factory]] "
"for description and available scaling modes.\n The value can be modified depending on the target coordinate "
"system, defined by the property '''OutputDimensions'''. "
   );


     setPropertySettings("Q3DFrames",new Kernel::VisibleWhenProperty("QDimensions",IS_EQUAL_TO,"Q3D"));
     setPropertySettings("QconversionScales",new Kernel::VisibleWhenProperty("QDimensions",IS_EQUAL_TO,"Q3D"));


     
    declareProperty(new ArrayProperty<std::string>("OtherDimensions",Direction::Input),
"List(comma separated) of additional to '''Q''' and '''DeltaE''' variables which form additional "
"(orthogonal) to '''Q''' dimensions in the target workspace (e.g. Temperature or Magnetic field).\n "
"These variables had to be logged during experiment and the names of these variables have to coincide "
"with the log names for the records of these variables in the source workspace."
                    );

    // this property is mainly for ChildAlgorithms to set-up as they have to identify if they use the same instrument. 
    declareProperty(new PropertyWithValue<std::string>("PreprocDetectorsWS","PreprocessedDetectorsWS",Direction::Input), 
"The name of the table workspace where the part of the detectors transformation into reciprocal space, "
"calculated by [[PreprocessDetectorsToMD]] algorithm stored. If the workspace is not found in analysis "
"data service, [[PreprocessDetectorsToMD]] used to calculate it. If found, the algorithm uses existing "
"workspace. The field is useful if one expects to analyse number of different experiments obtained on "
"the same instrument. <span style=\"color:#FF0000\"> Dangerous if one uses number of workspaces with "
"modified derived instrument one after another. </span> '''In this case this property has to be set to "
"<span style=\"color:#FF0000\">\"-\"</span> sting (without quotes) or empty (possible from script only) "
"to force the workspace recalculation each time the algorithm is invoked'''"
                    );

    declareProperty(new PropertyWithValue<bool>("UpdateMasks", false, Direction::Input),
"if PreprocessDetectorWS is used to build the workspace with preprocessed detectors at first algorithm "
"call and the input workspaces instruments are different by just different masked detectors, setting this "
"option to true forces [[PreprocessDetectorsToMD]] update only the detectors masks for all subsequent "
"calls to this algorithm. <span style=\"color:#FF0000\">This is temporary solution necessary until Mantid "
"masks spectra by 0 rather then by NaN</span> "
                    );

    // if one needs to use Lorentz corrections
    declareProperty(new PropertyWithValue<bool>("LorentzCorrection", false, Direction::Input),
"Correct the weights of events or signals and errors transformed into reciprocal space by multiplying them "
"by the Lorentz multiplier:\n <math>sin(\\theta)^2/\\lambda^4</math>. Currently works in Q3D Elastic case only "
"and is ignored in any other case."
                    );

    declareProperty(new ArrayProperty<double>("MinValues"),
"It has to be N comma separated values, where N is the number of dimensions of the target workspace. Values "
"smaller then specified here will not be added to workspace.\n Number N is defined by properties 4,6 and 7 and "
"described on [[MD Transformation factory]] page. See also [[ConvertToMDHelper]]");

//TODO:    " If a minimal target workspace range is higher then the one specified here, the target workspace range will be used instead " );

   declareProperty(new ArrayProperty<double>("MaxValues"),
"A list of the same size and the same units as MinValues list. Values higher or equal to the specified by "
"this list will be ignored");
//TODO:    "If a maximal target workspace range is lower, then one of specified here, the target workspace range will be used instead" );
    
    declareProperty(new ArrayProperty<double>("Uproj"),
//"The functionality of this parameter set to non-default value is still not fully verified (see ticket #5982). "
"Defines the first projection vector of the target Q coordinate system in '''Q3D''' mode - Default (1,0,0)");

    declareProperty(new ArrayProperty<double>("Vproj"),
//"The functionality of this parameter set to non-default value is still not fully verified (see ticket #5982). "
"Defines the second projection vector of the target Q coordinate system in '''Q3D''' mode - Default (0,1,0).");

    declareProperty(new ArrayProperty<double>("Wproj"),
//"The functionality of this parameter set to non-default value is still not fully verified (see ticket #5982). "
"Defines the third projection vector of the target Q coordinate system in '''Q3D''' mode. - Default (0,0,1)");

   // Box controller properties. These are the defaults
    this->initBoxControllerProps("5" /*SplitInto*/, 1000 /*SplitThreshold*/, 20 /*MaxRecursionDepth*/);
    // additional box controller settings property. 
    auto mustBeMoreThen1 = boost::make_shared<BoundedValidator<int> >();
    mustBeMoreThen1->setLower(1);

    declareProperty(
      new PropertyWithValue<int>("MinRecursionDepth", 1,mustBeMoreThen1),
"Optional. If specified, then all the boxes will be split to this minimum recursion depth. 0 = no splitting, "
"1 = one level of splitting, etc. \n Be careful using this since it can quickly create a huge number of boxes = "
"(SplitInto ^ (MinRercursionDepth * NumDimensions)). \n But setting this property equal to MaxRecursionDepth "
"property is necessary if one wants to generate multiple file based workspaces in order to merge them later.");
    setPropertyGroup("MinRecursionDepth", getBoxSettingsGroupName());
 
}

 //----------------------------------------------------------------------------------------------
/* Execute the algorithm.   */
void ConvertToMD::exec()
{
  // initiate class which would deal with any dimension workspaces requested by algorithm parameters
  if(!m_OutWSWrapper) m_OutWSWrapper = boost::shared_ptr<MDEvents::MDEventWSWrapper>(new MDEvents::MDEventWSWrapper());

   // -------- get Input workspace
    m_InWS2D = getProperty("InputWorkspace");
   
    // get the output workspace
    API::IMDEventWorkspace_sptr spws = getProperty("OutputWorkspace");
  
  // Collect and Analyze the requests to the job, specified by the input parameters:
    //a) Q selector:
    std::string QModReq                    = getProperty("QDimensions");
    //b) the energy exchange mode
    std::string dEModReq                   = getProperty("dEAnalysisMode");
    //c) other dim property;
    std::vector<std::string> otherDimNames = getProperty("OtherDimensions");
    //d) The output dimensions in the Q3D mode, processed together with QConversionScales
    std::string QFrame                     = getProperty("Q3DFrames");
    //e) part of the procedure, specifying the target dimensions units. Currently only Q3D target units can be converted to different flavours of hkl
    std::string convertTo_                 = getProperty("QConversionScales");

    // Build the target ws description as function of the input & output ws and the parameters, supplied to the algorithm 
    MDEvents::MDWSDescription targWSDescr;
    // get workspace parameters and build target workspace descritpion, report if there is need to build new target MD workspace
    bool createNewTargetWs = buildTargetWSDescription(spws,QModReq,dEModReq,otherDimNames,QFrame,convertTo_,targWSDescr);

     // create and initate new workspace or set up existing workspace as a target. 
    if(createNewTargetWs)  // create new
       spws = this->createNewMDWorkspace(targWSDescr);
    else // setup existing MD workspace as workspace target.
       m_OutWSWrapper->setMDWS(spws);
 
    // copy the necessary methadata and get the unique number, that identifies the run, the source workspace came from.
    copyMetaData(spws,targWSDescr);
     // preprocess detectors;
    targWSDescr.m_PreprDetTable = this->preprocessDetectorsPositions(m_InWS2D,dEModReq,getProperty("UpdateMasks"));

 
    //DO THE JOB:
     // get pointer to appropriate  algorithm, (will throw if logic is wrong and ChildAlgorithm is not found among existing)
     ConvToMDSelector AlgoSelector;
     m_Convertor  = AlgoSelector.convSelector(m_InWS2D,m_Convertor);

    // initate conversion and estimate amout of job to do
     size_t n_steps = m_Convertor->initialize(targWSDescr,m_OutWSWrapper);
    // progress reporter
     m_Progress.reset(new API::Progress(this,0.0,1.0,n_steps)); 

     g_log.information()<<" conversion started\n";
     m_Convertor->runConversion(m_Progress.get());
  

     //JOB COMPLETED:
     setProperty("OutputWorkspace", boost::dynamic_pointer_cast<IMDEventWorkspace>(spws));
   // free the algorithm from the responsibility for the target workspace to allow it to be deleted if necessary
     m_OutWSWrapper->releaseWorkspace();
    // free up the sp to the input workspace, which would be deleted if nobody needs it any more;
     m_InWS2D.reset();
     return;
}

/**
 * Copy over the metadata from the input matrix workspace to output MDEventWorkspace
 * @param mdEventWS :: The output MDEventWorkspace
 * @param targWSDescr :: The descrition of the target workspace, used in the algorithm 
 *
 * @return  :: the number of experiment info added from the current MD workspace
 */
void ConvertToMD::copyMetaData(API::IMDEventWorkspace_sptr mdEventWS, MDEvents::MDWSDescription &targWSDescr) const
{
 // Copy ExperimentInfo (instrument, run, sample) to the output WS
  API::ExperimentInfo_sptr ei(m_InWS2D->cloneExperimentInfo());

  ei->mutableRun().addProperty("RUBW_MATRIX",targWSDescr.m_Wtransf.getVector(),true);
  ei->mutableRun().addProperty("W_MATRIX",targWSDescr.getPropertyValueAsType<std::vector<double> >("W_MATRIX"),true);

  // run index as the number of experiment into megred within this run. It is possible to interpret it differently 
  // and should never expect it to start with 0 (for first experiment info)
  uint16_t runIndex = mdEventWS->addExperimentInfo(ei);

  const MantidVec & binBoundaries = m_InWS2D->readX(0);

  // Replacement for SpectraDetectorMap::createIDGroupsMap using the ISpectrum objects instead
  auto mapping = boost::make_shared<det2group_map>();
  for ( size_t i = 0; i < m_InWS2D->getNumberHistograms(); ++i )
  {
    const auto& dets = m_InWS2D->getSpectrum(i)->getDetectorIDs();
    if(!dets.empty())
    {
      std::vector<detid_t> id_vector;
      std::copy(dets.begin(), dets.end(), std::back_inserter(id_vector));
      mapping->insert(std::make_pair(id_vector.front(), id_vector));
    }
  }

  uint16_t nexpts = mdEventWS->getNumExperimentInfo();
  for(uint16_t i = 0; i < nexpts; ++i)
  {
    ExperimentInfo_sptr expt = mdEventWS->getExperimentInfo(i);
    expt->mutableRun().storeHistogramBinBoundaries(binBoundaries);
    expt->cacheDetectorGroupings(*mapping);
  }

 // add rinindex to the target workspace description for further usage as the identifier for the events, which come from this run. 
    targWSDescr.addProperty("RUN_INDEX",runIndex,true);  

}

/** Constructor */
ConvertToMD::ConvertToMD()
{}
/** handle the input parameters and build target workspace description as function of input parameters 
* @param spws shared pointer to target MD workspace (just created or already existing)
* @param QModReq -- mode to convert momentum
* @param dEModReq -- mode to convert energy 
* @param otherDimNames -- the vector of additional dimensions names (if any)
* @param QFrame      -- in Q3D case this describes target coordinate system and is ignored in any othre caste
* @param convertTo_  -- The parameter describing Q-scaling transformtations
* @param targWSDescr -- the resulting class used to interpret all parameters together and used to describe selected transformation. 
*/ 
bool ConvertToMD::buildTargetWSDescription(API::IMDEventWorkspace_sptr spws,const std::string &QModReq,const std::string &dEModReq,const std::vector<std::string> &otherDimNames,
                                           const std::string &QFrame,const std::string &convertTo_,MDEvents::MDWSDescription &targWSDescr)
{
  // ------- Is there need to creeate new ouptutworpaced?  
    bool createNewTargetWs =doWeNeedNewTargetWorkspace(spws);
 
   // set the min and max values for the dimensions from the input porperties
    std::vector<double> dimMin = getProperty("MinValues");
    std::vector<double> dimMax = getProperty("MaxValues");
    // verify that the number min/max values is equivalent to the number of dimensions defined by properties and min is less max
    targWSDescr.setMinMax(dimMin,dimMax);   
    targWSDescr.buildFromMatrixWS(m_InWS2D,QModReq,dEModReq,otherDimNames);

    bool LorentzCorrections = getProperty("LorentzCorrection");
    targWSDescr.setLorentsCorr(LorentzCorrections);

  // instanciate class, responsible for defining Mslice-type projection
    MDEvents::MDWSTransform MsliceProj;
    if(createNewTargetWs)
    {
        //identify if u,v are present among input parameters and use defaults if not
        std::vector<double> ut = getProperty("UProj");
        std::vector<double> vt = getProperty("VProj");
        std::vector<double> wt = getProperty("WProj");
        try
        {     MsliceProj.setUVvectors(ut,vt,wt);   }
        catch(std::invalid_argument &)
        {     g_log.error() << "The projections are coplanar. Will use defaults [1,0,0],[0,1,0] and [0,0,1]" << std::endl;     }

       // otherwise input uv are ignored -> later it can be modified to set ub matrix if no given, but this may overcomplicate things. 

        // check if we are working in powder mode
        // set up target coordinate system and identify/set the (multi) dimension's names to use
         targWSDescr.m_RotMatrix = MsliceProj.getTransfMatrix(targWSDescr,QFrame,convertTo_);           
    }
    else // user input is mainly ignored and everything is in old MD workspace
    {  
        // dimensions are already build, so build MDWS description from existing workspace
        MDEvents::MDWSDescription oldWSDescr;
        oldWSDescr.buildFromMDWS(spws);

        // some conversion parameters can not be defined by the target workspace. They have to be retrieved from the input workspace 
        // and derived from input parameters. 
        oldWSDescr.setUpMissingParameters(targWSDescr);      
       // set up target coordinate system and the dimension names/units
        targWSDescr.m_RotMatrix = MsliceProj.getTransfMatrix(targWSDescr,QFrame,convertTo_);   

        // check inconsistencies, if the existing workspace can be used as target workspace. 
        oldWSDescr.checkWSCorresponsMDWorkspace(targWSDescr);
        // reset new ws description name
        targWSDescr =oldWSDescr;
    }
    return createNewTargetWs;
}

/**
 * Create new MD workspace and set up its box controller using algorithm's box controllers properties
 * @param targWSDescr
 * @return
 */
API::IMDEventWorkspace_sptr ConvertToMD::createNewMDWorkspace(const MDEvents::MDWSDescription &targWSDescr)
{
   // create new md workspace and set internal shared pointer of m_OutWSWrapper to this workspace
    API::IMDEventWorkspace_sptr spws = m_OutWSWrapper->createEmptyMDWS(targWSDescr);
    if(!spws)
    {
        g_log.error()<<"can not create target event workspace with :"<<targWSDescr.nDimensions()<<" dimensions\n";
        throw(std::invalid_argument("can not create target workspace"));
    }
    // Build up the box controller
    Mantid::API::BoxController_sptr bc = m_OutWSWrapper->pWorkspace()->getBoxController();
    // Build up the box controller, using the properties in BoxControllerSettingsAlgorithm
    this->setBoxController(bc, m_InWS2D->getInstrument());
    // split boxes;
    spws->splitBox();
  // Do we split more due to MinRecursionDepth?
    int minDepth = this->getProperty("MinRecursionDepth");
    int maxDepth = this->getProperty("MaxRecursionDepth");
    if (minDepth>maxDepth) throw std::invalid_argument("MinRecursionDepth must be >= MaxRecursionDepth ");
    spws->setMinRecursionDepth(size_t(minDepth));  

    return spws;

}

/**Check if the target workspace new or exists and we need to create new workspace
 *@param spws -- shared pointer to target MD workspace, which can be undefined if the workspace does not exist
 *
 *@returns true if one needs to create new workspace and false otherwise
*/
bool ConvertToMD::doWeNeedNewTargetWorkspace(API::IMDEventWorkspace_sptr spws)
{

  bool createNewWs(false);
  if(!spws)
  {
    createNewWs = true;
  }
  else
  { 
      bool shouldOverwrite = getProperty("OverwriteExisting");
      if (shouldOverwrite )
      {
          createNewWs=true;
      }else{
          createNewWs=false;
      }
  }
  return createNewWs;
}

/**
 * The method responsible for analyzing input workspace parameters and preprocessing detectors positions into reciprocal space
 * @param InWS2D -- input Matrix workspace with defined instrument
 * @param dEModeRequested
 * @param updateMasks
 * @return
 */
DataObjects::TableWorkspace_const_sptr ConvertToMD::preprocessDetectorsPositions( Mantid::API::MatrixWorkspace_const_sptr InWS2D,const std::string &dEModeRequested,bool updateMasks)
{

    DataObjects::TableWorkspace_sptr TargTableWS;
    Kernel::DeltaEMode::Type Emode;

    // Do we need to reuse output workspace
    bool storeInDataService(true);
    std::string OutWSName = std::string(getProperty("PreprocDetectorsWS"));
    if(OutWSName=="-"||OutWSName.empty()) // TargTableWS is recalculated each time;
    {
      storeInDataService = false;
      OutWSName = "ServiceTableWS";  // TODO: should be hidden?
    }
    else
    {
      storeInDataService = true;
    }

     // if output workspace exists in dataservice, we may try to use it
    if(storeInDataService && API::AnalysisDataService::Instance().doesExist(OutWSName) ) 
    {
        TargTableWS = API::AnalysisDataService::Instance().retrieveWS<DataObjects::TableWorkspace>(OutWSName);
        // get number of all histograms (may be masked or invalid)
        size_t nHist = InWS2D->getNumberHistograms();
        size_t nDetMap=TargTableWS->rowCount();
        if(nHist==nDetMap)
        {
          // let's take at least some precaution to ensure that instrument have not changed
          std::string currentWSInstrumentName = InWS2D->getInstrument()->getName();
          std::string oldInstrName            = TargTableWS->getLogs()->getPropertyValueAsType<std::string>("InstrumentName");

          if(oldInstrName==currentWSInstrumentName)
          { 
            if(!updateMasks) return TargTableWS;
            //Target workspace with preprocessed detectors exists and seems is correct one. 
            // We still need to update masked detectors information
            TargTableWS = this->runPreprocessDetectorsToMDChildUpdatingMasks(InWS2D,OutWSName,dEModeRequested,Emode);
            return TargTableWS;
          }
        }
        else // there is a workspace in the data service with the same name but this ws is not suitable as target for this algorithm. 
        {    // Should delete this WS from the dataservice
          API::AnalysisDataService::Instance().remove(OutWSName);
        }
    }
    // No result found in analysis data service or the result is unsatisfactory. Try to calculate target workspace.  
    TargTableWS =this->runPreprocessDetectorsToMDChildUpdatingMasks(InWS2D,OutWSName,dEModeRequested,Emode);

    if(storeInDataService)
      API::AnalysisDataService::Instance().addOrReplace(OutWSName,TargTableWS);
//    else
//      TargTableWS->setName(OutWSName);

  
   // check if we got what we wanted:

   // in direct or indirect mode input ws has to have input energy
    if(Emode==Kernel::DeltaEMode::Direct||Emode==Kernel::DeltaEMode::Indirect)
    {
       double   m_Ei  = TargTableWS->getLogs()->getPropertyValueAsType<double>("Ei");
       if(isNaN(m_Ei))
       {
         // Direct mode needs Ei
         if(Emode==Kernel::DeltaEMode::Direct)throw(std::invalid_argument("Input neutron's energy has to be defined in inelastic mode "));

         // Do we have at least something for Indirect?
         float *eFixed = TargTableWS->getColDataArray<float>("eFixed");
         if(!eFixed)
           throw(std::invalid_argument("Input neutron's energy has to be defined in inelastic mode "));

         uint32_t NDetectors = TargTableWS->getLogs()->getPropertyValueAsType<uint32_t>("ActualDetectorsNum");
         for(uint32_t i=0;i<NDetectors;i++)
           if(isNaN(*(eFixed+i)))throw(std::invalid_argument("Undefined eFixed energy for detector N: "+boost::lexical_cast<std::string>(i)));
       }
    }

    return TargTableWS;
}

DataObjects::TableWorkspace_sptr  ConvertToMD::runPreprocessDetectorsToMDChildUpdatingMasks(Mantid::API::MatrixWorkspace_const_sptr InWS2D,
                                                                                                  const std::string &OutWSName,const std::string &dEModeRequested,Kernel::DeltaEMode::Type &Emode)
{
   // prospective result
    DataObjects::TableWorkspace_sptr TargTableWS;

    // if input workspace does not exist in analysis data service, we have to add it there to work with the Child Algorithm 
    std::string InWSName = InWS2D->getName();
    if(!API::AnalysisDataService::Instance().doesExist(InWSName))
    {
       if(InWSName.empty())InWSName = "ImputMatrixWS";
       // wery bad, but what can we do otherwise... -> pool out the class pointer which is not const 
       // add input matrix ws to the analysis data service in order for ChildAlgorithm to retrieve it. 
       API::AnalysisDataService::Instance().addOrReplace(InWSName,m_InWS2D);
    }

    Mantid::API::Algorithm_sptr childAlg = createChildAlgorithm("PreprocessDetectorsToMD",0.,1.);
    if(!childAlg)throw(std::runtime_error("Can not create child ChildAlgorithm to preprocess detectors"));
    childAlg->setProperty("InputWorkspace",InWSName);
    childAlg->setProperty("OutputWorkspace",OutWSName);
    childAlg->setProperty("GetMaskState",true);
    childAlg->setProperty("UpdateMasksInfo",true);
    childAlg->setProperty("OutputWorkspace",OutWSName);

 // check and get energy conversion mode to define additional ChildAlgorithm parameters
    Emode = Kernel::DeltaEMode().fromString(dEModeRequested);
    if(Emode == Kernel::DeltaEMode::Indirect) 
      childAlg->setProperty("GetEFixed",true); 


    childAlg->execute();
    if(!childAlg->isExecuted())throw(std::runtime_error("Can not properly execute child algorithm PreprocessDetectorsToMD"));

    TargTableWS = childAlg->getProperty("OutputWorkspace");
    if(!TargTableWS)throw(std::runtime_error("Can not retrieve results of child algorithm PreprocessDetectorsToMD"));

    return TargTableWS;
}

} // namespace Mantid
} // namespace MDAlgorithms


