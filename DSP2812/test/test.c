int condition=0;
int a=0;
int b=0;
int Flag_SJ=0;

////////////////////////////////////////////
int State_Confirm(int tj ,int *flag1,int *flag2)
{
  int flag=0;  //0:����
  if(tj==0) //�������� 
  {
    *flag2=0;
    if(*flag1==0)
    {
      *flag1=1;
    }
    else
    {
      flag=0;  //���ֵһ��
    } 
  }
  else          //���������� 
  {
    *flag1=0;
    if(*flag2==0)
    {
      *flag2=1;
    }
    else
    {
      flag=1;   //���ֵ�෴ 
    } 
  }
  return(flag);
} 
/////////////������//////////////////////////////////////

main()
{  
  int i=30;

  for(;;)  
  { 
    if(i<20)
    {
      condition=0;
    }
    else
    {
      condition=1;
    }

    Flag_SJ=State_Confirm(condition,&a,&b);
  }
} 
