#include <ros.h>
#include <std_msgs/Int32.h>
#include <PID_v1.h>

ros::NodeHandle nh;
std_msgs::Int32 num1, num2, num3;
ros::Publisher chatter1("chatter1", &num1);
// ros::Publisher chatter2("chatter2", &num2);
// ros::Publisher chatter3("chatter3", &num3);



int stage = 0;
int moveSpeed = 150;
int turnSpeed = 100;
int turn_cum=6;
bool button_l, button_r, button_b, photo;
int timer_start, timer_current;

int stage2_delay = 1500;
int stage3_time = 5000;
bool stage2_bool = true;
bool stage3_bool = true;
int stage_flash;

// PID parameters
const byte encoder0pinA_L = 2;
const byte encoder0pinB_L = 4;
byte encoder0PinALast_L;
double duration_L, abs_duration_L;
boolean Direction_L;
boolean result_L;
const byte encoder0pinA_R = 3;
const byte encoder0pinB_R = 5;
byte encoder0PinALast_R;
double duration_R, abs_duration_R;
boolean Direction_R;
boolean result_R;
double val_output_L;
double val_output_R;
double Setpoint;
double Kpl = 1.8, Kil = 1.25, Kdl = 12;
PID myPID_L(&abs_duration_L, &val_output_L, &Setpoint, Kpl, Kil, Kdl, DIRECT);
double Kp = 2, Ki = 1, Kd = 8;
PID myPID_R(&abs_duration_R, &val_output_R, &Setpoint, Kp, Ki, Kd, DIRECT);


void value_cmd(const std_msgs::Int32 &num)
{
   if (int(num.data)==-1)
   {
      stop();
      stage=0;
      stage_flash=0;
   }
   else
   {
      stage_flash=(int(num.data));  
   }
}
ros::Subscriber<std_msgs::Int32>chatter_cmd("chatter_cmd", &value_cmd);     

void EncoderInit()
{
  Direction_L = true;//default -> Forward
  Direction_R = true;
  pinMode(encoder0pinB_L,INPUT);
  attachInterrupt(0, wheelSpeed_L, CHANGE);
  pinMode(encoder0pinB_R, INPUT);
  attachInterrupt(1, wheelSpeed_R, CHANGE);
}

void setup()
{
    nh.initNode();
    //Serial.begin(115200);
    pinMode(6, OUTPUT);         // ENA
    pinMode(10, OUTPUT);        // IN1
    pinMode(11, OUTPUT);        // IN2
    pinMode(5, OUTPUT);         // ENB
    pinMode(8, OUTPUT);         // IN3
    pinMode(9, OUTPUT);         // IN4
    Setpoint = 0;               // Set the initial output value of the PID
    myPID_L.SetMode(AUTOMATIC); // PID is set to automatic mode
    myPID_R.SetMode(AUTOMATIC);
    myPID_L.SetSampleTime(100); // Set PID sampling frequency is 100ms
    myPID_R.SetSampleTime(100);
    EncoderInit();
    stage=0;
    stage_flash=0;
    nh.advertise(chatter1);
    // nh.advertise(chatter2);
    // nh.advertise(chatter3);
    nh.subscribe(chatter_cmd);
}

void loop()
{
    button_b = (int(analogRead(A1)) >= 100);
    button_r = (int(analogRead(A2)) >= 100);
    button_l = (int(analogRead(A3)) >= 100);
    photo = (int(analogRead(A0)) <= 600);
    stage=stage_flash;
    switch (stage)
    {
    case 0:  //stop
        stop();
        break;

    case 1:  //right
        right();
        break;

    case -1:  //left
        left();
        break;
    
    case 2: // moveforward
        Setpoint = moveSpeed;
        advance();
        abs_duration_L = abs(duration_L);
        abs_duration_R = abs(duration_R);
        result_L = myPID_L.Compute(); // PID conversion is complete and returns 1
        result_R = myPID_R.Compute();
        if (result_L && result_R)
        {
            duration_L = 0; // count clear
            duration_R = 0;
        }
        break;

    default:
        break;
    }
    num1.data = int(stage);
    // num2.data = int(digitalRead(A2));
    // num3.data = int(digitalRead(A3));
    chatter1.publish(&num1);
    // chatter2.publish(&num2);
    // chatter3.publish(&num3);
    nh.spinOnce();
    //Serial.print("stage");
    //Serial.println(stage);
    delay(100);
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

void advance() // Motor Forward
{
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    analogWrite(6, val_output_L);
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
    analogWrite(5, val_output_R);
}

void right() //right
{
    digitalWrite(11, LOW);
    digitalWrite(10, HIGH);
    analogWrite(6, turnSpeed);
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
    analogWrite(5, turnSpeed+turn_cum);
}

void left() // left
{
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    analogWrite(6, turnSpeed);
    digitalWrite(9, LOW);
    digitalWrite(8, HIGH);
    analogWrite(5, turnSpeed+turn_cum);
}

void stop()
{
    analogWrite(5, 0);
    analogWrite(6, 0);
}


