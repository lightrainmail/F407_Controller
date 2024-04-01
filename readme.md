# 4通道遥控器主控程序

## STM32F407VET6使用CCMRAM来加快数据运算

CCMRAM与内核紧密相连，内核访问CCMRAM的速度比普通RAM快很多

首先在STM32F407VETX_FLASH.ld文件的SECTION括号内添加下面一些代码

这些代码描述CCMRAM

```c
/* 来自CSDN复制*/
_siccmram = LOADADDR(.ccmram);
 /* CCM-RAM section
 *
 * IMPORTANT NOTE!
 * If initialized variables will be placed in this section,
 * the startup code needs to be modified to copy the init-values.
 */
 .ccmram (NOLOAD) :
 {
   . = ALIGN(4);
   _sccmram = .;       /* create a global symbol at ccmram start */
   *(.ccmram)
   *(.ccmram*)

   . = ALIGN(4);
   _eccmram = .;       /* create a global symbol at ccmram end */
 } >CCMRAM
```

之后就可以在变量或数组前面加上

```c
__attribute__((section(".ccmram")))
```

就可以使该变量位于CCMRAM中



例如：

```c
__attribute__((section(".ccmram"))) uint32_t lcdBuffer[128];	//lcdBUffer[128]就被分配在了CCMRAM中
```



为了使用方便，也可使用宏定义

```c
#define CCMRAM __attribute__((section(".ccmram")))
```



然后就可以这样

```c
CCMRAM uint32_t lcdBuffer[128];		//lcdBuffer[128]也别分配在CCMRAM中
```



**此外：**有的单片机还支持将函数放在CCMRAM中，以加快函数的执行速度，例如可将中断函数放入CCMRAM中

例如：

```c
CCMRAM void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_0) {
        NRF_TxIRQ();
        BaseType_t highTaskWoken = pdFALSE;
        if(NRFSem_TxHandle !=NULL) {
            xSemaphoreGiveFromISR(NRFSem_TxHandle,&highTaskWoken);
            portYIELD_FROM_ISR(highTaskWoken);
        }
    }
}
```



但是遗憾的使STM32F407不支持将函数放在CCMRAM中，仅仅支持将数组、变量放在CCMRAM中