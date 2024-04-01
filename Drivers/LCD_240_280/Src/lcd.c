#include "lcd.h"
#include "lcdfont.h"

/* @brief ???????????д??8λ????*/
void LCD_Write8BitData(uint8_t data) {
    lcd_interface_write_8bit_data(data);
}

/* @brief ???????????д??16bit????*/
void LCD_Write16BitData(uint16_t data) {
    lcd_interface_write_16bit_data(data);
}

void LCD_WriteCommand(uint8_t command) {
    LCD_DC_Clr();   //DC??GPIO??????????????,??????????????????????
    lcd_interface_write_8bit_data(command);
    LCD_DC_Set();
}

void LCD_WriteAnyLengthData(uint8_t *pdata,uint16_t len) {
    lcd_interface_write_any_data(pdata,len);
}

/* @note 这个函数为16bit画图函数设计*/
void LCD_WriteAnyLength16BitData(uint16_t pdata[],uint16_t len) {
    uint8_t data[len*2];    //这里可能会栈溢出
    for(uint16_t i = 0;i < len;i++) {
        data[2*i] = (pdata[i] >> 8);
        data[2*i + 1] = pdata[i];
    }
    lcd_interface_write_any_data(data,2*len);
}

void LCD_Delayms(uint32_t delay) {
    lcd_interface_delayms(delay);
}



/******************************************************************************
      ????????????????????????
      ????????x1,x2 ?????е????????????
                y1,y2 ?????е????????????
      ???????  ??
******************************************************************************/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WriteCommand(0x2a);//?е??????
		LCD_Write16BitData(x1);
		LCD_Write16BitData(x2);
		LCD_WriteCommand(0x2b);//?е??????
		LCD_Write16BitData(y1+20);
		LCD_Write16BitData(y2+20);
		LCD_WriteCommand(0x2c);//??????д
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WriteCommand(0x2a);//?е??????
		LCD_Write16BitData(x1);
		LCD_Write16BitData(x2);
		LCD_WriteCommand(0x2b);//?е??????
		LCD_Write16BitData(y1+20);
		LCD_Write16BitData(y2+20);
		LCD_WriteCommand(0x2c);//??????д
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WriteCommand(0x2a);//?е??????
		LCD_Write16BitData(x1+20);
		LCD_Write16BitData(x2+20);
		LCD_WriteCommand(0x2b);//?е??????
		LCD_Write16BitData(y1);
		LCD_Write16BitData(y2);
		LCD_WriteCommand(0x2c);//??????д
	}
	else
	{
		LCD_WriteCommand(0x2a);//?е??????
		LCD_Write16BitData(x1+20);
		LCD_Write16BitData(x2+20);
		LCD_WriteCommand(0x2b);//?е??????
		LCD_Write16BitData(y1);
		LCD_Write16BitData(y2);
		LCD_WriteCommand(0x2c);//??????д
	}
}


/******************************************************************************
      ????????????????????????
      ????????xsta,ysta   ???????
                xend,yend   ???????
								color       ????????
      ???????  ??
******************************************************************************/
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{          
	uint16_t i,j; 
	LCD_Address_Set(xsta,ysta,xend-1,yend-1);//?????????Χ
	for(i=ysta;i<yend;i++)
	{													   	 	
		for(j=xsta;j<xend;j++)
		{
			LCD_Write16BitData(color);
		}
	} 					  	    
}



/* @brief ???????????,?ú?????????*/
void LCD_FastFill(uint16_t color) {
    lcd_interface_fast_fill_color(color);
}

/******************************************************************************
      ??????????????λ?????
      ????????x,y ????????
                color ??????
      ???????  ??
******************************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_Address_Set(x,y,x,y);//???ù??λ?? 
	LCD_Write16BitData(color);
} 


/******************************************************************************
      ?????????????
      ????????x1,y1   ???????
                x2,y2   ???????
                color   ??????
      ???????  ??
******************************************************************************/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //???????????? 
	delta_y=y2-y1;
	uRow=x1;//???????????
	uCol=y1;
	if(delta_x>0)incx=1; //??????????? 
	else if (delta_x==0)incx=0;//????? 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//???? 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //???????????????? 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//????
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/******************************************************************************
      ???????????????
      ????????x1,y1   ???????
                x2,y2   ???????
                color   ???ε????
      ???????  ??
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      ????????????
      ????????x0,y0   ???????
                r       ??
                color   ??????
      ???????  ??
******************************************************************************/
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//?ж????????????
		{
			b--;
		}
	}
}

/******************************************************************************
      函数说明：显示汉字串
      入口数据：x,y显示坐标
                *s 要显示的汉字串
                fc 字的颜色
                bc 字的背景色
                sizey 字号 可选 16 24 32
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	while(*s!=0)
	{
		if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
		else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
		else return;
		s+=2;
		x+=sizey;
	}
}

/******************************************************************************
      函数说明：显示单个12x12汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese12x12(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//???????
	uint16_t TypefaceNum;//?????????????С
	uint16_t x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	                         
	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//?????????
	for(k=0;k<HZnum;k++) 
	{
		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//???????
					{
						if(tfont12[k].Msk[i]&(0x01<<j))LCD_Write16BitData(fc);
						else LCD_Write16BitData(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//??????
					{
						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//???????
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //??????????????????????????????????????????????
	}
} 

/******************************************************************************
      函数说明：显示单个16x16汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese16x16(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//???????
	uint16_t TypefaceNum;//?????????????С
	uint16_t x0=x;
  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//?????????
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//???????
					{
						if(tfont16[k].Msk[i]&(0x01<<j))LCD_Write16BitData(fc);
						else LCD_Write16BitData(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//??????
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//???????
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //??????????????????????????????????????????????
	}
} 


/******************************************************************************
      函数说明：显示单个24x24汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese24x24(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//???????
	uint16_t TypefaceNum;//?????????????С
	uint16_t x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//?????????
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//???????
					{
						if(tfont24[k].Msk[i]&(0x01<<j))LCD_Write16BitData(fc);
						else LCD_Write16BitData(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//??????
					{
						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//???????
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //??????????????????????????????????????????????
	}
} 

/******************************************************************************
      ????????????????32x32????
      ????????x,y???????
                *s ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChinese32x32(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//???????
	uint16_t TypefaceNum;//?????????????С
	uint16_t x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//?????????
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//???????
					{
						if(tfont32[k].Msk[i]&(0x01<<j))LCD_Write16BitData(fc);
						else LCD_Write16BitData(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//??????
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//???????
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //??????????????????????????????????????????????
	}
}


/******************************************************************************
      ???????????????????
      ????????x,y???????
                num ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t temp,sizex,t,m=0;
	uint16_t i,TypefaceNum;//?????????????С
	uint16_t x0=x;
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //?????????
	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //???ù??λ?? 
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];		       //????6x12????
		else if(sizey==16)temp=ascii_1608[num][i];		 //????8x16????
		else if(sizey==24)temp=ascii_2412[num][i];		 //????12x24????
		else if(sizey==32)temp=ascii_3216[num][i];		 //????16x32????
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//???????
			{
				if(temp&(0x01<<t))LCD_Write16BitData(fc);
				else LCD_Write16BitData(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//??????
			{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//???????
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}


/******************************************************************************
      ?????????????????
      ????????x,y???????
                *p ???????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;
		p++;
	}  
}


/******************************************************************************
      ????????????????
      ????????m??????n???
      ???????  ??
******************************************************************************/
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;
	return result;
}

/* @brief ????????????????*/
void LCD_ShowHexNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode) {
    uint8_t SignleNumber;
    LCD_ShowString(x,y,"0x",fc,bc,sizey,mode);
    for(uint8_t i = 0;i < len;i++){
        SignleNumber =num/ mypow(16,len - i - 1)%16;
        if(SignleNumber < 10){
            LCD_ShowChar(x + (i+2)*sizey/2,y,SignleNumber + '0',fc,bc,sizey,mode);
        } else{
            LCD_ShowChar(x + (i+2)*sizey/2,y,SignleNumber - 10 +'A',fc,bc,sizey,mode);
        }
    }
}

void LCD_ShowBinNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode) {
    for(uint8_t i = 0;i < len;i++){
        LCD_ShowChar(x + i*sizey/2,y,num/mypow(2,len - i -1)%2 + '0',fc,bc,sizey,mode);
    }
}

/******************************************************************************
      ????????????????????
      ????????x,y???????
                num ????????????
                len ??????λ??
                fc ??????
                bc ???????
                sizey ???
      ???????  ??
******************************************************************************/
void LCD_ShowIntNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;
	uint8_t sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}

/* @brief 显示完整带符号的整形变量*/
void LCD_ShowIntNumber(uint16_t x,uint16_t y,int num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey) {
    if(num >= 0) {
        LCD_ShowChar(x,y,' ',fc,bc,sizey,0);
        LCD_ShowIntNum(x + sizey/2,y,(uint16_t)num,len,fc,bc,sizey);
    }
    else {
        LCD_ShowChar(x,y,'-',fc,bc,sizey,0);
        LCD_ShowIntNum(x + sizey/2,y,(uint16_t)(-num),len,fc,bc,sizey);
    }
}


/******************************************************************************
      ??????????????λС??????
      ????????x,y???????
                num ????С??????
                len ??????λ??
                fc ??????
                bc ???????
                sizey ???
      ???????  ??
******************************************************************************/
void LCD_ShowFloatNum1(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{         	
	uint8_t t,temp,sizex;
	uint16_t num1;
	sizex=sizey/2;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}

/* @brief ???????????λС??
 * @param   x,y???????
            num ????С??????
            len ??????λ??
            fc ??????
            bc ???????
            sizey ???*/
void LCD_ShowFloatNumber(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey){
    if(num >= 0){
        LCD_Fill(x,y,x + sizey/2,y + sizey,bc);
        LCD_ShowChar(x,y,'+',fc,bc,sizey,1);
        LCD_ShowFloatNum1(x + sizey/2,y,num,len,fc,bc,sizey);
    } else{
        LCD_Fill(x,y,x + sizey/2,y + sizey,bc);
        LCD_ShowChar(x,y,'-',fc,bc,sizey,1);
        LCD_ShowFloatNum1(x + sizey/2,y,-num,len,fc,bc,sizey);
    }
}

/******************************************************************************
      ??????????????
      ????????x,y???????
                length ??????
                width  ?????
                pic[]  ??????    
      ???????  ??
******************************************************************************/
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[])
{
	uint16_t i,j;
	uint32_t k=0;
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_Write8BitData(pic[k*2]);
			LCD_Write8BitData(pic[k*2+1]);
			k++;
		}
	}
}

/* @brief 显示图片函数的优化版本*/
void LCD_ShowPicture2(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]) {
    //设定显示区域
    LCD_Address_Set(x,y,x + length - 1,y + width - 1);
    LCD_WriteAnyLengthData(pic,length*width*2);
}

/* @brief 显示图片，但是数据是16位
 * @note pic 的长度必须小于65536/2*/
void LCD_ShowPicture3(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint16_t pic[]) {
    //设定显示区域
    LCD_Address_Set(x,y,x + length - 1,y + width - 1);

    //写入颜色数据
    LCD_WriteAnyLength16BitData(pic,length*width);
}


/* @brief ????????????????????????
 * @param color ????????*/
void LCD_Init(uint16_t color) {
	lcd_interface_gpio_init();//?????GPIO
	
	LCD_RES_Clr();//??λ
	LCD_Delayms(100);
	LCD_RES_Set();
	LCD_Delayms(100);
	
	LCD_BLK_Set();//?????
    LCD_Delayms(100);
	
	//************* Start Initial Sequence **********//
	LCD_WriteCommand(0x11); //Sleep out 
	LCD_Delayms(120);              //Delay 120ms
	//************* Start Initial Sequence **********// 
	LCD_WriteCommand(0x36);
	if(USE_HORIZONTAL==0)LCD_Write8BitData(0x00);
	else if(USE_HORIZONTAL==1)LCD_Write8BitData(0xC0);
	else if(USE_HORIZONTAL==2)LCD_Write8BitData(0x70);
	else LCD_Write8BitData(0xA0);

	LCD_WriteCommand(0x3A);			
	LCD_Write8BitData(0x05);

	LCD_WriteCommand(0xB2);			
	LCD_Write8BitData(0x0C);
	LCD_Write8BitData(0x0C); 
	LCD_Write8BitData(0x00); 
	LCD_Write8BitData(0x33); 
	LCD_Write8BitData(0x33); 			

	LCD_WriteCommand(0xB7);			
	LCD_Write8BitData(0x35);

	LCD_WriteCommand(0xBB);			
	LCD_Write8BitData(0x32); //Vcom=1.35V
					
	LCD_WriteCommand(0xC2);
	LCD_Write8BitData(0x01);

	LCD_WriteCommand(0xC3);			
	LCD_Write8BitData(0x15); //GVDD=4.8V  ??????
				
	LCD_WriteCommand(0xC4);			
	LCD_Write8BitData(0x20); //VDV, 0x20:0v

	LCD_WriteCommand(0xC6);			
	LCD_Write8BitData(0x0F); //0x0F:60Hz        	

	LCD_WriteCommand(0xD0);			
	LCD_Write8BitData(0xA4);
	LCD_Write8BitData(0xA1); 

	LCD_WriteCommand(0xE0);
	LCD_Write8BitData(0xD0);   
	LCD_Write8BitData(0x08);   
	LCD_Write8BitData(0x0E);   
	LCD_Write8BitData(0x09);   
	LCD_Write8BitData(0x09);   
	LCD_Write8BitData(0x05);   
	LCD_Write8BitData(0x31);   
	LCD_Write8BitData(0x33);   
	LCD_Write8BitData(0x48);   
	LCD_Write8BitData(0x17);   
	LCD_Write8BitData(0x14);   
	LCD_Write8BitData(0x15);   
	LCD_Write8BitData(0x31);   
	LCD_Write8BitData(0x34);   

	LCD_WriteCommand(0xE1);     
	LCD_Write8BitData(0xD0);   
	LCD_Write8BitData(0x08);   
	LCD_Write8BitData(0x0E);   
	LCD_Write8BitData(0x09);   
	LCD_Write8BitData(0x09);   
	LCD_Write8BitData(0x15);   
	LCD_Write8BitData(0x31);   
	LCD_Write8BitData(0x33);   
	LCD_Write8BitData(0x48);   
	LCD_Write8BitData(0x17);   
	LCD_Write8BitData(0x14);   
	LCD_Write8BitData(0x15);   
	LCD_Write8BitData(0x31);   
	LCD_Write8BitData(0x34);
	LCD_WriteCommand(0x21); 

	LCD_WriteCommand(0x29);

    //????????,??????????????
    LCD_Fill(0,0,LCD_W,LCD_H,color);
//    LCD_FastFill(color);
} 

/* @brief 用于将RGB颜色转换成RGB565结构的16位颜色*/
uint16_t LCD_RGBTo565(uint8_t red,uint8_t green,uint8_t blue) {
    //线性映射
    red = 0.121569f*red;
    green = 0.247059f*green;
    blue = 0.121569f*blue;

    //取低5位
    red &= 0x1F;

    //取低6位
    green &= 0x3F;

    //取低5位
    blue &= 0x1F;

    //开始拼接颜色
    uint16_t RGB565 = 0;
    RGB565 |= blue;
    RGB565 |= (green << 5);
    RGB565 |= (red << 11);

    return RGB565;
}

/* @brief 将24位RGB颜色转换为565结构的RGB颜色*/
uint16_t LCD_RGB24To565(uint32_t rgb) {
    uint8_t red = 0,green = 0,blue = 0;
    blue = rgb & 0x000000FF;
    green = ((rgb & 0x0000FF00) >> 8);
    red = ((rgb & 0x00FF0000) >> 16);
    return LCD_RGBTo565(red,green,blue);
}
