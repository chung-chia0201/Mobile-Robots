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
int backSpeed = 110;
int right_com = 6;
bool button_l, button_r, button_b, photo_bool;
int photo;
int timer_start, timer_current;

int stage2_delay = 1500;
int stage3_time = 7500;
int stage5_time = 7500; //must large then stage3_time
bool stage2_bool = true;
bool stage3_bool = true;
int stage_flash;

bool stage5_bool=true;
int min_photo_final;
int min_photo=1000;


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
    // nh.advertise(chatter2);
    // nh.advertise(chatter3);
    nh.subscribe(chatter_cmd);
}

void loop()
{
    button_b = (int(analogRead(A1)) >= 100);
    button_r = (int(analogRead(A2)) >= 100);
    button_l = (int(analogRead(A3)) >= 100);
    photo = int(analogRead(A0));
    switch (stage)
    {
    case 0:
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
            stage=0;
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
            min_photo_final=min_photo+50;
            stage = 5;
            delay(500);
            break;
        }
        if (min_photo >= photo)          //detect light ball
        {
            min_photo= photo;
        }
        turn();
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
            stage=0;
            break;
        }
        advance();
        break;
        
        case 5: 
        if ((button_l || button_r) == true)      //collision detect
        {
            stop();
            stage = 2;
            break;
        }
        if (stage5_bool == true)                  //start timer
        {
            timer_start = int(millis());
            stage5_bool = false;
        }
        timer_current = int(millis() - timer_start);
        if (timer_current > stage5_time)        //check stage time
        {
            stop();
            stage5_bool = true;
            min_photo=1000;
            stage = 1;
            break;
        }
        if (photo <= min_photo_final)          //detect light ball
        {
            stop();
            stage5_bool = true;
            stage = 4;
            break;
        }
        turn();
        break;

    default:
        break;
    }
    
    num1.data = int(photo);
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
    analogWrite(6, 80);
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
    analogWrite(5, 100);
}

void stop()
{
    analogWrite(5, 0);
    analogWrite(6, 0);
}


