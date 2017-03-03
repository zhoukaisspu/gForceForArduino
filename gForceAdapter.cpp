#include "gForceAdapter.h"

//////////////////////////////////////////////////////////////////////////////////////
////////////private function in class gForceAdapter
 bool  gForceAdapter::GetGesture(Gesture_t *gesture)
 {
      if(mDataAvailabe) {
          if((mGforceData[EVENTTYP_INDEX] & 0x7F) == GFORCE_GESTURE)
          {
              *gesture = (mGforceData[EVENTTYP_INDEX]& 0x80) ? (Gesture_t)mGforceData[MSG_LEN_INDEX+2] : (Gesture_t)mGforceData[MSG_LEN_INDEX+1];
              return true; 
          } else {
            return false;
          }
      }
      return false;
 }

 bool  gForceAdapter::GetQuaternion(quaternion_t *quat)
 {
      if(mDataAvailabe) {
          if((mGforceData[EVENTTYP_INDEX] & 0x7F) == GFORCE_QUATERNION) {
                  unsigned int tmpindex = (mGforceData[EVENTTYP_INDEX]&0x80) ? 2 : 1;
                  for(unsigned char index =0;index <4; index++)
                  {
                    quat->W[index] = mGforceData[MSG_LEN_INDEX+tmpindex+index];
                  }
                  for(unsigned char index =0;index <4; index++)
                  {
                    quat->X[index] = mGforceData[MSG_LEN_INDEX+tmpindex+4+index];
                  }
                  for(unsigned char index =0;index <4; index++)
                  {
                    quat->Y[index] = mGforceData[MSG_LEN_INDEX+tmpindex+8+index];
                  }
                  for(unsigned char index =0;index <4; index++)
                  {
                    quat->Z[index] = mGforceData[MSG_LEN_INDEX+tmpindex+12+index];
                  }
          }
          return false;
      }
      return false;
 }

///////////////////////////////////////////////////////////////////////////////////////
////////////public function in class gForceAdapter
gForceAdapter::gForceAdapter()
{
      mDataAvailabe = false;
      mReceiveDataIndex = 0; 
      GforceSerial.begin(115200);
}

bool gForceAdapter::updateData(void)
{              
            while(1)
            {
              if(GforceSerial.available()) {
                     int tmpData = GforceSerial.read();
                     if(tmpData == -1) {
                          return false;
                     }
                 mDataAvailabe = false;
                if(mReceiveDataIndex == MAGNUM_LOW_INDEX) {
                      if(tmpData == MAGNUM_LOW) {
                          mGforceData[mReceiveDataIndex] = tmpData;
                          mReceiveDataIndex ++;
                      }
                 } else if(mReceiveDataIndex == MAGNUM_HIGH_INDEX) {
                        if(tmpData == MAGNUM_HIGH) {
                            mGforceData[mReceiveDataIndex] = tmpData;
                            mReceiveDataIndex ++;
                          }
                  } else if(mReceiveDataIndex <=3) {
                            mGforceData[mReceiveDataIndex] = tmpData;
                            mReceiveDataIndex ++;
                  } else if(mReceiveDataIndex > 3) {
                            mGforceData[mReceiveDataIndex] = tmpData;
                            mReceiveDataIndex++;
                  if(mReceiveDataIndex == mGforceData[MSG_LEN_INDEX] + GFORCE_HEADER_LEN){
                          mReceiveDataIndex = 0;
                          mDataAvailabe = true;
                          return true;
                      }
                 }      
              }
              return false;
            }
}

bool  gForceAdapter::GetAvaliableData(GforceData_t *gForcepkg)
{
    bool bAvaliableFlag = true;
    if(mDataAvailabe){
        GforceMsg_t msgtype = GetMsgType();
        switch(msgtype) {
          case Gforce_Quaternion:
                gForcepkg-> msgType = Gforce_Quaternion;
                 GetQuaternion(&((gForcepkg->value).quaternion));
          break;
          case Gforce_gesture:
                gForcepkg-> msgType = Gforce_gesture;
                GetGesture(&((gForcepkg->value).gesture));
          break;
          default:
                gForcepkg-> msgType = Gforce_Unknown;
                bAvaliableFlag = false;
          break;
        }
    } else {
        bAvaliableFlag = false;
    }
    return bAvaliableFlag;
}

bool  gForceAdapter::avaliable(void)
{
  return mDataAvailabe;
}

GforceMsg_t gForceAdapter::GetMsgType(void)
{
   if(mDataAvailabe) {
        return (GforceMsg_t)(mGforceData[EVENTTYP_INDEX]&0x7F);
   }
   return Gforce_Unknown;
}

