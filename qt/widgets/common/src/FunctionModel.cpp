// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#include "MantidQtWidgets/Common/FunctionModel.h"
#include "MantidAPI/CompositeFunction.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/MultiDomainFunction.h"
#include "MantidQtWidgets/Common/FunctionBrowser/FunctionBrowserUtils.h"

namespace MantidQt {
namespace MantidWidgets {

using namespace Mantid::API;

void IFunctionModel::setFunctionString(const QString & funStr)
{
  setFunction(FunctionFactory::Instance().createInitialized(funStr.toStdString()));
}

QString IFunctionModel::getFunctionString() const
{
  auto fun = getCurrentFunction();
  if (!fun)
    return "";
  return QString::fromStdString(fun->asString());
}

QString IFunctionModel::getFitFunctionString() const
{
  auto fun = getFitFunction();
  if (!fun)
    return "";
  return QString::fromStdString(fun->asString());
}

void IFunctionModel::clear()
{
  setFunction(IFunction_sptr());
}

void MultiDomainFunctionModel::setFunction(IFunction_sptr fun) {
  m_function = boost::dynamic_pointer_cast<MultiDomainFunction>(fun);
  if (m_function) {
    return;
  }
  m_function = MultiDomainFunction_sptr(new MultiDomainFunction);
  if (fun) {
    for (int i = 0; i < m_numberDomains; ++i) {
      m_function->addFunction(fun->clone());
      m_function->setDomainIndex(i, i);
    }
  }
}

IFunction_sptr MultiDomainFunctionModel::getFitFunction() const
{
  if (!m_function || m_function->nFunctions() > 1) {
    return m_function;
  }
  if (m_function->nFunctions() == 1) {
    auto fun = m_function->getFunction(0);
    auto compFun = boost::dynamic_pointer_cast<CompositeFunction>(fun);
    if (compFun && compFun->nFunctions() == 1) {
      return compFun->getFunction(0);
    }
    return fun;
  }
  return IFunction_sptr();
}

bool MultiDomainFunctionModel::hasFunction() const
{
  if (!m_function || m_function->nFunctions() == 0) return false;
  return true;
}

void MultiDomainFunctionModel::addFunction(const QString & prefix, const QString & funStr)
{
  if (!m_function) {
    setFunctionString(funStr);
    return;
  }
  auto newFun = FunctionFactory::Instance().createInitialized(funStr.toStdString());
  for (int i = 0; i < getNumberDomains(); ++i) {
    auto fun = getSingleFunction(i);
    auto parentFun = getFunctionWithPrefix(prefix, fun);
    auto cf = boost::dynamic_pointer_cast<CompositeFunction>(parentFun);
    if (cf) {
      cf->addFunction(newFun->clone());
    } else if(i == 0 && prefix.isEmpty()) {
      setFunctionString(getFunctionString() + ";" + funStr);
      break;
    } else {
      throw std::runtime_error("Function at " + prefix.toStdString() + " is not composite.");
    }
  }
}

void MultiDomainFunctionModel::setParameter(const QString & paramName, double value)
{
  getCurrentFunction()->setParameter(paramName.toStdString(), value);
}

void MultiDomainFunctionModel::setParamError(const QString & paramName, double value)
{
  auto fun = getCurrentFunction();
  auto const index = fun->parameterIndex(paramName.toStdString());
  fun->setError(index, value);
}

double MultiDomainFunctionModel::getParameter(const QString & paramName) const
{
  return getCurrentFunction()->getParameter(paramName.toStdString());
}

double MultiDomainFunctionModel::getParamError(const QString & paramName) const
{
  auto fun = getCurrentFunction();
  auto const index = fun->parameterIndex(paramName.toStdString());
  return fun->getError(index);
}

bool MultiDomainFunctionModel::isParameterFixed(const QString & parName) const
{
  return isLocalParameterFixed(parName, static_cast<int>(m_currentDomainIndex));
}

QString MultiDomainFunctionModel::getParameterTie(const QString & parName) const
{
  return getLocalParameterTie(parName, static_cast<int>(m_currentDomainIndex));
}

void MultiDomainFunctionModel::setParameterFixed(const QString & parName, bool fixed)
{
  setLocalParameterFixed(parName, static_cast<int>(m_currentDomainIndex), fixed);
}

void MultiDomainFunctionModel::setParameterTie(const QString & parName, QString tie)
{
  setLocalParameterTie(parName, static_cast<int>(m_currentDomainIndex), tie);
}

QStringList MultiDomainFunctionModel::getParameterNames() const
{
  QStringList names;
  const auto paramNames = getCurrentFunction()->getParameterNames();
  for (auto const name : paramNames) {
    names << QString::fromStdString(name);
  }
  return names;
}

IFunction_sptr MultiDomainFunctionModel::getSingleFunction(int index) const
{
  checkIndex(index);
  return m_function->getFunction(index);
}

IFunction_sptr MultiDomainFunctionModel::getCurrentFunction() const
{
  return getSingleFunction(static_cast<int>(m_currentDomainIndex));
}

void MultiDomainFunctionModel::setNumberDomains(int nDomains)
{
  auto const nd = static_cast<size_t>(nDomains);
  if (nd < 1) {
    throw std::runtime_error("Number of domains shouldn't be less than 1.");
  }
  if (nd == m_numberDomains) {
    return;
  }
  if (!m_function) {
    m_numberDomains = nd;
    return;
  }
  auto const lastIndex = m_numberDomains - 1;
  if (nd > m_numberDomains) {
    auto fun = m_function->getFunction(lastIndex);
    for (size_t i = m_numberDomains; i < nd; ++i) {
      m_function->addFunction(fun->clone());
      m_function->setDomainIndex(i, i);
    }
  } else {
    for (size_t i = lastIndex; i >= nd; --i) {
      m_function->removeFunction(i);
    }
  }
  m_numberDomains = nDomains;
}

void MultiDomainFunctionModel::setDatasetNames(const QStringList & names)
{
  if (static_cast<size_t>(names.size()) != m_numberDomains) {
    throw std::runtime_error("Number of dataset names doesn't match the number of domains.");
  }
  m_datasetNames = names;
}

QStringList MultiDomainFunctionModel::getDatasetNames() const
{
  if (static_cast<size_t>(m_datasetNames.size()) != m_numberDomains) {
    m_datasetNames.clear();
    for (size_t i = 0; i < m_numberDomains; ++i) {
      m_datasetNames << QString::number(i);
    }
  }
  return m_datasetNames;
}

int MultiDomainFunctionModel::getNumberDomains() const
{
  return static_cast<int>(m_numberDomains);
}

int MultiDomainFunctionModel::currentDomainIndex() const
{
  return static_cast<int>(m_currentDomainIndex);
}

void MultiDomainFunctionModel::setCurrentDomainIndex(int index)
{
  checkIndex(index);
  m_currentDomainIndex = static_cast<size_t>(index);
}

double MultiDomainFunctionModel::getLocalParameterValue(const QString & parName, int i) const
{
  return getSingleFunction(i)->getParameter(parName.toStdString());
}

bool MultiDomainFunctionModel::isLocalParameterFixed(const QString & parName, int i) const
{
  auto fun = getSingleFunction(i);
  auto const parIndex = fun->parameterIndex(parName.toStdString());
  return fun->isFixed(parIndex);
}

QString MultiDomainFunctionModel::getLocalParameterTie(const QString & parName, int i) const
{
  auto fun = getSingleFunction(i);
  auto const parIndex = fun->parameterIndex(parName.toStdString());
  auto const tie = fun->getTie(parIndex);
  if (!tie) return "";
  auto const tieStr = QString::fromStdString(tie->asString());
  auto const j = tieStr.indexOf('=');
  return tieStr.mid(j + 1);
}

void MultiDomainFunctionModel::setLocalParameterValue(const QString & parName, int i, double value)
{
  getSingleFunction(i)->setParameter(parName.toStdString(), value);
}

void MultiDomainFunctionModel::setLocalParameterValue(const QString & parName, int i, double value, double error)
{
  auto fun = getSingleFunction(i);
  auto const parIndex = fun->parameterIndex(parName.toStdString());
  fun->setParameter(parIndex, value);
  fun->setError(parIndex, error);
}

void MultiDomainFunctionModel::setLocalParameterFixed(const QString & parName, int i, bool fixed)
{
  auto fun = getSingleFunction(i);
  auto const parIndex = fun->parameterIndex(parName.toStdString());
  if (fixed) {
    fun->fix(parIndex);
  } else if (fun->isFixed(parIndex)) {
    fun->unfix(parIndex);
  }
}

void MultiDomainFunctionModel::setLocalParameterTie(const QString & parName, int i, QString tie)
{
  auto fun = getSingleFunction(i);
  auto const name = parName.toStdString();
  if (tie.isEmpty()) {
    fun->removeTie(fun->parameterIndex(name));
  } else {
    auto const j = tie.indexOf('=');
    fun->tie(name, tie.mid(j + 1).toStdString());
  }
}

void MultiDomainFunctionModel::changeTie(const QString & parName, const QString & tie)
{
  try {
    setLocalParameterTie(parName, static_cast<int>(m_currentDomainIndex), tie);
  } catch (std::exception &) {
    // the tie is probably being edited
  }
}

/// Check a domain/function index to be in range.
void MultiDomainFunctionModel::checkIndex(int index) const {
  if (index < 0 || index >= getNumberDomains()) {
    throw std::runtime_error("Domain index is out of range: " + std::to_string(index) + " out of " + std::to_string(getNumberDomains()));
  }
}

} // namespace API
} // namespace MantidQt
