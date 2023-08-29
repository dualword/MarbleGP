// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <string>
#include <vector>

namespace dustbin {
  namespace json {
    /**
    * @class CIrrJSON
    * @author Christian Keimel
    * A simple JSON parser which mostly acts similar to irrXML
    */
    class CIrrJSON {
    public:
      enum class enToken {
        Unknown,
        ObjectStart,
        ObjectEnd,
        ArrayStart,
        ArrayEnd,
        Colon,
        Separator,
        ValueString,
        ValueInt,
        ValueFloat,
        ValueBool,
        EndOfJSON,
        Error
      };

    protected:
      std::string m_sJSON;
      std::string m_sCurrent;
      std::string m_sError;

      enToken m_eToken;

      const char *m_pReader;

      void skipIgnoredChars();

      /**
      * Read the next token
      * @param a_eType [out] token type
      * @param a_sToken [out] string representation of the token
      * @return true if reading the next token succeeded, false otherwise
      */
      bool nextToken(enToken &a_eType, std::string &a_sToken);

    public:
      CIrrJSON(const std::string &a_sJSON);
      virtual ~CIrrJSON();

      bool read();

      CIrrJSON::enToken getType();

      bool isValueType();

      const std::string &asString();

      int asInt();
      double asFloat();

      bool asBool();

      bool hasError();

      const std::string &getError();
    };
  }
}