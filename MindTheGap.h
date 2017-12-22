#ifndef __DELAYPLUGIN__
#define __DELAYPLUGIN__

#include "IPlug_include_in_plug_hdr.h"
#include "resample.h"
#include "Filter.hpp"
#include <math.h>
#include"SyncSwitch.hpp"

class MindTheGap : public IPlug
{
public:
  MindTheGap(IPlugInstanceInfo instanceInfo);
  ~MindTheGap();
  
  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void cookVars();
  void resetDelay();
  void repitch();
  void calcNextDelay();
  void calculateLPFCoeffs(double fCutoffFreq, double fQ);
  void calculateHPFCoeffs(double fCutoffFreq, double fQ);
  void filterSwitch();
  void switchSyncMode();
  
  
  
private:
  double mDelaySam = 0.;
  int mNextDelaySams = 0;
  int mTargetDelaySam = 0;
  
  double mFeedback = 0.;
  double mWet = 0.;
  
  double* mpBufferL = NULL;
  double* mpBufferR = NULL;
  int mBufferSize = 0;
  
  int mReadIndex = 0;
  int mWriteIndex = 0;
  int mXfadeLen = 0;
  
  SyncParamConvert SyncConverter;
  
  WDL_Resampler rsL;
  WDL_Resampler rsR;
  
  int inBufSize = 0;
  int outBufSize = 0;
  IGraphics *pGraphics;
  
  double m_f_fc_Hz;
  const double m_f_Q = 0.5;
  Biquad m_LeftF;
  Biquad m_RightF;
  enum filterSetting
  {
    LPF,
    HPF,
    nSettingsF
  };

  bool misFilterOn;
  
  filterSetting mFilterSetting;
  
  bool misPingPong;
  
  bool misSync;
  double mDelayLenSync;
  double *xBufL;
  double *xBufR;
  
  WDL_ResampleSample* outBufL;
  WDL_ResampleSample* outBufR;
  
  ITextControl *dispControl;
};






#endif
