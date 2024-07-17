#ifndef MY_LD2410_H
#define MY_LD2410_H
#include <Arduino.h>
#define LD2410_BAUD_RATE 256000
#define LD2410_BUFFER_SIZE 0x40

class MyLD2410
{
public:
  enum Response
  {
    FAIL = 0,
    ACK,
    DATA
  };
  struct ValuesArray
  {
    byte values[9];
    byte N;
    ValuesArray &operator=(const ValuesArray &other)
    {
      if (this != &other)
      {
        N = other.N;
        for (byte i = 0; i < N; i++)
          values[i] = other.values[i];
      }
      return *this;
    }
    void forEach(void (*func)(const byte &val)) const
    {
      for (byte i = 0; i < N; i++)
        func(values[i]);
    }
  };
  struct SensorData
  {
    byte status;
    unsigned long timestamp;
    unsigned long mTargetDistance;
    byte mTargetSignal;
    unsigned long sTargetDistance;
    byte sTargetSignal;
    unsigned long distance;
    // Enhanced data
    ValuesArray mTargetSignals;
    ValuesArray sTargetSignals;
  };

private:
  SensorData sData;
  ValuesArray stationaryThresholds;
  ValuesArray movingThresholds;
  byte maxRange = 0;
  byte noOne_window = 0;
  unsigned long version = 0;
  unsigned long bufferSize = 0;
  byte MAC[6];
  String MACstr = "";
  String firmware = "";
  int fineRes = -1;
  bool isEnhanced = false;
  bool isConfig = false;
  unsigned long timeout = 2000;
  unsigned long dataLifespan = 500;
  byte inBuf[LD2410_BUFFER_SIZE];
  byte inBufI = 0;
  byte headBuf[4];
  byte headBufI = 0;
  Stream *sensor;
  bool _debug = false;
  bool isDataValid();
  bool readFrame();
  bool sendCommand(const byte *command);
  bool processAck();
  bool processData();

public:
  /**
   * @brief Construct a new MyLD2410 object
   *
   * @param serial - a reference to a stream object (sensorSerial)
   * @param debug - a flag that controls whether debug data will be sent to Serial
   */
  MyLD2410(Stream &serial, bool debug = false);

  // CONTROLS

  /**
   * @brief Call this function in setup() to ascertain whether the device is responding
   */
  bool begin();

  /**
   * @brief Call this function to gracefully close the sensor. Useful for entering sleep mode.
   */
  void end();

  /**
    @brief Call this function in the main loop
    @return MyLD2410::DATA = (true) if the latest frame contained data
    @return MyLD2410::ACK  = (true) if the latest frame contained a reply to a command
    @return MyLD2410::FAIL = (false) if no useful info was processed
    */
  Response check();

  // GETTERS

  /**
   * @brief Check whether the device is in config mode
   * (accepts commands)
   */
  bool inConfigMode();

  /**
   * @brief Check whether the device is in basic mode
   * (continuously sends basic presence data)
   */
  bool inBasicMode();

  /**
   * @brief Check whether the device is in enhanced mode
   * (continuously sends enhanced presence data)
   */
  bool inEnhancedMode();

  /**
   * @brief Check whether presence was detected in the latest frame
   */
  bool presenceDetected();

  /**
   * @brief Check whether a stationary target was detected in the latest frame
   */
  bool stationaryTargetDetected();

  /**
   * @brief Get the distance to the stationary target in [cm]
   *
   * @return const unsigned long - distance in [cm]
   */
  const unsigned long stationaryTargetDistance();

  /**
   * @brief Get the signal from the stationary target
   *
   * @return const byte - signal value [0:100]
   */
  const byte stationaryTargetSignal();

  /**
   * @brief Get the Stationary Signals object, if in enhanced mode
   *
   * @return const MyLD2410::ValuesArray& - the signals for each detection gate
   */
  const ValuesArray &getStationarySignals();

  /**
   * @brief Check whether a moving target was detected in the latest frame
   */
  bool movingTargetDetected();

  /**
   * @brief Get the distance to the moving target in [cm]
   *
   * @return const unsigned long - distance in [cm]
   */
  const unsigned long movingTargetDistance();

  /**
   * @brief Get the signal from the moving target
   *
   * @return const byte - signal value [0:100]
   */
  const byte movingTargetSignal();

  /**
   * @brief Get the Moving Signals object, if in enhanced mode
   *
   * @return const MyLD2410::ValuesArray& - the signals for each detection gate
   */
  const ValuesArray &getMovingSignals();

  const unsigned long detectedDistance();

  /**
   * @brief Get the Bluetooth MAC address as an array byte[6]
   *
   * @return const byte*
   */
  const byte *getMAC();

  /**
   * @brief Get the Bluetooth MAC address as a String
   *
   * @return String
   */
  String getMACstr();

  /**
   * @brief Get the Firmware as a String
   *
   * @return String
   */
  String getFirmware();

  /**
   * @brief Get the protocol version
   *
   * @return unsigned long
   */
  unsigned long getVersion();

  /**
   * @brief Get the SensorData object
   *
   * @return const SensorData&
   */
  const SensorData &getSensorData();

  /**
   * @brief Get the presence status as a c-string
   *
   * @return const char* ["No target", "Moving only",
   * "Stationary only", "Both moving and stationary"]
   */
  const char *statusString();

  /**
   * @brief Get the sensor resolution (gate-width) in [cm]
   *
   * @return byte either 20 or 75 on success, 0 on failure
   */
  byte getResolution();

  // parameters

  /**
   * @brief Get the detection thresholds for moving targets
   *
   * @return const ValuesArray&
   */
  const ValuesArray &getMovingThresholds();

  /**
   * @brief Get the detection thresholds for stationary targets
   *
   * @return const ValuesArray&
   */
  const ValuesArray &getStationaryThresholds();

  /**
   * @brief Get the maximum detection gate
   *
   * @return byte
   */
  byte getRange();

  /**
   * @brief Get the maximum detection range in [cm]
   *
   * @return unsigned long
   */
  unsigned long getRange_cm();

  /**
   * @brief Get the time-lag of "no presence" in [s].
   * The sensor begins reporting "no presence"
   * only after no motion has been detected for that many seconds.
   *
   * @return byte
   */
  byte getNoOneWindow();
  // end parameters

  // REQUESTS

  /**
   * @brief Request config mode
   *
   * @param enable [true]/false
   * @return true on success
   */
  bool configMode(bool enable = true);

  /**
   * @brief Request enhanced mode
   *
   * @param enable [true]/false
   * @return true on success
   */
  bool enhancedMode(bool enable = true);

  /**
   * @brief Request the Bluetooth MAC address
   *
   * @return true on success
   */
  bool requestMAC();

  /**
   * @brief Request the Firmware
   *
   * @return true on success
   */
  bool requestFirmware();

  /**
   * @brief Request the resolution (gate-width)
   *
   * @return true on success
   */
  bool requestResolution();

  /**
   * @brief Set the resolution of the sensor
   *
   * @param fine true=20cm; [false]=75cm
   * @return true on success
   */
  bool setResolution(bool fine = false);

  /**
   * @brief Request the sensor parameters:
   * range, motion thresholds, stationary thresholds, no-one window
   *
   * @return true on success
   */
  bool requestParameters();

  /**
   * @brief Set the gate parameters for a particular gate, or for all gates at once
   *
   * @param gate the gate to configure (only 2 - 8 are configurable),
   * pass 0 to apply the same thresholds to all gates
   * @param movingThreshold [0 - 100]
   * @param stationaryThreshold [0 - 100]
   * @return true on success
   */
  bool setGateParameters(byte gate, byte movingThreshold = 100, byte stationaryThreshold = 100);

  /**
   * @brief Set the detection range for moving targets, stationary targets,
   * as well as the no-one window
   *
   * @param movingGate
   * @param stationaryGate
   * @param noOneWindow
   * @return true on success
   */
  bool setMaxGate(byte movingGate, byte stationaryGate, byte noOneWindow = 5);

  bool setGateParameters(const ValuesArray &moving_thresholds, const ValuesArray &stationary_thresholds, byte noOneWindow = 5);

  /**
   * @brief Set the no-one window parameter
   *
   * @param noOneWindow in [s]
   * @return true on success
   */
  bool setNoOneWindow(byte noOneWindow);

  /**
   * @brief Set the maximum moving gate
   *
   * @param movingGate
   * @return true on success
   */
  bool setMaxMovingGate(byte movingGate);

  /**
   * @brief Set the maximum stationary gate
   *
   * @param stationaryGate
   * @return true on success
   */
  bool setMaxStationaryGate(byte stationaryGate);

  /**
   * @brief Request reset to factory default parameters
   *
   * @return true on success
   */
  bool requestReset();

  /**
   * @brief Request reboot
   *
   * @return true on success
   */
  bool requestReboot();

  /**
   * @brief Turn Bluetooth ON
   *
   * @return true on success
   */
  bool requestBTon();

  /**
   * @brief Turn Bluetooth OFF
   *
   * @return true on success
   */
  bool requestBToff();
};

#endif // MY_LD2410_H