#!/usr/bin/env python
# license removed for brevity
import cv2
import picamera
import picamera.array
import numpy as np
import time
import rospy
from std_msgs.msg import Int32

def find_location():
    count_bound=20
    count0=0
    count1=0
    location=0
    stage=-2
    with picamera.PiCamera() as camera:
        camera.resolution = (320,304)
        camera.framerate = 30
        with picamera.array.PiRGBArray(camera) as stream:
            while True:
                if (count0+count1)>count_bound:
                    if count1>=2:
                        location_cmd=int(location/count1)
                        if location_cmd<=int(0.375*c_w):
                            print("right")
                            stage=1
                        elif location_cmd>=int(0.675*c_w):
                            print("left")
                            stage=-1
                        else:
                            print("straight")
                            stage=2
                        print("location_cmd",location_cmd)
                    else:
                        print("stop")  
                        stage=0 
                    return stage            

                camera.capture(stream, format='bgr')
                img = stream.array       
                img=cv2.flip(img, 0)
                h,w,_ = img.shape
                img = img[:,int(w/4):int(w*0.75)]
                c_h,c_w,_=img.shape
                gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
                ##GaussianBlur()
                blur=cv2.GaussianBlur(gray,(9,9),2,2)
                ret,thresh=cv2.threshold(gray,127,255,cv2.THRESH_BINARY | cv2.THRESH_OTSU)

                kernel = np.ones((5,5),np.uint8)            
                erosion = cv2.erode(thresh,kernel,iterations = 1)      #erosion
                dilation = cv2.dilate(erosion,kernel,iterations = 1)   #dilation        
                _,contours,_ = cv2.findContours(dilation,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
                if True:
                    for contour in contours:
                        approx = cv2.approxPolyDP(contour, 0.01* cv2.arcLength(contour, True), True)
 
                        M = cv2.moments(approx)  
                        try:
                            cx=int(M['m10']/M['m00'])  #centroid
                            cy=int(M['m01']/M['m00'])
                        except:
                            pass
                        if len(approx) == 3:
                            cv2.circle(img, (cx, cy), 6, (0, 0, 255), thickness=5)     #draw centroid
                            count1+=1
                            location+=cx
                        else:
                            count0+=1
                #cv2.imshow('frame', img)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
                # reset the stream before the next capture
                stream.seek(0)
                stream.truncate()
            cv2.destroyAllWindows()

def project_pub():
    pub = rospy.Publisher('chatter', Int32, queue_size=10)
    rospy.init_node('project_pub', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    pub_cmd = Int32()
    while not rospy.is_shutdown():
        pub_cmd.data = find_location()
        pub.publish(pub_cmd)
        rate.sleep()


if __name__ == '__main__':
    print("start")
    #show start before starting
    try:
        project_pub()
    except rospy.ROSInterruptException:
        pass
    rospy.spin()





