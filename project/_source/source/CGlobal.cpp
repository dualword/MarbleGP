// (w) 2021 by Dustbin::Games / Christian Keimel

#include <CGlobal.h>

namespace dustbin {

  CGlobal* CGlobal::getInstance() {
    return m_pInstance;
  }

  /**
  * Get a setting as boolean
  * @param a_sKey the key of the setting
  * @return the value, false if the key is not stored
  */
  bool CGlobal::getSettingBool(const std::string& a_sKey) {
    return getSetting(a_sKey) != "" && getSetting(a_sKey) != "0";
  }

  /**
  * Get a setting as integer
  * @param a_sKey the key of the setting
  * @return the value, 0 if the key is not stored
  */
  int CGlobal::getSettingInt(const std::string& a_sKey) {
    return std::atoi(getSetting(a_sKey).c_str());
  }

  /**
  * Get a setting as double
  * @param a_sKey the key of the setting
  * @return the value, 0.0 if the key is not stored
  */
  double CGlobal::getSettingDouble(const std::string& a_sKey) {
    return std::atof(getSetting(a_sKey).c_str());
  }

  /**
  * Set a boolean setting
  * @param a_sKey the key of the setting
  * @param a_bValue the value
  */
  void CGlobal::setSettingBool(const std::string& a_sKey, bool a_bValue) {
    setSetting(a_sKey, a_bValue ? "1" : "0");
  }

  /**
  * Set an integer setting
  * @param a_sKey the key of the setting
  * @param a_iValue the value
  */
  void CGlobal::setSettingInt(const std::string& a_sKey, int a_iValue) {
    setSetting(a_sKey, std::to_string(a_iValue));
  }

  /**
  * Set a double setting
  * @param a_sKey the key of the setting
  * @param a_fValue the value
  */
  void CGlobal::setSettingDouble(const std::string& a_sKey, double a_fValue) {
    setSetting(a_sKey, std::to_string(a_fValue));
  }


  /**
  * Get a global as boolean
  * @param a_sKey the key of the setting
  * @return the value, 0 if the key is not stored
  */
  bool CGlobal::getGlobalBool(const std::string& a_sKey) {
    return getGlobal(a_sKey) != "" && getGlobal(a_sKey) != "0";
  }

  /**
  * Get a global as integer
  * @param a_sKey the key of the setting
  * @return the value, 0 if the key is not stored
  */
  int CGlobal::getGlobalInt(const std::string& a_sKey) {
    return std::atoi(getGlobal(a_sKey).c_str());
  }

  /**
  * Get a global as double
  * @param a_sKey the key of the setting
  * @return the value, 0 if the key is not stored
  */
  double CGlobal::getGlobalDouble(const std::string& a_sKey) {
    return std::atof(getGlobal(a_sKey).c_str());
  }

  /**
  * Set a boolean global
  * @param a_sKey the key of the setting
  * @param a_bValue the value
  */
  void CGlobal::setGlobalBoolean(const std::string& a_sKey, bool a_bValue) {
    setGlobal(a_sKey, a_bValue ? "1" : "0");
  }

  /**
  * Set an integer global
  * @param a_sKey the key of the setting
  * @param a_bValue the value
  */
  void CGlobal::setGlobalInt(const std::string& a_sKey, int a_iValue) {
    setGlobal(a_sKey, std::to_string(a_iValue));
  }

  /**
  * Set a double global
  * @param a_sKey the key of the setting
  * @param a_bValue the value
  */
  void CGlobal::setGlobalDouble(const std::string& a_sKey, double a_fValue) {
    setGlobal(a_sKey, std::to_string(a_fValue));
  }



  CGlobal *CGlobal::m_pInstance = nullptr;
}