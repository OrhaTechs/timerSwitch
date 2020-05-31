#include <LiquidCrystal.h>

#include <IRremote.h>

// define your remote buttons here
#define up 0x20DF02FD
#define down 0x20DF827D
#define right 0x20DF609F
#define left 0x20DFE01F
#define ok 0x20DF22DD
#define play 0x20DF0DF2
#define pause 0x20DF5DA2

#define B0 0x20DF08F7
#define B1 0x20DF8877
#define B2 0x20DF48B7
#define B3 0x20DFC837
#define B4 0x20DF28D7
#define B5 0x20DFA857
#define B6 0x20DF6897
#define B7 0x20DFE817
#define B8 0x20DF18E7
#define B9 0x20DF9867



int RECV_PIN = 9, relay = 13, buzzer = 7;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);    // (RS,EN,D4,D5,D6,D7)

IRrecv irrecv(RECV_PIN);
decode_results results;

byte onTime[3] = {0,0,0};
byte offTime[3] = {0,0,0};
byte onTimeStored[3] ={0, 0, 0};
byte offTimeStored[3] ={0, 0, 0};
byte hOn=0,mOn=0,sOn=0,hOff=0,mOff=0,sOff=0;
byte data = 45;
byte HMSon[6] ={0,0,0,0,0,0}; 
byte HMSoff[6] ={0,0,0,0,0,0}; 
 
String row1 ="", row2 = "";

bool repeatCheck = 1, repeatCycle = 0, state = 0;
byte startProcess = 0;

unsigned int menuCount = 1;
uint32_t buttonVal = 0;

void timerInit()
{

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 3036; //34286;    // preload timer 65536-16MHz/256/2Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  TCNT1 = 3036;             // timer for 1 second
  if( startProcess == 1)
  {
    servoProcess();
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(relay,OUTPUT);
  pinMode(buzzer,OUTPUT);
  digitalWrite(relay,LOW);
  digitalWrite(buzzer,LOW);
  irrecv.enableIRIn();  // Start the receiver
  lcd.begin(16,2);      // Start the LCD
  timerInit();          // Start the timer
}

void loop()
{
  remote();
  switch(buttonVal)
  {
   
    case pause:
      startProcess = 0;
      repeatCycle = 0;
      repeatCheck = 1;
      menuCount = 1;
      data = 45;
      buttonVal = 0;
      state = 0;
      digitalWrite(relay,LOW);
      hOn=0,mOn=0,sOn=0,hOff=0,mOff=0,sOff=0;
      for( int a; a<3; a++)
      {
        onTimeStored[a] = 0;
        offTimeStored[a] = 0;
        onTime[a] = 0;
        offTime[a] = 0;
        HMSon[a] = 0; HMSon[a+3] = 0;
        HMSoff[a] = 0; HMSoff[a+3] = 0;    
      }
      buzzerSound();
    break;
    
    case left:
      if( startProcess == 0)
      {
        switch( menuCount )
        { 
          case 1:
           if(onTime[0] < 24 && onTime[1] < 60 && onTime[2] < 60 )
          {
            menuCount = 3;
          }
          else
          {
            onTime[ 0 ] = 23;
            onTime[ 1 ] = 59;
            onTime[ 2 ] = 59;
          }
          break; 
  
          case 2:
            if(offTime[0] < 24 && offTime[1] < 60 && offTime[2] < 60 )
            {
              menuCount = 1;
            }
            else
            {
              onTime[ 0 ] = 23;
              onTime[ 1 ] = 59;
              onTime[ 2 ] = 59;
            }
          break;

          case 3:
            menuCount = 2;
          break;
        } 
      }
      buzzerSound();
    break;
    
    case ok:
        switch(menuCount)
        {
          case 1:
            if(onTime[0] < 24 && onTime[1] < 60 && onTime[2] < 60 )
            {
              menuCount = 2;
            }
            else
            {
              onTime[ 0 ] = 23;
              onTime[ 1 ] = 59;
              onTime[ 2 ] = 59;
            }
          break; 

          case 2:
            if(offTime[0] < 24 && offTime[1] < 60 && offTime[2] < 60 )
            {
              menuCount = 3;
            }
            else
            {
              onTime[ 0 ] = 23;
              onTime[ 1 ] = 59;
              onTime[ 2 ] = 59;
            }
          break;

          case 3:
            repeatCycle =! repeatCycle;
          break;
        }
        buzzerSound();
    break;
    
    case right:
      if( startProcess == 0)
      {
        switch( menuCount )
        { 
          case 1:
           if(onTime[0] < 24 && onTime[1] < 60 && onTime[2] < 60 )
          {
            menuCount = 2;
          }
          else
          {
            onTime[ 0 ] = 23;
            onTime[ 1 ] = 59;
            onTime[ 2 ] = 59;
          }
          break; 
  
          case 2:
            if(offTime[0] < 24 && offTime[1] < 60 && offTime[2] < 60 )
            {
              menuCount = 3;
            }
            else
            {
              onTime[ 0 ] = 23;
              onTime[ 1 ] = 59;
              onTime[ 2 ] = 59;
            }
          break;

          case 3:
            menuCount = 1;
          break;
        } 
      }
      buzzerSound();
    break;

    case B0:
      data = 0;
      buzzerSound();
    break;

    case play:
      if( startProcess == 0 )
      {
        for( int a; a<3 ; a++)
        {
          onTimeStored[a] = onTime[a];
          offTimeStored[a] = offTime[a];
        }
        startProcess = 1;
      }
      else if (startProcess == 1)
      {
        startProcess = 2;
      }
      else
      {
        startProcess = 1;
      }
      buzzerSound();
    break;

    case B1:
      data = 1;
      buzzerSound();
    break;

    case B2:
      data = 2;
      buzzerSound();
    break;

    case B3:
      data = 3;
      buzzerSound();
    break;
    
    case B4:
      data = 4;
      buzzerSound();
    break;
    
    case B5:
      data = 5;
      buzzerSound();
    break;
    
    case B6:
      data = 6;
      buzzerSound();
    break;
    
    case B7:
      data = 7;
      buzzerSound();
    break;
    
    case B8:
      data = 8;
      buzzerSound();
    break;
    
    case B9:
      data = 9;
      buzzerSound();
    break;
}

String Htemp = "00", Mtemp = "00", Stemp = "00";  
  if(startProcess == 1)
  {
      
      hOn = onTime[0];mOn = onTime[1];sOn = onTime[2];
      hOff = offTime[0];mOff = offTime[1];sOff = offTime[2];
      if(hOn == 0 && mOn == 0 && sOn == 0)
      {
        state = 1;
        buzzerSound();
        if( repeatCycle == 1)
        {
          for( int a; a<3;a++ )
          {
            onTime[a] = onTimeStored[a];
          }
        }
      }

      if(hOff == 0 && mOff == 0 && sOff == 0)
      {
        buzzerSound();
        if( repeatCycle == 1)
        {
          state = 0;
          startProcess = 1;
          for( int a; a<3;a++ )
          {
            offTime[a] = offTimeStored[a];
          }
        }
        else
        {
          startProcess = 0;
        }
     }
     
     switch(state)
     {
      case 0:
      digitalWrite(relay,HIGH);
      row1 = "    ON TIME     ";
      Htemp += String (hOn); Mtemp += String (mOn); Stemp += String (sOn);
      Htemp = Htemp.substring(Htemp.length()-2,Htemp.length());
      Mtemp = Mtemp.substring(Mtemp.length()-2,Mtemp.length());
      Stemp = Stemp.substring(Stemp.length()-2,Stemp.length());
      
      if( repeatCycle == 1)
      {
        row2 = "> R "+ Htemp;
        row2 += ":"+ Mtemp;
        row2 += ":" + Stemp;
        row2 += "    ";
      }     
      else
      {
        row2 = ">   "+ Htemp;
        row2 += ":"+ Mtemp;
        row2 += ":" + Stemp;
        row2 += "    ";
      }
      
      break;

      case 1:
        digitalWrite(relay,LOW);
        row1 = "    OFF TIME    ";
        Htemp += String (hOff); Mtemp += String (mOff); Stemp += String (sOff);
        Htemp = Htemp.substring(Htemp.length()-2,Htemp.length());
        Mtemp = Mtemp.substring(Mtemp.length()-2,Mtemp.length());
        Stemp = Stemp.substring(Stemp.length()-2,Stemp.length());
        
        if( repeatCycle == 1)
        {
          row2 = "> R "+ Htemp;
          row2 += ":"+ Mtemp;
          row2 += ":" + Stemp;
          row2 += "    ";
        }     
        else
        {
          row2 = ">   "+ Htemp;
          row2 += ":"+ Mtemp;
          row2 += ":" + Stemp;
          row2 += "    ";
        }
                
        
      break;
     }
  }
  else if (startProcess == 0)
  {
    menuDisplay();
  }
  else
  {
    switch(state)
     {
      case 0:
      row1 = "    ON TIME     ";
      Htemp += String (hOn); Mtemp += String (mOn); Stemp += String (sOn);
      Htemp = Htemp.substring(Htemp.length()-2,Htemp.length());
      Mtemp = Mtemp.substring(Mtemp.length()-2,Mtemp.length());
      Stemp = Stemp.substring(Stemp.length()-2,Stemp.length());

      if( repeatCycle == 1)
      {
        row2 = "||R "+ Htemp;
        row2 += ":"+ Mtemp;
        row2 += ":" + Stemp;
        row2 += "    ";
      }     
      else
      {
        row2 = "||  "+ Htemp;
        row2 += ":"+ Mtemp;
        row2 += ":" + Stemp;
        row2 += "    ";
      }
      break;

      case 1:
        row1 = "    OFF TIME    ";
        Htemp += String (hOff); Mtemp += String (mOff); Stemp += String (sOff);
        Htemp = Htemp.substring(Htemp.length()-2,Htemp.length());
        Mtemp = Mtemp.substring(Mtemp.length()-2,Mtemp.length());
        Stemp = Stemp.substring(Stemp.length()-2,Stemp.length());

        if( repeatCycle == 1)
        {
          row2 = "||R "+ Htemp;
          row2 += ":"+ Mtemp;
          row2 += ":" + Stemp;
          row2 += "    ";
        }     
        else
        {
          row2 = "||  "+ Htemp;
          row2 += ":"+ Mtemp;
          row2 += ":" + Stemp;
          row2 += "    ";
        }        
        
      break;
     }
  }
  fullDisplay();
  buttonVal = 0;
  data = 45;
}

void remote()
{
  if (irrecv.decode(&results)) 
  {
      buttonVal = results.value;
      Serial.println(buttonVal);
    irrecv.resume();          // Receive the next value
  }
}

void menuDisplay()
{
  String H1="00",H2="00";
  String M1="00",M2="00";
  String S1="00",S2="00";
 
  switch(menuCount)
  {
    case 1:  
      if( data != 45 )
        {
          for ( int a = 0; a < 5; a++)
          {
            HMSon[ a ] = HMSon[ a+1 ];
          }
          HMSon[ 5 ] = data;
          H2 = String(HMSon[ 0 ]);
          H2 += String(HMSon[ 1 ]);
          M2 = String(HMSon[ 2 ]);
          M2 += String(HMSon[ 3 ]);
          S2 = String(HMSon[ 4 ]);
          S2 += String(HMSon[ 5 ]);
          onTime[0] = H2.toInt();
          onTime[1] = M2.toInt();
          onTime[2] = S2.toInt();
        }
        
        row1 = "<    ON TIME   >";
        H1 += onTime[0]; M1 += onTime[1]; S1 += onTime[2];
        H1 = H1.substring(H1.length()-2,H1.length());
        M1 = M1.substring(M1.length()-2,M1.length());
        S1 = S1.substring(S1.length()-2,S1.length()); 
        row2 ="    "+H1+":"+M1+":"+S1+"    ";
    break;

    case 2:
      if( data != 45 )
        {
          for ( int a = 0; a < 5; a++)
          {
            HMSoff[ a ] = HMSoff[ a+1 ];
          }
          HMSoff[ 5 ] = data;
          H2 = String(HMSoff[ 0 ]);
          H2 += String(HMSoff[ 1 ]);
          M2 = String(HMSoff[ 2 ]);
          M2 += String(HMSoff[ 3 ]);
          S2 = String(HMSoff[ 4 ]);
          S2 += String(HMSoff[ 5 ]);
          offTime[0] = H2.toInt();
          offTime[1] = M2.toInt();
          offTime[2] = S2.toInt();
        }
        
        row1 = "<   OFF TIME   >";
        H1 += offTime[0]; M1 += offTime[1]; S1 += offTime[2];
        H1 = H1.substring(H1.length()-2,H1.length());
        M1 = M1.substring(M1.length()-2,M1.length());
        S1 = S1.substring(S1.length()-2,S1.length()); 
        row2 ="    "+H1+":"+M1+":"+S1+"    ";
    break;

    case 3:
      row1 = "<    REPEAT    >";
      if(repeatCycle == 0 )
      {
        row2 = "      OFF       ";
      }
      else
      {
        row2 = "  R   ON        ";
      }
    break;
  }
}

void fullDisplay()
{
  lcd.setCursor(0,0);
  lcd.print(row1);
  lcd.setCursor(0,1);
  lcd.print(row2);
}

void servoProcess()
{
      switch(state)
      {
        case 0:         
          onCountDown();
        break;
    
        case 1:
          offCountDown();
        break;
      }
}

void onCountDown()
{
  if( onTime[2] != 0 )
  {
  onTime[2]--;
  }
  else
  {
    if( onTime [1] != 0)
    {
      onTime[2] = 59;
      onTime[1]--;
    }
    else
    {
      if( onTime [0] != 0)
      {
        onTime[1] = 59;
        onTime[0]--;
      }
    }
  }
}

void offCountDown()
{
  if( offTime[2] != 0 )
  {
  offTime[2]--;
  }
  else          //secondsCheck
  {
    if( offTime [1] != 0)
    {
      offTime[2] = 59;
      offTime[1]--;
    }
    else
    {
      if( offTime [0] != 0)
      {
        offTime[1] = 59;
        offTime[0]--;
      }
    }
  }
}

void buzzerSound()
{
  digitalWrite(buzzer,HIGH);
  delay(50);
  digitalWrite(buzzer,LOW);
}
