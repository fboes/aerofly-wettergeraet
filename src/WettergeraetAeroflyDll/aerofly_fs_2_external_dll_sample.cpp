///////////////////////////////////////////////////////////////////////////////////////////////////
//
// file aerofly_fs_2_external_dll_sample.cpp
//
// PLEASE NOTE:  THE INTERFACE IN THIS FILE AND ALL DATA TYPES COULD BE SUBJECT TO SUBSTANTIAL
//               CHANGES WHILE AEROFLY FS 2 IS STILL RECEIVING UPDATES
//
// FURTHER NOTE: This sample just shows you how to read and send messages from the simulation
//               Some sample code is provided so see how to read and send messages
//
// 2017/11/23 - th/mb
//
// ---------------------------------------------------------------------------
//
// copyright (C) 2005-2017, Dr. Torsten Hans, Dr. Marc Borchers
// All rights reserved.
//
// Redistribution  and  use  in  source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of  source code must  retain the above  copyright notice,
//    this list of conditions and the disclaimer below.
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
//    documentation and/or other materials provided with the distribution.
//  - Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
// ARE  DISCLAIMED. 
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(WIN32) || defined(WIN64)
#if defined(_MSC_VER)
#pragma warning ( disable : 4530 )  // C++ exception handler used, but unwind semantics are not enabled
#pragma warning ( disable : 4577 )  // 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
#endif
#endif

#include "./shared/input/tm_external_message.h"

#include <windows.h>
#include <thread>
#include <vector>
#include <mutex>

// Wettergeraet
//#include "Argumentor.h"
//Argumentor argumentor;

static HINSTANCE global_hDLLinstance = NULL;




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// some ugly macros. we use this to be able to translate from string hash id to string
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#define TM_MESSAGE( a1, a2, a3, a4, a5, a6, a7 )       static tm_external_message Message##a1( ##a2, a3, a4, a5, a6 );
#define TM_MESSAGE_NAME( a1, a2, a3, a4, a5, a6, a7 )  a2,




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// list of messages that can be send/received
// to easy the interpretation of the messages, type, access flags and units are specified
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#define MESSAGE_LIST(F) \
F( AircraftAltitude               , "Aircraft.Altitude"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER                   , "altitude as measured by altimeter                                                                            " ) \
F( AircraftVerticalSpeed          , "Aircraft.VerticalSpeed"          , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "vertical speed                                                                                               " ) \
F( AircraftPitch                  , "Aircraft.Pitch"                  , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "pitch angle                                                                                                  " ) \
F( AircraftBank                   , "Aircraft.Bank"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "bank angle                                                                                                   " ) \
F( AircraftIndicatedAirspeed      , "Aircraft.IndicatedAirspeed"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "indicated airspeed                                                                                           " ) \
F( AircraftGroundSpeed            , "Aircraft.GroundSpeed"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "ground speed                                                                                                 " ) \
F( AircraftMagneticHeading        , "Aircraft.MagneticHeading"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "                                                                                                             " ) \
F( AircraftTrueHeading            , "Aircraft.TrueHeading"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "                                                                                                             " ) \
F( AircraftLatitude               , "Aircraft.Latitude"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "                                                                                                             " ) \
F( AircraftLongitude              , "Aircraft.Longitude"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "                                                                                                             " ) \
F( AircraftHeight                 , "Aircraft.Height"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER                   , "                                                                                                             " ) \
F( AircraftPosition               , "Aircraft.Position"               , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER                   , "                                                                                                             " ) \
F( AircraftOrientation            , "Aircraft.Orientation"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( AircraftVelocity               , "Aircraft.Velocity"               , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "velocity vector         in body system if 'Body' flag is set, in global system otherwise                     " ) \
F( AircraftAngularVelocity        , "Aircraft.AngularVelocity"        , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT_PER_SECOND      , "angular velocity        in body system if 'Body' flag is set (roll rate pitch rate yaw rate) in global system" ) \
F( AircraftAcceleration           , "Aircraft.Acceleration"           , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND_SQUARED, "aircraft acceleration   in body system if 'Body' flag is set, in global system otherwise                     " ) \
F( AircraftGravity                , "Aircraft.Gravity"                , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND_SQUARED, "gravity acceleration    in body system if 'Body' flag is set                                                 " ) \
F( AircraftWind                   , "Aircraft.Wind"                   , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "wind vector at current aircraft position                                                                     " ) \
F( AircraftRateOfTurn             , "Aircraft.RateOfTurn"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT_PER_SECOND      , "rate of turn                                                                                                 " ) \
F( AircraftMachNumber             , "Aircraft.MachNumber"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "mach number                                                                                                  " ) \
F( AircraftGear                   , "Aircraft.Gear"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "current gear position, zero is up, one is down, in between in transit                                        " ) \
F( AircraftFlaps                  , "Aircraft.Flaps"                  , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "flags                                                                                                        " ) \
F( AircraftThrottle               , "Aircraft.Throttle"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "current throttle setting                                                                                     " ) \
F( AircraftAirBrake               , "Aircraft.AirBrake"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( AircraftRadarAltitude          , "Aircraft.RadarAltitude"          , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER                   , "                                                                                                             " ) \
F( AircraftName                   , "Aircraft.Name"                   , tm_msg_datatype::TYPE_STRING,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "current aircraft short name ( name of folder in aircraft directory, eg c172 )                                " ) \
F( AircraftNearestAirport         , "Aircraft.NearestAirport"         , tm_msg_datatype::TYPE_STRING,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( AircraftCategoryJet            , "Aircraft.Category.Jet"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( AircraftCategoryGlider         , "Aircraft.Category.Glider"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( AircraftOnGround               , "Aircraft.OnGround"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "set if aircraft is on ground                                                                                 " ) \
F( AircraftOnRunway               , "Aircraft.OnRunway"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "set if aircraft is on ground and on a runway                                                                 " ) \
F( AircraftCrashed                , "Aircraft.Crashed"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( PerformanceSpeedVS0            , "Performance.Speed.VS0"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "minimum speed with flaps down, lower end of white arc                                                        " ) \
F( PerformanceSpeedVS1            , "Performance.Speed.VS1"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "minimum speed with flaps retracted, lower end of green arc                                                   " ) \
F( PerformanceSpeedVFE            , "Performance.Speed.VFE"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "maximum speed with flaps extended, upper end of white arc                                                    " ) \
F( PerformanceSpeedVNO            , "Performance.Speed.VNO"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "maneuvering speed, lower end of yellow arc                                                                   " ) \
F( PerformanceSpeedVNE            , "Performance.Speed.VNE"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "never exceed speed, red line                                                                                 " ) \
F( NavigationNAV1Frequency        , "Navigation.NAV1Frequency"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "NAV1 receiver active frequency                                                                               " ) \
F( NavigationNAV1StandbyFrequency , "Navigation.NAV1StandbyFrequency" , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "NAV1 receiver standby frequency                                                                              " ) \
F( NavigationNAV1FrequencySwap    , "Navigation.NAV1FrequencySwap"    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "swaps frequencies on NAV1 receiver                                                                           " ) \
F( NavigationDMEFrequency1        , "Navigation.DMEFrequency1"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "DME1 active frequency                                                                                        " ) \
F( NavigationNAV2Frequency        , "Navigation.NAV2Frequency"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "NAV2 receiver active frequency                                                                               " ) \
F( NavigationNAV2StandbyFrequency , "Navigation.NAV2StandbyFrequency" , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "NAV2 receiver standby frequency                                                                              " ) \
F( NavigationNAV2FrequencySwap    , "Navigation.NAV2FrequencySwap"    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "swaps frequencies on NAV2 receiver                                                                           " ) \
F( NavigationADF1Frequency        , "Navigation.ADF1Frequency"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "ADF1 receiver active frequency                                                                               " ) \
F( NavigationADF1StandbyFrequency , "Navigation.ADF1StandbyFrequency" , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "ADF1 receiver standby frequency                                                                              " ) \
F( NavigationCOM1Frequency        , "Navigation.COM1Frequency"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "COM1 transceiver active frequency                                                                            " ) \
F( NavigationCOM1StandbyFrequency , "Navigation.COM1StandbyFrequency" , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "COM1 transceiver standby frequency                                                                           " ) \
F( NavigationCOM1FrequencySwap    , "Navigation.COM1FrequencySwap"    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "swaps frequencies on COM1 transceiver                                                                        " ) \
F( NavigationCOM2Frequency        , "Navigation.COM2Frequency"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "COM2 transceiver active frequency                                                                            " ) \
F( NavigationCOM2StandbyFrequency , "Navigation.COM2StandbyFrequency" , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_HERTZ                   , "COM2 transceiver standby frequency                                                                           " ) \
F( NavigationCOM2FrequencySwap    , "Navigation.COM2FrequencySwap"    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "swaps frequencies on COM1 transceiver                                                                        " ) \
F( NavigationSelectedCourse1      , "Navigation.SelectedCourse1"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_RADIANT                 , "OBS / selected radial for NAV1 receiver                                                                      " ) \
F( NavigationSelectedCourse2      , "Navigation.SelectedCourse2"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_RADIANT                 , "OBS / selected radial for NAV1 receiver                                                                      " ) \
F( AutopilotMaster                , "Autopilot.Master"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( AutopilotDisengage             , "Autopilot.Disengage"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "disengage all autopilots                                                                                     " ) \
F( AutopilotHeading               , "Autopilot.Heading"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_RADIANT                 , "                                                                                                             " ) \
F( AutopilotVerticalSpeed         , "Autopilot.VerticalSpeed"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_METER_PER_SECOND        , "                                                                                                             " ) \
F( AutopilotSelectedSpeed         , "Autopilot.SelectedSpeed"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_METER_PER_SECOND        , "                                                                                                             " ) \
F( AutopilotSelectedAirspeed      , "Autopilot.SelectedAirspeed"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_METER_PER_SECOND        , "autopilot/flight director selected airspeed, speed bug                                                       " ) \
F( AutopilotSelectedHeading       , "Autopilot.SelectedHeading"       , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_RADIANT                 , "autopilot/flight director selected heading, heading bug                                                      " ) \
F( AutopilotSelectedAltitude      , "Autopilot.SelectedAltitude"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_METER                   , "autopilot/flight director selected altitude                                                                  " ) \
F( AutopilotSelectedVerticalSpeed , "Autopilot.SelectedVerticalSpeed" , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_METER_PER_SECOND        , "autopilot/flight director selected vertical speed                                                            " ) \
F( AutopilotEngaged               , "Autopilot.Engaged"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "set if autopilot is engaged                                                                                  " ) \
F( AutopilotActiveLateralMode     , "Autopilot.ActiveLateralMode"     , tm_msg_datatype::TYPE_STRING,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "internal name of the active lateral autopilot/flight director mode                                           " ) \
F( AutopilotArmedLateralMode      , "Autopilot.ArmedLateralMode"      , tm_msg_datatype::TYPE_STRING,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "internal name of the armed lateral autopilot/flight director mode                                            " ) \
F( AutopilotActiveVerticalMode    , "Autopilot.ActiveVerticalMode"    , tm_msg_datatype::TYPE_STRING,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "internal name of the active vertical autopilot/flight director mode                                          " ) \
F( AutopilotArmedVerticalMode     , "Autopilot.ArmedVerticalMode"     , tm_msg_datatype::TYPE_STRING,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "internal name of the armed lateral autopilot/flight director mode                                            " ) \
F( AutopilotActiveAutoThrottleMode, "Autopilot.ActiveAutoThrottleMode", tm_msg_datatype::TYPE_STRING,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "internal name of the active autothrottle mode                                                                " ) \
F( FlightDirectorPitch            , "FlightDirector.Pitch"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "flight director pitch angle relative to current pitch                                                        " ) \
F( FlightDirectorBank             , "FlightDirector.Bank"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "flight director bank angle relative to current bank                                                          " ) \
F( CopilotHeading                 , "Copilot.Heading"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_RADIANT                 , "                                                                                                             " ) \
F( CopilotAltitude                , "Copilot.Altitude"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER                   , "                                                                                                             " ) \
F( CopilotAirspeed                , "Copilot.Airspeed"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "                                                                                                             " ) \
F( CopilotVerticalSpeed           , "Copilot.VerticalSpeed"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_METER_PER_SECOND        , "                                                                                                             " ) \
F( CopilotAileron                 , "Copilot.Aileron"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CopilotElevator                , "Copilot.Elevator"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CopilotThrottle                , "Copilot.Throttle"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CopilotAutoRudder              , "Copilot.AutoRudder"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsSpeed                  , "Controls.Speed"                  , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "ignore/do not use  combined throttle, brake and reverse, copilot splits into other                           " ) \
F( ControlsThrottle1              , "Controls.Throttle1"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "throttle position for engine 1                                                                               " ) \
F( ControlsThrottle2              , "Controls.Throttle2"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "throttle position for engine 2                                                                               " ) \
F( ControlsThrottle3              , "Controls.Throttle3"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "throttle position for engine 3                                                                               " ) \
F( ControlsThrottle4              , "Controls.Throttle4"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "throttle position for engine 4                                                                               " ) \
F( ControlsThrottle1Move          , "Controls.Throttle1"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_PER_SECOND              , "throttle rate of change for engine 1                                                                         " ) \
F( ControlsThrottle2Move          , "Controls.Throttle2"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_PER_SECOND              , "throttle rate of change for engine 2                                                                         " ) \
F( ControlsThrottle3Move          , "Controls.Throttle3"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_PER_SECOND              , "throttle rate of change for engine 3                                                                         " ) \
F( ControlsThrottle4Move          , "Controls.Throttle4"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_PER_SECOND              , "throttle rate of change for engine 4                                                                         " ) \
F( ControlsPitchInput             , "Controls.Pitch.Input"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsPitchInputOffset       , "Controls.Pitch.Input"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Offset, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsRollInput              , "Controls.Roll.Input"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsRollInputOffset        , "Controls.Roll.Input"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Offset, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsYawInput               , "Controls.Yaw.Input"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsYawInputActive         , "Controls.Yaw.Input"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Active, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsFlaps                  , "Controls.Flaps"                  , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsFlapsEvent             , "Controls.Flaps"                  , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsGear                   , "Controls.Gear"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsGearToggle             , "Controls.Gear"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Toggle, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsWheelBrakeLeft         , "Controls.WheelBrake.Left"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsWheelBrakeRight        , "Controls.WheelBrake.Right"       , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsWheelBrakeLeftActive   , "Controls.WheelBrake.Left"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Active, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsWheelBrakeRightActive  , "Controls.WheelBrake.Right"       , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Active, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsAirBrake               , "Controls.AirBrake"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsAirBrakeActive         , "Controls.AirBrake"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Active, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsGliderAirBrake         , "Controls.GliderAirBrake"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsPropellerSpeed1        , "Controls.PropellerSpeed1"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsPropellerSpeed2        , "Controls.PropellerSpeed2"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsPropellerSpeed3        , "Controls.PropellerSpeed3"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsPropellerSpeed4        , "Controls.PropellerSpeed4"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsMixture1               , "Controls.Mixture1"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsMixture2               , "Controls.Mixture2"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsMixture3               , "Controls.Mixture3"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsMixture4               , "Controls.Mixture4"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsThrustReverse1         , "Controls.ThrustReverse1"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsThrustReverse2         , "Controls.ThrustReverse2"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsThrustReverse3         , "Controls.ThrustReverse3"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsThrustReverse4         , "Controls.ThrustReverse4"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsTrim                   , "Controls.Trim"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsTrimStep               , "Controls.Trim"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Step  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsTrimMove               , "Controls.Trim"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsAileronTrim            , "Controls.AileronTrim"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsRudderTrim             , "Controls.RudderTrim"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsTiller                 , "Controls.Tiller"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsPedalsDisconnect       , "Controls.PedalsDisconnect"       , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Toggle, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsNoseWheelSteering      , "Controls.NoseWheelSteering"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Toggle, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsLightingPanel          , "Controls.Lighting.Panel"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ControlsLightingInstruments    , "Controls.Lighting.Instruments"   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewDisplayName                , "View.DisplayName"                , tm_msg_datatype::TYPE_STRING,   tm_msg_flag::None  , tm_msg_access::ACCESS_R , tm_msg_units::UNIT_NONE                    , "name of current view                                                                                         " ) \
F( ViewInternal                   , "View.Internal"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "set view to last internal view                                                                               " ) \
F( ViewFollow                     , "View.Follow"                     , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "set view to last follow view                                                                                 " ) \
F( ViewExternal                   , "View.External"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "set view to last external view                                                                               " ) \
F( ViewCategory                   , "View.Category"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "change to next / previous view category (internal,follow,external), set last view in this category           " ) \
F( ViewMode                       , "View.Mode"                       , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "set next / previous view in current category                                                                 " ) \
F( ViewZoom                       , "View.Zoom"                       , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewPanHorizontal              , "View.Pan.Horizontal"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewPanHorizontalMove          , "View.Pan.Horizontal"             , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewPanVertical                , "View.Pan.Vertical"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewPanVerticalMove            , "View.Pan.Vertical"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewPanCenter                  , "View.Pan.Center"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewLookHorizontal             , "View.Look.Horizontal"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "momentarily look left / right                                                                                " ) \
F( ViewLookVertical               , "View.Look.Vertical"              , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "momentarily look up / down                                                                                   " ) \
F( ViewRoll                       , "View.Roll"                       , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewOffsetX                    , "View.OffsetX"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "offset (forward/backward) from view's default position                                                       " ) \
F( ViewOffsetXMove                , "View.OffsetX"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "change offset (forward/backward) from view's default position                                                " ) \
F( ViewOffsetY                    , "View.OffsetY"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "lateral offset from view's default position                                                                  " ) \
F( ViewOffsetYMove                , "View.OffsetY"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "change lateral offset from view's default position                                                           " ) \
F( ViewOffsetZ                    , "View.OffsetZ"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "vertical offset from view's default position                                                                 " ) \
F( ViewOffsetZMove                , "View.OffsetZ"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Move  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "change vertical offset from view's default position                                                          " ) \
F( ViewPosition                   , "View.Position"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewDirection                  , "View.Direction"                  , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewUp                         , "View.Up"                         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewFieldOfView                , "View.FieldOfView"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewAspectRatio                , "View.AspectRatio"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewFreePosition               , "View.FreePosition"               , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_METER                   , "the following 4 messages allow you to implement your own view                                                " ) \
F( ViewFreeLookDirection          , "View.FreeLookDirection"          , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewFreeUp                     , "View.FreeUp"                     , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( ViewFreeFieldOfView            , "View.FreeFieldOfView"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_RADIANT                 , "                                                                                                             " ) \
F( SimulationPause                , "Simulation.Pause"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Toggle, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "toggle pause on/off                                                                                          " ) \
F( SimulationFlightInformation    , "Simulation.FlightInformation"    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Toggle, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "show/hide the flight information at the top of the screen                                                    " ) \
F( SimulationMovingMap            , "Simulation.MovingMap"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Toggle, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "show/hide the moving map window                                                                              " ) \
F( SimulationSound                , "Simulation.Sound"                , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Toggle, tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "toggle sound on/off                                                                                          " ) \
F( SimulationLiftUp               , "Simulation.LiftUp"               , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "lift up the aircraft from current position                                                                   " ) \
F( SimulationSettingPosition      , "Simulation.SettingPosition"      , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_METER                   , "                                                                                                             " ) \
F( SimulationSettingOrientation   , "Simulation.SettingOrientation"   , tm_msg_datatype::TYPE_VECTOR4D, tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationSettingVelocity      , "Simulation.SettingVelocity"      , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_METER_PER_SECOND        , "                                                                                                             " ) \
F( SimulationSettingSet           , "Simulation.SettingSet"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationTimeChange           , "Simulation.TimeChange"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "change time of day                                                                                           " ) \
F( SimulationVisibility           , "Simulation.Visibility"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationTime                 , "Simulation.Time"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationWind                 , "Simulation.Wind"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationWindStrength         , "Simulation.WindStrength"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationWindDirection        , "Simulation.WindDirection"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationTurbulence           , "Simulation.Turbulence"           , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationThermalActivity      , "Simulation.ThermalActivity"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationUseMouseControl      , "Simulation.UseMouseControl"      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_RW, tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( SimulationPlaybackStart        , "Simulation.PlaybackStart"        , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "start playback if simulation is paused                                                                       " ) \
F( SimulationPlaybackStop         , "Simulation.PlaybackStop"         , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "stop playback                                                                                                " ) \
F( SimulationPlaybackSetPosition  , "Simulation.PlaybackPosition"     , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::None  , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "set playback position 0 - 1                                                                                  " ) \
F( SimulationExternalPosition     , "Simulation.ExternalPosition"     , tm_msg_datatype::TYPE_VECTOR3D, tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_METER                   , "                                                                                                             " ) \
F( SimulationExternalOrientation  , "Simulation.ExternalOrientation"  , tm_msg_datatype::TYPE_VECTOR4D, tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandExecute                 , "Command.Execute"                 , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandBack                    , "Command.Back"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandUp                      , "Command.Up"                      , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandDown                    , "Command.Down"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandLeft                    , "Command.Left"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandRight                   , "Command.Right"                   , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Event , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandMoveHorizontal          , "Command.MoveHorizontal"          , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandMoveVertical            , "Command.MoveVertical"            , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandRotate                  , "Command.Rotate"                  , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " ) \
F( CommandZoom                    , "Command.Zoom"                    , tm_msg_datatype::TYPE_DOUBLE,   tm_msg_flag::Value , tm_msg_access::ACCESS_W , tm_msg_units::UNIT_NONE                    , "                                                                                                             " )
	MESSAGE_LIST(TM_MESSAGE)


//////////////////////////////////////////////////////////////////////////////////////////////////
//
// a small helper function that shows the name of a message as plain text if an ID is passed
//
//////////////////////////////////////////////////////////////////////////////////////////////////
struct tm_message_type
{
	tm_string       String;
	tm_string_hash  StringHash;
	template <tm_uint32 N> constexpr tm_message_type(const char(&str)[N]) : String{ str }, StringHash{ str } { }
};

static std::vector<tm_message_type> MessageTypeList =
{
  MESSAGE_LIST(TM_MESSAGE_NAME)
};

static tm_string GetMessageName(const tm_external_message& message)
{
	for (const auto& mt : MessageTypeList)
	{
		if (message.GetID() == mt.StringHash.GetHash()) {
			return mt.String;
		}
		else {
			//return message.GetID();
		}
	}

	return tm_string("unknown");
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// code that opens a window and displays the received messages as text
// its meant just as a simple helper to get started with the DLL
//
// THIS CODE SHOULD NOT BE USED IN A PRODUCTION DLL
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <gdiplus.h>
#pragma message("including lib: gdiplus.lib")
#pragma comment(lib, "gdiplus.lib")

static ULONG_PTR                         Global_DebugOutput_gdiplusToken = NULL;
static std::thread                       Global_DebugOutput_Thread;
static HWND                              Global_DebugOutput_Window = NULL;
static bool                              Global_DebugOutput_WindowCloseMessage = false;

static std::vector<tm_external_message>  MessageListReceive;
static std::vector<tm_external_message>  MessageListCopy;
static std::vector<tm_external_message>  MessageListDebugOutput;
static std::mutex                        MessageListMutex;
static double                            MessageDeltaTime = 0;

static std::mutex                        VR_DataMutex;
static tm_vector3d                       VR_Head_Position;
static tm_matrix3d                       VR_Head_Orientation;
static int                               VR_NumControllers = 0;
static tm_vector3d                       VR_Controller0_Position;
static tm_quaterniond                    VR_Controller0_Orientation;
static tm_vector3d                       VR_Controller1_Position;
static tm_quaterniond                    VR_Controller1_Orientation;

const int SAMPLE_WINDOW_WIDTH = 640;
const int SAMPLE_WINDOW_HEIGHT = 1080;

void DebugOutput_Draw(HDC hDC)
{
	// clear and draw to a bitmap instead to the hdc directly to avoid flicker
	Gdiplus::Bitmap     backbuffer(SAMPLE_WINDOW_WIDTH, SAMPLE_WINDOW_HEIGHT, PixelFormat24bppRGB);
	Gdiplus::Graphics   graphics(&backbuffer);
	Gdiplus::SolidBrush black(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::FontFamily fontFamily(L"Courier New");
	Gdiplus::Font       font(&fontFamily, 12, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::Color      clearcolor(255, 220, 232, 244);

	graphics.Clear(clearcolor);

	{
		std::lock_guard<std::mutex> lock_guard{ MessageListMutex };
		MessageListDebugOutput.swap(MessageListCopy);
	}


	{
		float y = 4;
		WCHAR text1[256];
		WCHAR text2[256];

		_snwprintf_s(&text1[0], 255, _TRUNCATE, L"messages = %llu  dt=%f", MessageListDebugOutput.size(), MessageDeltaTime);
		graphics.DrawString(&text1[0], -1, &font, Gdiplus::PointF(10, y), &black);
		y += 16;

		int index = 0;

		// AircraftHeight
		// AircraftLongitude
		// AircraftLatitude

		for (auto& message : MessageListDebugOutput)
		{
			auto message_name = GetMessageName(message);
			auto message_name_string = message_name.c_str();

			if (strcmp(message_name_string, "Aircraft.NearestAirport") != 0
				|| (
					strncmp(message_name_string, "Aircraft", 8) != 0
					&& strncmp(message_name_string, "Autopilo", 8) != 0
					&& strncmp(message_name_string, "Flightdi", 8) != 0
					&& strncmp(message_name_string, "Performa", 8) != 0
					&& strncmp(message_name_string, "Controls", 8) != 0
					&& strncmp(message_name_string, "View", 4) != 0
					)) {
				switch (message.GetDataType())
				{
				case tm_msg_datatype::TYPE_VOID: {                                       _snwprintf_s(&text1[0], 255, _TRUNCATE, L"void");                                        } break;
				case tm_msg_datatype::TYPE_INT: { const auto v = message.GetInt();      _snwprintf_s(&text1[0], 255, _TRUNCATE, L"v=%lld", v);                                   } break;
				case tm_msg_datatype::TYPE_DOUBLE: { const auto v = message.GetDouble();   _snwprintf_s(&text1[0], 255, _TRUNCATE, L"v=%.3f", v);                                   } break;
				case tm_msg_datatype::TYPE_VECTOR2D: { const auto v = message.GetVector2d(); _snwprintf_s(&text1[0], 255, _TRUNCATE, L"v=(%.3f %.3f)", v.x, v.y);                     } break;
				case tm_msg_datatype::TYPE_VECTOR3D: { const auto v = message.GetVector3d(); _snwprintf_s(&text1[0], 255, _TRUNCATE, L"v=(%.3f %.3f %.3f)", v.x, v.y, v.z);           } break;
				case tm_msg_datatype::TYPE_VECTOR4D: { const auto v = message.GetVector4d(); _snwprintf_s(&text1[0], 255, _TRUNCATE, L"v=(%.3f %.3f %.3f %.3f)", v.x, v.y, v.z, v.w); } break;
				case tm_msg_datatype::TYPE_STRING: { const auto v = message.GetString();   _snwprintf_s(&text1[0], 255, _TRUNCATE, L"s='%hs'", v.c_str());                          } break;
				}

				//_snwprintf_s( &text2[0], 255, _TRUNCATE, L"%3u:  size=%u", ++index, message.GetMessageSize() );
				_snwprintf_s(&text2[0], 255, _TRUNCATE, L"%3u:", ++index);
				graphics.DrawString(&text2[0], -1, &font, Gdiplus::PointF(0, y), &black);

				graphics.DrawString(&text1[0], -1, &font, Gdiplus::PointF(40, y), &black);

				_snwprintf_s(&text1[0], 255, _TRUNCATE, L"'%hs'  flags=%u", message_name.c_str(), message.GetFlags().GetFlags());
				graphics.DrawString(&text1[0], -1, &font, Gdiplus::PointF(300, y), &black);

				y += 16;
				if (y > SAMPLE_WINDOW_HEIGHT)
					break;
			}
		}
	}

	// copy 'backbuffer' image to screen
	Gdiplus::Graphics graphics_final(hDC);
	graphics_final.DrawImage(&backbuffer, 0, 0);
}

LRESULT WINAPI DebugOutput_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		auto hDC = BeginPaint(hWnd, &ps);
		DebugOutput_Draw(hDC);
		EndPaint(hWnd, &ps);
	}
	return 0;

	case WM_TIMER:
		InvalidateRect(hWnd, 0, FALSE);
		return 0;

	case WM_CLOSE:
		Global_DebugOutput_WindowCloseMessage = true;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void DebugOutput_CreateWindow(HINSTANCE hInstance)
{
	const char classname[] = "aerofly_external_dll_sample";

	//
	// init gdi+
	//
	Gdiplus::GdiplusStartupInput startupinput;
	auto status = GdiplusStartup(&Global_DebugOutput_gdiplusToken, &startupinput, NULL);

	//
	// fill in window class structure and register the class
	//
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DebugOutput_WndProc;                // Window Procedure
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;                          // Owner of this class
	wc.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);      // Default color
	wc.lpszMenuName = NULL;
	wc.lpszClassName = classname;
	RegisterClass(&wc);

	Global_DebugOutput_WindowCloseMessage = false;

	Global_DebugOutput_Window = CreateWindow(classname, "Aerofly External DLL Sample",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT,
		0, SAMPLE_WINDOW_WIDTH, SAMPLE_WINDOW_HEIGHT,
		NULL,       // no parent window
		NULL,       // Use the window class menu.
		hInstance,  // This instance owns this window
		NULL);     // We don't use any extra data


	auto s_width = GetSystemMetrics(SM_CXSCREEN);
	auto s_height = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(Global_DebugOutput_Window, HWND_TOP, s_width - SAMPLE_WINDOW_WIDTH, 0, SAMPLE_WINDOW_WIDTH, SAMPLE_WINDOW_HEIGHT, SWP_SHOWWINDOW);
	// set up timers
	SetTimer(Global_DebugOutput_Window, 0, 500, 0);

	MSG msg;
	while (!Global_DebugOutput_WindowCloseMessage && GetMessage(&msg, Global_DebugOutput_Window, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(Global_DebugOutput_Window);
	Global_DebugOutput_Window = NULL;

	//
	// shutdown gdi+
	//
	Gdiplus::GdiplusShutdown(Global_DebugOutput_gdiplusToken);
}

void DebugOutput_WindowUpdate(const double delta_time, const std::vector<tm_external_message>& message_list_receive)
{
	// this is just for the debug output window
	std::lock_guard<std::mutex> lock_guard{ MessageListMutex };
	MessageListCopy = message_list_receive;
	MessageDeltaTime = delta_time;
}

void DebugOutput_WindowOpen()
{
	Global_DebugOutput_Thread = std::thread(DebugOutput_CreateWindow, global_hDLLinstance);
}

void DebugOutput_WindowClose()
{
	if (Global_DebugOutput_Window != NULL)
	{
		PostMessage(Global_DebugOutput_Window, WM_QUIT, 0, 0);
	}
	Global_DebugOutput_Thread.join();
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main entry point for the DLL
//
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HANDLE hdll, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_ATTACH:
		global_hDLLinstance = (HINSTANCE)hdll;
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// interface functions to Aerofly FS 2
//
//////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
{
	__declspec(dllexport) int Aerofly_FS_2_External_DLL_GetInterfaceVersion()
	{
		return TM_DLL_INTERFACE_VERSION;
	}

	__declspec(dllexport) bool Aerofly_FS_2_External_DLL_Init(const HINSTANCE Aerofly_FS_2_hInstance)
	{
		DebugOutput_WindowOpen();
		return true;
	}

	__declspec(dllexport) void Aerofly_FS_2_External_DLL_Shutdown()
	{
		DebugOutput_WindowClose();
	}

	__declspec(dllexport) void Aerofly_FS_2_External_DLL_Update(const tm_double         delta_time,
		const tm_uint8* const  message_list_received_byte_stream,
		const tm_uint32         message_list_received_byte_stream_size,
		const tm_uint32         message_list_received_num_messages,
		tm_uint8* message_list_sent_byte_stream,
		tm_uint32& message_list_sent_byte_stream_size,
		tm_uint32& message_list_sent_num_messages,
		const tm_uint32         message_list_sent_byte_stream_size_max)
	{
		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// build a list of messages that the simulation is sending
		//
		MessageListReceive.clear();

		tm_uint32 message_list_received_byte_stream_pos = 0;
		for (tm_uint32 i = 0; i < message_list_received_num_messages; ++i)
		{
			auto edm = tm_external_message::GetFromByteStream(message_list_received_byte_stream, message_list_received_byte_stream_pos);
			MessageListReceive.emplace_back(edm);
		}


		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// this is just for the debug output window
		//
		DebugOutput_WindowUpdate(delta_time, MessageListReceive);


		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// parse the message list
		//
		tm_vector3d aircraft_position;
		tm_double aircraft_latitude;
		tm_double aircraft_longitude;
		tm_double aircraft_heigth;

		for (const auto& message : MessageListReceive)
		{
			const auto sh = message.GetStringHash().GetHash();

			if (sh == MessageAircraftPosition.GetID())
			{
				aircraft_position = message.GetVector3d();
			}
			else if (sh == MessageAircraftAltitude.GetID())  // alternatively you can write -> if( message.GetStringHash() == "Aircraft.Altitude" )
			{
				tm_double altitude = message.GetDouble();
			}
			else if (message.GetStringHash() == "Aircraft.Latitude")
			{
				aircraft_latitude = message.GetDouble();
			}
			else if (message.GetStringHash() == "Aircraft.Longitude")
			{
				aircraft_longitude = message.GetDouble();
			}
			else if (message.GetStringHash() == "Aircraft.Height")
			{
				aircraft_heigth = message.GetDouble();
			}
			// and so on....
		}


		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// build a list of message that will be sent back to the simulation
		//
		message_list_sent_byte_stream_size = 0;
		message_list_sent_num_messages = 0;

		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// example 0: Get WGS84 position, ping every 30 sec
	//    static double extime = 0.0; 
	//    extime += delta_time;
	//
	//    if( extime > 30.0 )
	//    {
	//      extime = 0.0;
	//      // log aircraft_latitude; aircraft_longitude; aircraft_heigth; timecode
	//    }



		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// example 1: change time of day continously
		//
		//MessageSimulationTimeChange.SetValue( 1.0 );
		//MessageSimulationTimeChange.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );


		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// example 2: implement a free view. in this example the camera just rotates around the aircraft
		//
	//    static double time = 0;
	//    time += delta_time;
	//
	//    const auto world_east  = tmcoordinates_GetEastAt( aircraft_position );
	//    const auto world_north = tmcoordinates_GetNorthAt( aircraft_position );
	//    const auto world_up    = tmcoordinates_GetUpAt( aircraft_position );
	//    
	//    const auto view_up             = world_up;
	//    const auto view_position       = aircraft_position + 15.0 *( sin( time )*world_east + cos( time )*world_north ) + ( 1.0 + cos( time )*cos( time ) )*world_up;
	//    const auto view_look_direction = Normalized( aircraft_position - view_position );
	//
	//    MessageViewFreePosition.SetValue( view_position );
	//    MessageViewFreePosition.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//
	//    MessageViewFreeLookDirection.SetValue( view_look_direction );
	//    MessageViewFreeLookDirection.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//
	//    MessageViewFreeUp.SetValue( view_up );
	//    MessageViewFreeUp.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//
	//    MessageViewFreeFieldOfView.SetValue( tm_helper_deg_to_rad( 100.0 ) );
	//    MessageViewFreeFieldOfView.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );


		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// example 3: set aileron input
		//
	//    MessageControlsRollInput.SetValue( sin( time ) );
	//    MessageControlsRollInput.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );


		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// example 4: reposition the aircraft every 20 seconds to a position near birrfeld
		//
	//    static double extime = 0.0; 
	//    extime += delta_time;
	//
	//    if( extime > 20.0 )
	//    {
	//      extime = 0.0;
	//
	//      MessageSimulationSettingPosition.SetValue( tm_vector3d( 4264642.1 + 500.0, 616894.2 + 500.0, 4693293.4 + 500.0 ) );
	//      
	//      tm_matrix3d orientation = tm_matrix3d( -0.213045, -0.691774,  0.689970,
	//                                              0.957133, -0.289600,  0.005181,
	//                                              0.196231,  0.661497,  0.723818  );
	//
	//      tm_vector3d velocity = 100.0 * ( orientation * tm_vector3d( 1.0, 0.0, 0.0 ) );
	//
	//      tm_quaterniond q = tm_MatrixToQuaternion<tm_matrix3d,double>( orientation );
	//
	//      MessageSimulationSettingOrientation.SetValue( tm_vector4d( q.r, q.x, q.y, q.z ) );
	//      MessageSimulationSettingVelocity.SetValue( velocity );
	//
	//      MessageSimulationSettingPosition.AddToByteStream(    message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//      MessageSimulationSettingOrientation.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//      MessageSimulationSettingVelocity.AddToByteStream(    message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//      MessageSimulationSettingSet.AddToByteStream(         message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//    }


		//////////////////////////////////////////////////////////////////////////////////////////////
		//
		// example 5: pause the simulation after 10 seconds, set playback position and start playback 
		//
	//    static double time = 0;
	//
	//    if( time < 10 && time + delta_time >= 10 )
	//    {
	//      MessageSimulationPause.SetValue( 1.0 );
	//      MessageSimulationPause.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//    }
	//    
	//    if( time < 11 && time + delta_time >= 11 )
	//    {
	//      MessageSimulationPlaybackSetPosition.SetValue( 0.5 );
	//      MessageSimulationPlaybackSetPosition.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//    }
	//    
	//    if( time < 12 && time + delta_time >= 12 )
	//    {
	//      MessageSimulationPlaybackStart.SetValue( 1.0 );
	//      MessageSimulationPlaybackStart.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//    }
	//
	//    time += delta_time;


	//
	//    //
	//    // send messages
	//    //
	//    MessageSimulationExternalPosition.SetValue( position );
	//    MessageSimulationExternalPosition.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	//    MessageSimulationExternalOrientation.SetValue( tm_vector4d( q.r, q.x, q.y, q.z ) );
	//    MessageSimulationExternalOrientation.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
	}
}


