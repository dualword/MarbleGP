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
  const irr::s32 c_iRecalcTrack        = 46;
  const irr::s32 c_iRenderScene        = 58;

  const std::wstring c_sAddFlatSegment = L"Add Road Segment";
  const std::wstring c_sAddCheckpoint  = L"Add Checkpoint";
  const std::wstring c_sAddRamp        = L"Add Ramp";
  const std::wstring c_sAddJump        = L"Add Jump";
  const std::wstring c_sAddGap         = L"Add Gap";
  const std::wstring c_sFinishTrack    = L"Finish Track";
  const std::wstring c_sAddReference   = L"Add Reference Point";
  const std::wstring c_sAddStraight    = L"Add Straight";
  const std::wstring c_sAddTurnLeft    = L"Add Left turn";
  const std::wstring c_sAddTurnRight   = L"Add Right turn";
  const std::wstring c_sAddTurn45      = L"45 degrees";
  const std::wstring c_sAddTurn90      = L"90 degrees";
  const std::wstring c_sAddTurn135     = L"135 degrees";
  const std::wstring c_sAddTurn180     = L"180 degrees";
  const std::wstring c_sAddTurnFree    = L"Free turn";
  
  const std::string c_sTypeFlat       = "FlatSegment";
  const std::string c_sTypeCheckpoint = "Checkpoint";
  const std::string c_sTypeRamp       = "Ramp";
  const std::string c_sTypeTurn       = "Turn";
  const std::string c_sTypeStraight   = "Straight";
}
