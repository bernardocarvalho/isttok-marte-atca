/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id$
 *
**/

#if !defined (_ATCAIOC_BRD)
#define _ATCAIOC_BRD

#define DMA_SIZE 128

//OBJECT_DLL(ATCAIocBoard)
class ATCAIocBoard {
  //OBJECT_DLL_STUFF(ATCAIocBoard)

  private:
  int fD;
 public:
  /// Constructor
  ATCAIocBoard(){
    fD=-1;
  };
  /// Destructor
  ~ATCAIocBoard(){
    //close(fd);
  }
  int open();
  int init();
  int close();
  int write_reg(int reg, int val);    
  int reset_offsets();
  int write_adc_offset(int chan, int val);
  int write_int_offset(int chan, int val);
  bool dev_open(const char * fileName);
  int init_device(int chop_period, int chop_dc, int * adc_vector, int * int_vector, int numberOfAdcChannels);
  int stop_device();
  bool soft_trigger();
  void dev_close();
  int read_packet(void * , int );

};

#endif  // _ATCAIOC_BRD
