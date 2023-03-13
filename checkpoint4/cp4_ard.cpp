#include <ros.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Float32.h>
#include <PID_v1.h>
 
ros::NodeHandle nh;
std_msgs::Int32 num1;
std_msgs::Float32 flo1;

ros::Publisher chatter1("chatter1", &num1);
//ros::Publisher chatter1("chatter1", &flo1);
 
int stage = 0;
int moveSpeed = 220;
int backSpeed = 150;
int pushSpeed = 130;
int turnSpeed = 135;
int right_com = 2;
bool button_l, button_r, button_b, photo;
int timer_start, timer_current;

int photo_bound=650;      //need  test(550)
int find_door_delay=1;
int stage2_delay = 2000;  //back time
int stage3_time = 6500;   //turn time
int stage3_delay=350;     //turn delay time
int stage5_time = 15000;  //push ball time
int stage7_delay= 1500;
int stage8_time= 1000;
int stage9_time=500;

bool stage2_bool = true;
bool stage3_bool = true;
bool stage5_bool = true;
bool stage7_bool = true;
bool stage8_bool = true;
bool stage9_bool = true;
int stage_flash;
float rate;
 
 
void value_cmd(const std_msgs::Int32 &num)
{
   if (int(num.data)==-1)   //if command is -1, then stop
   {
      stop();
      stage=0;
      stage_flash=0;
   }
   else
   {
      stage_flash=(int(num.data))%2;  
   }
}
ros::Subscriber<std_msgs::Int32>chatter_cmd("chatter_cmd", &value_cmd);    
 
 
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
    stage=0;
    stage_flash=0;
    nh.advertise(chatter1);
    nh.subscribe(chatter_cmd);
}
 
void loop()
{
    button_b = (int(analogRead(A1)) >= 100);
    button_r = (int(analogRead(A2)) >= 100);
    button_l = (int(analogRead(A3)) >= 100);
    photo = (int(analogRead(A0)) <= photo_bound);
    find_door();
    switch (stage)
    {
    case 0:  //stand by
        if (stage_flash==1)
        {
            stage=1;
        }
        break;
    case 1: // moveforward
        if ((button_l || button_r) == true)
        {
            stop();
            stage = 2;
            break;
        }
        if (button_b == 1)
        {
            stage=5;
            break;
        }
        advance();
        break;
 
    case 2: // backward
        if (stage2_bool)
        {
            timer_start = int(millis());
            stage2_bool = false;
        }
        timer_current = int(millis() - timer_start);
        if (timer_current > stage2_delay)
        {
            stop();
            stage2_bool = true;
            stage = 3;
            break;
        }
        back();
        break;
 
    case 3: // turn
        turn();
        if ((button_l || button_r) == true)      //collision detect
        {
            stop();
            stage = 2;
            break;
        }
        if (stage3_bool)                  //start timer
        {
            timer_start = int(millis());
            stage3_bool = false;
        }
        timer_current = int(millis() - timer_start);
        if (timer_current > stage3_time)        //check stage time
        {
            stop();
            stage3_bool = true;
            stage = 1;
            break;
        }
        if (photo==1)          //detect light ball
        {
            delay(stage3_delay);
            stop();
            stage3_bool = true;
            stage = 4;
            break;
        }
        //turn();
        break;
 
    case 4: // catch the ball
        if (button_l == 1 || button_r == 1)
        {
            stop();
            stage = 2;
            break;
        }
        if (button_b == 1)
        {
            stop();
            stage=5;
            break;
        }
        advance();
        break;
    case 5: //push ball and detect door
        pushBall();
        //find_door();
        if (stage5_bool)                  //start timer
        {
            timer_start = int(millis());
            stage5_bool = false;
        }
        timer_current = int(millis() - timer_start);
        if (timer_current > stage5_time)        //check stage time
        {
            stop();
            stage5_bool = true;
            stage = 9;
            break;
        }
        if ((rate >= 0.12 && rate <= 0.26) || (rate >= 0.27 && rate <= 0.55))
        {
            stop();
            stage5_bool = true;
            stage=8;
            break;
        }
        
        if (button_l || button_r)
        {        
            stop();
            stage5_bool = true;
            stage = 7;
            break;
        }
        break;

    case 6: //push ball to door
        advance();
        if (button_l || button_r)
        {        
            stop();
            stage = 7;
            break;
        }
        break;
    case 7: 
        if (stage7_bool)
        {
            timer_start = int(millis());
            stage7_bool = false;
        }
        timer_current = int(millis() - timer_start);
        if (timer_current > stage7_delay)
        {
            stop();
            stage7_bool = true;
            stage = 5;
            break;
        }
        back();
        break;
     case 8:
        if (stage8_bool)
        {
            timer_start = int(millis());
            stage8_bool = false;
        }
        timer_current = int(millis() - timer_start);
        if (timer_current > stage8_time)
        {
            stop();
            stage8_bool = true;
            if ((rate >= 0.12 && rate <= 0.26) || (rate >= 0.27 && rate <= 0.55))
            {
                stop();
                stage=6;
                break;
            }
            else{
                stage = 5;
            }
            break;
        }
        break;
        
      case 9:
          advance();
          if (stage9_bool)
          {
              timer_start = int(millis());
              stage9_bool = false;
          }
          timer_current = int(millis() - timer_start);
          if (timer_current > stage9_time)
          {
              stop();
              stage9_bool=true;
              stage=5;
              break;
          }
      break;
    
    default:
        break;
    }
    num1.data = int(analogRead(A0));  //photo value
    //flo1.data=float(rate);
    chatter1.publish(&num1);
    //chatter1.publish(&flo1);
    nh.spinOnce();
    //Serial.print("stage");
    //Serial.println(stage);
    delay(100);
}
 
void advance() // Motor Forward
{
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    analogWrite(6, moveSpeed);
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
    analogWrite(5, moveSpeed+right_com);
}
 
void back() // Motor Backward
{
    digitalWrite(11, LOW);
    digitalWrite(10, HIGH);
    analogWrite(6, backSpeed);
    digitalWrite(9, LOW);
    digitalWrite(8, HIGH);
    analogWrite(5, backSpeed+right_com);
}
void turn() // turn
{
    digitalWrite(11, LOW);
    digitalWrite(10, HIGH);
    analogWrite(6, turnSpeed);
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
    analogWrite(5, turnSpeed + right_com);
}
 
void stop()
{
    analogWrite(5, 0);
    analogWrite(6, 0);
}

void pushBall()
{
    digitalWrite(10, LOW);      //left wheel
    digitalWrite(11, HIGH);
    analogWrite(6, pushSpeed);
    digitalWrite(9, LOW);
    digitalWrite(8, HIGH);
    analogWrite(5, pushSpeed+right_com);
}
 
void find_door()
{
    int val;
    float count1=0;
    float count0=0;
    for(int i=0; i<120; i++){
        val = digitalRead(A4);
        if(val) count1++;
        if(!val) count0++;
        delay(find_door_delay);
    }
    rate = count0 / (count1 + count0);
}

