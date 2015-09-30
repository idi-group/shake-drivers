#!/usr/bin/env python

#This code has been inspired by mattzz project available at #http://mattzz.no-ip.org/wiki/Projects/PlayingWithInertialMeasurementUnits

#Part of the code also include exerpts from the shake library examples from the Univeristy of Glasgow available at https://github.com/andrewramsay/shake-drivers


import time, sys, platform
from OpenGL.GL import *
from OpenGL.GLU import *
from pyshake import *
import pygame
from pygame.locals import *
import serial


if len(sys.argv) != 2:
    print "pyshake_test.py <device address>"
    print ""
    print "Examples:"
    print "pyshake_test.py 4 (connect to COM port 5 on Windows (pyserial port numbers are 0-based))"
    print "pyshake_test.py /dev/tty.SHAKESK7SN0077-SPPDev (connect to a specific device on OSX"
    sys.exit(-1)

# create a ShakeDevice object and attempt to create the connection
sd = ShakeDevice(SHAKE_SK7)

try:
    param = int(sys.argv[1])
except ValueError:
    param = sys.argv[1]

if not sd.connect(param):
    print "Failed to connect!"
    sys.exit(-1)

sd.write_data_format(0x02) # binary
sd.write_power_state(0xFF) # turn on everything
sd.write_sample_rate(SHAKE_SENSOR_ACC, 25)
sd.write_sample_rate(SHAKE_SENSOR_MAG, 25)
sd.write_sample_rate(SHAKE_SENSOR_GYRO, 25)
sd.write_sample_rate(SHAKE_SENSOR_CAP, 25)

ax = ay = az = 0.0
yaw_mode = False

def resize((width, height)):
    if height==0:
        height=1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1.0*width/height, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def init():
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

def drawText(position, textString):
    font = pygame.font.SysFont ("Courier", 18, True)
    textSurface = font.render(textString, True, (255,255,255,255), (0,0,0,255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

def draw():
    global rquad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity()
    glTranslatef(0,0.0,-7.0)

    osd_text = "pitch: " + str("{0:.2f}".format(ay)) + ", roll: " + str("{0:.2f}".format(ax))

    if yaw_mode:
        osd_line = osd_text + ", yaw: " + str("{0:.2f}".format(az))
    else:
        osd_line = osd_text

    drawText((-2,-2, 2), osd_line)

    # the way I'm holding the IMU board, X and Y axis are switched
    # with respect to the OpenGL coordinate system
    if yaw_mode:                             # experimental
        glRotatef(az, 0.0, 1.0, 0.0)  # Yaw,   rotate around y-axis
    else:
        glRotatef(0.0, 0.0, 1.0, 0.0)
    glRotatef(ay ,1.0,0.0,0.0)        # Pitch, rotate around x-axis
    glRotatef(-1*ax ,0.0,0.0,1.0)     # Roll,  rotate around z-axis

    glBegin(GL_QUADS)
    glColor3f(0.0,1.0,0.0)
    glVertex3f( 1.0, 0.2,-1.0)
    glVertex3f(-1.0, 0.2,-1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f( 1.0, 0.2, 1.0)

    glColor3f(1.0,0.5,0.0)
    glVertex3f( 1.0,-0.2, 1.0)
    glVertex3f(-1.0,-0.2, 1.0)
    glVertex3f(-1.0,-0.2,-1.0)
    glVertex3f( 1.0,-0.2,-1.0)

    glColor3f(1.0,0.0,0.0)
    glVertex3f( 1.0, 0.2, 1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(-1.0,-0.2, 1.0)
    glVertex3f( 1.0,-0.2, 1.0)

    glColor3f(1.0,1.0,0.0)
    glVertex3f( 1.0,-0.2,-1.0)
    glVertex3f(-1.0,-0.2,-1.0)
    glVertex3f(-1.0, 0.2,-1.0)
    glVertex3f( 1.0, 0.2,-1.0)

    glColor3f(0.0,0.0,1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(-1.0, 0.2,-1.0)
    glVertex3f(-1.0,-0.2,-1.0)
    glVertex3f(-1.0,-0.2, 1.0)

    glColor3f(1.0,0.0,1.0)
    glVertex3f( 1.0, 0.2,-1.0)
    glVertex3f( 1.0, 0.2, 1.0)
    glVertex3f( 1.0,-0.2, 1.0)
    glVertex3f( 1.0,-0.2,-1.0)
    glEnd()

def read_data():
    global ax, ay, az
    ax = ay = az = 0.0
    line_done = 0
    ax, ay, az = sd.sk7_roll_pitch_heading()
    ax = ax/10
    ay = ay/10
    az = az/10


def main():
    global yaw_mode

    video_flags = OPENGL|DOUBLEBUF

    pygame.init()
    screen = pygame.display.set_mode((640,480), video_flags)
    pygame.display.set_caption("Press Esc to quit, z toggles yaw mode")
    resize((640,480))
    init()
    frames = 0
    ticks = pygame.time.get_ticks()
    while 1:
        event = pygame.event.poll()
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            break
        if event.type == KEYDOWN and event.key == K_z:
            yaw_mode = not yaw_mode
        read_data()
        draw()

        pygame.display.flip()
        frames = frames+1

    sd.close()

if __name__ == '__main__': main()
