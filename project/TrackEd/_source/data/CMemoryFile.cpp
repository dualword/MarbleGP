// (w) 2024 by Dustbin::Games / Christian Keimel
#include <data/CMemoryFile.h>

namespace dustbin {
  namespace data {
    CMemoryWriteFile::CMemoryWriteFile(const std::string &a_sFileName) : irr::io::IWriteFile(), m_sFileName(a_sFileName.c_str()) {
      m_itData = m_vData.end();
    }

    CMemoryWriteFile::CMemoryWriteFile(const std::string &a_sFileName, const std::vector<unsigned char> &a_vData) : irr::io::IWriteFile(), m_sFileName(a_sFileName.c_str()) {
      m_vData = a_vData;
      m_itData = m_vData.end();
    }

    CMemoryWriteFile::~CMemoryWriteFile() {
    }

    const irr::io::path &CMemoryWriteFile::getFileName() const {
      return m_sFileName;
    }

    long CMemoryWriteFile::getPos() const {
      return (long)(m_itData - m_vData.begin());
    }

    bool CMemoryWriteFile::seek(long a_iFinalPos, bool a_bRelativeMovement) {
      if (a_bRelativeMovement)
        m_itData += a_iFinalPos;
      else
        m_itData = m_vData.begin() + a_iFinalPos;

      return true;
    }

    size_t CMemoryWriteFile::write(const void *a_pBuffer, size_t a_iSizeToWrite) {
      unsigned char *l_pBuffer = (unsigned char *)a_pBuffer;

      for (size_t i = 0; i < a_iSizeToWrite; i++) {
        if (m_itData == m_vData.end()) {
          m_vData.push_back(l_pBuffer[i]);
          m_itData = m_vData.end();
        }
        else {
          *m_itData = l_pBuffer[i];
          m_itData++;
        }
      }

      return a_iSizeToWrite;
    }

    bool CMemoryWriteFile::flush() {
      return true;
    }

    const std::vector<unsigned char> &CMemoryWriteFile::getData() {
      return m_vData;
    }

    CMemoryReadFile::CMemoryReadFile(const std::string &a_sFileName, const std::vector<unsigned char> &a_vData) : irr::io::IReadFile(), m_sFileName(a_sFileName.c_str()) {
      m_vData = a_vData;
      m_itData = m_vData.begin();
    }

    CMemoryReadFile::~CMemoryReadFile() {
      printf("Memory read file deleted.\n");
    }

		size_t CMemoryReadFile::read(void *a_pBuffer, size_t a_iSizeToRead) {
      unsigned char *l_pBuffer = (unsigned char *)a_pBuffer;

      for (size_t i = 0; i < a_iSizeToRead; i++) {
        *l_pBuffer = *m_itData;
        m_itData++;
        l_pBuffer++;
        if (m_itData == m_vData.end())
          return i;
      }

      return a_iSizeToRead;
    }

		bool CMemoryReadFile::seek(long a_iFinalPos, bool a_bRelativeMovement) {
      if (a_bRelativeMovement) {
        m_itData += a_iFinalPos;
      }
      else {
        m_itData = m_vData.begin() + a_iFinalPos;
      }

      return true;
    }

		long CMemoryReadFile::getSize() const {
      return (long)m_vData.size();
    }

		long CMemoryReadFile::getPos() const {
      return (long)(m_itData - m_vData.begin());
    }

		const irr::io::path &CMemoryReadFile::getFileName() const {
      return m_sFileName;
    }
  }
}