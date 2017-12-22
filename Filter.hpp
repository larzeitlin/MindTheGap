//
//  Filter.hpp
//  MindTheGap
//
//  Created by Luke Zeitlin on 04/07/2017.
//
//

#ifndef Filter_hpp
#define Filter_hpp

#include <stdio.h>

#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */


class Biquad
{
protected:
    
    //delay elements for biQuad
    
    double m_f_Xz_1;
    double m_f_Xz_2;
    double m_f_Yz_1;
    double m_f_Yz_2;
    const double pi = 3.14159265;
    
public:
    
    //Filter Coefficients
    
    double m_f_a0;
    double m_f_a1;
    double m_f_a2;
    double m_f_b1;
    double m_f_b2;
    
    //class methods
    
    void flushDelays();
    double doBiQuad(double f_xn);
};


#endif /* Filter_hpp */
