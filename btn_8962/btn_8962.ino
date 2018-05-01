//radio
const int sinal=A0;
const int maximo=1970;
const int meio=1470;
const int minimo=970;
//led
const int led=8;
//CI 1
const int enable1=5;
const int pin_corrente1=A1;
const int pwm1=1;
//variaveis
int pwm=0;
int failsafe=0;
int corrente1=0;
int ten1=0;
int res1=0;

void setup() {
pinMode(led,OUTPUT);
digitalWrite(led,LOW);
pinMode(enable1,OUTPUT);
pinMode(corrente1,INPUT);
pinMode(pwm1,OUTPUT);
}

void loop() {
//leitura radio
pwm=pulseIn(sinal,HIGH);
failsafe=pulseIn(sinal,HIGH);
/*
//condições descartadas
if(pwm>maximo)
pwm=maximo;
if(pwm<minimo)
pwm=minimo;

//controle da corrente (corrente dada em A)
corrente1=analogRead(pin_corrente1);
ten1=(corrente1*1023)/5;
corrente1=ten1/res1;
//colocar condição para o valor da corrente*/

//failsafe
if(failsafe==0)
    digitalWrite(enable1,LOW);

//rotina de saida
//sentido horario
if(pwm>minimo && pwm<meio-50){
    pwm=map(pwm,minimo,meio-50,255,0);
    digitalWrite(enable1,HIGH);
    analogWrite(pwm1,0);
    delay(25);
    analogWrite(pwm1,pwm);
    pwm=map(pwm,255,0,minimo,meio-50);}
//parado
if(pwm>meio-50 && pwm<meio+50){
    digitalWrite(enable1,LOW);
    analogWrite(pwm1,0);
    delay(25);
    analogWrite(pwm1,pwm); //mandando pwm com enable desligado para confirmar que vai ficar parado mesmo assim
}}
