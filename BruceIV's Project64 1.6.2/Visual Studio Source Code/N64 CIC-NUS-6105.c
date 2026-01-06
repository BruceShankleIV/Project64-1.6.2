/*
 * Copyright 2011 X-Scale. All rights reserved.
 *
 * Redistribution and use in source and binary forms,with or without
 * modification,are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice,this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY X-Scale ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES,INCLUDING,BUT NOT LIMITED TO,THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL X-Scale OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,INDIRECT,
 * INCIDENTAL,SPECIAL,EXEMPLARY,OR CONSEQUENTIAL DAMAGES (INCLUDING,BUT NOT
 * LIMITED TO,PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY,WHETHER IN CONTRACT,STRICT LIABILITY,OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * This software provides an algorithm that emulates the protection scheme of
 * N64 PIF/CIC-NUS-6105,by determining the proper response to each challenge.
 * It was synthesized after a careful,exhaustive and detailed analysis of the
 * challenge/response pairs stored in the 'pif2.dat' file from Project 64.
 * These challenge/response pairs were the only resource used during this
 * project. There was no kind of physical access to N64 hardware.
 */
#include <Windows.h>
#include <math.h>
#include "N64 CIC-NUS-6105.h"
void N64_CIC_NUS_6105(char chl[],char rsp[],int len)
{
    static char lut0[0x10]={ 0x4,0x7,0xA,0x7,0xE,0x5,0xE,0x1,0xC,0xF,0x8,0xF,0x6,0x3,0x6,0x9 };
    static char lut1[0x10]={ 0x4,0x1,0xA,0x7,0xE,0x5,0xE,0x1,0xC,0x9,0x8,0x5,0x6,0x3,0xC,0x9 };
    char key,*lut;
    int i,sgn,mag,mod;
    for (key=0xB,lut=lut0,i=0; i<len; i++) {
        rsp[i]=(key+5 * chl[i]) & 0xF;
        key=lut[rsp[i]];
        sgn=(rsp[i]>>3) & 0x1;
        mag=((sgn==1)?~rsp[i]:rsp[i]) & 0x7;
        mod=(mag % 3==1)?sgn:1-sgn;
        if (lut==lut1 && (rsp[i]==0x1||rsp[i]==0x9)) mod=1;
        if (lut==lut1 && (rsp[i]==0xB||rsp[i]==0xE)) mod=0;
        lut=(mod==1)?lut1:lut0;
    }
}