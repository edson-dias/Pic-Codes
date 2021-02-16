/*
   Cron�metro Digital com Acionamento por Sensor.
   Vers�o: 1.0
   Autor: Edson Dias.
   E-mail: edson_dias4@hotmail.com
   Data de Cria��o: 22/11/20
   Data de Modifica��o: 22/11/20 
*/

#include <16f877A.h>

#FUSES NOWDT                  // Sem Watch Dog Timer
#FUSES NOPUT                  // Sem Power Up Timer
#FUSES NOPROTECT              // Sem prote��o de leitura de c�digo!!
#FUSES NOBROWNOUT             // Sem brownout reset
#FUSES NODEBUG                // Sem modo de DebugNo Debug mode for ICD
#FUSES HS                     // Oscilador cristal > 4MHz (HS - High Speed)

#use delay (clock = 8MHz)


#define LED1 pin_a0           // Led referente ao Sensor 1
#define LED2 pin_a1           // Led referente ao Sensor 2
#define LED3 pin_a2           // Led referente ao Sensor 3
#define LED4 pin_a3           // Led referente ao Sensor 4
#define LED5 pin_c3           // Led referente ao start do cron�metro

#define DIG1 pin_b3           // Acionamento do display 1
#define DIG2 pin_b2           // Acionamento do display 2
#define DIG3 pin_b0           // Acionamento do display 3
#define DIG4 pin_b1           // Acionamento do display 4

#define SENS1 pin_b7          // Sensor 1
#define SENS2 pin_b6          // Sensor 2
#define SENS3 pin_b5          // Sensor 3
#define SENS4 pin_b4          // Sensor 4

#define BOT_ENTER pin_c5
#define BOT_UP pin_c6
#define BOT_RIGHT pin_c7

#define NUM0 0b11100111       // Vari�veis para acionamento dos displays.
#define NUM1 0b00100001
#define NUM2 0b11001011
#define NUM3 0b01101011
#define NUM4 0b00101101
#define NUM5 0b01101110
#define NUM6 0b11101110
#define NUM7 0b00100011
#define NUM8 0b11101111
#define NUM9 0b00101111


unsigned int count, unidade, decimo, centesimo, a, b, c, d = 0;
unsigned int flag1, flag2, flag3, flag4, option_switch = 0;
unsigned int flag_once_1, flag_once_2, flag_once_3, flag_once_4 = 0;
unsigned int select_display = 1;
int deze_display, unid_display, deci_display, cent_display, value1, value2, value3, value4 = 0;
int dezena,  sensor_flag = 0; 


#INT_TIMER0
void timer0_isr(void){
    
    // T = (256 - TMR0) * (4 / 8*10^6) * 16
    // Usando TMR0 como 231 --> T = 0,0002
    // T * 50 estouros do timer0 = 0,01 segundos!
    
    clear_interrupt(INT_TIMER0);
    set_timer0(231);
     
    if (sensor_flag > 0){
       count++;
       
       if(count > 49){
         count = 0;
         centesimo++;
         
       }
   
       if(centesimo>9){
         decimo++;
         centesimo = 0;
       }
   
       if(decimo>9){
         unidade++;
         decimo = 0;
       }
   
       if (unidade>9){
         dezena++;
         unidade = 0;
       }
        
       if (dezena>9){
         dezena = 0;
       }
    }
    
   if (sensor_flag == 0){
      count = 0;
      centesimo = 0;
      decimo = 0;
      unidade = 0;
      dezena = 0;
   }
}



#INT_RB
void interrupcao_externa(){ 

  if (input(SENS1) && flag1 == 1 && flag_once_1 == 0){
    a = dezena;
    b = unidade;
    c = decimo;
    d = centesimo;
    sensor_flag++;
    flag_once_1 = 1;
    output_low(LED1);
  }
  

  if (input(SENS2) && flag2 == 1 && flag_once_2 == 0){
    a = dezena;
    b = unidade;
    c = decimo;
    d = centesimo;
    sensor_flag++;
    flag_once_2 = 1;
    output_low(LED2);
  }

  
  if (input(SENS3) && flag3 == 1 && flag_once_3 == 0){
    a = dezena;
    b = unidade;
    c = decimo;
    d = centesimo;
    sensor_flag++;
    flag_once_3 = 1;
    output_low(LED3);
  }


  if (input(SENS4) && flag4 == 1 && flag_once_4 == 0){
    a = dezena;
    b = unidade;
    c = decimo;
    d = centesimo;
    sensor_flag++;
    flag_once_4 = 1;
    output_low(LED4);
  }
  
  if(sensor_flag>2){
  sensor_flag = 0;
  }
}



int retorna_disp();                       // Inicializa��o das fun��es.
void display_converter();
void show_display();
void set_value_display();
void internal_use_blink_display();
void blink_selected_display();
void menu();
void cronos();



void main(){

  set_tris_a(0b00000000);
  set_tris_b(0b11110000);
  set_tris_c(0b11100000);  
  set_tris_d(0b00000000);   
   
  output_a(0b00000000);
  output_low(LED5);

  setup_timer_0(RTCC_INTERNAL|RTCC_DIV_16);    // Configura��o do timer0 para clock interno e Pre-escaler de 16. 
  set_timer0(231);                             // Inicia a o flag do timer 0 no valor 231.
  clear_interrupt(INT_TIMER0);                 // Limpa o flag bit do Timer0
  enable_interrupts(INT_TIMER0);               // Ativa a interrup��o do Timer0
  enable_interrupts(INT_RB);                   // Ativa a interrup��o por mudan�a de estado
  enable_interrupts(GLOBAL);                   // Ativa as interrup��es globais
  
  ext_int_edge ( 1,  h_to_l );                 // Interrup��o ativada na descida do pulso
   
  while(true){

    switch(option_switch){
      
      case 0: menu();
        break;
      
      case 1: cronos();
        break;
    }
  }
}



int retorna_disp(int num){

  byte disp;

  if (num==0){ disp = NUM0;}
  if (num==1){ disp = NUM1;}  
  if (num==2){ disp = NUM2;}  
  if (num==3){ disp = NUM3;}  
  if (num==4){ disp = NUM4;}
  if (num==5){ disp = NUM5;}  
  if (num==6){ disp = NUM6;}
  if (num==7){ disp = NUM7;}
  if (num==8){ disp = NUM8;}  
  if (num==9){ disp = NUM9;}

  return disp;
}



void display_converter(int dez, int uni, int dec, int cen){
  cent_display = retorna_disp(cen);
  deci_display = retorna_disp(dec);
  unid_display = retorna_disp(uni);
  deze_display = retorna_disp(dez);
}



void show_display(int cent, int deci, int unid, int deze){

  output_d(cent);
  output_high(DIG4);
  delay_us(7);
  output_low(DIG4);

  output_d(deci);
  output_high(DIG3);
  delay_us(7);
  output_low(DIG3);
  
  output_d(unid);
  output_high(DIG2);
  output_high(pin_d4);
  delay_us(7);
  output_low(DIG2);
  output_low(pin_d4);

  output_d(deze);
  output_high(DIG1);
  delay_us(7);
  output_low(DIG1);

}



void set_value_display(int selected_display){      // Fun��o para alterar valores dos displays e para definir quais sensores estar�o ativos.

  if (selected_display == 1){
    
    value1++;
    output_high(LED1);
    
    if(value1>1){
      value1 = 0;
      output_low(LED1);
    } 
  }

  if (selected_display == 2){
    
    value2++;
    output_high(LED2);
    
    if(value2>1){
      value2 = 0;
      output_low(LED2);} 
  }

  if (selected_display == 3){
    value3++;
    output_high(LED3);
    
    if(value3>1){
      value3 = 0;
      output_low(LED3);
    } 
  }

  if (selected_display == 4){
    
    value4++;
    output_high(LED4);
    
    if(value4>1){
      value4 = 0;
      output_low(LED4);
    } 
  }
      
}



void internal_use_blink_display(int time_a, int time_b, int time_c, int time_d, int int_cent, int int_deci, int int_unid, int int_deze){
  
  // Fun��o de uso interno da fun��o blink_selected_display.
  
  output_d(int_cent);
  output_high(DIG4);
  delay_us(time_d);
  output_low(DIG4);

  output_d(int_deci);
  output_high(DIG3);
  delay_us(time_c);
  output_low(DIG3);
     
  output_d(int_unid);
  output_high(DIG2);
  output_high(pin_d4);
  delay_us(time_b);
  output_low(DIG2);
  output_low(pin_d4);
  
  output_d(int_deze);
  output_high(DIG1);
  delay_us(time_a);
  output_low(DIG1);

}


void blink_selected_display(int actual_pin, int cent, int deci, int unid, int deze){
  
  // Altera os valores de delay conforme o display atual, assim o display selecionado apresenta um brilho maior que os demais.
   
  if(actual_pin == 1){
    internal_use_blink_display(400, 7, 7, 7, cent, deci, unid, deze);        
  }
    
  if(actual_pin == 2){
    internal_use_blink_display(7, 400, 7, 7, cent, deci, unid, deze);
  }
    
  if(actual_pin == 3){
    internal_use_blink_display(7, 7, 400, 7, cent, deci, unid, deze);
  }
    
  if(actual_pin == 4){ 
    internal_use_blink_display(7, 7, 7, 400, cent, deci, unid, deze); 
  }
}



void menu(){
  
  int disp1, disp2, disp3, disp4 = 0;
  int test_condition = 0;
  boolean menu_flag = true;

   value1 = 0;
   value2 = 0;
   value3 = 0;
   value4 = 0;
  
  while(menu_flag == true){

    disp1 = retorna_disp(value1); 
    disp2 = retorna_disp(value2);
    disp3 = retorna_disp(value3);
    disp4 = retorna_disp(value4);
    
   blink_selected_display(select_display, disp4, disp3, disp2, disp1);
    
    if (input(BOT_UP)){
      delay_ms(200);
      set_value_display(select_display);
    }
     
    if (input(BOT_RIGHT)){
      delay_ms(200);  
      select_display++;
      
      if (select_display>4){
         select_display = 1;
      }
    }

    if (input(BOT_ENTER)){
      delay_ms(200);
      
      test_condition = value1 + value2 + value3 + value4;
      
      if (test_condition == 2){
          
          if (value1==1){flag1 = 1;}
          if (value2==1){flag2 = 1;}
          if (value3==1){flag3 = 1;}
          if (value4==1){flag4 = 1;}        
          
          menu_flag = false;

      }

      else {
          output_low(DIG1);
          output_low(DIG2);
          output_low(DIG3);
          output_low(DIG4);  
          delay_ms(1000);
      }       
    }   
  }

  menu_flag = true;
  option_switch = 1;

  output_low(DIG1);
  output_low(DIG2);
  output_low(DIG3);
  output_low(DIG4);
  output_high(LED5);
  
}



void cronos(){
  
  int cronos_flag = 0;
  int flag_enter = 0;
  int final_dez, final_uni, final_dec, final_cen = 0;
  
  while(cronos_flag == 0){

    if (sensor_flag == 2){
      final_dez = a;
      final_uni = b;
      final_dec = c;
      final_cen = d;
      cronos_flag = 1;
    }
  }

  display_converter(final_dez, final_uni, final_dec, final_cen);

  while(flag_enter == 0){
    show_display(cent_display, deci_display, unid_display, deze_display);  
    
    if (input(BOT_ENTER)){
      delay_ms(200);
      flag_enter = 1;
    }
  }
  
  flag1 = 0;
  flag2 = 0;
  flag3 = 0;
  flag4 = 0;
  flag_once_1 = 0;
  flag_once_2 = 0;
  flag_once_3 = 0;
  flag_once_4 = 0;
  sensor_flag = 0;
  option_switch = 0;
  cronos_flag = 0;
  flag_enter = 0;
  output_low(LED5);
}

