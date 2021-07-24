#include "gw_audio.h"
#include <stdio.h>
#include <string.h>

extern SAI_HandleTypeDef hsai_BlockA1;
extern DMA_HandleTypeDef hdma_sai1_a;

int16_t audiobuffer_dma[AUDIO_BUFFER_LENGTH * 2]  __attribute__((section (".audio")));

uint32_t audio_dma_counter=0;//debug

void gw_audio_start(void){
    memset(audiobuffer_dma, 0, sizeof(audiobuffer_dma));
    HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)audiobuffer_dma, AUDIO_BUFFER_LENGTH*2);
}

void gw_audio_stop(void){
	HAL_SAI_DMAStop(&hsai_BlockA1);
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai){
    audio_dma_counter++;
	gw_audio_process(audiobuffer_dma,AUDIO_BUFFER_LENGTH);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai){
    audio_dma_counter++;
	gw_audio_process(audiobuffer_dma+AUDIO_BUFFER_LENGTH,AUDIO_BUFFER_LENGTH);
}
