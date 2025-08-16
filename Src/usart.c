#include "includes.h"
#include "defines.h"
#include "global_var.h"
#include "usart.h"
#include "main_func.h"

unsigned char fc301d_rec(){
  
  unsigned char  in_data,chk_sum=0,i;
  
//  in_data=(uint8_t)((&huart3)->Instance->DR & (uint8_t)0x00FF);
  
  rf_pwr_on;
  flag.wakeup=1;
  flag.comm=1;
  wakeup_cou=0;
  fc_clr_time=0;
  
  
  if(rec_cou==0){
    if(in_data==fc_start){
      fc301d_rec_buffer[rec_cou]=in_data;
      rec_cou++;
      
    }
  }
  else{
    fc301d_rec_buffer[rec_cou]=in_data;
    rec_cou++;
    if(rec_cou==2) len = in_data;
    
    if (len>249){
      rec_cou=0;
      return 0;
    }
    else if(rec_cou==len)
    {
            
      for(i=0;i<len-1;i++)  chk_sum+=fc301d_rec_buffer[i];
            
      if(chk_sum==fc301d_rec_buffer[len-1] ){
        
        if(address_process(fc301d_rec_buffer)==1)
          analyse();
        flag.fc_net_busy=1;
        rec_cou=0;
        
      }
      
      else 
        rec_cou=0;
      
    }
  }
  
  return 0;	
  
}

/***************************************************************/
void fc301d_send(unsigned char*frame){
  
  unsigned char i,chk_sum=0;
  
  for(i=0;i<frame[1]-1;i++){
        
    chk_sum+=frame[i];
  }

//  HAL_UART_Transmit(&huart3,frame,frame[1]-1,100);
//  
//  HAL_UART_Transmit(&huart3,&chk_sum,1,100);
    
  
}

//-----------------------------------------------------
void RS232_send(unsigned char *frame,unsigned int len){
      
  HAL_UART_Transmit(&huart3,frame,len,600);
  
}
//--------------------------------------------------------
void RS485_send(unsigned char *frame,unsigned int len){
  rs485_tx_mode;    
  HAL_UART_Transmit(&huart2,frame,len,100);
	rs485_rx_mode;
}
//---------------------------------------------------
void usart_init(USART_TypeDef* uart,uint32_t buad){
	UART_HandleTypeDef huart;
	
	huart.Instance = uart;
  huart.Init.BaudRate = buad;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits = UART_STOPBITS_1;
  huart.Init.Parity = UART_PARITY_NONE;
  huart.Init.Mode = UART_MODE_TX_RX;
  huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}