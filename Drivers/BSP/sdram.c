#include "sdram.h"

extern SDRAM_HandleTypeDef hsdram1;
FMC_SDRAM_CommandTypeDef command;

/* Status variables */
__IO uint32_t uwWriteReadStatus = 0;

/* Counter index */
uint32_t uwIndex = 0;


void Sdram_Init()
{
////	/* Read/Write Buffers */
//uint32_t aTxBuffer[BUFFER_SIZE];
//uint32_t aRxBuffer[BUFFER_SIZE];
//	
	SDRAM_Initialization_Sequence(&hsdram1, &command);
	
//	/*##-2- SDRAM memory read/write access #####################################*/  
//  
//  /* Fill the buffer to write */
//  Fill_Buffer(aTxBuffer, BUFFER_SIZE, 0xA244250F);   
//  
//  /* Write data to the SDRAM memory */
//	for(uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++)
//  {
//    *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex) = aTxBuffer[uwIndex];
//  }    
//  
//  /* Read back data from the SDRAM memory */
//  for (uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++)
//  {
//    aRxBuffer[uwIndex] = *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex);
//   } 

//  /*##-3- Checking data integrity ############################################*/    

//  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
//  {
//    if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex])
//    {
//      uwWriteReadStatus++;
//    }
//  }	

//  if (uwWriteReadStatus)
//  {
//    beep(100);   
//  }
//  else
//  { 
//    beep(1000);
//  }
 
}

static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd =0;
  /* Step 3:  Configure a clock configuration enable command */
  Command->CommandMode 			 = FMC_SDRAM_CMD_CLK_ENABLE;
  Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 1;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

  /* Step 4: Insert 100 ms delay */
  HAL_Delay(1);
    
  /* Step 5: Configure a PALL (precharge all) command */ 
  Command->CommandMode 			 = FMC_SDRAM_CMD_PALL;
  Command->CommandTarget 	     = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 1;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);  
  
  /* Step 6 : Configure a Auto-Refresh command */ 
  Command->CommandMode 			 = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 4;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  
  /* Step 7: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 1;
  Command->ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  
  /* Step 8: Set the refresh rate counter */
  /* (15.62 us x Freq) - 20 */
  /* Set the device refresh counter */
  HAL_SDRAM_ProgramRefreshRate(hsdram, REFRESH_COUNT); 
}

//static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
//{
//  uint32_t tmpIndex = 0;

//  /* Put in global buffer different values */
//  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
//  {
//    pBuffer[tmpIndex] = tmpIndex + uwOffset;
//  }
//}   
