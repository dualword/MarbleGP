// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <json/CIrrJSON.h>

#include <map>

namespace dustbin {
  namespace json {
    const std::string c_sIgnore = " \r\n\t";
    const std::string c_sNumber = "0123456789.";

    const std::map<char, CIrrJSON::enToken> c_mSingleCharTokens = {
      { '{', CIrrJSON::enToken::ObjectStart },
      { '}', CIrrJSON::enToken::ObjectEnd   },
      { '[', CIrrJSON::enToken::ArrayStart  },
      { ']', CIrrJSON::enToken::ArrayEnd    },
      { ':', CIrrJSON::enToken::Colon       },
      { ',', CIrrJSON::enToken::Separator   }
    };

    CIrrJSON::CIrrJSON(const std::string& a_sJSON) : m_sJSON(a_sJSON), m_sCurrent(""), m_sError(""), m_pReader(nullptr), m_eToken(enToken::Unknown) {
      m_pReader = m_sJSON.c_str();
    }

    CIrrJSON::CIrrJSON(irr::io::IReadFile* a_pFile) : m_sJSON(""), m_sCurrent(""), m_sError(""), m_pReader(nullptr), m_eToken(enToken::Unknown) {
      char *p = new char[a_pFile->getSize() + 1];
      memset(p, 0, a_pFile->getSize() + 1);
      a_pFile->read(p, a_pFile->getSize());
      m_sJSON = p;
      delete []p;
      m_pReader = m_sJSON.c_str();
    }

    CIrrJSON::~CIrrJSON() {
    }

    void CIrrJSON::skipIgnoredChars() {
      while (c_sIgnore.find(*m_pReader) != std::string::npos) {
        m_pReader++;
      }
    }


    /**
    * Read the next token
    * @param a_eType [out] token type
    * @param a_sToken [out] string representation of the token
    * @return true if reading the next token succeeded, false otherwise
    */
    bool CIrrJSON::nextToken(enToken& a_eToken, std::string& a_sToken) {
      bool l_bRet = false;
      a_eToken = enToken::Error;
      a_sToken = "";

      skipIgnoredChars();

      if (*m_pReader == '\0') {
        l_bRet = true;
        a_eToken = enToken::EndOfJSON;
        return false;
      }

      if (c_mSingleCharTokens.find(*m_pReader) != c_mSingleCharTokens.end()) {
        a_eToken = c_mSingleCharTokens.at(*m_pReader);
        a_sToken = *m_pReader;
        l_bRet   = true;
        m_pReader++;
      }

      if (!l_bRet) {
        if (*m_pReader == '"') {
          // We have a string
          a_eToken = enToken::ValueString;
          a_sToken = "";
          m_pReader++;

          while (*m_pReader != '"' && *m_pReader != '\0') {
            if (*m_pReader == '\\')
              m_pReader++;

            a_sToken += *m_pReader;
            m_pReader++;
          }

          if (*m_pReader == '\0') {
            m_sError = "Error: unterminated string.";
            l_bRet = false;
          }
          else {
            m_pReader++;
            l_bRet = true;
          }
        }
        else if (c_sNumber.find(*m_pReader) != std::string::npos && *m_pReader != '.') {
          // Seems that we have a number
          a_eToken = enToken::ValueInt;

          while (c_sNumber.find(*m_pReader) != std::string::npos && *m_pReader != ',') {
            if (*m_pReader == '.') {
              if (a_eToken == enToken::ValueInt)
                a_eToken = enToken::ValueFloat;
              else {
                l_bRet = false;
                m_sError = "Invalid number with two dots found.";
                break;
              }
            }
            a_sToken += *m_pReader;
            m_pReader++;
          }

          l_bRet = c_sIgnore.find(*m_pReader) != std::string::npos || *m_pReader == ',';
        }
        else {
          // Only option left is a boolean
          while (c_sIgnore.find(*m_pReader) == std::string::npos && *m_pReader != ',') {
            a_sToken += *m_pReader;
            m_pReader++;
          }

          l_bRet = a_sToken == "true" || a_sToken == "false";

          if (l_bRet)
            a_eToken = enToken::ValueBool;
        }
      }

      return l_bRet;
    }

    bool CIrrJSON::read() {
      return nextToken(m_eToken, m_sCurrent);
    }

    CIrrJSON::enToken CIrrJSON::getType() {
      return m_eToken;
    }

    bool CIrrJSON::isValueType() {
      return m_eToken == enToken::ValueBool || m_eToken == enToken::ValueFloat || m_eToken == enToken::ValueInt || m_eToken == enToken::ValueString;
    }

    const std::string& CIrrJSON::asString() {
      return m_sCurrent;
    }

    int CIrrJSON::asInt() {
      return std::atoi(m_sCurrent.c_str());
    }

    double CIrrJSON::asFloat() {
      return std::atof(m_sCurrent.c_str());
    }

    bool CIrrJSON::asBool() {
      return m_sCurrent == "true";
    }

    const std::string& CIrrJSON::getError() {
      return m_sError;
    }

    bool CIrrJSON::hasError() {
      return m_eToken == enToken::Error;
    }

    const std::string CIrrJSON::getTokenTypeAsString() {
      return m_eToken == json::CIrrJSON::enToken::ArrayEnd    ? "ArrayEnd"    :
             m_eToken == json::CIrrJSON::enToken::ArrayStart  ? "ArrayStart"  :
             m_eToken == json::CIrrJSON::enToken::Colon       ? "Colon"       :
             m_eToken == json::CIrrJSON::enToken::EndOfJSON   ? "EndOfJson"   :
             m_eToken == json::CIrrJSON::enToken::Error       ? "Error"       :
             m_eToken == json::CIrrJSON::enToken::ObjectEnd   ? "ObjectEnd"   :
             m_eToken == json::CIrrJSON::enToken::ObjectStart ? "ObjectStart" :
             m_eToken == json::CIrrJSON::enToken::Separator   ? "Separator"   :
             m_eToken == json::CIrrJSON::enToken::Unknown     ? "Unkown"      :
             m_eToken == json::CIrrJSON::enToken::ValueBool   ? "ValueBool"   :
             m_eToken == json::CIrrJSON::enToken::ValueString ? "ValueString" :
             m_eToken == json::CIrrJSON::enToken::ValueFloat  ? "ValueFloat"  :
             m_eToken == json::CIrrJSON::enToken::ValueInt    ? "ValueInt"    : "?";
    }
  }
}