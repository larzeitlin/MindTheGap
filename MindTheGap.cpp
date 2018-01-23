#include "MindTheGap.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <algorithm>
#include <vector>


const int kNumPrograms = 1;

enum EParams
{
  kDelayMS,
  kFeedbackPC,
  kWetPC,
  kXfadeLen,
  kfHz,
  kFilterSetting,
  kIsPingPong,
  kIsFilterOn,
  kIsSync,
  kDelayLenSync,
  kNumParams
  
};


enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  kGainX = 125,
  kGainY = 125,
  kKnobFrames = 100,
  kBigKnobFrames = 100,
  k3switchFrames = 3,
  k2switchFrames = 2,
  kbuttonFrames = 2
  
};



MindTheGap::MindTheGap(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
//  TRACE;
  
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  
  rsL.SetMode(true, 0 , false, 32, 32);
  rsR.SetMode(true, 0 , false, 32, 32);
//  rs. SetFilterParms(0.4, 0.707);
  GetParam(kDelayMS)->InitDouble("Delay", 100., 2., 400., 0.01, "Milliseconds");
  GetParam(kFeedbackPC)->InitDouble("Feedback", 50., 0., 99.95, 0.01, "%");
  GetParam(kWetPC)->InitDouble("Wet/Dry", 50., 0., 100.0, 0.01, "%");
  GetParam(kXfadeLen)->InitDouble("Smoothness", 20, 0, 100, 1., "%");
  GetParam(kfHz)->InitDouble("Cutoff", 1000., 150., 5000., 1., "Hz");
  GetParam(kDelayLenSync)->InitInt("Sync Delay Length", 4, 0, 19);
  GetParam(kfHz)->SetShape(2.);
  GetParam(kFilterSetting)->InitEnum("Filter Setting", LPF, nSettingsF);
  GetParam(kIsPingPong)->InitBool("Ping Pong", false);
  GetParam(kIsFilterOn)->InitBool("Filter On/Off", false);
  GetParam(kIsSync)->InitBool("Tempo Sync", false);

  
  
  pGraphics = MakeGraphics(this, kWidth, kHeight);

  pGraphics->AttachBackground(BG_ID, BG_FN);
  
  IBitmap* knob = pGraphics->LoadPointerToBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap* littleknob = pGraphics->LoadPointerToBitmap(LITTLEKNOB_ID, LITTLEKNOB_FN, kKnobFrames);
  IBitmap* bigknob = pGraphics->LoadPointerToBitmap(BIGKNOB_ID, BIGKNOB_FN, kBigKnobFrames);
  IBitmap* twoSwitch = pGraphics->LoadPointerToBitmap(TWOSWITCH_ID, TWOSWITCH_FN, k2switchFrames);
  IBitmap* button = pGraphics->LoadPointerToBitmap(BUTTON_ID, BUTTON_FN, kbuttonFrames);
  
  
  IColor textColor = IColor(255, 0, 0, 0);
  IText textProps4(24, &textColor, "Verdana", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
  
  
  dispControl = new ITextControl(this, DRAW_RECT(IRECT(80, 124, 220, 164)), &textProps4, "X");
  
  pGraphics->AttachControl(dispControl);
  
  
  
  pGraphics->AttachControl(new IKnobMultiControl(this, 75, 62, kDelayMS, bigknob));
  pGraphics->AttachControl(new IKnobMultiControl(this, 75, 62, kDelayLenSync, bigknob));
  pGraphics->AttachControl(new IKnobMultiControl(this, 20, 190, kFeedbackPC, knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, 215, 190, kWetPC, knob));
  
  pGraphics->AttachControl(new IKnobMultiControl(this, 136, 225, kXfadeLen, littleknob));
  pGraphics->AttachControl(new IKnobMultiControl(this, 195, 345, kfHz, littleknob));
  
  pGraphics->AttachControl(new ISwitchControl(this, 70, 345, kFilterSetting, twoSwitch));
  pGraphics->AttachControl(new ISwitchControl(this, 54, 417, kIsPingPong, button));
  pGraphics->AttachControl(new ISwitchControl(this, 118, 372, kIsFilterOn, button));
  pGraphics->AttachControl(new ISwitchControl(this, 180, 416, kIsSync, button));
  
  
  
  
  AttachGraphics(pGraphics);
  
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

void MindTheGap::switchSyncMode()
{
  if (!misSync)
  {
    pGraphics->HideControl(kDelayLenSync, true);
    pGraphics->HideControl(kDelayMS, false);
  }
  else
  {
    pGraphics->HideControl(kDelayLenSync, false);
    pGraphics->HideControl(kDelayMS, true);
  }
  
}

MindTheGap::~MindTheGap()
{
  if(mpBufferL)   {delete [] mpBufferL;}
  if(mpBufferR)   {delete [] mpBufferR;}
}



void MindTheGap::calcNextDelay()
{
  double A = 0.1;
  mNextDelaySams = (1.0-A)*mNextDelaySams+A*mTargetDelaySam;
  
  if (mNextDelaySams != mTargetDelaySam) { repitch(); }


}

void MindTheGap::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.a
  calcNextDelay();
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    if(misPingPong)
    {
      *in2 = 0.;
      double ynR;
      double ynL;
      //...Check filter setting
      if(!misFilterOn)   { ynR = (mpBufferR[mReadIndex]); }
      else                        { ynR = m_RightF.doBiQuad(mpBufferR[mReadIndex]);}
      if(!misFilterOn)   { ynL = (mpBufferL[mReadIndex]); }
      else                        { ynL = m_LeftF.doBiQuad(mpBufferL[mReadIndex]);}
      
      if (mDelaySam == 0) { ynR = *in2; }
      if (mDelaySam == 0) { ynL = *in1; }
      
      
      
      
      mpBufferR[mWriteIndex] = *in1 + mFeedback * ynL;
      *out2 =  ( mWet * ynR + (1 - mWet) * *in2 );
      
      mpBufferL[mWriteIndex] = *in2 + mFeedback * ynR;
      *out1 =  ( mWet * ynL + (1 - mWet) * *in1 );
    }
    
    
    // Regular Stereo delay (no Ping Pong)
    else
    {
      //Left Channel
      double ynL;
      //...Check filter setting
      if(!misFilterOn)   { ynL = (mpBufferL[mReadIndex]); }
      else                        { ynL = m_LeftF.doBiQuad(mpBufferL[mReadIndex]);}
    

      if (mDelaySam == 0) { ynL = *in1; }
      mpBufferL[mWriteIndex] = *in1 + mFeedback * ynL;
      *out1 =  ( mWet * ynL + (1 - mWet) * *in1 );
    
      //Right Channel
    
      double ynR;
      //...Check filter setting
      if(!misFilterOn)   { ynR = (mpBufferR[mReadIndex]); }
      else                        { ynR = m_RightF.doBiQuad(mpBufferR[mReadIndex]);}
    
      if (mDelaySam == 0) { ynR = *in2; }
      mpBufferR[mWriteIndex] = *in2 + mFeedback * ynR;
      *out2 =  ( mWet * ynR + (1 - mWet) * *in2 );
    }

    
    //inc Pointers
    ++mWriteIndex;
    if(mWriteIndex >= mBufferSize)  { mWriteIndex = 0;  }

    ++mReadIndex;
    if(mReadIndex >= mBufferSize)   { mReadIndex = 0;   }
  }
  
}

void MindTheGap::Reset()
{
  TRACE;
  GetSampleRate();
  IMutexLock lock(this);
  mBufferSize = 2*GetSampleRate();
  if(mpBufferL) { delete [] mpBufferL;}
  if(mpBufferR) { delete [] mpBufferR;}
  mpBufferL = new double[mBufferSize];
  mpBufferR = new double[mBufferSize];
  resetDelay();
  cookVars();
  m_LeftF.flushDelays();
  m_RightF.flushDelays();
  calculateLPFCoeffs(m_f_fc_Hz, m_f_Q);
}

void MindTheGap::resetDelay()
{
  if(mpBufferL) { memset(mpBufferL, 0, mBufferSize*sizeof(double)); }
  if(mpBufferR) { memset(mpBufferR, 0, mBufferSize*sizeof(double)); }
  mWriteIndex = 0;
  mReadIndex = 0;
}

void MindTheGap::repitch()
{
  IMutexLock lock(this);
  if(mpBufferL)
  {
    inBufSize = mDelaySam;
    
    outBufSize = mNextDelaySams;
    
    
    //std::cout<<outBufSize<<std::endl;
    
    
    
    WDL_ResampleSample* ResamptrL;
    WDL_ResampleSample* ResamptrR;
    rsL.SetRates(inBufSize, outBufSize);
    rsR.SetRates(inBufSize, outBufSize);
    outBufL = new double [outBufSize];
    outBufR = new double [outBufSize];
    
    rsL.ResamplePrepare(outBufSize, 1, &ResamptrL);
    rsR.ResamplePrepare(outBufSize, 1, &ResamptrR);
    
    int bufRemaining = mBufferSize - mReadIndex;
    if ( bufRemaining > inBufSize)
    {
      memcpy(&ResamptrL[0], &mpBufferL[mReadIndex], inBufSize* sizeof(WDL_ResampleSample));
      memcpy(&ResamptrR[0], &mpBufferR[mReadIndex], inBufSize* sizeof(WDL_ResampleSample));
    }
    else
    {
      memcpy(&ResamptrL[0], &mpBufferL[mReadIndex], bufRemaining * sizeof(WDL_ResampleSample));
      memcpy(&ResamptrR[0], &mpBufferR[mReadIndex], bufRemaining * sizeof(WDL_ResampleSample));
      memcpy(&ResamptrL[bufRemaining], &mpBufferL[0], (inBufSize - bufRemaining) * sizeof(WDL_ResampleSample));
      memcpy(&ResamptrR[bufRemaining], &mpBufferR[0], (inBufSize - bufRemaining) * sizeof(WDL_ResampleSample));
    }

    
    int m = rsL.ResampleOut(outBufL, inBufSize, outBufSize, 1);
    const int n = rsR.ResampleOut(outBufR, inBufSize, outBufSize, 1);

    double crossfadeCnt = 1.0;
    const double crossfadeDec = 1.0 / (double)mXfadeLen;
    
    int tempIndex = mReadIndex;
    xBufL = new double [mXfadeLen];
    xBufR = new double [mXfadeLen];
    
    for (int i = 0; i < mXfadeLen; ++i)
    {
      if(tempIndex >= mBufferSize) { tempIndex = 0;}
      xBufL[i] = mpBufferL[tempIndex];
      xBufR[i] = mpBufferR[tempIndex];
      ++tempIndex;
    }
    
    int tempIndex2 = mWriteIndex - outBufSize;
    if(tempIndex2 < 0) { tempIndex2 += mBufferSize; }

    for (int i = 0; i < n; ++i)
    {
      if(tempIndex2 >= mBufferSize) { tempIndex2 = 0;}
      if(i < mXfadeLen)
      {
        mpBufferL[tempIndex2] =   (xBufL[i]*crossfadeCnt)  + (outBufL[i] * (1. - crossfadeCnt));
        mpBufferR[tempIndex2] =   (xBufR[i]*crossfadeCnt)  + (outBufR[i] * (1. - crossfadeCnt));
        crossfadeCnt -= crossfadeDec;
      }
      else
      {
        mpBufferL[tempIndex2] = outBufL[i];
        mpBufferR[tempIndex2] = outBufR[i];
      }
      
      ++tempIndex2;
    }
  }
  mDelaySam = outBufSize;
  mReadIndex = mWriteIndex - mDelaySam;
  if(mReadIndex < 0) { mReadIndex += mBufferSize; }
  
  delete []xBufL;
  delete []xBufR;
  delete []outBufL;
  delete []outBufR;
}

void MindTheGap::cookVars()
{
  mFeedback = GetParam(kFeedbackPC)->Value() / 100.0;
  mWet = GetParam(kWetPC)->Value() / 100.0;
  mReadIndex = mWriteIndex - mDelaySam;
  if(mReadIndex < 0) { mReadIndex += mBufferSize; }
}


void MindTheGap::filterSwitch()
{
  switch (mFilterSetting)
  {
    case LPF:
      calculateLPFCoeffs(m_f_fc_Hz, m_f_Q);
      break;
    case HPF:
      calculateHPFCoeffs(m_f_fc_Hz, m_f_Q);
      break;
    default:
      break;
  }
}

void MindTheGap::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
    case kDelayMS:
    {
      int inDelaySam = ((int)(GetParam(kDelayMS)->Value())) * GetSampleRate() / 1000.0;
      mTargetDelaySam = inDelaySam;
      char newString[7];
      sprintf(newString, "%d", (int)(GetParam(kDelayMS)->Value()));
      dispControl->SetTextFromPlug(newString);
      break;
    }
    case kFeedbackPC:
    {
      mFeedback = GetParam(kFeedbackPC)->Value() / 100.0;
      break;
    }
    case kWetPC:
    {
      mWet = GetParam(kWetPC)->Value() / 100.0;
      break;
    }
    case kXfadeLen:
    {
      mXfadeLen = (int)((GetParam(kXfadeLen)->Value() * 5) + 40);
      break;
    }
    case kfHz:
    {
      m_f_fc_Hz = GetParam(kfHz)->Value();
      filterSwitch();
      break;
    }
    case kFilterSetting:
    {
      mFilterSetting = (filterSetting)(GetParam(kFilterSetting)->Value());
      filterSwitch();
      break;
    }
    case kIsPingPong:
    {
      misPingPong =  GetParam(kIsPingPong)->Value();
      break;
    }
    case kIsFilterOn:
    {
      misFilterOn =  GetParam(kIsFilterOn)->Value();
      break;
    }
    case kIsSync:
    {
      misSync =  GetParam(kIsSync)->Value();
      switchSyncMode();
      break;
    }
    case kDelayLenSync:
    {
      double samplesPerBeat = ((GetSampleRate() * 60.0) / GetTempo());
      double DelFactor= SyncConverter.IntToSyncCoeff(GetParam(kDelayLenSync)->Value(), dispControl);
      mTargetDelaySam = (int)(samplesPerBeat * DelFactor);
      break;
    }
    default:
      break;
  }
}

void MindTheGap::calculateHPFCoeffs(double fCutoffFreq, double fQ)
{
  const double fs = GetSampleRate();
  const double C = tan(pi*fCutoffFreq / fs);
  const double a0 = 1 / (  1 + (sqrt(2.0)*C) + pow(C, 2) );
  
  //left channel:
  
  m_LeftF.m_f_a0 = a0;
  m_LeftF.m_f_a1 = -2.0*a0;
  m_LeftF.m_f_a2 = a0;
  m_LeftF.m_f_b1 = 2.0*a0*(pow(C, 2) -1);
  m_LeftF.m_f_b2 = a0*(1 - (sqrt(2.0)*C) + pow(C, 2));
  
  //Right channel:
  
  m_RightF.m_f_a0 = a0;
  m_RightF.m_f_a1 = -2.0*a0;
  m_RightF.m_f_a2 = a0;
  m_RightF.m_f_b1 = 2.0*a0*(pow(C, 2) -1);
  m_RightF.m_f_b2 = a0*(1 - (sqrt(2.0)*C) + pow(C, 2));
}

void MindTheGap::calculateLPFCoeffs(double fCutoffFreq, double fQ)
{
  const double fs = GetSampleRate();
  const double C = 1.0 / (tan(pi*fCutoffFreq / fs));
  const double a0 = 1 / (  1 + (sqrt(2.0)*C) + pow(C, 2) );
  
  //left channel:
  
  m_LeftF.m_f_a0 = a0;
  m_LeftF.m_f_a1 = 2.0*a0;
  m_LeftF.m_f_a2 = a0;
  m_LeftF.m_f_b1 = 2.0*a0*(1 - pow(C, 2));
  m_LeftF.m_f_b2 = a0*(1 - (sqrt(2.0)*C) + pow(C, 2));
  
  //Right channel:
  
  m_RightF.m_f_a0 = a0;
  m_RightF.m_f_a1 = 2.0*a0;
  m_RightF.m_f_a2 = a0;
  m_RightF.m_f_b1 = 2.0*a0*(1 - pow(C, 2));
  m_RightF.m_f_b2 = a0*(1 - (sqrt(2.0)*C) + pow(C, 2));
}
