/* 
 *Programa para testar o ci BTN8962 com dois motores.
 *A saída para o motor será de acordo com o valor lido do controle.
 *Um led indicará se o circuito está ligado e o outro quando houver algum erro.
 *
 *Frequência padrão do Arduino foi alterada para 31372.55 Hz
 * 
 *CI1 e CI2 - MOTOR 1                                                                                    Lógica controle dos motores:
 *CI3 e CI4 - MOTOR 2                                                                 | esquerda | direita | frente | trás | direita+frente | direita+trás | esquerda+frente | esquerda+trás
 *                                                                        anti 1      |     0    |   pwm1  |    0   | pwm2 |       0        |  pwm1+pwm2/2 |        0        |      0
 *RECEPTOR:                                                               horário 1   |   pwm1   |    0    |  pwm2  |   0  |       0        |      0       |   pwm1+pwm2/2   |      0
 *CANAL 1 - ESQUERDA E DIREITA                                            anti 2      |   pwm1   |    0    |    0   | pwm2 |       0        |      0       |        0        |  pwm1+pwm2/2
 *CANAL 2 - FRENTE E TRÁS                                                 horário 2   |     0    |   pwm1  |  pwm2  |   0  |   pwm1+pwm2/2  |      0       |        0        |      0
 *
 *
 *CONEXÃO BTN8962:
 *
 *PIN 1 - GND
 *PIN 2 - PWM
 *PIN 3 - ENABLE (LOW=SLEEP MODE)
 *PIN 4,8 - MOTOR
 *PIN 5 - RESISTOR 47k
 *PIN 6 - SENSOR DE CORRENTE (ADC)
 *PIN 7 - TENSÃO DO MOTOR
 *
 *Para o controle da corrente, os pinos IS do CI1 e CI2 são conectados na mesma entrada analógica. O mesmo vale para os pinos IS do CI3 e CI4.
 *
 *Cálculo do valor do resistor do sensor de corrente:
 *
 *A relação entre a tensão e a corrente é da seguinte forma: Varduino=(R*Imotor)/razão
 *A razão varia linearmente dependendo do valor do resistor.
 *Foi considerado como base o resistor com Rbase=1k e o fator é o valor que multiplica a relação base.
 *Esse fator é dado como R/Rbase. Por exemplo:
 *R=750  => fator=0,75
 *R=1,5k => fator=1,5
 *R=2k   => fator=2
 *
 */
//radio
const int sinal_1=7;                  //canal 1
const int sinal_2=5;                  //canal 2
const int maximo=1970;                //valores
const int meio=1400;                  //lidos
const int minimo=970;                 //do controle
const int fator_seg=100;
//led
const int led_on=8;                   //led para indicar que a placa está ligada
const int led_erro=4;                 //led para indicar erro (corrente alta ou failsafe), também usado na calibragem do sinal
//MOTOR 1
const int enable1=12;                 //enable CI1 e CI2
const int pin_corrente_1=1;           //sensor de corrente CI1 e CI2
const int pwm_anti_1=3;               //PWM CI1
const int pwm_horario_1=11;           //PWM CI2
//MOTOR 2
const int enable2=2;                  //enable CI3 e CI4
const int pin_corrente_2=4;           //sensor de corrente CI3 e CI4
const int pwm_anti_2=9;               //PWM CI3
const int pwm_horario_2=10;           //PWM CI4
//variaveis
int pwm1=0;                           //armazena valor lido do canal 1
int failsafe1=0;                      //armazena valor lido do canal 1
int pwm2=0;                           //armazena valor lido do canal 2
int failsafe2=0;                      //armazena valor lido do canal 2
float corrente1=0;                    //corrente CI1 e CI2
float corrente2=0;                    //corrente CI3 e CI4
float tensao1;
float tensao2=0;
//resistor e corrente máxima do ci
const int resbase=1000;
const int res=1500;                   //resistor utilizado nos CIs (em Ohms)
const int corrente_maxima=3;          //ci deve ser desligado caso passe do valor máximo da corrente, em A
int fator=0;                          //res/resbase
int a=0;                              //a e b são os coeficientes da reta
int b=0;

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

fator=res/resbase;
a=(0.09*fator)*1000;
b=(0.2*fator)*1000;

TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to 1 for PWM frequency of 31372.55 Hz
TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to 1 for PWM frequency of 31372.55 Hz

Serial.begin(9600);
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

//controle da corrente (corrente dada em A)
tensao1=analogRead(pin_corrente_1);
tensao1=map(tensao1,0,1023,0,5000);              //valor da tensão em mV
tensao2=analogRead(pin_corrente_2);
tensao2=map(tensao2,0,1023,0,5000);

Serial.print("tensao1:");
Serial.println(tensao1);
Serial.print("tensao2:");
Serial.println(tensao2);

corrente1=(tensao1-b)/a;                         //valor da corrente em A
corrente2=(tensao2-b)/a; 

Serial.print("corrente1:");
Serial.println(corrente1);
Serial.print("corrente2:");
Serial.println(corrente2);
delay(500);

/*
//condição para o valor da corrente maior que o valor máximo
if(corrente1>corrente_maxima || corrente2>corrente_maxima){
    digitalWrite(enable1,LOW);            //desliga CI1 e CI2
    digitalWrite(enable2,LOW);            //desliga CI3 e CI4
    digitalWrite(led_erro,HIGH);          //pisca led que indica erro
    delay(1000);
    digitalWrite(led_erro,LOW);
  }
  */
//failsafe
if(failsafe1==0 || failsafe2==0){
    digitalWrite(enable1,LOW);            //desliga CI1 e CI2
    digitalWrite(enable2,LOW);            //desliga CI3 e CI4
//    analogWrite(pwm_anti_1,0);
//    analogWrite(pwm_horario_1,0);
//    analogWrite(pwm_anti_2,0);
//    analogWrite(pwm_horario_2,0);
    digitalWrite(led_erro,HIGH);          //pisca led que indica erro
    delay(250);
    digitalWrite(led_erro,LOW);
  }
  
//ROTINA DE SAIDA
//PARADO
else if((pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg)) && (pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg))){
    digitalWrite(enable1,LOW);                                    //CI1 e CI2 desligados
    digitalWrite(enable2,LOW);                                    //CI3 e CI4 desligados 
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0);      
}
//PARA ESQUERDA
else if(pwm1>minimo && pwm1<meio-fator_seg && pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg)){
    pwm1=map(pwm1,minimo,meio-fator_seg,245,90);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_horario_1,pwm1);                              //MOTOR 1 SENTIDO HORARIO
    analogWrite(pwm_anti_2,pwm1);                                 //MOTOR 2 SENTIDO ANTI HORARIO
}
  
//PARA DIREITA
else if(pwm1>meio+fator_seg && pwm1<maximo && pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg)){
    pwm1=map(pwm1,maximo,meio+fator_seg,245,90);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados 
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_anti_1,pwm1);                                  //MOTOR 1 SENTIDO ANTI HORARIO
    analogWrite(pwm_horario_2,pwm1);                               //MOTOR 2 SENTIDO HORARIO  
}

//PARA FRENTE 
else if(pwm2>minimo && pwm2<meio-fator_seg && pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg)){
    pwm2=map(pwm2,minimo,meio-fator_seg,245,90);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_1,pwm2);                              //MOTOR 1 SENTIDO HORARIO
    analogWrite(pwm_horario_2,pwm2);                              //MOTOR 2 SENTIDO HORARIO
}

//PARA TRÁS
else if(pwm2>meio+fator_seg && pwm2<maximo && pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg)){
    pwm2=map(pwm2,maximo,meio+fator_seg,245,90);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_horario_2,0); 
    analogWrite(pwm_anti_1,pwm2);                                 //MOTOR 1 SENTIDO ANTI HORARIO
    analogWrite(pwm_anti_2,pwm2);                                 //MOTOR 2 SENTIDO ANTI HORARIO
} 
/*
//DIAGONAL DIREITA FRENTE
else if(pwm1>(meio+fator_seg) && pwm2<(meio-fator_seg)){
    pwm1=map(pwm1,maximo,meio+fator_seg,245,90);                  //direita
    pwm2=map(pwm2,minimo,meio-fator_seg,245,90);                  //frente
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,((pwm1+pwm2)/2));
}

//DIAGONAL DIREITA TRÁS
else if(pwm1>(meio+fator_seg) && pwm2>(meio+fator_seg)){
    pwm1=map(pwm1,maximo,meio+fator_seg,245,90);                  //direita
    pwm2=map(pwm2,maximo,meio+fator_seg,245,90);                  //trás
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0);
    analogWrite(pwm_anti_1,((pwm1+pwm2)/2));
}

//DIAGONAL ESQUERDA FRENTE
else if(pwm1<(meio-fator_seg) && pwm2<(meio-fator_seg)){
    pwm1=map(pwm1,minimo,meio-fator_seg,245,90);                  //esquerda
    pwm2=map(pwm2,minimo,meio-fator_seg,245,90);                  //frente
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_anti_2,0);
    analogWrite(pwm_horario_2,0);
    analogWrite(pwm_horario_1,((pwm1+pwm2)/2));
}

//DIAGONAL ESQUERDA TRÁS
else if(pwm1<(meio-fator_seg) && pwm2>(meio+fator_seg)){
    pwm1=map(pwm1,minimo,meio-fator_seg,245,90);                  //esquerda
    pwm2=map(pwm2,maximo,meio+fator_seg,245,90);                  //trás
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    analogWrite(pwm_anti_1,0);
    analogWrite(pwm_horario_1,0);
    analogWrite(pwm_horario_2,0);
    analogWrite(pwm_anti_2,((pwm1+pwm2)/2));
}
*/
}
