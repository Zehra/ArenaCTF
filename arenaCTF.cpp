// arenaCTF.cpp

#include "bzfsAPI.h"

class arenaCTF : public bz_Plugin
{
public:
    virtual const char* Name (){return "arenaCTF";}
    virtual void Init ( const char* config);
    virtual void Event ( bz_EventData *eventData );
    virtual void Cleanup ( void );
    int grabStatus[4] = {-1, -1, -1, -1};
//
int teamToNum(bz_eTeamType team) {
  if (team == eRedTeam) {
    return 0;
  } else if (team == eGreenTeam) {
    return 1;
  } else if (team == eBlueTeam) {
    return 2;
  } else if (team == ePurpleTeam) {
    return 3;
  } else {
    return -1;
  }
}

bz_eTeamType flagAbbToTeam(const char* flagType) { 
  if (strcmp(flagType, "R*") == 0) {
    return eRedTeam;
  } else if (strcmp(flagType, "G*") == 0) {
    return eGreenTeam;
  } else if (strcmp(flagType, "B*") == 0) {
    return eBlueTeam;
  } else if (strcmp(flagType, "P*") == 0) {
    return ePurpleTeam;
  } else {
    return eNoTeam;
  }
}
//
};

BZ_PLUGIN(arenaCTF)

void arenaCTF::Init (const char* commandLine) {
    Register(bz_eAllowFlagGrab);
    Register(bz_eAllowCTFCaptureEvent);
    Register(bz_eFlagGrabbedEvent);
    Register(bz_eFlagDroppedEvent);
    Register(bz_eCaptureEvent);
}

void arenaCTF::Event(bz_EventData *eventData ){
    switch (eventData->eventType) {
    
     case bz_eFlagGrabbedEvent: {
        bz_FlagGrabbedEventData_V1* flagGrab = (bz_FlagGrabbedEventData_V1*)eventData;
        bz_eTeamType flagValue = flagAbbToTeam(flagGrab->flagType);
        if (flagValue != eNoTeam) {
            int playerID = flagGrab->playerID;
            int flagID = flagGrab->flagID;
            int slot = teamToNum(flagValue);
            if (slot != -1) {
                grabStatus[slot] = playerID;
            }
        }
     }break;
     
     case bz_eFlagDroppedEvent: {
        bz_FlagDroppedEventData_V1* flagDrop = (bz_FlagDroppedEventData_V1*)eventData;
        bz_eTeamType flagValue = flagAbbToTeam(flagDrop->flagType);
        if (flagValue != eNoTeam) {
            int playerID = flagDrop->playerID;
            int flagID = flagDrop->flagID;
            int slot = teamToNum(flagValue);
            if (slot != -1) {
                grabStatus[slot] = playerID;
            }
        }
     }break;
    
     case bz_eAllowFlagGrab: {
     bz_AllowFlagGrabData_V1* allowFlagGrab = (bz_AllowFlagGrabData_V1*)eventData;
     bz_eTeamType playerTeam = bz_getPlayerTeam(allowFlagGrab->playerID);
     bz_eTeamType flagValue = flagAbbToTeam(allowFlagGrab->flagType);

     if (playerTeam == flagValue && flagValue != eNoTeam) {
        allowFlagGrab->allow = false;
        int flagID = allowFlagGrab->flagID;
        int slot  = teamToNum(playerTeam);
        if ((slot != -1) && (flagID != -1)) {
            if (grabStatus[slot] != -1) {
                bz_resetFlag(flagID);
                grabStatus[slot] = -1;
            }
        }
     }
     
     }break;
     
     case bz_eAllowCTFCaptureEvent: {
     bz_AllowCTFCaptureEventData_V1* allowCapData = (bz_AllowCTFCaptureEventData_V1*)eventData;

     bz_eTeamType cappingTeam = allowCapData->teamCapping;
     bz_eTeamType cappedTeam  = allowCapData->teamCapped;
     int slot = teamToNum(cappingTeam);
     if (slot != -1) {
        if (grabStatus[slot] != -1) {
            allowCapData->allow = false;
        }
     } 
     }break;
     
     case bz_eCaptureEvent: {
        bz_CTFCaptureEventData_V1* captureData = (bz_CTFCaptureEventData_V1*)eventData;
        bz_eTeamType cappingTeam = captureData->teamCapping;
        bz_eTeamType cappedTeam  = captureData->teamCapped;

        int slot = teamToNum(cappedTeam);
        if (slot != -1) {
            if (grabStatus[slot] != -1) {
                grabStatus[slot] = -1;
            }
        }
     }break;

     default: { 
     }break;
     
    }
}

void arenaCTF::Cleanup (void) {
    Flush();
}
