//
//  Filter.cpp
//  MindTheGap
//
//  Created by Luke Zeitlin on 04/07/2017.
//
//

#include "Filter.hpp"
#include <cmath>


void Biquad::flushDelays()
{
    m_f_Xz_1 = 0;
    m_f_Xz_2 = 0;
    m_f_Yz_1 = 0;
    m_f_Yz_2 = 0;
    
}

double Biquad::doBiQuad(double f_xn)
{
    
    //simply implements transfer function of a standard BiQuad filter
    double yn = m_f_a0*f_xn + m_f_a1*m_f_Xz_1 + m_f_a2*m_f_Xz_2 - m_f_b1*m_f_Yz_1 - m_f_b2*m_f_Yz_2;
    
    // underflow check
    if(yn > 0.0 && yn < FLT_MIN_PLUS) yn = 0;
    if(yn < 0.0 && yn > FLT_MIN_MINUS) yn = 0;
    
    // shuffle delays
    // Y delays
    m_f_Yz_2 = m_f_Yz_1;
    m_f_Yz_1 = yn;
    
    // X delays
    m_f_Xz_2 = m_f_Xz_1;
    m_f_Xz_1 = f_xn;
    
    return  yn;
}

