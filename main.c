
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"
#include "XO.h"


int main(void){
	
	enum turn player_turn=X_turn;
	char isWinner;	
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  
	PortF_Init();
  Nokia5110_Init();	 
	Nokia5110_ClearBuffer();
	Nokia5110_SetCursor(5, 2);
	Nokia5110_OutString("XO");
	Nokia5110_SetCursor(4, 4);
	Nokia5110_OutString("GAME");
	Delay(1);
	Nokia5110_Clear();
	
  
	while(1)
		{
		  Init_postions();	
	    Nokia5110_PrintBMP(18, 46, XOgame, 0);
	    Nokia5110_PrintBMP(postions[current_postion].x,postions[current_postion].y, XOcursor, 0);
	    Nokia5110_DisplayBuffer(); 
		  GPIO_PORTF_DATA_R = 2+(2*player_turn);//turn the led on with the player turn
		  while (1)
			{
				if (!(GPIO_PORTF_DATA_R&0x10))
				{
					move_cursor(next);
					while(!(GPIO_PORTF_DATA_R&0x10));
					Nokia5110_PrintBMP(postions[current_postion].x,postions[current_postion].y, XOcursor, 0);
					Nokia5110_DisplayBuffer(); 
				}
				if (!(GPIO_PORTF_DATA_R&0x01))
				{
					while(!(GPIO_PORTF_DATA_R&0x01));
				  drawSymbol(player_turn);
		      isWinner =check();
		      if (isWinner==1)
					{
						Delay(1);
						Nokia5110_Clear();
						displayWinner(player_turn);
						Delay(3); 
						break;			
				  }
	        else if (avalible_postions==0)
			    {
						Delay(1);
						Nokia5110_Clear();
						displayDraw ();
						Delay(3); 
						break;
			    }
		      else
		      {
						if (player_turn==X_turn)
						{
							player_turn=O_turn;		
						}
						else
						{
							 player_turn=X_turn;
						}
						GPIO_PORTF_DATA_R = 2+(2*player_turn);//change the led to the new player color
						move_cursor(first_avalible);
						Nokia5110_PrintBMP(postions[current_postion].x,postions[current_postion].y, XOcursor, 0);
						Nokia5110_DisplayBuffer(); 						
					}
		    }
	    }		
	  }
}


void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}



void Delay(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  
    while(time){
	  	time--;
    }
    count--;
  }
}
// check the winning condations and return 1 if there's a winner ,0 if no
char check (void)
{
	if ((postions[0].player==postions[1].player&&postions[0].player==postions[2].player&&postions[0].occupied==1)||
			(postions[0].player==postions[3].player&&postions[0].player==postions[6].player&&postions[0].occupied==1)||
		  (postions[0].player==postions[4].player&&postions[0].player==postions[8].player&&postions[0].occupied==1)||
		  (postions[3].player==postions[4].player&&postions[3].player==postions[5].player&&postions[3].occupied==1)||
		  (postions[6].player==postions[7].player&&postions[6].player==postions[8].player&&postions[6].occupied==1)||
		  (postions[1].player==postions[4].player&&postions[1].player==postions[7].player&&postions[1].occupied==1)||
		  (postions[2].player==postions[5].player&&postions[2].player==postions[8].player&&postions[2].occupied==1)||
		  (postions[2].player==postions[4].player&&postions[2].player==postions[6].player&&postions[2].occupied==1))
	  {
		 return 1;
	  }
	  return 0;	
}

// initalize the the array of postions
void Init_postions (void)
{
	unsigned char intial_x =20; 
	unsigned char intial_y =14;
	int X_postion;
	int Y_postion;
	int i=0;
	for (Y_postion=0;Y_postion<3;Y_postion++)
	{	
		for(X_postion=0;X_postion<3;X_postion++)
		{
			postions[i].x=intial_x;
			postions[i].y=intial_y;
			postions[i].occupied=0;
			postions[i].player=none;
			intial_x=intial_x+15;
		  i++;	
		}
		intial_x =20;
		intial_y=intial_y+15;	
	}
	current_postion =0;
	avalible_postions=9;
      
}
//move the cursor to next avalible or first avalible depending on input 
void move_cursor (enum dirOfMove m)
{
	int i=0;
	if (m==0)	 //next postion
		{
			Nokia5110_PrintBMP(postions[current_postion].x,postions[current_postion].y, ClearCursor, 0);	 
			if (current_postion==8)
				{
					current_postion=0;
			  }
			else
				{
					current_postion++;
				} 
	 
			while (postions[current_postion].occupied==1)
				{
					if (current_postion==8)
						{
							current_postion=0;
			      }
			   else
					 {
				     current_postion++;
					 } 
	      }
		}
	else //first avaliable
	{
		for (i=0;i<9;i++)
		{
			if (postions[i].occupied==0)
				{
					current_postion=i;
					break;
			  }
		}
	 }		 
	 
 }
// display the winner player and the score  
void displayWinner (enum turn player)
{
	if (player==X_turn)
	{
		Xwins++;
		Nokia5110_SetCursor(6, 0);
		Nokia5110_OutString("X");
	}	
	else
	{
		Owins++;
		Nokia5110_SetCursor(6, 0);
		Nokia5110_OutString("O");
	}
	Nokia5110_SetCursor(3, 2);
	Nokia5110_OutString("WINNER!");
	Nokia5110_SetCursor(2, 4);
	Nokia5110_OutString("X  VS  O");
	Nokia5110_SetCursor(2, 5);
	Nokia5110_OutUDec(Xwins);
	Nokia5110_SetCursor(9, 5);
	Nokia5110_OutUDec(Owins);
}
//display draw and the score ,and chand the leds to yellow.
void displayDraw (void)
{	
	Nokia5110_SetCursor(1, 1);
	Nokia5110_OutString("GAME OVER");
	Nokia5110_SetCursor(3, 2);
	Nokia5110_OutString("DRAW!");
	Nokia5110_SetCursor(2, 4);
	Nokia5110_OutString("X  VS  O");
	Nokia5110_SetCursor(2, 5);
	Nokia5110_OutUDec(Xwins);
	Nokia5110_SetCursor(9, 5);
	Nokia5110_OutUDec(Owins);
	GPIO_PORTF_DATA_R = 0x0A;
}
//draw the player symbol in the current postion ,and change the space to be owned by the player
void drawSymbol (enum turn player)
{
		if (player==X_turn)
	{
		postions[current_postion].player=player_X;
		Nokia5110_PrintBMP(postions[current_postion].x,postions[current_postion].y, Xsymbol, 0);
	}
	else 
	{
		postions[current_postion].player=player_O;
		Nokia5110_PrintBMP(postions[current_postion].x,postions[current_postion].y, Osymbol, 0);
	}

	postions[current_postion].occupied=1;
	avalible_postions--;
	Nokia5110_DisplayBuffer();
}



