#ifndef GFORCEADAPTER_H
#define GFORCEADAPTER_H
#include "Arduino.h"
////////////////////////////////////////////////////
////marco define about gforce data protocol
#define MAGNUM_LOW_INDEX      0       // 0xFF
#define MAGNUM_HIGH_INDEX     1       // 0xAA
#define EVENTTYP_INDEX        2       // event type
#define MSG_LEN_INDEX         3       // avaliable data length


#define MAGNUM_LOW           0xFF
#define MAGNUM_HIGH          0xAA

#define GFORCE_HEADER_LEN     0x04
//max byte of receive message
#define MSG_MAX_SIZE          50

//msg type definde
#define GFORCE_GESTURE        0x0F
#define GFORCE_QUATERNION     0x02

#define GforceSerial          Serial

/////////////////////////////////////////////////////
///////////enum defines
typedef enum Gesture
{
    Release,
    Fist,
    Spread,
    WaveIn,
    WaveOut,
    Pinch,
    Shoot,
    Unknown,
}Gesture_t;

typedef enum {
    Gforce_Quaternion = 0x02,
    Gforce_gesture = 0x0F,
    Gforce_Unknown         // have none available data
} GforceMsg_t;

typedef struct Quat
{
    unsigned char W[4];
    unsigned char X[4];
    unsigned char Y[4];
    unsigned char Z[4];
} quaternion_t;

typedef struct Euler
{
    float Yaw;
    float Pitch;
    float Roll;
} euler_t;

typedef struct GforceData{
  GforceMsg_t   msgType;
  union
  {
    quaternion_t quaternion;
    Gesture_t      gesture;
  } value;
}GforceData_t;


//It is only used in single thread
class gForceAdapter
{
    public:
      gForceAdapter();
      bool      updateData(void);
      GforceMsg_t  GetMsgType(void);
      bool      ConvertQuatToEuler(quaternion_t *quat,euler_t *euler);
      bool      avaliable(void);
      bool      GetAvaliableData(GforceData_t *gForcepkg);
    private:
      bool      GetQuaternion(quaternion_t *quat);
      bool      GetGesture(Gesture_t *gesture);
      int mGforceData[MSG_MAX_SIZE];
      bool mDataAvailabe;
      int mReceiveDataIndex;    
};
#endif
