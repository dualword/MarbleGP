#include <sound/OggHelpers.h>
#include <string>

namespace dustbin {
  namespace audio {

    size_t readOggCallback(void *l_pDst, size_t l_iSize1, size_t l_iSize2, void *l_pFh) {
      SOggFile *l_pOggFile = reinterpret_cast<SOggFile*>(l_pFh);
      size_t l_iLen = l_iSize1 * l_iSize2;
      if (l_pOggFile->m_pCurrent + l_iLen > l_pOggFile->m_pFileData + l_pOggFile->m_iFileSize) {
        l_iLen = l_pOggFile->m_pFileData + l_pOggFile->m_iFileSize - l_pOggFile->m_pCurrent;
      }
      memcpy(l_pDst, l_pOggFile->m_pCurrent, l_iLen);
      l_pOggFile->m_pCurrent += l_iLen;
      return l_iLen;
    }

    int seekOggCallback(void *l_pFh, ogg_int64_t l_iTo, int l_iType) {
      SOggFile *l_pOggFile = reinterpret_cast<SOggFile*>(l_pFh);

      switch( l_iType ) {
        case SEEK_CUR:
          l_pOggFile->m_pCurrent += l_iTo;
          break;
        case SEEK_END:
          l_pOggFile->m_pCurrent = l_pOggFile->m_pFileData + l_pOggFile->m_iFileSize - l_iTo;
          break;
        case SEEK_SET:
          l_pOggFile->m_pCurrent = l_pOggFile->m_pFileData + l_iTo;
          break;
        default:
          return -1;
      }
      if ( l_pOggFile->m_pCurrent < l_pOggFile->m_pFileData ) {
        l_pOggFile->m_pCurrent = l_pOggFile->m_pFileData;
        return -1;
      }
      if ( l_pOggFile->m_pCurrent > l_pOggFile->m_pFileData + l_pOggFile->m_iFileSize ) {
        l_pOggFile->m_pCurrent = l_pOggFile->m_pFileData + l_pOggFile->m_iFileSize;
        return -1;
      }
      return 0;
    }

    int closeOggCallback(void *l_pFh) {
      return 0;
    }

    long tellOggCallback(void *l_pFh) {
      SOggFile *l_pFile = reinterpret_cast<SOggFile *>(l_pFh);
      return (long)(l_pFile->m_pCurrent - l_pFile->m_pFileData);
    }
  }
}