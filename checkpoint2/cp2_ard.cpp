#include <ros.h>
#include <std_msgs/Int32.h>
#include <PID_v1.h>

ros::NodeHandle nh;
std_msgs::Int32 num1;
std_msgs::Int32 num2;

ros::Subscriber<std_msgs::Int32>chatter1("chatter1", &value_r);
ros::Subscriber<std_msgs::Int32>chatter2("chatter2", &value_l);

bool in1, in2, in3, in4;

const byte encoder0pinA_L = 2;      //A pin -> the interrupt pin 0
const byte encoder0pinB_L = 4;      //B pin -> the digital pin 3
byte encoder0PinALast_L;
double duration_L,abs_duration_L;   //the number of the pulses
boolean Direction_L;                //the rotation direction
boolean result_L;

const byte encoder0pinA_R = 3;
const byte encoder0pinB_R = 5;
byte encoder0PinALast_R;
double duration_R,abs_duration_R;
boolean Direction_R;
boolean result_R;

double val_output_L;                //Power supplied to the motor PWM value.
double val_output_R;
double Setpoint;
double Kp=0.6, Ki=5, Kd=0;
PID myPID_L(&abs_duration_L, &val_output_L, &Setpoint, Kp, Ki, Kd, DIRECT);
PID myPID_R(&abs_duration_R, &val_output_R, &Setpoint, Kp, Ki, Kd, DIRECT);

void value_r(const std_msgs::Int32 &num)
{
  num1=num;
  Setpoint=abs(num1.data);//setpoint must be positive
}
void value_l(const std_msgs::Int32 &num)
{
  num2=num;
}

void setup()
{
  nh.initNode();
  pinMode(6, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  Setpoint =0;                  //Set the output value of the PID
  myPID_L.SetMode(AUTOMATIC);   //PID is set to automatic mode
  myPID_R.SetMode(AUTOMATIC);
  myPID_L.SetSampleTime(100);   //Set PID sampling frequency is 100ms
  myPID_R.SetSampleTime(100);
  EncoderInit();                //Initialize the module
}

void loop()
{
  nh.subscribe(chatter1);
  nh.subscribe(chatter2);
  if(num1.data == num2.data)//straight
  {
    if(num1.data>0)         //forward
    {
      advance();
    }
    else                    //backward
    {
      back();
    }
    abs_duration_L=abs(duration_L);
    abs_duration_R=abs(duration_R);
    result_L=myPID_L.Compute();//PID conversion is complete and returns 1
    result_R=myPID_R.Compute();

    if (result_L && result_R)
    {
      duration_L=0;
      duration_R=0;
    }
  }
  else //turn
  {
    turn();
  }
  nh.spinOnce();
}

void EncoderInit()
{
  Direction_L = true;//default -> Forward
  Direction_R = true;
  pinMode(encoder0pinB_L,INPUT);
  attachInterrupt(0, wheelSpeed_L, CHANGE);
  pinMode(encoder0pinB_R, INPUT);
  attachInterrupt(1, wheelSpeed_R, CHANGE);
}

void wheelSpeed_L()
{
  int Lstate_L = digitalRead(encoder0pinA_L);
  if((encoder0PinALast_L == LOW) && Lstate_L==HIGH)
  {
    int val_L = digitalRead(encoder0pinB_L);
    if(val_L == LOW && Direction_L)
    {
      Direction_L = false; //Reverse
    }
    else if(val_L == HIGH && !Direction_L)
    {
      Direction_L = true;  //Forward
    }
  }
  encoder0PinALast_L = Lstate_L;

  if(!Direction_L)  duration_L++;
  else  duration_L--;
}

void wheelSpeed_R()
{
  int Lstate_R = digitalRead(encoder0pinA_R);
  if((encoder0PinALast_R == LOW) && Lstate_R == HIGH)
  {
    int val_R = digitalRead(encoder0pinB_R);
    if(val_R == LOW && Direction_R)
    {
      Direction_R = false;
    }
    else if (val_R == HIGH && !Direction_R)
    {
      Direction_R =true;
    }
  }
  encoder0PinALast_R = Lstate_R;

  if(!Direction_R)  duration_R++;
  else  duration_R--;
}

void advance()            //Motor Forward
{
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  analogWrite(6,val_output_L);
  digitalWrite(8,LOW);
  digitalWrite(9,HIGH);
  analogWrite(5,val_output_R);
}

void back()              //Motor Backward
{
  digitalWrite(11,LOW);
  digitalWrite(10,HIGH);
  analogWrite(6,val_output_L);
  digitalWrite(9,LOW);
  digitalWrite(8,HIGH);
  analogWrite(5,val_output_R);
}

void turn()
{
  if(num1.data>=0){
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH);
    analogWrite(5,num2.data);
  }
  else{
    digitalWrite(9,LOW);
    digitalWrite(8,HIGH);
    analogWrite(5,abs(num2.data));
  }
  if(num2.data>=0){
    digitalWrite(10,LOW);
    digitalWrite(11,HIGH);
    analogWrite(6,num1.data);
  }
  else{
    digitalWrite(11,LOW);
    digitalWrite(10,HIGH);
    analogWrite(6,abs(num1.data));
  }
}
