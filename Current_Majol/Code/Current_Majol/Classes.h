class Times{
  public:
    unsigned long times;
    void ontime(void)
    {
      times=millis();
    }
    unsigned long timediff(void)
    {
      return (millis()-times);
    }
};

class Ampermeter{
  public:
    float ACS_Value;                                                            // Here we keep the raw data valuess
    float testFrequency = 50;                                                   // test signal frequency (Hz)
    float windowLength = 40.0/testFrequency;                                    // how long to average the signal, for statistist
    float intercept = -0.1;                                                        // to be adjusted based on calibration testing
    float slope_ACS_30A = 0.0752; 
    float slope_ACS_20A =  0.052;
    float TRMS;                                                                 // estimated actual current in amps
};
