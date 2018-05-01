/* Código para usar a Monster Motor Shield a partir de comandos de um radio controle

Monster Shield:
A0 - ENABLE MOTOR 1
A1 - ENABLE MOTOR 2
A2 - SENSOR CORRENTE MOTOR 1
A3 - SENSOR CORRENTE MOTOR 2
D7 - SENTIDO HORÁRIO MOTOR 1
D8 - SENTIDO ANTI HORÁRIO MOTOR 1
D4 - SENTIDO HORARIO MOTOR 2
D9 - SENTIDO ANTI HORARIO MOTOR 2
D5 - PWM MOTOR 1
D6 - PWM MOTOR 2

Receptor:
D10 - CANAL 1
D11 - CANAL 2

Sensor de corrente:
Iout/Isense = 11370
Rsense = 1500
Assim: Iout=(Vsense*11370)/1500, onde Vsense=(Varduino*5)/1023


*/
//radio
const int sinal_1=10;                  //canal 1
const int sinal_2=11;                  //canal 2
const int maximo=1970;                //valores
const int meio=1400;                  //lidos
const int minimo=970;                 //do controle
const int fator_seg=100;
//led
const int led=13;                     //erro
//MOTOR 1
const int enable1=14;                 //enable motor 1 D14=A0
const int pin_corrente_1=A2;           //sensor de corrente CI1 e CI2
const int anti_1=8;               
const int horario_1=7;             
const int pwm_motor_1=5;
//MOTOR 2
const int enable2=15;                  //enable motor 2 D15=A1
const int pin_corrente_2=A3;           //sensor de corrente CI3 e CI4
const int anti_2=9;               
const int horario_2=4; 
const int pwm_motor_2=6;            
//variaveis
int pwm1=0;                           //armazena valor lido do canal 1
int failsafe1=0;                      //armazena valor lido do canal 1
int pwm2=0;                           //armazena valor lido do canal 2
int failsafe2=0;                      //armazena valor lido do canal 2
float corrente1=0;                    //corrente CI1 e CI2
float corrente2=0;                    //corrente CI3 e CI4
float tensao1=0;
float tensao2=0;
//resistor e corrente máxima do ci
const int res=1500;                   //resistor utilizado nos CIs (em Ohms)
const int corrente_maxima=15;         //ci deve ser desligado caso passe do valor máximo da corrente, em A

void setup() {
  
pinMode(led,OUTPUT);
digitalWrite(led,LOW);
pinMode(sinal_1,INPUT);
pinMode(sinal_2,INPUT);
pinMode(enable1,OUTPUT);
pinMode(enable2,OUTPUT);
pinMode(anti_1,OUTPUT);
pinMode(horario_1,OUTPUT);
pinMode(anti_2,OUTPUT);
pinMode(horario_2,OUTPUT);  

Serial.begin(9600);

}

void loop() {
  
//leitura radio
pwm1=pulseIn(sinal_1,HIGH);
failsafe1=pwm1;
pwm2=pulseIn(sinal_2,HIGH);
failsafe2=pwm2;

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
//controle da corrente (valor em A)
tensao1=(analogRead(pin_corrente_1)*5)/1023;
corrente1=(tensao1*11370)/res;
tensao2=(analogRead(pin_corrente_2)*5)/1023;
corrente2=(tensao2*11370)/res;

//condição para o valor da corrente maior que o valor máximo
if(corrente1>corrente_maxima || corrente2>corrente_maxima){
    digitalWrite(enable1,LOW);            //desliga CI1 e CI2
    digitalWrite(enable2,LOW);            //desliga CI3 e CI4
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,LOW);
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,LOW);
    digitalWrite(led,HIGH);          //pisca led que indica erro
    delay(500);
    digitalWrite(led,LOW);
  }
*/ 

//failsafe
if(failsafe1==0 || failsafe2==0){
    digitalWrite(enable1,LOW);            //desliga CI1 e CI2
    digitalWrite(enable2,LOW);            //desliga CI3 e CI4
    
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,LOW);
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,LOW);
    
    digitalWrite(led,HIGH);               //pisca led que indica erro
    delay(250);
    digitalWrite(led,LOW);
  }
  
//ROTINA DE SAIDA
//PARADO
else if((pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg)) && (pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg))){
    digitalWrite(enable1,HIGH);                                    //CI1 e CI2 desligados
    digitalWrite(enable2,HIGH);                                    //CI3 e CI4 desligados 
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,LOW);
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,LOW);    
}
//PARA ESQUERDA
else if(pwm1>minimo && pwm1<meio-fator_seg && pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg)){
    pwm1=map(pwm1,minimo,meio-fator_seg,255,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,HIGH);                                 //MOTOR 1 SENTIDO HORARIO
    digitalWrite(anti_2,HIGH);                                    //MOTOR 2 SENTIDO ANTI HORARIO
    digitalWrite(horario_2,LOW);
    analogWrite(pwm_motor_1,pwm1);
    analogWrite(pwm_motor_2,pwm1);
}
 
//PARA DIREITA
else if(pwm1>meio+fator_seg && pwm1<maximo && pwm2>(meio-fator_seg) && pwm2<(meio+fator_seg)){
    pwm1=map(pwm1,maximo,meio+fator_seg,255,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados 
    digitalWrite(anti_1,HIGH);                                    //MOTOR 1 SENTIDO ANTI HORARIO
    digitalWrite(horario_1,LOW);                                  //MOTOR 2 SENTIDO HORARIO
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,HIGH);
    analogWrite(pwm_motor_1,pwm1);
    analogWrite(pwm_motor_2,pwm1);  
}

//PARA FRENTE 
else if(pwm2>minimo && pwm2<meio-fator_seg && pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg)){
    pwm2=map(pwm2,minimo,meio-fator_seg,255,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,HIGH);                                 //MOTOR 1 SENTIDO HORARIO
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,HIGH);                                 //MOTOR 2 SENTIDO HORARIO
    analogWrite(pwm_motor_1,pwm2);
    analogWrite(pwm_motor_2,pwm2);
}

//PARA TRÁS
else if(pwm2>meio+fator_seg && pwm2<maximo && pwm1>(meio-fator_seg) && pwm1<(meio+fator_seg)){
    pwm2=map(pwm2,maximo,meio+fator_seg,255,0);
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    digitalWrite(anti_1,HIGH);
    digitalWrite(horario_1,LOW);                                  //MOTOR 1 SENTIDO ANTI HORARIO  
    digitalWrite(anti_2,HIGH);                                    //MOTOR 2 SENTIDO ANTI HORARIO
    digitalWrite(horario_2,LOW); 
    analogWrite(pwm_motor_1,pwm2);
    analogWrite(pwm_motor_2,pwm2);
} 

//DIAGONAL DIREITA FRENTE
else if(pwm1>(meio+fator_seg) && pwm2<(meio-fator_seg)){
    pwm1=map(pwm1,maximo,meio+fator_seg,255,0);                  //direita
    pwm2=map(pwm2,minimo,meio-fator_seg,255,0);                  //frente
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,LOW);
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,HIGH);
    //analogWrite(pwm_motor_1,pwm1);
    analogWrite(pwm_motor_2,((pwm1+pwm2)/2));
}

//DIAGONAL DIREITA TRÁS
else if(pwm1>(meio+fator_seg) && pwm2>(meio+fator_seg)){
    pwm1=map(pwm1,maximo,meio+fator_seg,255,0);                  //direita
    pwm2=map(pwm2,maximo,meio+fator_seg,255,0);                  //trás
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    digitalWrite(anti_1,HIGH);
    digitalWrite(horario_1,LOW);
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,LOW);
    analogWrite(pwm_motor_1,((pwm1+pwm2)/2));
    //analogWrite(pwm_motor_2,((pwm1+pwm2)/2));
}

//DIAGONAL ESQUERDA FRENTE
else if(pwm1<(meio-fator_seg) && pwm2<(meio-fator_seg)){
    pwm1=map(pwm1,minimo,meio-fator_seg,255,0);                  //esquerda
    pwm2=map(pwm2,minimo,meio-fator_seg,255,0);                  //frente
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,HIGH);
    digitalWrite(anti_2,LOW);
    digitalWrite(horario_2,LOW);
    analogWrite(pwm_motor_1,((pwm1+pwm2)/2));
}

//DIAGONAL ESQUERDA TRÁS
else if(pwm1<(meio-fator_seg) && pwm2>(meio+fator_seg)){
    pwm1=map(pwm1,minimo,meio-fator_seg,255,0);                  //esquerda
    pwm2=map(pwm2,maximo,meio+fator_seg,255,0);                  //trás
    digitalWrite(enable1,HIGH);                                   //CI1 e CI2 ligados
    digitalWrite(enable2,HIGH);                                   //CI3 e CI4 ligados
    digitalWrite(anti_1,LOW);
    digitalWrite(horario_1,LOW);
    digitalWrite(anti_2,HIGH);
    digitalWrite(horario_2,LOW);
    analogWrite(pwm_motor_2,((pwm1+pwm2)/2));
}

}
