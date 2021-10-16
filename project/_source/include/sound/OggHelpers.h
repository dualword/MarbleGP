// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <stddef.h>
#include <ogg/ogg.h>

namespace dustbin {
  namespace audio {

    struct SOggFile {
      char   *m_pCurrent;
      char   *m_pFileData;
      size_t  m_iFileSize;
    };

    size_t readOggCallback(void *a_pDst, size_t a_iSize1, size_t a_iSize2, void *a_pFh);
    int seekOggCallback(void *a_pFh, ogg_int64_t a_iTo, int a_iType);
    int closeOggCallback(void *l_pFh);
    long tellOggCallback(void *l_pFh);

  }
}