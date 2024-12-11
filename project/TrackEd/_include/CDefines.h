// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  // Some defines for user events
  const irr::s32 c_iStartupFinished    = 23;
  const irr::s32 c_iSceneWillBeCleared = 28;
  const irr::s32 c_iSceneLoaded        = 34;
  const irr::s32 c_iUpdateShader       = 42;

  const std::wstring c_sAddSegment    = L"Add Road Segment";
  const std::wstring c_sAddCheckpoint = L"Add Checkpoint";
  const std::wstring c_sAddIncline    = L"Add Incline";
  const std::wstring c_sAddDecline    = L"Add Decline";
  const std::wstring c_sAddJump       = L"Add Jump";
  const std::wstring c_sAddGap        = L"Add Gap";
  const std::wstring c_sFinishTrack   = L"Finish Track";
  const std::wstring c_sAddReference  = L"Add Reference Point";
  const std::wstring c_sFinishSegment = L"Finish Segment";

  const std::string c_sTypeBezier     = "Bezier";
  const std::string c_sTypeCheckpoint = "Checkpoint";
}
