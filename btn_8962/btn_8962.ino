/* 
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
 *PIN 5 - GND (CONFIRMAR)
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

void setup() {
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
failsafe1=pwm1;
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
//failsafe
if(failsafe1==0 || failsafe2==0){
    digitalWrite(enable1,LOW);            //desliga CI1 e CI2
    digitalWrite(enable2,LOW);            //desliga CI3 e CI4
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0); 
  }
*/
//ROTINA DE SAIDA  
//PARADO
if(pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg) && pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg)){
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
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_horario_1,pwm1);                              //MOTOR 1 SENTIDO HORARIO
    analogWrite(pwm_anti_2,pwm1);                                 //MOTOR 2 SENTIDO ANTI HORARIO
}
  
//PARA DIREITA
if(pwm1>meio+fator_seg && pwm1<maximo){
    pwm1=map(pwm1,maximo,meio+fator_seg,245,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_anti_1,pwm1);                                //MOTOR 1 SENTIDO ANTI HORARIO
    analogWrite(pwm_horario_2,pwm1);                             //MOTOR 2 SENTIDO HORARIO
}

//PARA FRENTE 
if(pwm2>minimo && pwm2<meio-fator_seg){
    pwm2=map(pwm2,minimo,meio-fator_seg,245,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_horario_1,pwm2);                              //MOTOR 1 SENTIDO HORARIO
    analogWrite(pwm_horario_2,pwm2);                              //MOTOR 2 SENTIDO HORARIO
}

//PARA TRÁS
if(pwm2>meio+fator_seg && pwm2<maximo){
    pwm2=map(pwm2,maximo,meio+fator_seg,245,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_anti_1,pwm2);                                //MOTOR 1 SENTIDO ANTI HORARIO 
    analogWrite(pwm_anti_2,pwm2);                                //MOTOR 2 SENTIDO ANTI HORARIO
}
}
