int condition=0;
int a=0;
int b=0;
int Flag_SJ=0;

////////////////////////////////////////////
int State_Confirm(int tj ,int *flag1,int *flag2)
{
  int flag=0;  //0:正常
  if(tj==0) //条件满足 
  {
    *flag2=0;
    if(*flag1==0)
    {
      *flag1=1;
    }
    else
    {
      flag=0;  //与初值一致
    } 
  }
  else          //条件不满足 
  {
    *flag1=0;
    if(*flag2==0)
    {
      *flag2=1;
    }
    else
    {
      flag=1;   //与初值相反 
    } 
  }
  return(flag);
} 
/////////////主程序//////////////////////////////////////

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
