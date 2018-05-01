/* 
 *Programa para testar o ci BTN8962 com dois motores.
 *A saída para o motor será de acordo com o valor lido do controle.
 *Um led indicará se o circuito está ligado e o outro quando houver erro.
 *
 *Frequência padrão do Arduino foi alterada para 31372.55 Hz
 *
 *CI1 e CI2 - MOTOR 1
 *CI3 e CI4 - MOTOR 2
 *
 *RECEPTOR:
 *CANAL 1 - ESQUERDA E DIREITA
 *CANAL 2 - FRENTE E TRÁS
 *
 *CONEXÃO BTN8962:
 *
 *PIN 1 - GND
 *PIN 2 - PWM
 *PIN 3 - ENABLE (LOW=SLEEP MODE)
 *PIN 4,8 - MOTOR
 *PIN 5 - RESISTOR (MAIOR POSSÍVEL -> 51K (Usando 47K)
 *PIN 6 - SENSOR DE CORRENTE (ADC)
 *PIN 7 - TENSÃO DO MOTOR
*/
//radio
const int sinal_1=7;                 //canal 1
const int sinal_2=5;                 //canal 2
const int maximo=1970;                //valores
const int meio=1470;                  //lidos
const int minimo=970;                 //do controle
const int fator_seg=100;
//led
const int led_on=8;                   //led para indicar que a placa está ligada
const int led_erro=4;                 //led para indicar erro (corrente alta ou failsafe)
//MOTOR 1
const int enable1=12;                 //enable CI1 e CI2
const int pin_cor_anti_1=A2;          //sensor de corrente CI1
const int pin_cor_horario_1=A3;       //sensor de corrente CI2
const int pwm_anti_1=3;               //PWM CI1
const int pwm_horario_1=11;           //PWM CI2
//MOTOR 2
const int enable2=2;                  //enable CI3 e CI4
const int pin_cor_anti_2=A4;          //sensor de corrente CI3
const int pin_cor_horario_2=A5;       //sensor de corrente CI4
const int pwm_anti_2=9;               //PWM CI3
const int pwm_horario_2=10;           //PWM CI4
//variaveis
int pwm1=0;                           //armazena valor lido do canal 1
int failsafe1=0;                      //armazena valor lido do canal 1
int pwm2=0;                           //armazena valor lido do canal 2
int failsafe2=0;                      //armazena valor lido do canal 2
int corrente1=0;                      //corrente CI1
int corrente2=0;                      //corrente CI2
int corrente3=0;                      //corrente CI3
int corrente4=0;                      //corrente CI4
//resistores e corrente máxima do ci
const int res1=0;                     //resistor utilizado no CI1 (em Ohms)
const int res2=0;                     //resistor utilizado no CI2 (em Ohms)
const int res3=0;                     //resistor utilizado no CI3 (em Ohms)
const int res4=0;                     //resistor utilizado no CI4 (em Ohms)
const int corrente_maxima=0;          //ci deve ser desligado caso passe do valor máximo da corrente, em A

void setup() {
pinMode(led_on,OUTPUT);
pinMode(led_erro,OUTPUT);
digitalWrite(led_on,HIGH);
digitalWrite(led_erro,LOW);
pinMode(sinal_1,INPUT);
pinMode(sinal_2,INPUT);
pinMode(enable1,OUTPUT);
pinMode(enable2,OUTPUT);
pinMode(pwm_anti_1,OUTPUT);
pinMode(pwm_horario_1,OUTPUT);
pinMode(pwm_anti_2,OUTPUT);
pinMode(pwm_horario_2,OUTPUT);

TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to 1 for PWM frequency of 31372.55 Hz
TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to 1 for PWM frequency of 31372.55 Hz
}

void loop() {

//leitura radio
pwm1=pulseIn(sinal_1,HIGH);
failsafe1=pulseIn(sinal_1,HIGH);
pwm2=pulseIn(sinal_2,HIGH);
failsafe2=pulseIn(sinal_2,HIGH);

//condições descartadas
if(pwm1>maximo)
pwm1=maximo;
if(pwm1<minimo)
pwm1=minimo;
if(pwm2>maximo)
pwm2=maximo;
if(pwm2<minimo)
pwm2=minimo;

/*
//controle da corrente (corrente dada em A)
corrente1=analogRead(pin_cor_anti_1);
corrente2=analogRead(pin_cor_horario_1);
corrente3=analogRead(pin_cor_anti_2);
corrente4=analogRead(pin_cor_horario_2);

corrente1=((corrente1*1023)/5)/res1;
corrente2=((corrente2*1023)/5)/res2;
corrente3=((corrente3*1023)/5)/res3;
corrente4=((corrente4*1023)/5)/res4;

//condição para o valor da corrente maior que o valor máximo
if(corrente1>corrente_maxima || corrente2>corrente_maxima || corrente3>corrente_maxima || corrente4>corrente_maxima){
    digitalWrite(enable1,LOW);            //desliga CI1 e CI2
    digitalWrite(enable2,LOW);            //desliga CI3 e CI4
    digitalWrite(led_erro,HIGH);          //acende led que indica erro
    delay(1000);
  }
  else digitalWrite(led_erro,LOW);        //se não, desliga led que indica erro
*/  

//failsafe
if(failsafe1==0 || failsafe2==0){
    digitalWrite(enable1,LOW);            //desliga CI1 e CI2
    digitalWrite(enable2,LOW);            //desliga CI3 e CI4
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0);
    digitalWrite(led_erro,HIGH);          //acende led que indica erro
  }
  else digitalWrite(led_erro,LOW);        //se não, desliga led que indica erro


//ROTINA DE SAIDA
//PARADO
if((pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg)) && (pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg))){
    digitalWrite(enable1,LOW);                                    //CI1 e CI2 desligados
    digitalWrite(enable2,LOW);                                    //CI3 e CI4 desligados 
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0);      
}
//PARA ESQUERDA
if(pwm1>minimo && pwm1<meio-fator_seg){
    pwm1=map(pwm1,minimo,meio-fator_seg,245,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
//    analogWrite(pwm_horario_1,0);
//    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_horario_1,pwm1);                              //MOTOR 1 SENTIDO HORARIO
    analogWrite(pwm_anti_2,pwm1);                                 //MOTOR 2 SENTIDO ANTI HORARIO
}
  
//PARA DIREITA
else if(pwm1>meio+fator_seg && pwm1<maximo){
    pwm1=map(pwm1,maximo,meio+fator_seg,245,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados 
//    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
//    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_anti_1,pwm1);                                  //MOTOR 1 SENTIDO ANTI HORARIO
    analogWrite(pwm_horario_2,pwm1);                               //MOTOR 2 SENTIDO HORARIO  
}

//PARA FRENTE 
if(pwm2>minimo && pwm2<meio-fator_seg){
    pwm2=map(pwm2,minimo,meio-fator_seg,245,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
//    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
//    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_horario_1,pwm2);                              //MOTOR 1 SENTIDO HORARIO
    analogWrite(pwm_horario_2,pwm2);                              //MOTOR 2 SENTIDO HORARIO
}

//PARA TRÁS
else if(pwm2>meio+fator_seg && pwm2<maximo){
    pwm2=map(pwm2,maximo,meio+fator_seg,245,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
//    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
//    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_anti_1,pwm2);                                  //MOTOR 1 SENTIDO ANTI HORARIO
    analogWrite(pwm_anti_2,pwm2);                                  //MOTOR 1 SENTIDO ANTI HORARIO
} 
}
