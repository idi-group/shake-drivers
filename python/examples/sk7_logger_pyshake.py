import pygame, sys, time
from pyshake import *
from pygame.locals import *

SCREEN_WIDTH = 1000
SCREEN_HEIGHT = 750

acc_rect = pygame.Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/3)
mag_rect = pygame.Rect(0, SCREEN_HEIGHT/3, SCREEN_WIDTH, SCREEN_HEIGHT/3)
heading_rect = pygame.Rect(0, 2*(SCREEN_HEIGHT/3), SCREEN_WIDTH, SCREEN_HEIGHT/3)

acc_bg = pygame.Color(22, 22, 22)
mag_bg = pygame.Color(44, 44, 44)
heading_bg = pygame.Color(22, 22, 22)

graphs_mode = True

stream_colours = [pygame.Color(255, 0, 0), pygame.Color(0, 255, 0), pygame.Color(0, 0, 255)]

class ScrollingGraph:
    
    def __init__(self, _bgcolor, _name, _rect, _xpoints, _min, _max, _numstreams):
        self.bgcolor = _bgcolor
        self.name = _name
        self.rect = _rect
        self.xpoints = _xpoints
        self.min = _min
        self.max = _max
        self.numstreams = _numstreams
        self.font = pygame.font.Font(None, 20)
        self.fontsurf = self.font.render(_name, 1, (255, 255, 255))
            
        self.xstep = self.rect.width / (self.xpoints * 1.0)
        
        self.yscale = self.rect.height / (1.0 * (_max - _min))
        self.yoffset = 0
        if self.min < 0:
            self.yoffset = abs(self.min)

        tmp = [self.scale((abs(_max) - abs(_min))/2) for s in range(_numstreams)]
        self.data = [tmp for x in range(_xpoints)]

    def scale(self, val):
        # get value into 0-n range
        nv = val + self.yoffset 

        # scale
        nv *= self.yscale

        # adjust
        nv = self.rect.height - nv

        # vertical offset for display rectangle
        nv += self.rect.y

        return nv
        
    def draw(self, surface, streamvals):
        surface.fill(self.bgcolor, self.rect)
        
        self.data = self.data[1:]
        
        adjvals = []
        for s in streamvals:
            adjvals.append(self.scale(s))


        self.data.append(adjvals)

        pointlist = [[] for x in range(self.numstreams)]
        for d in range(len(self.data)):
            for s in range(len(self.data[d])):
                y = self.data[d][s]
                pointlist[s].append((self.xstep * d, y))

        for s in range(self.numstreams):
            pygame.draw.lines(surface, stream_colours[s], False, pointlist[s], 1)
        surface.blit(self.fontsurf, (self.rect.x+10, self.rect.y+10))

def pygame_loop(shake_address):
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("SHAKE logger")
    running = True


    font = pygame.font.Font(None, 20)
    fontsurf = font.render("Connecting to the SHAKE using port " + str(shake_address) + "...", 1, (255, 255, 255))
    screen.blit(fontsurf, (20, 20))
    pygame.display.flip()

    # create a connection to the SHAKE 
    shake = ShakeDevice(SHAKE_SK7)
    if not shake.connect(shake_address):
        print "Failed to connect to the SHAKE using port " + str(shake_address)
        sys.exit(-1)
    
    shake.write_data_format(0x02) # binary
    shake.write_power_state(0xFF) # turn on everything
    shake.write_sample_rate(SHAKE_SENSOR_ACC, 100)
    shake.write_sample_rate(SHAKE_SENSOR_MAG, 100)
    shake.write_sample_rate(SHAKE_SENSOR_GYRO, 100)
    shake.write_sample_rate(SHAKE_SENSOR_CAP, 100)
    
    acc_graph = ScrollingGraph(acc_bg, "Accelerometer", acc_rect, 250, -2500, 2500, 3)
    mag_graph = ScrollingGraph(mag_bg, "Magnetometer", mag_rect, 250, -3000, 3000, 3)
    #heading_graph = ScrollingGraph(heading_bg, "Heading", heading_rect, 250, -100, 3700, 1)
    gyro_graph = ScrollingGraph(heading_bg, "Gyroscope", heading_rect, 250, -10000, 10000, 3)
    
    while running:
        event = pygame.event.poll()
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYUP:
            print event.key
            if event.key == K_ESCAPE:
                running = False
            if event.key == ord('c'):
                global graphs_mode
                graphs_mode = not graphs_mode

        if graphs_mode:
            # read SHAKE accelerometer data into a 3 element list [x, y, z]
            acc = shake.acc()
            acc_graph.draw(screen, acc)
        
            # read SHAKE magnetometer data into a 3 element list [x, y, z]
            mag = shake.mag()
            mag_graph.draw(screen, mag)
        
            # read SHAKE heading data
            #heading_graph.draw(screen, [shake.heading()])

            gyro = shake.gyro()
            gyro_graph.draw(screen, gyro)
        else:
            pygame.draw.rect(screen, (75, 75, 75), (0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), 0)
            cap = shake.cap()
            # draw capacitive grid
            basex = (SCREEN_WIDTH - 300) / 2
            basey = (SCREEN_HEIGHT - 300) / 2

            for i in range(4):
                yoffset = basey + (i*100)
                for j in range(3):
                    xoffset = basex + (j*100)
                    
                    curval = cap[(3*i)+j]
                    pygame.draw.rect(screen, (curval, curval, curval), (xoffset, yoffset, 100, 100), 0)
        
        pygame.display.flip()
        pygame.time.wait(25)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Usage: shake_logger <port number>"
        sys.exit(-1)
        
    portnum = sys.argv[1]
    try:
        portnum = int(portnum)
    except:
        pass
        
    print "Opening port " + str(portnum)
    pygame_loop(portnum)
