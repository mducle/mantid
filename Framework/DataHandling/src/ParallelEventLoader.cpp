#include "MantidDataHandling/ParallelEventLoader.h"
#include "MantidDataObjects/EventWorkspace.h"
#include "MantidGeometry/Instrument/DetectorInfo.h"
#include "MantidParallel/IO/EventLoader.h"
#include "MantidTypes/SpectrumDefinition.h"
#include "MantidTypes/Event/TofEvent.h"

namespace Mantid {
namespace DataHandling {

/// Return offset between global spectrum index and detector ID for given banks.
std::vector<int32_t> bankOffsets(const API::ExperimentInfo &ws,
                                 const std::string &filename,
                                 const std::string &groupName,
                                 const std::vector<std::string> &bankNames) {
  // Build detector ID to spectrum index map. Used only in LoadEventNexus so we
  // know there is a 1:1 mapping, omitting monitors.
  const auto &detInfo = ws.detectorInfo();
  const auto &detIDs = detInfo.detectorIDs();
  std::unordered_map<detid_t, int32_t> map;
  int32_t spectrumIndex{0}; // *global* index
  for (size_t i = 0; i < detInfo.size(); ++i)
    if (!detInfo.isMonitor(i))
      map[detIDs[i]] = spectrumIndex++;

  // Load any event ID and determine offset from it. This is always a detector
  // ID since the parallel loader is disabled otherwise. It is assumed that
  // detector IDs within a bank are contiguous.
  std::vector<int32_t> bankOffsets;
  for (const auto &eventId : Parallel::IO::EventLoader::anyEventIdFromBanks(
           filename, groupName, bankNames)) {
    // The offset is the difference between the event ID and the spectrum index
    // and can then be used to translate from the former to the latter by simple
    // subtraction.
    // If no eventId could be read for a bank it implies that there are no
    // events, so any offset will do since it is unused. Set to 0.
    if (eventId)
      bankOffsets.emplace_back(*eventId - map.at(*eventId));
    else
      bankOffsets.emplace_back(0);
  }

  return bankOffsets;
}

/// Load events from given banks into given EventWorkspace.
void ParallelEventLoader::load(DataObjects::EventWorkspace &ws,
                               const std::string &filename,
                               const std::string &groupName,
                               const std::vector<std::string> &bankNames) {
  const size_t size = ws.getNumberHistograms();
  std::vector<std::vector<Types::Event::TofEvent> *> eventLists(size, nullptr);
  for (size_t i = 0; i < size; ++i)
    DataObjects::getEventsFrom(ws.getSpectrum(i), eventLists[i]);

  Parallel::IO::EventLoader::load(
      filename, groupName, bankNames,
      bankOffsets(ws, filename, groupName, bankNames), std::move(eventLists));
}

} // namespace DataHandling
} // namespace Mantid
