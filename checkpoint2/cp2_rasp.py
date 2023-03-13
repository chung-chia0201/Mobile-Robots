#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import Int32

def cp2_pub():
    pub_1 = rospy.Publisher('chatter1', Int32, queue_size=10) 
    #create a publisher who is name is pub_1,publish topic(chatter1), messange type is Int32 and queue size is 10.
    pub_2 = rospy.Publisher('chatter2', Int32, queue_size=10)
    #create a publisher who is name is pub_2,publish topic(chatter2), messange type is Int32 and queue size is 10.
    rospy.init_node('cp2_pub', anonymous=True)
    #initialize node, set node name is cp2_pub
    rate = rospy.Rate(10) # 10hz
    pro_pub_1 = Int32()
    pro_pub_2 = Int32()
    while not rospy.is_shutdown():
        pro_input_right = input("user's right is:") 
        pro_pub_1.data = int(pro_input_right)
        pro_input_left = input("user's left is:")
        pro_pub_2.data = int(pro_input_left)
        pub_1.publish(pro_pub_1)
        pub_2.publish(pro_pub_2)
        rate.sleep()
        #the right wheel setting parameters are published by pub_1, and the left wheel setting parameters are published by pub_2

if __name__ == '__main__':
    print("start")
    #show start before starting
    try:
        cp2_pub() 
    except rospy.ROSInterruptException:
        pass
    
    rospy.spin()