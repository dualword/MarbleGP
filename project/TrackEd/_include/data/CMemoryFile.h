// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>

namespace dustbin {
  namespace data {
    /**
    * @class CMemoryWriteFile
    * @author Christian Keimel
    * This class acts as a dynamic memory file for the
    * Irrlicht file system, used for saving stuff
    */
    class CMemoryWriteFile : public irr::io::IWriteFile {
      private:
        irr::io::path m_sFileName;    /**< The filename */

        std::vector<unsigned char> m_vData;   /**< The data of the file */

        std::vector<unsigned char>::iterator m_itData;    /**< The current position in the file */

      public:
        CMemoryWriteFile(const std::string &a_sFileName);
        CMemoryWriteFile(const std::string &a_sFileName, const std::vector<unsigned char> &a_vData);

        virtual ~CMemoryWriteFile();

        virtual const irr::io::path &getFileName() const override;

        virtual long getPos() const override;

        virtual bool seek(long a_iFinalPos, bool a_bRelativeMovement = false) override;

        virtual size_t write (const void *a_pBuffer, size_t a_iSizeToWrite) override;

        virtual bool flush() override;

        const std::vector<unsigned char> &getData();
    };

    class CMemoryReadFile : public irr::io::IReadFile {
      private:
        irr::io::path m_sFileName;

        std::vector<unsigned char> m_vData;

        std::vector<unsigned char>::iterator m_itData;

	    public:
        CMemoryReadFile(const std::string &a_sFileName, const std::vector<unsigned char> &a_vData);
        virtual ~CMemoryReadFile();

		    virtual size_t read(void *a_pBuffer, size_t a_iSizeToRead) override;

		    virtual bool seek(long a_iFinalPos, bool a_bRelativeMovement = false) override;

		    virtual long getSize() const override;

		    virtual long getPos() const override;

		    virtual const irr::io::path &getFileName() const override;
    };
  }
}
