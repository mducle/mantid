#ifndef MANTID_CUSTOMINTERFACES_REFLPROCESSCOMMAND_H
#define MANTID_CUSTOMINTERFACES_REFLPROCESSCOMMAND_H

#include "MantidQtCustomInterfaces/Reflectometry/ReflCommandBase.h"

namespace MantidQt {
namespace CustomInterfaces {
/** @class ReflProcessCommand

ReflProcessCommand defines the action "Process"

Copyright &copy; 2011-14 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
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
class ReflProcessCommand : public ReflCommandBase {
public:
  ReflProcessCommand(IReflTablePresenter *tablePresenter)
      : ReflCommandBase(tablePresenter){};
  virtual ~ReflProcessCommand(){};

  void execute() override {
    m_tablePresenter->notify(IReflTablePresenter::ProcessFlag);
  };
  std::string name() override { return std::string("Process"); }
  std::string icon() override { return std::string("://stat_rows.png"); }
};
}
}
#endif /*MANTID_CUSTOMINTERFACES_REFLPROCESSCOMMAND_H*/