//
//  SyncSwitch.hpp
//  MindTheGap
//
//  Created by Luke Zeitlin on 05/07/2017.
//
//

#ifndef SyncSwitch_hpp
#define SyncSwitch_hpp

#include <stdio.h>

#endif /* SyncSwitch_hpp */


class SyncParamConvert
{
private:
    double outVal;
public:
    double IntToSyncCoeff(int inVal, ITextControl * dispControl)
    {
        switch (inVal)
        {
            case 19:
                //  1 beat
                outVal = 4.;
                dispControl->SetTextFromPlug("4");
                break;
                
            case 18:

                outVal = 3.;
                dispControl->SetTextFromPlug("3");
                break;
                
            case 17:
   
                outVal = 2.;
                dispControl->SetTextFromPlug("2");
                break;
            case 16:

                outVal = 3./2.;
                dispControl->SetTextFromPlug("3/2");
                break;
            case 15:

                outVal = 1;
                dispControl->SetTextFromPlug("1");
                break;
            case 14:
    
                outVal = 3./4.;
                dispControl->SetTextFromPlug("3/4");
                break;
            case 13:

                outVal = 2./3.;
                dispControl->SetTextFromPlug("2/3");
                break;
            case 12:

                outVal = 1./2.;
                dispControl->SetTextFromPlug("1/2");
                break;
                
            case 11:

                outVal = 3./8.;
                dispControl->SetTextFromPlug("3/8");
                break;
                
            case 10:
 
                outVal = 1./3.;
                dispControl->SetTextFromPlug("1/3");
                break;
                
            case 9:

                outVal = 1./4.;
                dispControl->SetTextFromPlug("1/4");
                break;
                
            case 8:

                outVal = 3./16.;
                dispControl->SetTextFromPlug("3/16");
                break;
                
            case 7:

                outVal = 1./6.;
                dispControl->SetTextFromPlug("1/6");
                break;
                
            case 6:
  
                outVal = 1./8.;
                dispControl->SetTextFromPlug("1/8");
                break;
            case 5:

                outVal = 3./32.;
                dispControl->SetTextFromPlug("3/32");
                break;
                
            case 4:

                outVal = 1./12.;
                dispControl->SetTextFromPlug("1/12");
                break;
                
            case 3:

                outVal =1./16.;
                dispControl->SetTextFromPlug("1/16");
                break;
                
            case 2:

                outVal = 3./64.;
                dispControl->SetTextFromPlug("3/64");
                break;
                
            case 1:
     
                outVal = 1./24.;
                dispControl->SetTextFromPlug("1/24");
                break;
                
            case 0:
                outVal = 1./32.;
                dispControl->SetTextFromPlug("1/32");
                break;
                
            
            default:
                break;
        }
        return outVal;
    }
};
