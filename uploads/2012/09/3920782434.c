#include "xpt2046.h" 
#include "ili93xx.h" 
#include "stdlib.h"
#include "math.h"

Pen_Holder Pen_Point;//�����ʵ��

void delay_us(u32 us)
{
    u32 time=100*us/7;    
    while(--time);   
}


//SPIд����
//��7846/7843/XPT2046/UH7843/UH7846д��1byte����   
void ADS_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)
			Set_TOUCH_TDIN 
		else 
			Clr_TOUCH_TDIN;   
		num<<=1;    
		Clr_TOUCH_TCLK;//��������Ч	   	 
		Set_TOUCH_TCLK;      
	} 			    
} 		 

//SPI������ 
//��7846/7843/XPT2046/UH7843/UH7846��ȡadcֵ	   
u16 ADS_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	
	Clr_TOUCH_TCLK;//������ʱ�� 	 
	Clr_TOUCH_TCS; //ѡ��ADS7843	 
	ADS_Write_Byte(CMD);//����������
	delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	Set_TOUCH_TCLK;//��1��ʱ�ӣ����BUSY   	    
	Clr_TOUCH_TCLK; 	 
	
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		Clr_TOUCH_TCLK;//�½�����Ч  	    	   
		Set_TOUCH_TCLK;
		if(TOUCH_DOUT)
		Num++; 		 
	}  	
	
	Num>>=4;   //ֻ�и�12λ��Ч.
	Set_TOUCH_TCS;//�ͷ�ADS7843	 
	return(Num);   
}


//��ȡһ������ֵ
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
#define READ_TIMES 15 //��ȡ����
#define LOST_VAL 5	  //����ֵ
u16 ADS_Read_XY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 


//���˲��������ȡ
//��Сֵ��������100.
u8 Read_ADS(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}	


//2�ζ�ȡADS7846,������ȡ2����Ч��ADֵ,�������ε�ƫ��ܳ���
//50,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
#define ERR_RANGE 50 //��Χ 
u8 Read_ADS2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else{
		return 0;	  
	}	
} 


//��ȡһ������ֵ	
//������ȡһ��,֪��PEN�ɿ��ŷ���!					   
u8 Read_TP_Once(void)
{
	u8 t=0;	    
	Pen_Int_Set(0);//�ر��ж�
	Pen_Point.Key_Sta=Key_Up;
	Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
	while(TOUCH_PEN==0&&t<=250)
	{
		t++;
		delay_us(10000);
	};
	Pen_Int_Set(1);//�����ж�		 
	if(t>=250)
		return 0;//����2.5s ��Ϊ��Ч
	else 
		return 1;	
}


/**************************************��LCD�����йصĺ���****************************************/

//��һ��������
//����У׼�õ�
void Drow_Touch_Point(u8 x,u16 y)
{
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6);//������Ȧ
}	  
//��һ�����
//2*2�ĵ�			   
void Draw_Big_Point(u8 x,u16 y)
{	    
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}

/***************************************************************************************************/


//ת�����
//���ݴ�������У׼����������ת����Ľ��,������X0,Y0��
void Convert_Pos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
	}
}	   



//PEN�ж�����
//����һ���ⲿ�жϵ������������º�����ⲿ�ж�
//�ⲿ�жϺ���:
//	���PEN�ŵ�һ���½���
//	void EXTI1_IRQHandler(void)
//	{ 		   			 
//		Pen_Point.Key_Sta=Key_Down;//�������� 
//    	EXTI_ClearFlag(EXTI_Line1);//���LINE1�ϵ��жϱ�־λ 	  				 
//	} 	 
void Pen_Int_Set(u8 en)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	if(en)
	{	  		
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;	//�ⲿ��·EXIT1
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж�����
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�ⲿ�жϴ�����ѡ��:����������·�½���Ϊ�ж�����
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//ʹ���ⲿ�ж���״̬
		EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���	

	}else{ 	

		EXTI_InitStructure.EXTI_Line = EXTI_Line1;	//�ⲿ��·EXIT1
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж�����
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�ⲿ�жϴ�����ѡ��:����������·�½���Ϊ�ж�����
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;		//�ر��ⲿ�ж���״̬
		EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���	  

	}
		
}	  


//������У׼����
//�õ��ĸ�У׼����
void Touch_Adjust(void)
{								 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	   
	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//����   
	POINT_COLOR=RED;//��ɫ 
	LCD_Clear(WHITE);//���� 
	Drow_Touch_Point(20,20);//����1 
	Pen_Point.Key_Sta=Key_Up;//���������ź� 
	Pen_Point.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//����������
		{
			if(Read_TP_Once())//�õ����ΰ���ֵ
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(WHITE);//���� 
					Drow_Touch_Point(220,20);//����2
					break;
				case 2:
					LCD_Clear(WHITE);//���� 
					Drow_Touch_Point(20,300);//����3
					break;
				case 3:
					LCD_Clear(WHITE);//���� 
					Drow_Touch_Point(220,300);//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
						LCD_Clear(WHITE);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(WHITE);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(WHITE);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}//��ȷ��
					//������
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//����
					LCD_ShowString(35,110,"Touch Screen Adjust OK!");//У�����
					delay_us(1000000);
					LCD_Clear(WHITE);//����   
					return;//У�����				 
			}
		}
	} 
}		

    
//�ⲿ�жϳ�ʼ������
void Touch_Init(void)
{			    		   

	Read_ADS(&Pen_Point.X,&Pen_Point.Y);//��һ�ζ�ȡ��ʼ��			 
	LCD_Clear(WHITE);//����
	Touch_Adjust();  //��ĻУ׼,���Զ�����			   

	//	printf("Pen_Point.xfac:%f\n",Pen_Point.xfac);
	//	printf("Pen_Point.yfac:%f\n",Pen_Point.yfac);
	//	printf("Pen_Point.xoff:%d\n",Pen_Point.xoff);
	//	printf("Pen_Point.yoff:%d\n",Pen_Point.yoff);
}

