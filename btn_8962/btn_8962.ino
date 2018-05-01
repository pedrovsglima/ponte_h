//radio
const int sinal=A0;
const int maximo=1970;    //valores
const int meio=1470;      //lidos
const int minimo=970;     //do controle
//led
const int led=8;
//CI 1
const int enable1=5;
const int pin_corrente1=A1;
const int pwm1=1;
//CI 2
const int enable2=6;
const int pin_corrente2=A2;
const int pwm2=2;
//variaveis
int pwm=0;
int failsafe=0;
int corrente1=0;
int corrente2=0;
int ten1=0;
int ten2=0;
int res1=0;
int res2=0;

void setup() {
pinMode(led,OUTPUT);
digitalWrite(led,LOW);
pinMode(enable1,OUTPUT);
pinMode(enable2,OUTPUT);
pinMode(corrente1,INPUT);
pinMode(corrente2,INPUT);
pinMode(pwm1,OUTPUT);
pinMode(pwm2,OUTPUT);
}

void loop() {

//leitura radio
pwm=pulseIn(sinal,HIGH);
failsafe=pulseIn(sinal,HIGH);

//condições descartadas
if(pwm>maximo)
pwm=maximo;
if(pwm<minimo)
pwm=minimo;

//controle da corrente (corrente dada em A)
corrente1=analogRead(pin_corrente1);
corrente2=analogRead(pin_corrente2);
ten1=(corrente1*1023)/5;
ten2=(corrente2*1023)/5;
corrente1=ten1/res1;
corrente2=ten2/res2;
//colocar condição para o valor da corrente

//failsafe
if(failsafe==0){
    digitalWrite(enable1,LOW);
    digitalWrite(enable2,LOW);}

//rotina de saida
//sentido horario
if(pwm>minimo && pwm<meio-50){
    pwm=map(pwm,minimo,meio-50,255,0);
    digitalWrite(enable1,HIGH);
    digitalWrite(enable2,HIGH);
    analogWrite(pwm1,0);
    analogWrite(pwm2,0);
    delay(25);
    analogWrite(pwm1,pwm);
    analogWrite(pwm2,0);
    pwm=map(pwm,255,0,minimo,meio-50);}
//sentido anti horario
if(pwm>meio+50 && pwm<maximo){
    pwm=map(pwm,meio+50,maximo,0,255);
    //pwm para 2 e gnd para 1 (IN)
    digitalWrite(enable1,HIGH);
    digitalWrite(enable2,HIGH);
    analogWrite(pwm1,0);
    analogWrite(pwm2,0);
    delay(25);
    analogWrite(pwm1,0);
    analogWrite(pwm2,pwm);
    pwm=map(pwm,0,255,meio+50,maximo);} 
//parado
if(pwm>meio-50 && pwm<meio+50){
    digitalWrite(enable1,LOW);
    digitalWrite(enable2,LOW);}
}
